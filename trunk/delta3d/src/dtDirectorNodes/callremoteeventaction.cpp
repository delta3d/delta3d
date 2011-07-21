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
#include <dtDirector/directortypefactory.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CallRemoteEventAction::CallRemoteEventAction()
      : LatentActionNode()
      , mEventScope(SCRIPT_SCOPE)
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
      LatentActionNode::Init(nodeType, graph);

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
      LatentActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::StringSelectorActorProperty* eventNameProp = new dtDAL::StringSelectorActorProperty(
         "EventName", "Remote Event Name",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &CallRemoteEventAction::SetEventName),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &CallRemoteEventAction::GetEventName),
         dtDAL::StringSelectorActorProperty::GetListFuncType(this, &CallRemoteEventAction::GetEventList),
         "The name of the remote event to call.", "", true);
      AddProperty(eventNameProp);

      dtDAL::StringSelectorActorProperty* typeProp = new dtDAL::StringSelectorActorProperty(
         "Event Scope", "Event Scope",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &CallRemoteEventAction::SetEventScope),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &CallRemoteEventAction::GetEventScope),
         dtDAL::StringSelectorActorProperty::GetListFuncType(this, &CallRemoteEventAction::GetEventScopeList),
         "The scope in which this action will search for Remote Events.");
      AddProperty(typeProp);

      dtDAL::ActorIDActorProperty* instigatorProp = new dtDAL::ActorIDActorProperty(
         "Instigator", "Instigator", 
         dtDAL::ActorIDActorProperty::SetFuncType(this, &CallRemoteEventAction::SetInstigator),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &CallRemoteEventAction::GetInstigator),
         "", "An instigator for this event.");
      AddProperty(instigatorProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, eventNameProp, false, false, true, false));
      mValues.push_back(ValueLink(this, instigatorProp, false, false, true, false));
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::ActorProperty> CallRemoteEventAction::GetDeprecatedProperty(const std::string& name)
   {
      if (name == "Local Event")
      {
         dtDAL::BooleanActorProperty* localProp = new dtDAL::BooleanActorProperty(
            "Local Event", "Local Event",
            dtDAL::BooleanActorProperty::SetFuncType(this, &CallRemoteEventAction::SetLocalEvent),
            dtDAL::BooleanActorProperty::GetFuncType(this, &CallRemoteEventAction::IsLocalEvent),
            "False to search the entire Director script for these events.  True to only search the current graph and sub-graphs.");
         return localProp;
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   bool CallRemoteEventAction::Update(float simDelta, float delta, int input, bool firstUpdate, void*& data)
   {
      if (firstUpdate)
      {
         std::string eventName = GetString("EventName");
         if (eventName.empty()) return false;

         dtCore::UniqueId instigator = GetActorID("Instigator");

         // Find the remote event that we want to trigger.
         std::vector<Node*> nodes;

         switch (mEventScope)
         {
         case LOCAL_SCOPE:
            {
               GetGraph()->GetNodes("Remote Event", "Core", "EventName", eventName, nodes);
            }
            break;

         case SCRIPT_SCOPE:
            {
               GetDirector()->GetNodes("Remote Event", "Core", "EventName", eventName, nodes);
            }
            break;

         case GLOBAL_SCOPE:
            {
               DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
               if (factory)
               {
                  const std::vector<Director*>& scriptList = factory->GetScriptInstances();
                  int count = (int)scriptList.size();
                  for (int index = 0; index < count; ++index)
                  {
                     Director* script = scriptList[index];
                     if (script)
                     {
                        script->GetNodes("Remote Event", "Core", "EventName", eventName, nodes);
                     }
                  }
               }
            }
            break;
         }

         std::vector<TrackingData>* trackList = NULL;

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
            TrackingData trackData;
            trackData.script = event->GetTopDirector();
            trackData.id = event->Trigger("Out", &instigator);

            // Track this data if we are executing an event
            // outside the scope of this script.
            if (trackData.script != GetTopDirector() &&
                trackData.id != -1)
            {
               if (!trackList)
               {
                  trackList = new std::vector<TrackingData>();
                  data = trackList;
               }

               trackList->push_back(trackData);
            }
         }

         return true;
      }
      // Once we get back here again, it means we have finished calling
      // our remote event and can trigger our output now.
      else
      {
         std::vector<TrackingData>* trackList = static_cast<std::vector<TrackingData>*>(data);
         if (trackList)
         {
            // Test all threads being tracked to test if they are all finished.
            int count = (int)trackList->size();
            for (int index = 0; index < count; ++index)
            {
               TrackingData& trackData = (*trackList)[index];
               if (!trackData.script.valid() ||
                   !trackData.script->IsRunning(trackData.id))
               {
                  trackList->erase(trackList->begin() + index);
                  index--;
                  count--;
               }
            }

            if (!trackList->empty())
            {
               return true;
            }

            delete trackList;
            data = NULL;
         }

         OutputLink* link = GetOutputLink("Event Finished");
         if (link) link->Activate();
         return false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::OnLinkValueChanged(const std::string& linkName)
   {
      if (linkName == "EventName")
      {
         UpdateName();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::UpdateName()
   {
      mName = GetString("EventName");
      switch (mEventScope)
      {
      case LOCAL_SCOPE:
         mName += " (Local)";
         break;
      case GLOBAL_SCOPE:
         mName += " (Global)";
         break;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetEventName(const std::string& value)
   {
      mEventName = value;
      UpdateName();
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
      std::map<std::string, bool> stringMap;
      std::vector<Node*> nodes;

      switch (mEventScope)
      {
      case LOCAL_SCOPE:
         {
            GetGraph()->GetNodes("Remote Event", "Core", nodes);
         }
         break;

      case SCRIPT_SCOPE:
         {
            GetDirector()->GetNodes("Remote Event", "Core", nodes);
         }
         break;

      case GLOBAL_SCOPE:
         {
            DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
            if (factory)
            {
               const std::vector<Director*>& scriptList = factory->GetScriptInstances();
               int count = (int)scriptList.size();
               for (int index = 0; index < count; ++index)
               {
                  Director* script = scriptList[index];
                  if (script)
                  {
                     script->GetNodes("Remote Event", "Core", nodes);
                  }
               }
            }
         }
         break;
      }

      int count = (int)nodes.size();
      for (int index = 0; index < count; index++)
      {
         EventNode* event = nodes[index]->AsEventNode();
         if (!event) continue;

         stringMap[event->GetString("EventName")] = true;
      }

      std::map<std::string, bool>::iterator iter;
      for (iter = stringMap.begin(); iter != stringMap.end(); ++iter)
      {
         stringList.push_back(iter->first);
      }

      return stringList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetLocalEvent(bool value)
   {
      if (value)
      {
         mEventScope = LOCAL_SCOPE;
      }
      else
      {
         mEventScope = SCRIPT_SCOPE;
      }
      UpdateName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CallRemoteEventAction::IsLocalEvent() const
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetEventScope(const std::string& value)
   {
      if (value == "Local Scope")
      {
         mEventScope = LOCAL_SCOPE;
      }
      else if (value == "Script Scope")
      {
         mEventScope = SCRIPT_SCOPE;
      }
      else if (value == "Global Scope")
      {
         mEventScope = GLOBAL_SCOPE;
      }
      UpdateName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string CallRemoteEventAction::GetEventScope() const
   {
      switch (mEventScope)
      {
      case LOCAL_SCOPE:
         return "Local Scope";
      case SCRIPT_SCOPE:
         return "Script Scope";
      case GLOBAL_SCOPE:
         return "Global Scope";
      }

      return "Unknown";
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> CallRemoteEventAction::GetEventScopeList()
   {
      std::vector<std::string> result;

      result.push_back("Local Scope");
      result.push_back("Script Scope");
      result.push_back("Global Scope");

      return result;
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
