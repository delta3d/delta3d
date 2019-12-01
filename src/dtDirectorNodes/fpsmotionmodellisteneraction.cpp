/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/fpsmotionmodellisteneraction.h>

#include <dtCore/stringactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   FPSMotionModelListenerAction::FPSMotionModelListenerAction()
      : LatentActionNode()
      , mModelName("")
      , mMotionModel(NULL)
      , mCurrentMode(-1)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   FPSMotionModelListenerAction::~FPSMotionModelListenerAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FPSMotionModelListenerAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      LatentActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Listen", "Starts the listener."));
      mInputs.push_back(InputLink(this, "Stop",   "Stops the listener."));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Idle",    "Activated when the motion model state has been changed to idle mode."));
      mOutputs.push_back(OutputLink(this, "Walking", "Activated when the motion model state has been changed to walking mode."));
      mOutputs.push_back(OutputLink(this, "Sliding", "Activated when the motion model state has been changed to sliding mode."));
      mOutputs.push_back(OutputLink(this, "Jumping", "Activated when the motion model state has been changed to jumping mode."));
      mOutputs.push_back(OutputLink(this, "Falling", "Activated when the motion model state has been changed to falling mode."));
      mOutputs.push_back(OutputLink(this, "Changed", "Activates when the motion model state has been changed."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FPSMotionModelListenerAction::BuildPropertyMap()
   {
      LatentActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::StringActorProperty* nameProp = new dtCore::StringActorProperty(
         "Motion Model", "Motion Model",
         dtCore::StringActorProperty::SetFuncType(this, &FPSMotionModelListenerAction::SetModelName),
         dtCore::StringActorProperty::GetFuncType(this, &FPSMotionModelListenerAction::GetModelName),
         "The name of the motion model to attach.");
      AddProperty(nameProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, nameProp, false, false, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool FPSMotionModelListenerAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      if (input == INPUT_LISTEN)
      {
         if (!mMotionModel && firstUpdate)
         {
            std::string modelName = GetString("Motion Model");
            if (!modelName.empty())
            {
               mMotionModel = dtCore::FPSMotionModel::GetInstance(modelName);
            }
         }

//         if (mMotionModel)
//         {
//            int currentMode = mMotionModel->GetFPSCollider().GetMode();
//            if (currentMode != mCurrentMode)
//            {
//               GetDirector()->PushStack(this, INPUT_CHANGED_EVENT);
//
//               mCurrentMode = currentMode;
//
//               switch (mCurrentMode)
//               {
//               case dtCore::FPSCollider::IDLE:    ActivateOutput("Idle");    break;
//               case dtCore::FPSCollider::WALKING: ActivateOutput("Walking"); break;
//               case dtCore::FPSCollider::SLIDING: ActivateOutput("Sliding"); break;
//               case dtCore::FPSCollider::JUMPING: ActivateOutput("Jumping"); break;
//               case dtCore::FPSCollider::FALLING: ActivateOutput("Falling"); break;
//               }
//            }
//
//            return true;
//         }
      }
      else if (input == INPUT_STOP)
      {
         mMotionModel = NULL;
      }
      else if (input == INPUT_CHANGED_EVENT)
      {
         ActivateOutput("Changed");
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FPSMotionModelListenerAction::SetModelName(const std::string& value)
   {
      mModelName = value;
      mName = mModelName;

      mMotionModel = NULL;
      mCurrentMode = -1;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string FPSMotionModelListenerAction::GetModelName() const
   {
      return mModelName;
   }
}

////////////////////////////////////////////////////////////////////////////////
