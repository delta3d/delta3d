/* -*-c++-*-
 * Delta3D
 * Copyright 2009, Alion Science and Technology
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#include "aiutilityapp.h"

#include <osgViewer/GraphicsWindow>
#include <osgGA/EventQueue>

#include <dtUtil/exception.h>

#include <dtCore/project.h>

#include <dtCore/deltawin.h>
#include <dtCore/system.h>

#include <dtCore/flymotionmodel.h>
#include <dtCore/objectmotionmodel.h>
#include <dtCore/scene.h> //for GetHeightOfTerrain()
#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointrenderinfo.h>
#include <dtAI/waypointpair.h>
#include <dtAI/waypointgraph.h>

#include <QtGui/QUndoCommand>

#include "aicomponent.h"
#include "aiutilityinputcomponent.h"
#include "waypointselection.h"
#include "waypointmotionmodel.h"
#include "undocommands.h"


////////////////////////////////////////////////////////////////////////////////
AIUtilityApp::AIUtilityApp()
: dtABC::Application("config.xml")
, mGM(new dtGame::GameManager(*GetScene()))
, mSelectionBasedRendering(false)
{
   mGM->SetApplication(*this);
   mLastCameraTransform.MakeIdentity();

   // Mark this app as an editor so custom libraries
   // can query what they're running in and act appropriately
   dtCore::Project::GetInstance().SetEditMode(true);
}

////////////////////////////////////////////////////////////////////////////////
AIUtilityApp::~AIUtilityApp()
{
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::Config()
{
   dtABC::Application::Config();

   AIComponent* aicomp = new AIComponent();
   mGM->AddComponent(*aicomp, dtGame::GameManager::ComponentPriority::NORMAL);

   mMotionModel = new dtCore::FlyMotionModel(GetKeyboard(), GetMouse());
   mMotionModel->SetTarget(GetCamera());

   mWaypointMotionModel = new WaypointMotionModel(GetView());
   mWaypointMotionModel->SetEnabled(false);
   connect(&WaypointSelection::GetInstance(), SIGNAL(WaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>&)),
           mWaypointMotionModel.get(), SLOT(OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>&)));

   connect(mWaypointMotionModel, SIGNAL(UndoCommandGenerated(QUndoCommand*)), this, SLOT(OnUndoCommandCreated(QUndoCommand*)));

   dtCore::System::GetInstance().Start();
   mStepper.Start();
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::SetAIPluginInterface(dtAI::AIPluginInterface* interface, bool selectionBasedRenderingHint)
{
   if (!mInputComponent.valid())
   {
      mInputComponent = new AIUtilityInputComponent();
      mGM->AddComponent(*mInputComponent, dtGame::GameManager::ComponentPriority::NORMAL);

      dtCore::FlyMotionModel* fmm = dynamic_cast<dtCore::FlyMotionModel*>(mMotionModel.get());
      if(fmm != NULL)
      {
         mInputComponent->SetCameraMotionModel(*fmm);
      }
   }

   // We can now setup the input component
   mInputComponent->SetAIPluginInterface(interface);

   //input component needs to know about the ObjectMotionModel, so they don't collide on mouse events
   mInputComponent->SetWaypointMotionModel(mWaypointMotionModel.get());

   mWaypointMotionModel->SetAIInterface(interface);
   mAIInterface = interface;

   emit AIPluginInterfaceChanged(interface, selectionBasedRenderingHint);
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::DoQuit()
{
   mStepper.Stop();
   dtCore::System::GetInstance().Stop();
   Quit();
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::SetProjectContext(const std::string& path)
{
   try
   {
      dtCore::Project::GetInstance().SetContext(path);
   }
   catch (const dtUtil::Exception& ex)
   {
      ex.LogException();
   }
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::ChangeMap(const std::string& map)
{
   mGM->ChangeMap(map);
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::CloseMap()
{
   mGM->CloseCurrentMap();
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::PreFrame(const double deltaSimTime)
{
   BaseClass::PreFrame(deltaSimTime);
   dtCore::Transform xform;
   GetCamera()->GetTransform(xform);

   if (!xform.EpsilonEquals(mLastCameraTransform))
   {
      mLastCameraTransform = xform;
      emit CameraTransformChanged(xform);
   }
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::TransformCamera(const dtCore::Transform& xform)
{
   GetCamera()->SetTransform(xform);
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::AddAIInterfaceToMap(const std::string& map)
{
   AIComponent* aicomp = NULL;
   mGM->GetComponentByName(AIComponent::DEFAULT_NAME, aicomp);
   try
   {
      aicomp->AddAIInterfaceToMap(map);
   }
   catch (const dtUtil::Exception& ex)
   {
      emit Error(ex.ToString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::OnSelectWaypontBrushMode(bool enable)
{
   if (mInputComponent.valid())
   {
      mInputComponent->OnSelectWaypontBrushMode(enable);
   }
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::OnWaypointBrushSizeChanged(double value)
{
   if (mInputComponent.valid())
   {
      mInputComponent->OnSelectBrushSizeChanged(value);
   }
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::OnRenderOnSelectChanged(bool enabled)
{
   mSelectionBasedRendering = enabled;

   if (mSelectionBasedRendering)
   {
      OnWaypointSelectionChanged(WaypointSelection::GetInstance().GetWaypointList());
   }
   else
   {
      //redraw the whole nav mesh
      if (mAIInterface)
      {
         if (mAIInterface->GetDebugDrawable()->GetRenderInfo()->GetRenderNavMesh())
         {
            dtAI::NavMesh* navmesh = mAIInterface->GetWaypointGraph().GetNavMeshAtSearchLevel(0);
            if (navmesh != NULL)
            {
               mAIInterface->GetDebugDrawable()->UpdateWaypointGraph(*navmesh);
            }
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::OnRenderBackfacesChanged(bool enabled)
{
   osg::StateAttribute::Values value = (enabled) ? osg::StateAttribute::OFF: osg::StateAttribute::ON;

   // Apply the backface setting to the whole scene
   osg::Node* sceneNode = GetScene()->GetSceneNode();
   sceneNode->getOrCreateStateSet()->setMode(GL_CULL_FACE, value | osg::StateAttribute::OVERRIDE);
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::OnUndoCommandCreated(QUndoCommand* undoCommand)
{
   emit UndoCommandGenerated(undoCommand);
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::OnGroundClampSelectedWaypoints()
{
   std::vector<dtAI::WaypointInterface*> selected = WaypointSelection::GetInstance().GetWaypointList();

   bool motionWasEnabled = false;
   bool idWasEnabled = false;

   if (mWaypointMotionModel->IsEnabled())
   {
      motionWasEnabled = true;
      mWaypointMotionModel->SetEnabled(false);
   }

   if (mAIInterface->GetDebugDrawable()->GetRenderInfo()->GetRenderWaypointID())
   {
      idWasEnabled = true;
      mAIInterface->GetDebugDrawable()->GetRenderInfo()->SetRenderWaypointID(false);
      mAIInterface->GetDebugDrawable()->OnRenderInfoChanged();
   }

   QUndoCommand* undoCmd = GroundClampWaypoints(selected);
   if (undoCmd)
   {
      emit UndoCommandGenerated(undoCmd);
   }

   if (motionWasEnabled)
   {
      mWaypointMotionModel->SetEnabled(true);
   }

   if (idWasEnabled)
   {
      mAIInterface->GetDebugDrawable()->GetRenderInfo()->SetRenderWaypointID(true);
      mAIInterface->GetDebugDrawable()->OnRenderInfoChanged();
   }
}

////////////////////////////////////////////////////////////////////////////////
QUndoCommand* AIUtilityApp::GroundClampWaypoints(std::vector<dtAI::WaypointInterface*>& selected)
{
   if (selected.empty())
   {
      return NULL;
   }

   //if there's more than one to move, batch the commands under one parent undo command
   QUndoCommand* parentUndo(selected.size()==1 ? NULL : new QUndoCommand("Move Waypoints"));
   MoveWaypointCommand* undoMove = NULL;

   std::vector<dtAI::WaypointInterface*>::iterator itr = selected.begin();
   while (itr != selected.end())
   {
      const osg::Vec3 wpXYZ = (*itr)->GetPosition();
      float hot = 0.f;

      if (GetScene()->GetHeightOfTerrain(hot, wpXYZ[0], wpXYZ[1]))
      {
         undoMove = new MoveWaypointCommand(wpXYZ,
                           osg::Vec3(wpXYZ[0], wpXYZ[1], hot),
                           **itr,
                           mAIInterface.get(), parentUndo);

         connect(undoMove, SIGNAL(WaypointsMoved()), mWaypointMotionModel, SLOT(OnWaypointsMoved()));
      }

      ++itr;
   }

   if (selected.size() == 1)
   {
      return undoMove;
   }
   else
   {
      return parentUndo;
   }
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::OnWaypointPropertyBaseChanged()
{
   if (mWaypointMotionModel.valid())
   {
      mWaypointMotionModel->UpdateWidgetsForSelection();
   }
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>& selectedWaypoints)
{
   if (mAIInterface == NULL)
   {
      return;
   }

   const osg::Vec4 kSelectedColor(1.0f, 0.1f, 0.1f, 1.0f);
   mAIInterface->GetDebugDrawable()->ResetWaypointColorsToDefault();

   std::vector<dtAI::WaypointPair> edgesForSelection;

   for (size_t i = 0; i < selectedWaypoints.size(); ++i)
   {
      dtAI::WaypointInterface* wpi = selectedWaypoints[i];

      if (wpi)
      {
         mAIInterface->GetDebugDrawable()->SetWaypointColor(*wpi, kSelectedColor);
      }

      if (mSelectionBasedRendering)
      {
         std::vector<const dtAI::WaypointInterface*> edgePoints;
         mAIInterface->GetAllEdgesFromWaypoint(wpi->GetID(), edgePoints);

         for (size_t edgeIndex = 0; edgeIndex < edgePoints.size(); ++edgeIndex)
         {
            edgesForSelection.push_back(dtAI::WaypointPair(wpi, edgePoints[edgeIndex]));
         }
      }
   }

   if (mSelectionBasedRendering)
   {
      if (mAIInterface->GetDebugDrawable()->GetRenderInfo()->GetRenderNavMesh())
      {
         mAIInterface->GetDebugDrawable()->SetEdges(edgesForSelection);
      }

      if (mAIInterface->GetDebugDrawable()->GetRenderInfo()->GetRenderWaypointID())
      {
         mAIInterface->GetDebugDrawable()->SetText(selectedWaypoints);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
