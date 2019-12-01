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
 * Author: MG
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/loopaction.h>

#include <dtDirector/director.h>

#include <dtCore/booleanactorproperty.h>
#include <dtCore/floatactorproperty.h>

namespace dtDirector
{
   enum
   {
      INPUT_START = 0,
      INPUT_STOP,
      INPUT_OUT,
      INPUT_FIRE_CYCLE
   };

   ////////////////////////////////////////////////////////////////////////////////
   LoopAction::LoopAction()
      : LatentActionNode()
      , mIsLooping(false)
      , mLoopPeriod(0.0f)
      , mLoopTimeElapsed(0.0f)
      , mDeltaTime(0.0f)
      , mUseSimTime(true)
   {
      AddAuthor("MG");
   }

   ////////////////////////////////////////////////////////////////////////////////
   LoopAction::~LoopAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LoopAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      LatentActionNode::Init(nodeType, graph);

      mInputs.clear();
      mInputs.push_back(InputLink(this, "Start", "Make the Cycle output fire after the LoopPeriod has elapsed."));
      mInputs.push_back(InputLink(this, "Stop", "Make the Cycle stop firing."));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Out", "Activates when the loop gets started."));
      mOutputs.push_back(OutputLink(this, "Cycle", "Activates when the time elapsed is greater than the period."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LoopAction::BuildPropertyMap()
   {
      LatentActionNode::BuildPropertyMap();

      dtCore::FloatActorProperty* periodProp = new dtCore::FloatActorProperty(
         "LoopPeriod", "Loop Period",
         dtCore::FloatActorProperty::SetFuncType(this, &LoopAction::SetLoopPeriod),
         dtCore::FloatActorProperty::GetFuncType(this, &LoopAction::GetLoopPeriod),
         "Sets the number of seconds per cycle", "");
      AddProperty(periodProp);

      dtCore::FloatActorProperty* deltaProp = new dtCore::FloatActorProperty(
         "DeltaTime", "Time Delta",
         dtCore::FloatActorProperty::SetFuncType(this, &LoopAction::SetDeltaTime),
         dtCore::FloatActorProperty::GetFuncType(this, &LoopAction::GetDeltaTime),
         "Time in seconds since last update.", "");
      AddProperty(deltaProp);

      dtCore::BooleanActorProperty* simTimeProp = new dtCore::BooleanActorProperty(
         "UseSimTime", "Use Sim Time",
         dtCore::BooleanActorProperty::SetFuncType(this, &LoopAction::SetUseSimTime),
         dtCore::BooleanActorProperty::GetFuncType(this, &LoopAction::GetUseSimTime),
         "True to use game/sim time, false to use real time.");
      AddProperty(simTimeProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, periodProp, false, false, true, false));
      mValues.push_back(ValueLink(this, deltaProp, true, false, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool LoopAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      if (firstUpdate)
      {
         // If the loop has not already been added to the stack(mIsLooping), add it
         if (input == INPUT_START && !mIsLooping)
         {
            mIsLooping = true;
            mLoopTimeElapsed = 0.0f;

            if (mUseSimTime)
            {
               SetFloat(simDelta, "DeltaTime");
            }
            else
            {
               SetFloat(delta, "DeltaTime");
            }

            // Put this on the stack so it will finish
            // execution of its chain before we continue
            GetDirector()->PushStack(this, INPUT_OUT);
         }
         else if (input == INPUT_STOP)
         {
            mIsLooping = false;
         }
         else if (input == INPUT_OUT)
         {
            // If we're here, we're on the stack, fire and bail out
            ActivateOutput("Out");
            return false;
         }
         else //if (input == INPUT_FIRE_CYCLE)
         {
            // If we're here, we're on the stack, fire and bail out
            ActivateOutput("Cycle");
            return false;
         }
      }
      else
      {
         if (mIsLooping && input == INPUT_START)
         {
            if (mUseSimTime)
            {
               mLoopTimeElapsed += simDelta;
            }
            else
            {
               mLoopTimeElapsed += delta;
            }

            float loopPeriod = GetFloat("LoopPeriod");
            if (mLoopTimeElapsed > loopPeriod)
            {
               mLoopTimeElapsed -= loopPeriod;

               if (mUseSimTime)
               {
                  SetFloat(simDelta, "DeltaTime");
               }
               else
               {
                  SetFloat(delta, "DeltaTime");
               }

               // Put this on the stack so it will finish
               // execution of its chain before we continue
               GetDirector()->PushStack(this, INPUT_FIRE_CYCLE);
            }
         }
      }

      return mIsLooping;
   }


   ////////////////////////////////////////////////////////////////////////////////
   float LoopAction::GetLoopPeriod() const
   {
      return mLoopPeriod;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LoopAction::SetLoopPeriod(float secondsPerCycle)
   {
      mLoopPeriod = secondsPerCycle;
   }

   //////////////////////////////////////////////////////////////////////////
   void LoopAction::SetUseSimTime(bool value)
   {
      mUseSimTime = value;
   }

   //////////////////////////////////////////////////////////////////////////
   bool LoopAction::GetUseSimTime()
   {
      return mUseSimTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LoopAction::GetDeltaTime() const
   {
      return mDeltaTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LoopAction::SetDeltaTime(float delta)
   {
      mDeltaTime = delta;
   }

}

////////////////////////////////////////////////////////////////////////////////
