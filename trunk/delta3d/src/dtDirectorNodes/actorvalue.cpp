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

#include <dtDirectorNodes/actorvalue.h>

#include <dtDirector/director.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ActorValue::ActorValue()
       : ValueNode()
   {
      mName = "Actor";
      mValue = "";
      AddAuthor("Jeff P. Houde");
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
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ActorValue::GetValueLabel()
   {
      std::string label = "";
      dtDAL::ActorIDActorProperty* prop = dynamic_cast<dtDAL::ActorIDActorProperty*>(mProperty.get());
      if (prop && prop->GetRealActor()) label = "(" + prop->GetRealActor()->GetName() + ")";

      return label;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorValue::SetValue(const dtCore::UniqueId& value)
   {
      mValue = value;
      ValueNode::OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& ActorValue::GetValue()
   {
      return mValue;
   }
}
