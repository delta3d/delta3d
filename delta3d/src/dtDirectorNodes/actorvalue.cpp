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
#include <dtDirector/colors.h>
#include <dtDirectorNodes/actorvalue.h>

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/actorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ActorValue::ActorValue()
       : ValueNode()
   {
      mValue = "";
      mInitialValue = "";
      AddAuthor("Jeff P. Houde");
      SetColorRGB(Colors::MANGENTA);
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActorValue::~ActorValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtDAL::ActorIDActorProperty(
         "Value", "Value",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &ActorValue::SetValue),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &ActorValue::GetValue),
         "", "The value.");
      AddProperty(mProperty);

      mInitialProperty = new dtDAL::ActorIDActorProperty(
         "Initial Value", "Initial Value",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &ActorValue::SetInitialValue),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &ActorValue::GetInitialValue),
         "", "The initial value.");
      AddProperty(mInitialProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ActorValue::GetValueLabel()
   {
      std::string label = "";
      if (mProperty)
      {
         std::string actorName = mProperty->GetValueString();

         dtGame::GameManager* gm = GetDirector()->GetGameManager();
         if (gm)
         {
            dtDAL::BaseActorObject* actor = gm->FindActorById(dtCore::UniqueId(mProperty->ToString().c_str()));
            if (actor)
            {
               actorName = actor->GetName();
            }
         }

         label = "(" + actorName + ")";
      }
      return label;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorValue::SetValue(const dtCore::UniqueId& value)
   {
      if (mValue != value)
      {
         mValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& ActorValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorValue::SetInitialValue(const dtCore::UniqueId& value)
   {
      std::string oldValue = mInitialProperty->ToString();
      mInitialValue = value;
      OnInitialValueChanged(oldValue);
   }

   //////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& ActorValue::GetInitialValue()
   {
      return mInitialValue;
   }
}
