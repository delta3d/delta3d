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
#include <dtDirectorNodes/intvalue.h>

#include <dtCore/intactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   IntValue::IntValue()
       : ValueNode()
       , mValue(0)
       , mInitialValue(0)
   {
      AddAuthor("Jeff P. Houde");
      //SetColorRGB(Colors::RED); 
  }

   ///////////////////////////////////////////////////////////////////////////////////////
   IntValue::~IntValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void IntValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IntValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtCore::IntActorProperty(
         "Value", "Value",
         dtCore::IntActorProperty::SetFuncType(this, &IntValue::SetValue),
         dtCore::IntActorProperty::GetFuncType(this, &IntValue::GetValue),
         "The value.");
      AddProperty(mProperty);

      mInitialProperty = new dtCore::IntActorProperty(
         "Initial Value", "Initial Value",
         dtCore::IntActorProperty::SetFuncType(this, &IntValue::SetInitialValue),
         dtCore::IntActorProperty::GetFuncType(this, &IntValue::GetInitialValue),
         "The initial value.");
     AddProperty(mInitialProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   bool IntValue::CanBeType(dtCore::DataType& type)
   {
      if (type == dtCore::DataType::INT ||
         type == dtCore::DataType::UINT ||
         type == dtCore::DataType::BIT_MASK)
      {
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void IntValue::SetValue(int value)
   {
      if (mValue != value)
      {
         mValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   int IntValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }

   //////////////////////////////////////////////////////////////////////////
   void IntValue::SetInitialValue(int value)
   {
      std::string oldValue = mInitialProperty->ToString();
      mInitialValue = value;
      ValueNode::OnInitialValueChanged(oldValue);
   }

   //////////////////////////////////////////////////////////////////////////
   int IntValue::GetInitialValue()
   {
      return mInitialValue;
   }
}
