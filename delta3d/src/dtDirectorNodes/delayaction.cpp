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

#include <dtDirectorNodes/delayaction.h>

#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/floatactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   DelayAction::DelayAction()
      : ActionNode()
      , mDelay(1.0f)
      , mElapsedTime(0.0f)
      , mIsActive(false)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   DelayAction::~DelayAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DelayAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Start"));
      mInputs.push_back(InputLink(this, "Stop"));
      mInputs.push_back(InputLink(this, "Pause"));

      mOutputs.push_back(OutputLink(this, "Time Elapsed"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DelayAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::FloatActorProperty* delayProp = new dtDAL::FloatActorProperty(
         "Delay", "Delay",
         dtDAL::FloatActorProperty::SetFuncType(this, &DelayAction::SetDelay),
         dtDAL::FloatActorProperty::GetFuncType(this, &DelayAction::GetDelay),
         "The time delay (in seconds).");
      AddProperty(delayProp);

      dtDAL::FloatActorProperty* elapsedTimeProp = new dtDAL::FloatActorProperty(
         "Elapsed Time", "Elapsed Time",
         dtDAL::FloatActorProperty::SetFuncType(this, &DelayAction::SetElapsedTime),
         dtDAL::FloatActorProperty::GetFuncType(this, &DelayAction::GetElapsedTime),
         "The elapsed time (in seconds).");

      dtDAL::BooleanActorProperty* simTimeProp = new dtDAL::BooleanActorProperty(
         "UseSimTime", "Use Sim Time",
         dtDAL::BooleanActorProperty::SetFuncType(this, &DelayAction::SetUseSimTime),
         dtDAL::BooleanActorProperty::GetFuncType(this, &DelayAction::GetUseSimTime),
         "True to use game/sim time, false to use real time.");
      AddProperty(simTimeProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, delayProp, false, false, false));
      mValues.push_back(ValueLink(this, elapsedTimeProp, true, false, true, false));
      mValues.push_back(ValueLink(this, simTimeProp, false, false, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool DelayAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      float deltaTime = simDelta;
      if (!mUseSimTime) deltaTime = delta;

      switch (input)
      {
      case INPUT_START:
         {
            // Start this node only once to avoid time-stepping to be multiplied!
            if (mIsActive && firstUpdate)
            {
               return false;
            }

            // We need to check the current active status because this node
            // will be updated multiple times using this same index until
            // the desired time has elapsed.  And we only want to trigger
            // the "Out" output once at the beginning.
            if (!mIsActive)
            {
               if (firstUpdate)
               {
                  mIsActive = true;
                  SetFloat(0.0f, "Elapsed Time");

                  // Call the parent so the default "Out" link is triggered.
                  ActionNode::Update(simDelta, delta, input, firstUpdate);
               }
               // If this is not the first update for this node, then
               // we must be paused or stopped, so we want to stop this update.
               else
               {
                  return false;
               }
            }

            float elapsedTime = GetFloat("Elapsed Time");
            float goalTime = GetFloat("Delay");

            // Test if the desired time has elapsed.
            if (elapsedTime >= goalTime)
            {
               // Reset the time and trigger the "Time Elapsed" output.
               SetFloat(0.0f, "Elapsed Time");
               OutputLink* link = GetOutputLink("Time Elapsed");
               if (link) link->Activate();

               mIsActive = false;
            }
            else
            {
               // Continue the timer (force at least one update before this node
               // can be completed to ensure that it will break any chain).
               elapsedTime += deltaTime;

               SetFloat(elapsedTime, "Elapsed Time");
            }
         }

         // return true to keep this node active in the current thread.
         return mIsActive;

      case INPUT_STOP:
         // Reset the elapsed time and deactivate it.
         if (mIsActive)
         {
            SetFloat(0.0f, "Elapsed Time");
            mIsActive = false;
         }
         return false;

      case INPUT_PAUSE:
         // Deactivate the node, but do not reset the timer.
         mIsActive = false;
         return false;
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   //////////////////////////////////////////////////////////////////////////
   bool DelayAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (Node::CanConnectValue(link, value))
      {
         // Delay link can only connect to basic types.
         if (link == GetValueLink("Delay"))
         {
            dtDAL::DataType& type = value->GetPropertyType();
            switch (type.GetTypeId())
            {
            case dtDAL::DataType::INT_ID:
            case dtDAL::DataType::FLOAT_ID:
            case dtDAL::DataType::DOUBLE_ID:
               return true;

            default:
               return false;
            }
         }

         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void DelayAction::SetDelay(float value)
   {
      mDelay = value;
   }

   //////////////////////////////////////////////////////////////////////////
   float DelayAction::GetDelay()
   {
      return mDelay;
   }

   //////////////////////////////////////////////////////////////////////////
   void DelayAction::SetElapsedTime(float value)
   {
      mElapsedTime = value;
   }

   //////////////////////////////////////////////////////////////////////////
   float DelayAction::GetElapsedTime()
   {
      return mElapsedTime;
   }

   //////////////////////////////////////////////////////////////////////////
   void DelayAction::SetUseSimTime(bool value)
   {
      mUseSimTime = value;
   }

   //////////////////////////////////////////////////////////////////////////
   bool DelayAction::GetUseSimTime()
   {
      return mUseSimTime;
   }
}

////////////////////////////////////////////////////////////////////////////////
