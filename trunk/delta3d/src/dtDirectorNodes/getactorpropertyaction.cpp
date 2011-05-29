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
#include <dtDirectorNodes/getactorpropertyaction.h>

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/stringactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   GetActorPropertyAction::GetActorPropertyAction()
      : ActionNode()
   {
      mActor = "";
      mPropertyName = "";

      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   GetActorPropertyAction::~GetActorPropertyAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetActorPropertyAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetActorPropertyAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::StringActorProperty* nameProp = new dtDAL::StringActorProperty(
         "PropertyName", "Property Name",
         dtDAL::StringActorProperty::SetFuncType(this, &GetActorPropertyAction::SetPropertyName),
         dtDAL::StringActorProperty::GetFuncType(this, &GetActorPropertyAction::GetPropertyName),
         "The name of the actor property to retrieve.");
      AddProperty(nameProp);

      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &GetActorPropertyAction::SetCurrentActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &GetActorPropertyAction::GetCurrentActor),
         "", "The actor with the property to retrieve.");
      AddProperty(actorProp);

      mResultProp = new dtDAL::StringActorProperty(
         "Result", "Result",
         dtDAL::StringActorProperty::SetFuncType(this, &GetActorPropertyAction::SetResult),
         dtDAL::StringActorProperty::GetFuncType(this, &GetActorPropertyAction::GetResult),
         "The current value of the property.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp, false, false, true));
      mValues.push_back(ValueLink(this, mResultProp.get(), true, true, false));
      mValues.push_back(ValueLink(this, nameProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GetActorPropertyAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtDAL::BaseActorObject* actor = GetActor("Actor");
      if (actor)
      {
         std::string propName = GetString("PropertyName");

         // First attempt to find the property based on property name,
         // if this fails, then we attempt to find the property based
         // on label name instead.
         dtDAL::ActorProperty* prop = actor->GetProperty(propName);
         if (!prop)
         {
            std::vector<dtDAL::ActorProperty*> propList;
            actor->GetPropertyList(propList);
            int count = (int)propList.size();
            for (int index = 0; index < count; ++index)
            {
               dtDAL::ActorProperty* testProp = propList[index];
               if (testProp && testProp->GetLabel() == propName)
               {
                  prop = testProp;
                  break;
               }
            }
         }

         if (prop)
         {
            SetValueNodeValue(prop->ToString(), "Result");

            return ActionNode::Update(simDelta, delta, input, firstUpdate);
         }
      }

      OutputLink* link = GetOutputLink("Failed");
      if (link) link->Activate();
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetActorPropertyAction::SetPropertyName(const std::string& value)
   {
      mPropertyName = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string GetActorPropertyAction::GetPropertyName() const
   {
      return mPropertyName;
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetActorPropertyAction::SetCurrentActor(const dtCore::UniqueId& value)
   {
      mActor = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId GetActorPropertyAction::GetCurrentActor()
   {
      return mActor;
   }

   //////////////////////////////////////////////////////////////////////////
   void GetActorPropertyAction::SetResult(const std::string& value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   std::string GetActorPropertyAction::GetResult() const
   {
      return "";
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string& GetActorPropertyAction::GetName()
   {
      return mPropertyName;
   }
}

////////////////////////////////////////////////////////////////////////////////
