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

#include <dtDirectorNodes/callremoteeventaction.h>
#include <dtDirectorNodes/remoteevent.h>

#include <dtDirector/director.h>

#include <dtDAL/enginepropertytypes.h>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CallRemoteEventAction::CallRemoteEventAction()
      : ActionNode()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   CallRemoteEventAction::~CallRemoteEventAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Call Event"));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Event Finished"));

      SetEventName("");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::StringActorProperty* eventNameProp = new dtDAL::StringActorProperty(
         "EventName", "Remote Event Name",
         dtDAL::StringActorProperty::SetFuncType(this, &CallRemoteEventAction::SetEventName),
         dtDAL::StringActorProperty::GetFuncType(this, &CallRemoteEventAction::GetEventName),
         "The name of the remote event to call.");
      AddProperty(eventNameProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, eventNameProp, false, false, true));
   }

   //////////////////////////////////////////////////////////////////////////
   bool CallRemoteEventAction::Update(float simDelta, float delta, int inputIndex)
   {
      // No matter what, call the finished event.  This will work because
      // this thread will be pushed lower on the stack if a remote event
      // was called, essentially pausing the execution of this thread.
      OutputLink* link = GetOutputLink("Event Finished");
      if (link) link->Activate();

      // Find the remote event that we want to trigger.
      std::vector<Node*> nodes;
      GetDirector()->GetNodes("Remote Event", "Core", nodes);

      std::string eventName = GetString("EventName");
      if (eventName.empty()) return false;

      bool madeStack = false;
      int count = (int)nodes.size();
      for (int index = 0; index < count; index++)
      {
         RemoteEvent* event = dynamic_cast<RemoteEvent*>(nodes[index]);
         if (event && event->GetEventName() == eventName)
         {
            // If we have not created a new call stack yet, create it.
            if (!madeStack)
            {
               // I use a NULL node here, because when I trigger the
               // event later, it will cause the event to be in
               // two separate threads.
               GetDirector()->PushStack(NULL, 0);
               madeStack = true;
            }

            // Now trigger the event.
            event->Trigger();
         }
      }

      return ActionNode::Update(simDelta, delta, inputIndex);
   }

   //////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetEventName(const std::string& value)
   {
      mEventName = value;
      mLabel = GetType().GetName() + " (" + mEventName + ")";
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& CallRemoteEventAction::GetEventName()
   {
      return mEventName;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& CallRemoteEventAction::GetName()
   {
      return mLabel;
   }
}

////////////////////////////////////////////////////////////////////////////////
