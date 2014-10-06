/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
#include <dtDirectorNodes/setactorpropertyaction.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/stringactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SetActorPropertyAction::SetActorPropertyAction()
      : ActionNode()
   {
      mActor = "";
      mPropertyName = "";

      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   SetActorPropertyAction::~SetActorPropertyAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetActorPropertyAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed", "Activated when the actor property could not be found."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetActorPropertyAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::StringActorProperty* nameProp = new dtCore::StringActorProperty(
         "PropertyName", "Property Name",
         dtCore::StringActorProperty::SetFuncType(this, &SetActorPropertyAction::SetPropertyName),
         dtCore::StringActorProperty::GetFuncType(this, &SetActorPropertyAction::GetPropertyName),
         "The name of the actor property to retrieve.");
      AddProperty(nameProp);

      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Actor", "Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &SetActorPropertyAction::SetCurrentActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &SetActorPropertyAction::GetCurrentActor),
         "", "The actor with the property to retrieve.");
      AddProperty(actorProp);

      dtCore::StringActorProperty* valueProp = new dtCore::StringActorProperty(
         "NewValue", "New Value",
         dtCore::StringActorProperty::SetFuncType(this, &SetActorPropertyAction::SetNewValue),
         dtCore::StringActorProperty::GetFuncType(this, &SetActorPropertyAction::GetNewValue),
         "The new value for the property.");
      AddProperty(valueProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp, true, true, true));
      mValues.push_back(ValueLink(this, valueProp, false, false, false));
      mValues.push_back(ValueLink(this, nameProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SetActorPropertyAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      std::string propName = GetString("PropertyName");
      std::string propValue = GetString("NewValue");

      bool setProp = false;
      int count = GetPropertyCount("Actor");
      for (int index = 0; index < count; index++)
      {
         dtCore::ActorProxy* actor = GetActor("Actor", index);
         if (actor)
         {
            // First attempt to find the property based on property name,
            // if this fails, then we attempt to find the property based
            // on label name instead.
            dtCore::ActorProperty* prop = actor->GetProperty(propName);
            if (!prop)
            {
               std::vector<dtCore::ActorProperty*> propList;
               actor->GetPropertyList(propList);
               int count = (int)propList.size();
               for (int index = 0; index < count; ++index)
               {
                  dtCore::ActorProperty* testProp = propList[index];
                  if (testProp && testProp->GetLabel() == propName)
                  {
                     prop = testProp;
                     break;
                  }
               }
            }

            if (prop)
            {
               prop->FromString(propValue);
               setProp = true;
            }
         }
      }

      if (setProp)
      {
         ActionNode::Update(simDelta, delta, input, firstUpdate);
      }
      else
      {
         ActivateOutput("Failed");
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetActorPropertyAction::SetPropertyName(const std::string& value)
   {
      mPropertyName = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string SetActorPropertyAction::GetPropertyName() const
   {
      return mPropertyName;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetActorPropertyAction::SetCurrentActor(const dtCore::UniqueId& value)
   {
      mActor = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId SetActorPropertyAction::GetCurrentActor()
   {
      return mActor;
   }

   //////////////////////////////////////////////////////////////////////////
   void SetActorPropertyAction::SetNewValue(const std::string& value)
   {
      mNewValue = value;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string SetActorPropertyAction::GetNewValue() const
   {
      return mNewValue;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string& SetActorPropertyAction::GetName()
   {
      return mPropertyName;
   }
}

////////////////////////////////////////////////////////////////////////////////
