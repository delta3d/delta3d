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
#include <dtDirectorNodes/scheduleraction.h>

#include <dtCore/arrayactorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/stringactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   SchedulerAction::SchedulerAction()
      : LatentActionNode()
      , mElapsedTime(0.0f)
      , mTotalTime(0.0f)
      , mIsActive(false)
      , mEventIndex(-1)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   SchedulerAction::~SchedulerAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SchedulerAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      LatentActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Play", "Begins or resumes the scheduler."));
      mInputs.push_back(InputLink(this, "Reverse", "Plays or resumes the scheduler in reverse."));
      mInputs.push_back(InputLink(this, "Stop", "Stops and resets the scheduler."));
      mInputs.push_back(InputLink(this, "Pause", "Pauses the scheduler so it can be resumed later."));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Started", "Activated when the scheduler has started in either normal or reverse."));
      mOutputs.push_back(OutputLink(this, "Ended", "Activated when the scheduler has finished in either normal or reverse."));
      mOutputs.push_back(OutputLink(this, "Stopped", "Activated when the scheduler has been forcibly stopped."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SchedulerAction::BuildPropertyMap()
   {
      LatentActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::FloatActorProperty* timeProp = new dtCore::FloatActorProperty(
         "Time", "Time",
         dtCore::FloatActorProperty::SetFuncType(this, &SchedulerAction::SetTime),
         dtCore::FloatActorProperty::GetFuncType(this, &SchedulerAction::GetTime),
         "The current time (in seconds).");

      dtCore::FloatActorProperty* totalTimeProp = new dtCore::FloatActorProperty(
         "TotalTime", "Total Time",
         dtCore::FloatActorProperty::SetFuncType(this, &SchedulerAction::SetTotalTime),
         dtCore::FloatActorProperty::GetFuncType(this, &SchedulerAction::GetTotalTime),
         "The total time (in seconds).");
      AddProperty(totalTimeProp);

      dtCore::BooleanActorProperty* simTimeProp = new dtCore::BooleanActorProperty(
         "UseSimTime", "Use Sim Time",
         dtCore::BooleanActorProperty::SetFuncType(this, &SchedulerAction::SetUseSimTime),
         dtCore::BooleanActorProperty::GetFuncType(this, &SchedulerAction::GetUseSimTime),
         "True to use game/sim time, false to use real time.");
      AddProperty(simTimeProp);

      dtCore::ContainerActorProperty* eventGroupProp = new dtCore::ContainerActorProperty(
         "OutputEventData", "Output Event Data", "This is an output event.", "");

      dtCore::StringActorProperty* eventNameProp = new dtCore::StringActorProperty(
         "EventName", "Event Name",
         dtCore::StringActorProperty::SetFuncType(this, &SchedulerAction::SetEventName),
         dtCore::StringActorProperty::GetFuncType(this, &SchedulerAction::GetEventName),
         "The name of the output to fire.");
      eventGroupProp->AddProperty(eventNameProp);

      dtCore::FloatActorProperty* eventTimeProp = new dtCore::FloatActorProperty(
         "EventTime", "Event Time",
         dtCore::FloatActorProperty::SetFuncType(this, &SchedulerAction::SetEventTime),
         dtCore::FloatActorProperty::GetFuncType(this, &SchedulerAction::GetEventTime),
         "The time that the output will fire.");
      eventGroupProp->AddProperty(eventTimeProp);

      dtCore::BooleanActorProperty* eventNormalProp = new dtCore::BooleanActorProperty(
         "TriggerNormal", "Trigger on Play",
         dtCore::BooleanActorProperty::SetFuncType(this, &SchedulerAction::SetEventNormal),
         dtCore::BooleanActorProperty::GetFuncType(this, &SchedulerAction::GetEventNormal),
         "True to trigger this event on normal play.");
      eventGroupProp->AddProperty(eventNormalProp);

      dtCore::BooleanActorProperty* eventReverseProp = new dtCore::BooleanActorProperty(
         "TriggerReverse", "Trigger on Reverse",
         dtCore::BooleanActorProperty::SetFuncType(this, &SchedulerAction::SetEventReverse),
         dtCore::BooleanActorProperty::GetFuncType(this, &SchedulerAction::GetEventReverse),
         "True to trigger this event on reverse play.");
      eventGroupProp->AddProperty(eventReverseProp);

      dtCore::ArrayActorPropertyBase* eventListProp = new dtCore::ArrayActorProperty<OutputEventData>(
         "EventList", "Event List", "The list of events.",
         dtCore::ArrayActorProperty<OutputEventData>::SetIndexFuncType(this, &SchedulerAction::SetEventIndex),
         dtCore::ArrayActorProperty<OutputEventData>::GetDefaultFuncType(this, &SchedulerAction::GetDefaultEvent),
         dtCore::ArrayActorProperty<OutputEventData>::GetArrayFuncType(this, &SchedulerAction::GetEventList),
         dtCore::ArrayActorProperty<OutputEventData>::SetArrayFuncType(this, &SchedulerAction::SetEventList),
         eventGroupProp, "");
      AddProperty(eventListProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, timeProp, true, true, true));
      mValues.push_back(ValueLink(this, totalTimeProp, false, false, false, false));
      mValues.push_back(ValueLink(this, simTimeProp, false, false, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool SchedulerAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      bool result = false;

      float elapsedTime = simDelta;
      if (!GetBoolean("UseSimTime")) elapsedTime = delta;

      switch (input)
      {
      case INPUT_PLAY:
      case INPUT_REVERSE:
         {
            float totalTime = GetFloat("TotalTime");

            // We need to check the current active status because this node
            // will be updated multiple times using this same index until
            // the desired time has elapsed.  And we only want to trigger
            // the "Out" output once at the beginning.
            if (firstUpdate)
            {
               // If this is a first update and we already started, then kill the
               // new thread since we're running on another thread
               if (mIsActive && input == mPlayDirection)
               {
                  return false;
               }

               mIsActive = true;

               if (input == INPUT_PLAY)
               {
                  // If we are playing from the beginning and our
                  // elapsed time is at the end, reset it back
                  // to the start.
                  if (mElapsedTime >= totalTime)
                  {
                     mElapsedTime = 0.0f;
                  }
               }
               else if (input == INPUT_REVERSE)
               {
                  // If we are playing in reverse and our elapsed time
                  // is at the beginning, flip it to start at the end.
                  if (mElapsedTime <= 0.0f)
                  {
                     mElapsedTime = totalTime;
                  }
               }

               mPlayDirection = input;

               OutputLink* link = GetOutputLink("Started");
               if (link) link->Activate();
               result = true;
            }

            // If we are not active, stop this thread.
            if (!mIsActive) return false;

            // If we are updating a thread that is flowing in the wrong direction,
            // close that thread.
            if (mPlayDirection != input) return false;

            float start = 0.0f;
            float end = 0.0f;

            if (input == INPUT_PLAY)
            {
               start = mElapsedTime;
               mElapsedTime += elapsedTime;
               end = mElapsedTime;
            }
            else
            {
               end = mElapsedTime;
               mElapsedTime -= elapsedTime;
               start = mElapsedTime;
            }

            // Trigger any events that are between the current time and the
            // time delta.
            TestEvents(start, end, input == INPUT_PLAY);

            // Test if the desired time has elapsed.
            result = true;
            if ((input == INPUT_PLAY && mElapsedTime >= totalTime) ||
               (input == INPUT_REVERSE && mElapsedTime <= 0))
            {
               mIsActive = false;

               // Clamp the time to the bounds of the track.
               if (mElapsedTime < 0.0f) mElapsedTime = 0.0f;
               else if (mElapsedTime > totalTime) mElapsedTime = totalTime;

               // Fire the "Stopped" output
               OutputLink* link = GetOutputLink("Ended");
               if (link) link->Activate();

               // Return false so this node does not remain active.
               result = false;
            }

            SetFloat(mElapsedTime, "Time");
         }
         break;

      case INPUT_STOP:
         // Reset the elapsed time and deactivate it.
         if (mIsActive)
         {
            mElapsedTime = 0.0f;
            SetFloat(mElapsedTime, "Time");
            mIsActive = false;

            // Fire the "Stopped" output
            OutputLink* link = GetOutputLink("Stopped");
            if (link) link->Activate();
         }
         break;

      case INPUT_PAUSE:
         // Deactivate the node, but do not reset the timer.
         mIsActive = false;
         break;
      }

      return result;
   }

   //////////////////////////////////////////////////////////////////////////
   bool SchedulerAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (LatentActionNode::CanConnectValue(link, value))
      {
         // Delay link can only connect to basic types.
         if (link->GetName() == "Time")
         {
            if (value->CanBeType(dtCore::DataType::FLOAT) ||
                value->CanBeType(dtCore::DataType::DOUBLE))
            {
               return true;
            }
            return false;
         }
         return true;
      }
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void SchedulerAction::SetTime(float value)
   {
      mElapsedTime = value;
   }

   //////////////////////////////////////////////////////////////////////////
   float SchedulerAction::GetTime()
   {
      return mElapsedTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SchedulerAction::SetTotalTime(float value)
   {
      mTotalTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float SchedulerAction::GetTotalTime()
   {
      return mTotalTime;
   }

   //////////////////////////////////////////////////////////////////////////
   void SchedulerAction::SetUseSimTime(bool value)
   {
      mUseSimTime = value;
   }

   //////////////////////////////////////////////////////////////////////////
   bool SchedulerAction::GetUseSimTime()
   {
      return mUseSimTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SchedulerAction::SetEventName(const std::string& value)
   {
      if (mEventIndex >= 0 && mEventIndex < (int)mEventList.size())
      {
         OutputEventData& data = mEventList[mEventIndex];
         data.name = value;
         UpdateOutputs();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string SchedulerAction::GetEventName()
   {
      if (mEventIndex >= 0 && mEventIndex < (int)mEventList.size())
      {
         OutputEventData& data = mEventList[mEventIndex];
         return data.name;
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SchedulerAction::SetEventTime(float value)
   {
      if (mEventIndex >= 0 && mEventIndex < (int)mEventList.size())
      {
         OutputEventData& data = mEventList[mEventIndex];
         data.time = value;
         UpdateOutputs();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   float SchedulerAction::GetEventTime()
   {
      if (mEventIndex >= 0 && mEventIndex < (int)mEventList.size())
      {
         OutputEventData& data = mEventList[mEventIndex];
         return data.time;
      }

      return 0.0f;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SchedulerAction::SetEventNormal(bool value)
   {
      if (mEventIndex >= 0 && mEventIndex < (int)mEventList.size())
      {
         OutputEventData& data = mEventList[mEventIndex];
         data.triggerNormal = value;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SchedulerAction::GetEventNormal()
   {
      if (mEventIndex >= 0 && mEventIndex < (int)mEventList.size())
      {
         OutputEventData& data = mEventList[mEventIndex];
         return data.triggerNormal;
      }

      return 0.0f;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SchedulerAction::SetEventReverse(bool value)
   {
      if (mEventIndex >= 0 && mEventIndex < (int)mEventList.size())
      {
         OutputEventData& data = mEventList[mEventIndex];
         data.triggerReverse = value;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SchedulerAction::GetEventReverse()
   {
      if (mEventIndex >= 0 && mEventIndex < (int)mEventList.size())
      {
         OutputEventData& data = mEventList[mEventIndex];
         return data.triggerReverse;
      }

      return 0.0f;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SchedulerAction::SetEventIndex(int index)
   {
      mEventIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   SchedulerAction::OutputEventData SchedulerAction::GetDefaultEvent()
   {
      OutputEventData data;
      data.name = "";
      data.time = 0.0f;
      data.triggerNormal = true;
      data.triggerReverse = true;
      return data;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<SchedulerAction::OutputEventData> SchedulerAction::GetEventList()
   {
      return mEventList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SchedulerAction::SetEventList(const std::vector<OutputEventData>& value)
   {
      mEventList = value;
      UpdateOutputs();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SchedulerAction::UpdateOutputs()
   {
      OutputLink startOutput(this, "Started", "Activated when the scheduler has started in either normal or reverse.");
      OutputLink endOutput(this, "Ended", "Activated when the scheduler has finished in either normal or reverse.");
      OutputLink stopOutput(this, "Stopped", "Activated when the scheduler has been forcibly stopped.");

      OutputLink* link = GetOutputLink("Started");
      if (link) startOutput = *link;
      link = GetOutputLink("Ended");
      if (link) endOutput = *link;
      link = GetOutputLink("Stopped");
      if (link) stopOutput = *link;

      std::vector<OutputLink> outputs = mOutputs;
      mOutputs.clear();
      mOutputs.push_back(startOutput);

      int count = (int)mEventList.size();
      for (int index = 0; index < count; index++)
      {
         OutputEventData& data = mEventList[index];
         if (data.name.empty()) continue;

         // Create a new output link if the current name does not exist.
         OutputLink* link = GetOutputLink(data.name);
         if (!link)
         {
            bool found = false;
            int linkCount = (int)outputs.size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               if (outputs[linkIndex].GetName() == data.name)
               {
                  found = true;
                  mOutputs.push_back(outputs[linkIndex]);
                  break;
               }
            }

            if (!found) mOutputs.push_back(OutputLink(this, data.name, "Activated when the custom event time has been reached."));
         }

         // Update the total time
         if (mTotalTime < data.time)
         {
            mTotalTime = data.time;
         }
      }

      mOutputs.push_back(endOutput);
      mOutputs.push_back(stopOutput);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SchedulerAction::TestEvents(float start, float end, bool normalPlay)
   {
      int count = (int)mEventList.size();
      for (int index = 0; index < count; index++)
      {
         OutputEventData& data = mEventList[index];
         if (data.name.empty()) continue;

         if (data.time >= start && (
            data.time < end ||
            (data.time == end && end == mTotalTime)))
         {
            if ((normalPlay && data.triggerNormal) ||
               (!normalPlay && data.triggerReverse))
            {
               OutputLink* link = GetOutputLink(data.name);
               if (link) link->Activate();
            }
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
