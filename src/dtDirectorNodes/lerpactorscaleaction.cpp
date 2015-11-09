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
#include <dtDirectorNodes/lerpactorscaleaction.h>

#include <dtCore/object.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/vectoractorproperties.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   LerpActorScaleAction::LerpActorScaleAction()
      : LatentActionNode()
      , mStartTime(0.0f)
      , mEndTime(1.0f)
      , mTime(0.0f)
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
      LatentActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Start", "Begins the scale."));
      mInputs.push_back(InputLink(this, "Stop", "Ends the scale."));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Started", "Activates when the scaling has started."));
      mOutputs.push_back(OutputLink(this, "Stopped", "Activates when the scaling was forcibly stopped."));
      mOutputs.push_back(OutputLink(this, "Finished", "Activates when the scaling has finished."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorScaleAction::BuildPropertyMap()
   {
      LatentActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Actor", "Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &LerpActorScaleAction::SetLerpActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &LerpActorScaleAction::GetLerpActor),
         "dtCore::Object", "The actor to linearly interpolate.");
      AddProperty(actorProp);

      dtCore::FloatActorProperty* startTimeProp = new dtCore::FloatActorProperty(
         "StartTime", "Start Time",
         dtCore::FloatActorProperty::SetFuncType(this, &LerpActorScaleAction::SetStartTime),
         dtCore::FloatActorProperty::GetFuncType(this, &LerpActorScaleAction::GetStartTime),
         "The time to start the lerp.");
      AddProperty(startTimeProp);

      dtCore::FloatActorProperty* endTimeProp = new dtCore::FloatActorProperty(
         "EndTime", "End Time",
         dtCore::FloatActorProperty::SetFuncType(this, &LerpActorScaleAction::SetEndTime),
         dtCore::FloatActorProperty::GetFuncType(this, &LerpActorScaleAction::GetEndTime),
         "The time to end the lerp.");
      AddProperty(endTimeProp);

      dtCore::FloatActorProperty* timeProp = new dtCore::FloatActorProperty(
         "Time", "Time",
         dtCore::FloatActorProperty::SetFuncType(this, &LerpActorScaleAction::SetTime),
         dtCore::FloatActorProperty::GetFuncType(this, &LerpActorScaleAction::GetTime),
         "The current time.");

      dtCore::Vec3ActorProperty* startPosProp = new dtCore::Vec3ActorProperty(
         "StartScale", "Start Scale",
         dtCore::Vec3ActorProperty::SetFuncType(this, &LerpActorScaleAction::SetStartScale),
         dtCore::Vec3ActorProperty::GetFuncType(this, &LerpActorScaleAction::GetStartScale),
         "The starting rotation of the actor.");
      AddProperty(startPosProp);

      dtCore::Vec3ActorProperty* endPosProp = new dtCore::Vec3ActorProperty(
         "EndScale", "End Scale",
         dtCore::Vec3ActorProperty::SetFuncType(this, &LerpActorScaleAction::SetEndScale),
         dtCore::Vec3ActorProperty::GetFuncType(this, &LerpActorScaleAction::GetEndScale),
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
      if (input == INPUT_START)
      {
         // Activate the "Started" output link.
         if (firstUpdate)
         {
            OutputLink* link = GetOutputLink("Started");
            if (link) link->Activate();

            // If we have our own internal clock, restart it since someone
            // fired Start again
            if (IsTimeInternal())
            {
               SetFloat(GetFloat("StartTime"), "Time");
            }

            // If this is a first update and we already started, then kill the
            // new thread since we're running on another thread
            if (mIsActive)
            {
               return false;
            }
         }

         // On the first activation, initialize.
         if (!mIsActive)
         {
            if (firstUpdate)
            {
               mWaitingForStart = true;
               mIsActive = true;

               // Activate the "Out" output link.
               LatentActionNode::Update(simDelta, delta, input, firstUpdate);
            }
            else // We shouldn't be updating anymore
            {
               return false;
            }
         }
      }

      // Now check if we are within our start and end times.
      float startTime = GetFloat("StartTime");
      float endTime = GetFloat("EndTime");
      float curTime = GetFloat("Time");

      if (mWaitingForStart &&
         curTime >= startTime && curTime <= endTime)
      {
         mWaitingForStart = false;
      }

      if (!mWaitingForStart)
      {
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

         if (mIsActive)
         {
            float timeScalar = 1.0f;
            float lerpDelta = endTime - startTime;
            if (lerpDelta != 0.0f)
            {
               timeScalar = 1.0f / lerpDelta;
            }
            float alpha = (curTime - startTime) * timeScalar;

            osg::Vec3 startScale = GetVec3("StartScale");
            osg::Vec3 endScale = GetVec3("EndScale");
            osg::Vec3 track = endScale - startScale;

            osg::Vec3 newScale = startScale + (track * alpha);

            int count = GetPropertyCount("Actor");
            for (int index = 0; index < count; index++)
            {
               dtCore::BaseActorObject* actor = GetActor("Actor", index);
               if (actor)
               {
                  dtCore::Object* object = NULL;
                  actor->GetDrawable(object);
                  if (object)
                  {
                     object->SetScale(osg::Vec3(newScale.x(), newScale.y(), newScale.z()));
                  }
               }
            }
         }
      }

      // Update the current time if we're internally tracking it
      if (IsTimeInternal())
      {
         SetFloat(curTime + simDelta, "Time");
      }

      bool result = true;

      // If we aren't active anymore, then we are finished
      if (!mIsActive)
      {
         OutputLink* link = GetOutputLink("Finished");
         if (link) link->Activate();
         result = false;
      }

      if (input == INPUT_STOP)
      {
         mIsActive = false;
         // Activate the "Stopped" output link.
         OutputLink* link = GetOutputLink("Stopped");
         if (link) link->Activate();
         result = false;
      }

      return result;
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
