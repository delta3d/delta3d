/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, Alion Science and Technology, BMH Operation
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
 * Eric Heine
 */

#include "aiutilityinputcomponent.h"
#include "waypointselection.h"
#include <dtCore/flymotionmodel.h>
#include <dtABC/application.h>
#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointgraph.h>

#include <iostream>

const dtCore::RefPtr<dtCore::SystemComponentType> AIUtilityInputComponent::TYPE(new dtCore::SystemComponentType("AIUtilityInputComponent","GMComponents",
      "Internal input component for the AI Utility.",
      dtGame::BaseInputComponent::DEFAULT_TYPE));


////////////////////////////////////////////////////////////////////////////////
AIUtilityInputComponent::AIUtilityInputComponent()
: dtGame::BaseInputComponent(*TYPE)
, mpAIInterface(NULL)
, mCameraSpeed(5.0f)
, mCameraMotionModel(NULL)
, mPickDistanceBuffer(0.5f)
, mSelectBrushMode(false)
, mSelectBrushSize(1.0)
{
}

////////////////////////////////////////////////////////////////////////////////
AIUtilityInputComponent::~AIUtilityInputComponent()
{
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityInputComponent::SetCameraMotionModel(dtCore::FlyMotionModel& fm)
{
    mCameraMotionModel = &fm;
}

////////////////////////////////////////////////////////////////////////////////
bool AIUtilityInputComponent::HandleKeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   switch (key)
   {

   case '-':
      {
         mCameraSpeed -= 10.0f;

         if(mCameraSpeed <= 5.0f)
         {
            mCameraSpeed = 5.0f;
         }

         if(mCameraMotionModel.valid())
         {
            mCameraMotionModel->SetMaximumFlySpeed(mCameraSpeed);
            return true;
         }
      }
      break;

   case '=':
      {
         mCameraSpeed += 10.0f;

         if(mCameraSpeed > 250.0f)
         {
            mCameraSpeed = 250.0f;
         }

         if(mCameraMotionModel.valid())
         {
            mCameraMotionModel->SetMaximumFlySpeed(mCameraSpeed);
            return true;
         }
      }
      break;
   }
      
   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool AIUtilityInputComponent::HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   bool handled = false;

   if (mpAIInterface.valid() == false)
   {
      return handled;
   }

   if (!IsOkToSelect())
   {
      return false;
   }

   switch (button)
   {
      case dtCore::Mouse::LeftButton:
      {
         osg::Vec3f pickedPosition;
         GetGameManager()->GetApplication().GetView()->GetMousePickPosition(pickedPosition);

         // See if we are adding to the selection list or not
         if (IsShiftHeld() == false)
         {
            WaypointSelection::GetInstance().DeselectAllWaypoints();
         }

         // Try and find the waypoint we clicked on if any
         if (mSelectBrushMode)
         {
            dtAI::AIPluginInterface::WaypointArray selectedWaypoints;
            mpAIInterface->GetWaypointsAtRadius(pickedPosition, GetSelectionBrushSize(), selectedWaypoints);
            WaypointSelection::GetInstance().AddWaypointListToSelection(selectedWaypoints);
            handled = true;
         }
         else
         {
            dtAI::WaypointInterface* waypointInterface = mpAIInterface->GetClosestWaypoint(pickedPosition, GetPickDistanceBuffer());
            if (waypointInterface != NULL)
            {
               WaypointSelection::GetInstance().ToggleWaypointSelection(waypointInterface);
               handled = true;
               mCameraMotionModel->SetEnabled(false);
            }
         }

         if(!handled)
            mCameraMotionModel->SetEnabled(true);  

         break;
      }
      default:
         break;
   }
   
   return handled;
}

////////////////////////////////////////////////////////////////////////////////
bool AIUtilityInputComponent::HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y)
{
   bool handled = false;

   if (mpAIInterface.valid() == false)
   {
      return handled;
   }

   // Don't select if we're using the motion model
   if (!IsOkToSelect())
   {
      return false;
   }

   osg::Vec3f pickedPosition;
   GetGameManager()->GetApplication().GetView()->GetMousePickPosition(pickedPosition);

   // See if we are adding to the selection list or not
   if (IsShiftHeld() == false)
   {
      WaypointSelection::GetInstance().DeselectAllWaypoints();
   }

   // Try and find the waypoint we clicked on if any
   if (mSelectBrushMode)
   {
      dtAI::AIPluginInterface::WaypointArray selectedWaypoints;
      mpAIInterface->GetWaypointsAtRadius(pickedPosition, GetSelectionBrushSize(), selectedWaypoints);
      WaypointSelection::GetInstance().AddWaypointListToSelection(selectedWaypoints);
      handled = true;
   }
   else
   {
      dtAI::WaypointInterface* waypointInterface = mpAIInterface->GetClosestWaypoint(pickedPosition, GetPickDistanceBuffer());
      if (waypointInterface != NULL)
      {
         if (!WaypointSelection::GetInstance().HasWaypoint(waypointInterface))
         {
            WaypointSelection::GetInstance().AddWaypointToSelection(waypointInterface);
            handled = true;
         }
      }
   }

   return handled;
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityInputComponent::SetAIPluginInterface(dtAI::AIPluginInterface* aiInterface)
{
   mpAIInterface = aiInterface;
}

////////////////////////////////////////////////////////////////////////////////
bool AIUtilityInputComponent::IsShiftHeld()
{
   dtCore::Keyboard* keyboard = GetGameManager()->GetApplication().GetKeyboard();
   bool isShiftHeld = keyboard && (keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Shift_L) ||
                      keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Shift_R));

   return isShiftHeld;
}

////////////////////////////////////////////////////////////////////////////////
bool AIUtilityInputComponent::IsCtrlHeld()
{
   dtCore::Keyboard* keyboard = GetGameManager()->GetApplication().GetKeyboard();
   bool isShiftHeld = keyboard && (keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Control_L) ||
      keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Control_R));

   return isShiftHeld;
}

////////////////////////////////////////////////////////////////////////////////
float AIUtilityInputComponent::GetPickDistanceBuffer() const
{
   return mPickDistanceBuffer;
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityInputComponent::SetPickDistanceBuffer(float val)
{
   mPickDistanceBuffer = val;
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityInputComponent::OnSelectWaypontBrushMode(bool enable)
{
   mSelectBrushMode = enable;
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityInputComponent::OnSelectBrushSizeChanged(double size)
{
   mSelectBrushSize = size;
}

////////////////////////////////////////////////////////////////////////////////
double AIUtilityInputComponent::GetSelectionBrushSize() const
{
   return mSelectBrushSize;
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityInputComponent::SetWaypointMotionModel(WaypointMotionModel* waypointMotionModel)
{
   mWaypointMotionModel = waypointMotionModel;
   connect(mWaypointMotionModel.get(), SIGNAL(WaypointTranslationBeginning()), this, SLOT(OnWaypointTranslationBeginning()));
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityInputComponent::OnWaypointTranslationBeginning()
{
   if (IsCtrlHeld())
   {
      std::vector<dtAI::WaypointInterface*> cloneList;
      std::vector<dtAI::WaypointInterface*>& waypointList =
         WaypointSelection::GetInstance().GetWaypointList();

      for (size_t pointIndex = 0; pointIndex < waypointList.size(); ++pointIndex)
      {
         const osg::Vec3& pos = waypointList[pointIndex]->GetPosition();
         const dtCore::ObjectType& type = waypointList[pointIndex]->GetWaypointType();

         cloneList.push_back(mpAIInterface->CreateWaypoint(pos, type));
      }

      WaypointSelection::GetInstance().SetWaypointSelectionList(cloneList);
   }
}

////////////////////////////////////////////////////////////////////////////////
bool AIUtilityInputComponent::IsOkToSelect() const
{
   if (mWaypointMotionModel.valid() &&
       mWaypointMotionModel->IsEnabled() &&
       mWaypointMotionModel->GetMotionType() != dtCore::ObjectMotionModel::MOTION_TYPE_MAX)
   {
      return false;
   }
   else
   {
      return true;
   }
}
////////////////////////////////////////////////////////////////////////////////
