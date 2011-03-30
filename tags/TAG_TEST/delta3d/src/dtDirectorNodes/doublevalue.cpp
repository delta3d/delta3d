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
#include <dtDirectorNodes/doublevalue.h>

#include <dtDAL/actorproperty.h>
#include <dtDAL/doubleactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   DoubleValue::DoubleValue()
       : ValueNode()
       , mValue(0.0)
   {
      AddAuthor("Jeff P. Houde");
      //SetColorRGB(Colors::RED); 
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   DoubleValue::~DoubleValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void DoubleValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DoubleValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtDAL::DoubleActorProperty(
         "Value", "Value",
         dtDAL::DoubleActorProperty::SetFuncType(this, &DoubleValue::SetValue),
         dtDAL::DoubleActorProperty::GetFuncType(this, &DoubleValue::GetValue),
         "The value.");
      AddProperty(mProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void DoubleValue::SetValue(double value)
   {
      mValue = value;
      ValueNode::OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   double DoubleValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }
}
