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

#include <dtCore/actoridactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtCore/vectoractorproperties.h>
#include <dtDirector/director.h>
#include <dtUtil/matrixutil.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   LerpActorRotationAction::LerpActorRotationAction()
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
   LerpActorRotationAction::~LerpActorRotationAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorRotationAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      LatentActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Start", "Begins the rotation."));
      mInputs.push_back(InputLink(this, "Stop", "Ends the rotation."));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Started", "Activates when the rotation has started."));
      mOutputs.push_back(OutputLink(this, "Stopped", "Activated when the rotation was forcibly stopped."));
      mOutputs.push_back(OutputLink(this, "Finished", "Activated when the rotation has finished."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LerpActorRotationAction::BuildPropertyMap()
   {
      LatentActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Actor", "Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &LerpActorRotationAction::SetLerpActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &LerpActorRotationAction::GetLerpActor),
         "dtCore::Transformable", "The actor to linearly interpolate.");
      AddProperty(actorProp);

      dtCore::FloatActorProperty* startTimeProp = new dtCore::FloatActorProperty(
         "StartTime", "Start Time",
         dtCore::FloatActorProperty::SetFuncType(this, &LerpActorRotationAction::SetStartTime),
         dtCore::FloatActorProperty::GetFuncType(this, &LerpActorRotationAction::GetStartTime),
         "The time to start the lerp.");
      AddProperty(startTimeProp);

      dtCore::FloatActorProperty* endTimeProp = new dtCore::FloatActorProperty(
         "EndTime", "End Time",
         dtCore::FloatActorProperty::SetFuncType(this, &LerpActorRotationAction::SetEndTime),
         dtCore::FloatActorProperty::GetFuncType(this, &LerpActorRotationAction::GetEndTime),
         "The time to end the lerp.");
      AddProperty(endTimeProp);

      dtCore::FloatActorProperty* timeProp = new dtCore::FloatActorProperty(
         "Time", "Time",
         dtCore::FloatActorProperty::SetFuncType(this, &LerpActorRotationAction::SetTime),
         dtCore::FloatActorProperty::GetFuncType(this, &LerpActorRotationAction::GetTime),
         "The current time.");

      dtCore::Vec3ActorProperty* startRotProp = new dtCore::Vec3ActorProperty(
         "StartRotation", "Start Rotation",
         dtCore::Vec3ActorProperty::SetFuncType(this, &LerpActorRotationAction::SetStartRot),
         dtCore::Vec3ActorProperty::GetFuncType(this, &LerpActorRotationAction::GetStartRot),
         "The starting rotation of the actor.");
      AddProperty(startRotProp);

      dtCore::Vec3ActorProperty* endRotProp = new dtCore::Vec3ActorProperty(
         "EndRotation", "End Rotation",
         dtCore::Vec3ActorProperty::SetFuncType(this, &LerpActorRotationAction::SetEndRot),
         dtCore::Vec3ActorProperty::GetFuncType(this, &LerpActorRotationAction::GetEndRot),
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

            // Rotation vectors are pitch, roll, heading, so convert it to hpr
            osg::Vec3 startPRHVec = GetVec3("StartRotation");
            osg::Vec3 endPRHVec = GetVec3("EndRotation");
            osg::Vec3 startHPRVec, endHPRVec;
            startHPRVec.x() = startPRHVec.z();
            startHPRVec.y() = startPRHVec.x();
            startHPRVec.z() = startPRHVec.y();
            endHPRVec.x() = endPRHVec.z();
            endHPRVec.y() = endPRHVec.x();
            endHPRVec.z() = endPRHVec.y();

            // Now convert the HPRVector to a matrix and then to a quaternion
            osg::Matrix startMatrix, endMatrix;
            dtUtil::MatrixUtil::HprToMatrix(startMatrix, startHPRVec);
            dtUtil::MatrixUtil::HprToMatrix(endMatrix, endHPRVec);
            osg::Quat startRot = startMatrix.getRotate();
            osg::Quat endRot = endMatrix.getRotate();

            osg::Quat newRot;
            newRot.slerp(alpha, startRot, endRot);

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
                     transform.SetRotation(newRot);
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
