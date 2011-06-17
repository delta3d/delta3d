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
#include <dtDirectorNodes/callremoteeventaction.h>

#include <dtDAL/stringselectoractorproperty.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/actoridactorproperty.h>

#include <dtDirector/director.h>
#include <dtDirectorNodes/remoteevent.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CallRemoteEventAction::CallRemoteEventAction()
      : ActionNode()
      , mIsLocalEvent(false)
   {
      mInstigator = "";
      AddAuthor("Jeff P. Houde");
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
      mInputs.push_back(InputLink(this, "Call Event", "Executes the remote event."));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Event Finished", "Activates after the entire chain connected to the remote event has finished."));

      SetEventName("");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::StringSelectorActorProperty* eventNameProp = new dtDAL::StringSelectorActorProperty(
         "EventName", "Remote Event Name",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &CallRemoteEventAction::SetEventName),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &CallRemoteEventAction::GetEventName),
         dtDAL::StringSelectorActorProperty::GetListFuncType(this, &CallRemoteEventAction::GetEventList),
         "The name of the remote event to call.", "", true);
      AddProperty(eventNameProp);

      dtDAL::BooleanActorProperty* localProp = new dtDAL::BooleanActorProperty(
         "Local Event", "Local Event",
         dtDAL::BooleanActorProperty::SetFuncType(this, &CallRemoteEventAction::SetLocalEvent),
         dtDAL::BooleanActorProperty::GetFuncType(this, &CallRemoteEventAction::IsLocalEvent),
         "False to search the entire Director script for these events.  True to only search the current graph and sub-graphs.");
      AddProperty(localProp);

      dtDAL::ActorIDActorProperty* instigatorProp = new dtDAL::ActorIDActorProperty(
         "Instigator", "Instigator", 
         dtDAL::ActorIDActorProperty::SetFuncType(this, &CallRemoteEventAction::SetInstigator),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &CallRemoteEventAction::GetInstigator),
         "", "An instigator for this event.");
      AddProperty(instigatorProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, eventNameProp, false, false, true, false));
      mValues.push_back(ValueLink(this, localProp, false, false, true, false));
      mValues.push_back(ValueLink(this, instigatorProp, false, false, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool CallRemoteEventAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      if (firstUpdate)
      {
         std::string eventName = GetString("EventName");
         if (eventName.empty()) return false;

         dtCore::UniqueId instigator = GetActorID("Instigator");

         // Find the remote event that we want to trigger.
         std::vector<Node*> nodes;
         if (!GetBoolean("Local Event"))
         {
            GetDirector()->GetNodes("Remote Event", "Core", "EventName", eventName, nodes);
         }
         else
         {
            GetGraph()->GetNodes("Remote Event", "Core", "EventName", eventName, nodes);
         }

         bool madeStack = false;
         int count = (int)nodes.size();
         for (int index = 0; index < count; index++)
         {
            EventNode* event = nodes[index]->AsEventNode();
            if (!event) continue;

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
            event->Trigger("Out", &instigator, true, false);
         }

         return true;
      }
      // Once we get back here again, it means we have finished calling
      // our remote event and can trigger our output now.
      else
      {
         OutputLink* link = GetOutputLink("Event Finished");
         if (link) link->Activate();
         return false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetEventName(const std::string& value)
   {
      mEventName = mName = value;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& CallRemoteEventAction::GetEventName()
   {
      return mEventName;
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<std::string> CallRemoteEventAction::GetEventList()
   {
      std::vector<std::string> stringList;
      std::vector<Node*> nodes;
      if (!GetBoolean("Local Event"))
      {
         GetDirector()->GetNodes("Remote Event", "Core", nodes);
      }
      else
      {
         GetGraph()->GetNodes("Remote Event", "Core", nodes);
      }

      int count = (int)nodes.size();
      for (int index = 0; index < count; index++)
      {
         EventNode* event = nodes[index]->AsEventNode();
         if (!event) continue;

         stringList.push_back(event->GetString("EventName"));
      }

      return stringList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetLocalEvent(bool value)
   {
      mIsLocalEvent = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CallRemoteEventAction::IsLocalEvent() const
   {
      return mIsLocalEvent;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetInstigator(const dtCore::UniqueId& value)
   {
      mInstigator = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& CallRemoteEventAction::GetInstigator() const
   {
      return mInstigator;
   }
}

////////////////////////////////////////////////////////////////////////////////
