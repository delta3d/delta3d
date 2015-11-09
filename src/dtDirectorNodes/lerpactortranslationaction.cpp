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
#include <dtDirectorNodes/lerpactortranslationaction.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/vectoractorproperties.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   LerpActorTranslationAction::LerpActorTranslationAction()
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
   LerpActorTranslationAction::~LerpActorTranslationAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorTranslationAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      LatentActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Start", "Begins the translation."));
      mInputs.push_back(InputLink(this, "Stop", "Ends the translation."));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Started", "Activates when the translation has started."));
      mOutputs.push_back(OutputLink(this, "Stopped", "Activates when the translation was forcibly stopped."));
      mOutputs.push_back(OutputLink(this, "Finished", "Activates when the translation has finished."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorTranslationAction::BuildPropertyMap()
   {
      LatentActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Actor", "Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &LerpActorTranslationAction::SetLerpActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &LerpActorTranslationAction::GetLerpActor),
         "dtCore::Transformable", "The actor to linearly interpolate.");
      AddProperty(actorProp);

      dtCore::FloatActorProperty* startTimeProp = new dtCore::FloatActorProperty(
         "StartTime", "Start Time",
         dtCore::FloatActorProperty::SetFuncType(this, &LerpActorTranslationAction::SetStartTime),
         dtCore::FloatActorProperty::GetFuncType(this, &LerpActorTranslationAction::GetStartTime),
         "The time to start the lerp.");
      AddProperty(startTimeProp);

      dtCore::FloatActorProperty* endTimeProp = new dtCore::FloatActorProperty(
         "EndTime", "End Time",
         dtCore::FloatActorProperty::SetFuncType(this, &LerpActorTranslationAction::SetEndTime),
         dtCore::FloatActorProperty::GetFuncType(this, &LerpActorTranslationAction::GetEndTime),
         "The time to end the lerp.");
      AddProperty(endTimeProp);

      dtCore::FloatActorProperty* timeProp = new dtCore::FloatActorProperty(
         "Time", "Time",
         dtCore::FloatActorProperty::SetFuncType(this, &LerpActorTranslationAction::SetTime),
         dtCore::FloatActorProperty::GetFuncType(this, &LerpActorTranslationAction::GetTime),
         "The current time.");
      AddProperty(timeProp);

      dtCore::Vec3ActorProperty* startPosProp = new dtCore::Vec3ActorProperty(
         "StartPosition", "Start Position",
         dtCore::Vec3ActorProperty::SetFuncType(this, &LerpActorTranslationAction::SetStartPos),
         dtCore::Vec3ActorProperty::GetFuncType(this, &LerpActorTranslationAction::GetStartPos),
         "The starting rotation of the actor.");
      AddProperty(startPosProp);

      dtCore::Vec3ActorProperty* endPosProp = new dtCore::Vec3ActorProperty(
         "EndPosition", "End Position",
         dtCore::Vec3ActorProperty::SetFuncType(this, &LerpActorTranslationAction::SetEndPos),
         dtCore::Vec3ActorProperty::GetFuncType(this, &LerpActorTranslationAction::GetEndPos),
         "The ending rotation of the actor.");
      AddProperty(endPosProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp, true, true, true));
      mValues.push_back(ValueLink(this, startTimeProp, false, false, true, false));
      mValues.push_back(ValueLink(this, endTimeProp, false, false, true, false));
      mValues.push_back(ValueLink(this, timeProp));
      mValues.push_back(ValueLink(this, startPosProp, false, false, true, false));
      mValues.push_back(ValueLink(this, endPosProp, false, false, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool LerpActorTranslationAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      if (input == INPUT_START)
      {
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

            // If this is a first update and we already started, then re-init
            // our old thread and kill this new thread since we're already
            // running
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
         if (curTime < startTime)
         {
            curTime = startTime;
            mIsActive = false;
         }
         else if (curTime > endTime)
         {
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

            osg::Vec3 startPos = GetVec3("StartPosition");
            osg::Vec3 endPos = GetVec3("EndPosition");
            osg::Vec3 track = endPos - startPos;

            osg::Vec3 newPos = startPos + (track * alpha);

            int count = GetPropertyCount("Actor");
            for (int index = 0; index < count; index++)
            {
               dtCore::BaseActorObject* actor = GetActor("Actor", index);
               if (actor)
               {
                  dtCore::Transformable* drawable = NULL;
                  actor->GetDrawable(drawable);
                  if (drawable)
                  {
                     dtCore::Transform transform;
                     drawable->GetTransform(transform);
                     transform.SetTranslation(newPos.x(), newPos.y(), newPos.z());
                     drawable->SetTransform(transform);
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
   void LerpActorTranslationAction::SetLerpActor(const dtCore::UniqueId& value)
   {
      mLerpActor = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId LerpActorTranslationAction::GetLerpActor()
   {
      return mLerpActor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorTranslationAction::SetStartTime(float value)
   {
      mStartTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LerpActorTranslationAction::GetStartTime()
   {
      return mStartTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorTranslationAction::SetEndTime(float value)
   {
      mEndTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LerpActorTranslationAction::GetEndTime()
   {
      return mEndTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorTranslationAction::SetTime(float value)
   {
      mTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LerpActorTranslationAction::GetTime()
   {
      return mTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorTranslationAction::SetStartPos(const osg::Vec3& value)
   {
      mStartPos = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LerpActorTranslationAction::GetStartPos()
   {
      return mStartPos;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorTranslationAction::SetEndPos(const osg::Vec3& value)
   {
      mEndPos = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LerpActorTranslationAction::GetEndPos()
   {
      return mEndPos;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool LerpActorTranslationAction::IsTimeInternal()
   {
      // If we have any external links to our Time property, then we are not
      // internally tracking the time
      return GetValueLink("Time")->GetLinks().size() == 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
