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

#include <dtCore/stringselectoractorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/actoridactorproperty.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/arrayactorpropertybase.h>
#include <dtCore/arrayactorproperty.h>

#include <dtDirector/director.h>
#include <dtDirectorNodes/remoteevent.h>
#include <dtDirector/directortypefactory.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CallRemoteEventAction::CallRemoteEventAction()
      : LatentActionNode()
      , mEventScope(SCRIPT_SCOPE)
      , mOrignalValueCount(0)
      , mParameterIndex(0)
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
      mOutputs.push_back(OutputLink(this, "Out", "Activates after all remote events have been called, but before they all finish."));
      mOutputs.push_back(OutputLink(this, "Event Finished", "Activates after the entire chain connected to the remote event has finished."));

      SetEventName("");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::BuildPropertyMap()
   {
      LatentActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::StringSelectorActorProperty* eventNameProp = new dtCore::StringSelectorActorProperty(
         "EventName", "Remote Event Name",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &CallRemoteEventAction::SetEventName),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &CallRemoteEventAction::GetEventName),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &CallRemoteEventAction::GetEventList),
         "The name of the remote event to call.", "", true);
      AddProperty(eventNameProp);

      dtCore::StringSelectorActorProperty* typeProp = new dtCore::StringSelectorActorProperty(
         "Event Scope", "Event Scope",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &CallRemoteEventAction::SetEventScope),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &CallRemoteEventAction::GetEventScope),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &CallRemoteEventAction::GetEventScopeList),
         "The scope in which this action will search for Remote Events.");
      AddProperty(typeProp);

      dtCore::ActorIDActorProperty* instigatorProp = new dtCore::ActorIDActorProperty(
         "Instigator", "Instigator",
         dtCore::ActorIDActorProperty::SetFuncType(this, &CallRemoteEventAction::SetInstigator),
         dtCore::ActorIDActorProperty::GetFuncType(this, &CallRemoteEventAction::GetInstigator),
         "", "An instigator for this event.");
      AddProperty(instigatorProp);

      dtCore::StringActorProperty* paramNameProp = new dtCore::StringActorProperty(
         "Param Name", "Param Name",
         dtCore::StringActorProperty::SetFuncType(this, &CallRemoteEventAction::SetParamName),
         dtCore::StringActorProperty::GetFuncType(this, &CallRemoteEventAction::GetParamName),
         "The name of this parameter.");

      dtCore::StringActorProperty* paramValueProp = new dtCore::StringActorProperty(
         "Param Value", "Param Value",
         dtCore::StringActorProperty::SetFuncType(this, &CallRemoteEventAction::SetParamValue),
         dtCore::StringActorProperty::GetFuncType(this, &CallRemoteEventAction::GetParamValue),
         "The value of this parameter.");

      dtCore::ContainerActorProperty* paramProp = new dtCore::ContainerActorProperty(
         "Parameter", "Parameter", "Custom parameter", "");
      paramProp->AddProperty(paramNameProp);
      paramProp->AddProperty(paramValueProp);

      dtCore::ArrayActorPropertyBase* paramListProp = new dtCore::ArrayActorProperty<ParamData>(
         "Parameters", "Parameters", "Custom parameters to be sent and received from any corresponding Remote Events.",
         dtCore::ArrayActorProperty<ParamData>::SetIndexFuncType(this, &CallRemoteEventAction::SetParameterIndex),
         dtCore::ArrayActorProperty<ParamData>::GetDefaultFuncType(this, &CallRemoteEventAction::GetDefaultParameter),
         dtCore::ArrayActorProperty<ParamData>::GetArrayFuncType(this, &CallRemoteEventAction::GetParameterList),
         dtCore::ArrayActorProperty<ParamData>::SetArrayFuncType(this, &CallRemoteEventAction::SetParameterList),
         paramProp, "");
      AddProperty(paramListProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, eventNameProp, false, false, true, false));
      mValues.push_back(ValueLink(this, instigatorProp, false, false, true, false));
      mOrignalValueCount = mValues.size();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtCore::ActorProperty> CallRemoteEventAction::GetDeprecatedProperty(const std::string& name)
   {
      if (name == "Local Event")
      {
         dtCore::BooleanActorProperty* localProp = new dtCore::BooleanActorProperty(
            "Local Event", "Local Event",
            dtCore::BooleanActorProperty::SetFuncType(this, &CallRemoteEventAction::SetLocalEvent),
            dtCore::BooleanActorProperty::GetFuncType(this, &CallRemoteEventAction::IsLocalEvent),
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
               GetGraph()->GetNodes("Remote Event", "Core", "EventName", eventName, nodes, true, true);
            }
            break;

         case SCRIPT_SCOPE:
            {
               GetDirector()->GetNodes("Remote Event", "Core", "EventName", eventName, nodes, true);
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
            trackData.event = event;

            // Set up any custom parameters.
            int paramCount = (int)mValues.size();
            for (int paramIndex = mOrignalValueCount; paramIndex < paramCount; ++paramIndex)
            {
               ValueLink& param = mValues[paramIndex];
               event->SetString(GetString(param.GetName()), param.GetName());
            }

            trackData.id = event->Trigger("Out", &instigator);

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

         OutputLink* link = GetOutputLink("Out");
         if (link) link->Activate();
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
      LatentActionNode::OnLinkValueChanged(linkName);

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

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetParameterIndex(int index)
   {
      mParameterIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetParameter(const ParamData& value)
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         mParameterList[mParameterIndex] = value;
         UpdateParameterLinks();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   CallRemoteEventAction::ParamData CallRemoteEventAction::GetParameter() const
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         return mParameterList[mParameterIndex];
      }

      return ParamData(-1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   CallRemoteEventAction::ParamData CallRemoteEventAction::GetDefaultParameter() const
   {
      int index = (int)mParameterList.size() + 1;
      ParamData data(index);
      return data;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetParamName(const std::string& value)
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         mParameterList[mParameterIndex].name = value;
         UpdateParameterLinks();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string CallRemoteEventAction::GetParamName() const
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         return mParameterList[mParameterIndex].name;
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetParamValue(const std::string& value)
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         mParameterList[mParameterIndex].value = value;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string CallRemoteEventAction::GetParamValue() const
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         return mParameterList[mParameterIndex].value;
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::SetParameterList(const std::vector<ParamData>& value)
   {
      mParameterList = value;
      UpdateParameterLinks();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<CallRemoteEventAction::ParamData> CallRemoteEventAction::GetParameterList() const
   {
      return mParameterList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CallRemoteEventAction::UpdateParameterLinks()
   {
      std::vector<ValueLink> oldValues = mValues;

      if ((int)mValues.size() > mOrignalValueCount)
      {
         mValues.erase(mValues.begin() + mOrignalValueCount, mValues.end());
      }

      int count = (int)mParameterList.size();
      mValues.reserve(count + mOrignalValueCount);
      for (int index = 0; index < count; index++)
      {
         const std::string& name = mParameterList[index].name;

         // Make sure the name is not a core value link name.
         bool isNameValid = true;
         for (int testIndex = 0; testIndex < mOrignalValueCount; ++testIndex)
         {
            if (mValues[testIndex].GetName() == name)
            {
               isNameValid = false;
               break;
            }
         }
         if (name.empty())
         {
            isNameValid = false;
         }

         if (isNameValid)
         {
            mParameterList[index].displayProp = new dtCore::StringActorProperty(
               name, name,
               dtCore::StringActorProperty::SetFuncType(&mParameterList[index], &ParamData::SetValue),
               dtCore::StringActorProperty::GetFuncType(&mParameterList[index], &ParamData::GetValue),
               "The value of this parameter.");

            bool found = false;
            int testCount = (int)oldValues.size();
            for (int testIndex = mOrignalValueCount; testIndex < testCount; ++testIndex)
            {
               if (oldValues[testIndex].GetName() == mParameterList[index].name)
               {
                  ValueLink& link = oldValues[testIndex];
                  link.SetDefaultProperty(mParameterList[index].displayProp);

                  mValues.push_back(link);
                  found = true;
                  break;
               }
            }

            if (!found)
            {
               ValueLink newLink = ValueLink(this,
                  mParameterList[index].displayProp,
                  false, false, false, true);
               mValues.push_back(newLink);
            }
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
