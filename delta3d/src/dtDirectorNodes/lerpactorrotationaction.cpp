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
#include <dtDirectorNodes/lerpactorrotationaction.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/floatactorproperty.h>
#include <dtDAL/vectoractorproperties.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   LerpActorRotationAction::LerpActorRotationAction()
      : ActionNode()
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
   LerpActorRotationAction::~LerpActorRotationAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorRotationAction::Init(const NodeType& nodeType, DirectorGraph* graph)
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
   void LerpActorRotationAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &LerpActorRotationAction::SetLerpActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &LerpActorRotationAction::GetLerpActor),
         "dtCore::Transformable", "The actor to linearly interpolate.");
      AddProperty(actorProp);

      dtDAL::FloatActorProperty* startTimeProp = new dtDAL::FloatActorProperty(
         "StartTime", "Start Time",
         dtDAL::FloatActorProperty::SetFuncType(this, &LerpActorRotationAction::SetStartTime),
         dtDAL::FloatActorProperty::GetFuncType(this, &LerpActorRotationAction::GetStartTime),
         "The time to start the lerp.");
      AddProperty(startTimeProp);

      dtDAL::FloatActorProperty* endTimeProp = new dtDAL::FloatActorProperty(
         "EndTime", "End Time",
         dtDAL::FloatActorProperty::SetFuncType(this, &LerpActorRotationAction::SetEndTime),
         dtDAL::FloatActorProperty::GetFuncType(this, &LerpActorRotationAction::GetEndTime),
         "The time to end the lerp.");
      AddProperty(endTimeProp);

      dtDAL::FloatActorProperty* timeProp = new dtDAL::FloatActorProperty(
         "Time", "Time",
         dtDAL::FloatActorProperty::SetFuncType(this, &LerpActorRotationAction::SetTime),
         dtDAL::FloatActorProperty::GetFuncType(this, &LerpActorRotationAction::GetTime),
         "The current time.");

      dtDAL::Vec3ActorProperty* startRotProp = new dtDAL::Vec3ActorProperty(
         "StartRotation", "Start Rotation",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &LerpActorRotationAction::SetStartRot),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &LerpActorRotationAction::GetStartRot),
         "The starting rotation of the actor.");
      AddProperty(startRotProp);

      dtDAL::Vec3ActorProperty* endRotProp = new dtDAL::Vec3ActorProperty(
         "EndRotation", "End Rotation",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &LerpActorRotationAction::SetEndRot),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &LerpActorRotationAction::GetEndRot),
         "The ending rotation of the actor.");
      AddProperty(endRotProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp, true, true, true));
      mValues.push_back(ValueLink(this, startTimeProp, false, false, true, false));
      mValues.push_back(ValueLink(this, endTimeProp, false, false, true, false));
      mValues.push_back(ValueLink(this, timeProp, false, false, true));
      mValues.push_back(ValueLink(this, startRotProp, false, false, true, false));
      mValues.push_back(ValueLink(this, endRotProp, false, false, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool LerpActorRotationAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      switch (input)
      {
      case INPUT_START:
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
                  ActionNode::Update(simDelta, delta, input, firstUpdate);
               }
               else // We shouldn't be updating anymore
               {
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
            float timeScalar = 1.0f;
            float delta = endTime - startTime;
            if (delta != 0.0f)
            {
               timeScalar = 1.0f / delta;
            }
            float alpha = (curTime - startTime) * timeScalar;

            osg::Quat startRot(osg::Vec4(GetVec3("StartRotation"), 0.0f));
            osg::Quat endRot(osg::Vec4(GetVec3("EndRotation"), 0.0f));

            osg::Quat newRot;
            newRot.slerp(alpha, startRot, endRot);

            osg::Vec3 rotation = newRot.asVec3();

            int count = GetPropertyCount("Actor");
            for (int index = 0; index < count; index++)
            {
               dtDAL::BaseActorObject* proxy = GetActor("Actor", index);
               if (proxy)
               {
                  dtCore::Transformable* actor = NULL;
                  proxy->GetActor(actor);
                  if (actor)
                  {
                     dtCore::Transform transform;
                     actor->GetTransform(transform);
                     transform.SetRotation(rotation.z(), rotation.x(), rotation.y());
                     actor->SetTransform(transform);
                  }
               }
            }

            // If we aren't active anymore, then we are finished
            if (!mIsActive)
            {
               OutputLink* link = GetOutputLink("Finished");
               if (link) link->Activate();
               return false;
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
   void LerpActorRotationAction::SetLerpActor(const dtCore::UniqueId& value)
   {
      mLerpActor = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId LerpActorRotationAction::GetLerpActor()
   {
      return mLerpActor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorRotationAction::SetStartTime(float value)
   {
      mStartTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LerpActorRotationAction::GetStartTime()
   {
      return mStartTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorRotationAction::SetEndTime(float value)
   {
      mEndTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LerpActorRotationAction::GetEndTime()
   {
      return mEndTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorRotationAction::SetTime(float value)
   {
      mTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LerpActorRotationAction::GetTime()
   {
      return mTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorRotationAction::SetStartRot(const osg::Vec3& value)
   {
      mStartRot = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LerpActorRotationAction::GetStartRot()
   {
      return mStartRot;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorRotationAction::SetEndRot(const osg::Vec3& value)
   {
      mEndRot = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LerpActorRotationAction::GetEndRot()
   {
      return mEndRot;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool LerpActorRotationAction::IsTimeInternal()
   {
      // If we have any external links to our Time property, then we are not
      // internally tracking the time
      return GetValueLink("Time")->GetLinks().size() == 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
