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

#include <dtDAL/floatactorproperty.h>

namespace dtDirector
{
   enum
   {
      INPUT_START = 0,
      INPUT_STOP,
      INPUT_FIRE_CYCLE
   };

   ////////////////////////////////////////////////////////////////////////////////
   LoopAction::LoopAction()
      : ActionNode()
      , mIsLooping(false)
      , mLoopPeriod(0.0f)
      , mLoopTimeElapsed(0.0f)
      , mDeltaTime(0.0f)
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
      ActionNode::Init(nodeType, graph);

      mInputs.clear();
      mInputs.push_back(InputLink(this, "Start", "Make the Cycle output fire after the LoopPeriod has elapsed."));
      mInputs.push_back(InputLink(this, "Stop", "Make the Cycle stop firing."));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Cycle", "Activates when the time elapsed is greater than the period."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LoopAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::FloatActorProperty* periodProp = new dtDAL::FloatActorProperty(
         "LoopPeriod", "Loop Period",
         dtDAL::FloatActorProperty::SetFuncType(this, &LoopAction::SetLoopPeriod),
         dtDAL::FloatActorProperty::GetFuncType(this, &LoopAction::GetLoopPeriod),
         "Sets the number of seconds per cycle", "");
      AddProperty(periodProp);

      dtDAL::FloatActorProperty* deltaProp = new dtDAL::FloatActorProperty(
         "DeltaTime", "Time Delta",
         dtDAL::FloatActorProperty::SetFuncType(this, &LoopAction::SetDeltaTime),
         dtDAL::FloatActorProperty::GetFuncType(this, &LoopAction::GetDeltaTime),
         "Time in seconds since last update.", "");
      AddProperty(deltaProp);


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
         if (input == INPUT_START)
         {
            mIsLooping = true;
            mLoopTimeElapsed = 0.0f;

            SetFloat(simDelta, "DeltaTime");

            // Put this on the stack so it will finish
            // execution of its chain before we continue
            GetDirector()->PushStack(this, INPUT_FIRE_CYCLE);
         }
         else if (input == INPUT_STOP)
         {
            mIsLooping = false;
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
            mLoopTimeElapsed += simDelta;

            if (mLoopTimeElapsed > mLoopPeriod)
            {
               mLoopTimeElapsed -= mLoopPeriod;

               SetFloat(simDelta, "DeltaTime");

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
