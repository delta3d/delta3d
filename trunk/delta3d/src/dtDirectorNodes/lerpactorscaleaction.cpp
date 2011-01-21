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

#include <dtDirectorNodes/lerpactorscaleaction.h>

#include <dtCore/object.h>

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/floatactorproperty.h>
#include <dtDAL/vectoractorproperties.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   LerpActorScaleAction::LerpActorScaleAction()
      : ActionNode()
      , mStartTime(0.0f)
      , mEndTime(1.0f)
      , mTime(0.0f)
      , mLerpTimeScalar(1.0f)
      , mWaitingForStart(true)
      , mIsActive(false)
   {
      mLerpActor = "";

      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   LerpActorScaleAction::~LerpActorScaleAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorScaleAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Start"));
      mInputs.push_back(InputLink(this, "Stop"));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Started"));
      mOutputs.push_back(OutputLink(this, "Stopped"));
      mOutputs.push_back(OutputLink(this, "Finished"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorScaleAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &LerpActorScaleAction::SetLerpActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &LerpActorScaleAction::GetLerpActor),
         "dtCore::Object", "The actor to linearly interpolate.");
      AddProperty(actorProp);

      dtDAL::FloatActorProperty* startTimeProp = new dtDAL::FloatActorProperty(
         "StartTime", "Start Time",
         dtDAL::FloatActorProperty::SetFuncType(this, &LerpActorScaleAction::SetStartTime),
         dtDAL::FloatActorProperty::GetFuncType(this, &LerpActorScaleAction::GetStartTime),
         "The time to start the lerp.");
      AddProperty(startTimeProp);

      dtDAL::FloatActorProperty* endTimeProp = new dtDAL::FloatActorProperty(
         "EndTime", "End Time",
         dtDAL::FloatActorProperty::SetFuncType(this, &LerpActorScaleAction::SetEndTime),
         dtDAL::FloatActorProperty::GetFuncType(this, &LerpActorScaleAction::GetEndTime),
         "The time to end the lerp.");
      AddProperty(endTimeProp);

      dtDAL::FloatActorProperty* timeProp = new dtDAL::FloatActorProperty(
         "Time", "Time",
         dtDAL::FloatActorProperty::SetFuncType(this, &LerpActorScaleAction::SetTime),
         dtDAL::FloatActorProperty::GetFuncType(this, &LerpActorScaleAction::GetTime),
         "The current time.");

      dtDAL::Vec3ActorProperty* startPosProp = new dtDAL::Vec3ActorProperty(
         "StartScale", "Start Scale",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &LerpActorScaleAction::SetStartScale),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &LerpActorScaleAction::GetStartScale),
         "The starting rotation of the actor.");
      AddProperty(startPosProp);

      dtDAL::Vec3ActorProperty* endPosProp = new dtDAL::Vec3ActorProperty(
         "EndScale", "End Scale",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &LerpActorScaleAction::SetEndScale),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &LerpActorScaleAction::GetEndScale),
         "The ending rotation of the actor.");
      AddProperty(endPosProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp, true, true, true));
      mValues.push_back(ValueLink(this, startTimeProp, false, false, true, false));
      mValues.push_back(ValueLink(this, endTimeProp, false, false, true, false));
      mValues.push_back(ValueLink(this, timeProp, false, false, true));
      mValues.push_back(ValueLink(this, startPosProp, false, false, true, false));
      mValues.push_back(ValueLink(this, endPosProp, false, false, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool LerpActorScaleAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      switch (input)
      {
      case INPUT_START:
         {
            // Activate the "Started" output link.
            if (firstUpdate)
            {
               // If this is a first update and we already started, then kill the
               // new thread since we're running on another thread
               if (mIsActive)
               {
                  return false;
               }

               OutputLink* link = GetOutputLink("Started");
               if (link) link->Activate();
            }

            // On the first activation, initialize.
            if (!mIsActive)
            {
               if (firstUpdate)
               {
                  InitLerp();
                  mIsActive = true;

                  // Activate the "Out" output link.
                  ActionNode::Update(simDelta, delta, input, firstUpdate);
               }
               else
               {
                  // Reset the current time to the beginning since we finished
                  // if we're internally tracking it
                  if (IsTimeInternal())
                  {
                     SetFloat(GetFloat("StartTime"), "Time");
                  }

                  OutputLink* link = GetOutputLink("Finished");
                  if (link) link->Activate();
                  return false;
               }
            }

            // Now check if we are within our start and end times.
            float startTime = GetFloat("StartTime");
            float endTime = GetFloat("EndTime");
            float curTime = GetFloat("Time");

            if (mWaitingForStart)
            {
               if (curTime < startTime || curTime > endTime)
               {
                  return true;
               }

               mWaitingForStart = false;
            }

            if (GetNodeLogging())
            {
               LOG_ALWAYS(dtUtil::ToString(curTime));
            }

            if (curTime < startTime)
            {
               if (GetNodeLogging())
               {
                  LOG_ALWAYS(dtUtil::ToString(startTime));
               }
               curTime = startTime;
               mIsActive = false;
            }
            else if (curTime > endTime)
            {
               if (GetNodeLogging())
               {
                  LOG_ALWAYS(dtUtil::ToString(endTime));
               }
               curTime = endTime;
               mIsActive = false;
            }
            float alpha = (curTime - startTime) * mLerpTimeScalar;

            osg::Vec3 startScale = GetVec3("StartScale");
            osg::Vec3 endScale = GetVec3("EndScale");
            osg::Vec3 track = endScale - startScale;

            osg::Vec3 newScale = startScale + (track * alpha);

            int count = GetPropertyCount("Actor");
            for (int index = 0; index < count; index++)
            {
               dtDAL::BaseActorObject* proxy = GetActor("Actor", index);
               if (proxy)
               {
                  dtCore::Object* actor = NULL;
                  proxy->GetActor(actor);
                  if (actor)
                  {
                     actor->SetScale(osg::Vec3(newScale.x(), newScale.y(), newScale.z()));
                  }
               }
            }

            // Update the current time if we're internally tracking it
            if (IsTimeInternal())
            {
               SetFloat(curTime + simDelta, "Time");
            }

            return true;
         }
         break;

      case INPUT_STOP:
         {
            mIsActive = false;

            // Activate the "Stopped" output link.
            if (firstUpdate)
            {
               OutputLink* link = GetOutputLink("Stopped");
               if (link) link->Activate();
            }
         }
         break;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorScaleAction::InitLerp()
   {
      // Calculate the total lerp time scalar.
      float startTime = GetFloat("StartTime");
      float endTime = GetFloat("EndTime");

      mLerpTimeScalar = 1.0f;
      float delta = endTime - startTime;
      if (delta != 0.0f)
      {
         mLerpTimeScalar = 1.0f / delta;
      }

      mWaitingForStart = true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorScaleAction::SetLerpActor(const dtCore::UniqueId& value)
   {
      mLerpActor = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId LerpActorScaleAction::GetLerpActor()
   {
      return mLerpActor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorScaleAction::SetStartTime(float value)
   {
      mStartTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LerpActorScaleAction::GetStartTime()
   {
      return mStartTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorScaleAction::SetEndTime(float value)
   {
      mEndTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LerpActorScaleAction::GetEndTime()
   {
      return mEndTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorScaleAction::SetTime(float value)
   {
      mTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LerpActorScaleAction::GetTime()
   {
      return mTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorScaleAction::SetStartScale(const osg::Vec3& value)
   {
      mStartScale = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LerpActorScaleAction::GetStartScale()
   {
      return mStartScale;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorScaleAction::SetEndScale(const osg::Vec3& value)
   {
      mEndScale = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LerpActorScaleAction::GetEndScale()
   {
      return mEndScale;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool LerpActorScaleAction::IsTimeInternal()
   {
      // If we have any external links to our Time property, then we are not
      // internally tracking the time
      return GetValueLink("Time")->GetLinks().size() == 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
