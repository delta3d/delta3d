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

#include <dtDirectorNodes/setactorpropertyaction.h>

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/stringactorproperty.h>

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

      mOutputs.push_back(OutputLink(this, "Failed"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetActorPropertyAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::StringActorProperty* nameProp = new dtDAL::StringActorProperty(
         "PropertyName", "Property Name",
         dtDAL::StringActorProperty::SetFuncType(this, &SetActorPropertyAction::SetPropertyName),
         dtDAL::StringActorProperty::GetFuncType(this, &SetActorPropertyAction::GetPropertyName),
         "The name of the actor property to retrieve.");
      AddProperty(nameProp);

      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &SetActorPropertyAction::SetCurrentActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &SetActorPropertyAction::GetCurrentActor),
         "", "The actor with the property to retrieve.");
      AddProperty(actorProp);

      dtDAL::StringActorProperty* valueProp = new dtDAL::StringActorProperty(
         "NewValue", "New Value",
         dtDAL::StringActorProperty::SetFuncType(this, &SetActorPropertyAction::SetNewValue),
         dtDAL::StringActorProperty::GetFuncType(this, &SetActorPropertyAction::GetNewValue),
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
         dtDAL::ActorProxy* actor = GetActor("Actor", index);
         if (actor)
         {
            dtDAL::ActorProperty* prop = actor->GetProperty(propName);
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
         OutputLink* link = GetOutputLink("Failed");
         if (link) link->Activate();
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
