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

#include <dtDAL/project.h>

#include <dtCore/deltawin.h>
#include <dtCore/system.h>

#include <dtCore/objectmotionmodel.h>
#include <dtCore/rtsmotionmodel.h>

#include "aicomponent.h"
#include "aiutilityinputcomponent.h"

////////////////////////////////////////////////////////////////////////////////
AIUtilityApp::AIUtilityApp()
: dtABC::Application("config.xml")
, mGM(new dtGame::GameManager(*GetScene()))
{
   mGM->SetApplication(*this);
   mLastCameraTransform.MakeIdentity();

   // Mark this app as an editor so custom libraries
   // can query what they're running in and act appropriately
   dtDAL::Project::GetInstance().SetEditMode(true);
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

   mMotionModel = new dtCore::RTSMotionModel(GetKeyboard(), GetMouse(), false, false);
   mMotionModel->SetTarget(GetCamera());

   dtCore::System::GetInstance().Start();
   mStepper.Start();
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityApp::SetAIPluginInterface(dtAI::AIPluginInterface* interface)
{
   emit AIPluginInterfaceChanged(interface);

   // We can now setup the input component
   mInputComponent = new AIUtilityInputComponent();
   mInputComponent->SetAIPluginInterface(interface);
   mGM->AddComponent(*mInputComponent, dtGame::GameManager::ComponentPriority::NORMAL);
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
      dtDAL::Project::GetInstance().SetContext(path);
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
void AIUtilityApp::OnPreferencesUpdated()
{

}

////////////////////////////////////////////////////////////////////////////////

