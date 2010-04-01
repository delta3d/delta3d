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

#include <sstream>
#include <algorithm>

#include <dtDirectorNodes/playervalue.h>

#include <dtDirector/director.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   PlayerValue::PlayerValue()
       : ValueNode()
   {
      mName = "Player";
      AddAuthor("Jeff P. Houde");
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

      // Make the name property read only, so it always says "Player".
      dtDAL::ActorProperty* nameProp = dtDAL::PropertyContainer::GetProperty("Name");
      if (nameProp) nameProp->SetReadOnly(true);

      mProperty = new dtDAL::ActorIDActorProperty(
         "Value", "Value",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &PlayerValue::SetValue),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &PlayerValue::GetValue),
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
      ValueNode::OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId PlayerValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return GetDirector()->GetPlayer();
   }
}
