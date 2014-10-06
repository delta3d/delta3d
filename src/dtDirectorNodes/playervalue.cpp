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
#include <dtDirectorNodes/playervalue.h>

#include <dtCore/actoridactorproperty.h>
#include <dtDirector/colors.h>
#include <dtDirector/director.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   PlayerValue::PlayerValue()
       : ValueNode()
   {
      AddAuthor("Jeff P. Houde");
      SetColorRGB(Colors::MANGENTA2);
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   PlayerValue::~PlayerValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void PlayerValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PlayerValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtCore::ActorIDActorProperty(
         "Value", "Value",
         dtCore::ActorIDActorProperty::SetFuncType(this, &PlayerValue::SetValue),
         dtCore::ActorIDActorProperty::GetFuncType(this, &PlayerValue::GetValue),
         "", "The value.");
   }

   //////////////////////////////////////////////////////////////////////////
   std::string PlayerValue::GetValueLabel()
   {
      return "";
   }

   //////////////////////////////////////////////////////////////////////////
   void PlayerValue::SetValue(const dtCore::UniqueId& value)
   {
      GetDirector()->SetPlayer(value);
      ValueNode::OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId PlayerValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return GetDirector()->GetPlayer();
   }
}
