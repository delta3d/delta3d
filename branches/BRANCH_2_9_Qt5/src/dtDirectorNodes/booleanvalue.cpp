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
#include <dtDirectorNodes/booleanvalue.h>

#include <dtCore/actorproperty.h>
#include <dtCore/booleanactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   BooleanValue::BooleanValue()
       : ValueNode()
       , mValue(false)
       , mInitialValue(false)
   {
      AddAuthor("Jeff P. Houde");
      SetColorRGB(Colors::RED);
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

      mProperty = new dtCore::BooleanActorProperty(
         "Value", "Value",
         dtCore::BooleanActorProperty::SetFuncType(this, &BooleanValue::SetValue),
         dtCore::BooleanActorProperty::GetFuncType(this, &BooleanValue::GetValue),
         "The value.");
      AddProperty(mProperty);

      mInitialProperty = new dtCore::BooleanActorProperty(
         "Initial Value", "Initial Value",
         dtCore::BooleanActorProperty::SetFuncType(this, &BooleanValue::SetInitialValue),
         dtCore::BooleanActorProperty::GetFuncType(this, &BooleanValue::GetInitialValue),
         "The initial value.");
      AddProperty(mInitialProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void BooleanValue::SetValue(bool value)
   {
      if (mValue != value)
      {
         mValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool BooleanValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }

   //////////////////////////////////////////////////////////////////////////
   void BooleanValue::SetInitialValue(bool value)
   {
      std::string oldValue = mInitialProperty->ToString();
      mInitialValue = value;
      OnInitialValueChanged(oldValue);
   }

   //////////////////////////////////////////////////////////////////////////
   bool BooleanValue::GetInitialValue()
   {
      return mInitialValue;
   }
}
