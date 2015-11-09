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
#include <dtDirectorNodes/remoteevent.h>

#include <dtCore/actorproxy.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/arrayactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   RemoteEvent::RemoteEvent()
       : EventNode()
       , mOrignalValueCount(0)
       , mParameterIndex(0)
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   RemoteEvent::~RemoteEvent()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void RemoteEvent::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      SetEventName("Event Name");

      EventNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RemoteEvent::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();
      RemoveProperty("Name");

      dtCore::StringActorProperty* nameProp = new dtCore::StringActorProperty(
         "EventName", "Event Name",
         dtCore::StringActorProperty::SetFuncType(this, &RemoteEvent::SetEventName),
         dtCore::StringActorProperty::GetFuncType(this, &RemoteEvent::GetEventName),
         "The name of the event.");
      AddProperty(nameProp);

      dtCore::StringActorProperty* paramNameProp = new dtCore::StringActorProperty(
         "Param Name", "Param Name",
         dtCore::StringActorProperty::SetFuncType(this, &RemoteEvent::SetParamName),
         dtCore::StringActorProperty::GetFuncType(this, &RemoteEvent::GetParamName),
         "The name of this parameter.");

      dtCore::StringActorProperty* paramValueProp = new dtCore::StringActorProperty(
         "Param Value", "Param Value",
         dtCore::StringActorProperty::SetFuncType(this, &RemoteEvent::SetParamValue),
         dtCore::StringActorProperty::GetFuncType(this, &RemoteEvent::GetParamValue),
         "The value of this parameter.");

      dtCore::ContainerActorProperty* paramProp = new dtCore::ContainerActorProperty(
         "Parameter", "Parameter", "Custom parameter", "");
      paramProp->AddProperty(paramNameProp);
      paramProp->AddProperty(paramValueProp);

      dtCore::ArrayActorPropertyBase* paramListProp = new dtCore::ArrayActorProperty<ParamData>(
         "Parameters", "Parameters", "Custom parameters to be sent and received from any corresponding Remote Events.",
         dtCore::ArrayActorProperty<ParamData>::SetIndexFuncType(this, &RemoteEvent::SetParameterIndex),
         dtCore::ArrayActorProperty<ParamData>::GetDefaultFuncType(this, &RemoteEvent::GetDefaultParameter),
         dtCore::ArrayActorProperty<ParamData>::GetArrayFuncType(this, &RemoteEvent::GetParameterList),
         dtCore::ArrayActorProperty<ParamData>::SetArrayFuncType(this, &RemoteEvent::SetParameterList),
         paramProp, "");
      AddProperty(paramListProp);

      ValueLink* link = GetValueLink("ActorFilters");
      if (link)
      {
         link->SetExposed(false);
      }

      mValues.push_back(ValueLink(this, nameProp, false, false, true, false));
      mOrignalValueCount = mValues.size();
   }

   //////////////////////////////////////////////////////////////////////////
   void RemoteEvent::SetEventName(const std::string& eventName)
   {
      mEventName = mName = eventName;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& RemoteEvent::GetEventName() const
   {
      return mEventName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RemoteEvent::SetParameterIndex(int index)
   {
      mParameterIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RemoteEvent::SetParameter(const ParamData& value)
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         mParameterList[mParameterIndex] = value;
         UpdateParameterLinks();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   RemoteEvent::ParamData RemoteEvent::GetParameter() const
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         return mParameterList[mParameterIndex];
      }

      return ParamData(-1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   RemoteEvent::ParamData RemoteEvent::GetDefaultParameter() const
   {
      int index = (int)mParameterList.size() + 1;
      ParamData data(index);
      return data;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RemoteEvent::SetParamName(const std::string& value)
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         mParameterList[mParameterIndex].name = value;
         UpdateParameterLinks();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string RemoteEvent::GetParamName() const
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         return mParameterList[mParameterIndex].name;
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RemoteEvent::SetParamValue(const std::string& value)
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         mParameterList[mParameterIndex].value = value;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string RemoteEvent::GetParamValue() const
   {
      if (mParameterIndex > -1 && mParameterIndex < (int)mParameterList.size())
      {
         return mParameterList[mParameterIndex].value;
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RemoteEvent::SetParameterList(const std::vector<ParamData>& value)
   {
      mParameterList = value;
      UpdateParameterLinks();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<RemoteEvent::ParamData> RemoteEvent::GetParameterList() const
   {
      return mParameterList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RemoteEvent::UpdateParameterLinks()
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
                  true, false, false, true);
               mValues.push_back(newLink);
            }
         }
      }
   }
}
