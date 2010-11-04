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

#include <dtDirector/colors.h>
#include <dtDirectorNodes/booleanvalue.h>

#include <dtDAL/actorproperty.h>
#include <dtDAL/booleanactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   BooleanValue::BooleanValue()
       : ValueNode()
       , mValue(false)
   {
      AddAuthor("Jeff P. Houde");
      //SetColorRGB(Colors::RED); 
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   BooleanValue::~BooleanValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void BooleanValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BooleanValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtDAL::BooleanActorProperty(
         "Value", "Value",
         dtDAL::BooleanActorProperty::SetFuncType(this, &BooleanValue::SetValue),
         dtDAL::BooleanActorProperty::GetFuncType(this, &BooleanValue::GetValue),
         "The value.");
      AddProperty(mProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void BooleanValue::SetValue(bool value)
   {
      mValue = value;
      ValueNode::OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   bool BooleanValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }
}
