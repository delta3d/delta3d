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
#include <dtDirectorNodes/floatvalue.h>

#include <dtCore/floatactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   FloatValue::FloatValue()
       : ValueNode()
       , mValue(0.0f)
       , mInitialValue(0.0f)
   {
      AddAuthor("Jeff P. Houde");
      //SetColorRGB(Colors::RED); 
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   FloatValue::~FloatValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void FloatValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FloatValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtCore::FloatActorProperty(
         "Value", "Value",
         dtCore::FloatActorProperty::SetFuncType(this, &FloatValue::SetValue),
         dtCore::FloatActorProperty::GetFuncType(this, &FloatValue::GetValue),
         "The value.");
      AddProperty(mProperty);

      mInitialProperty = new dtCore::FloatActorProperty(
         "Initial Value", "Initial Value",
         dtCore::FloatActorProperty::SetFuncType(this, &FloatValue::SetInitialValue),
         dtCore::FloatActorProperty::GetFuncType(this, &FloatValue::GetInitialValue),
         "The initial value.");
      AddProperty(mInitialProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void FloatValue::SetValue(float value)
   {
      if (mValue != value)
      {
         mValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   float FloatValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }

   //////////////////////////////////////////////////////////////////////////
   void FloatValue::SetInitialValue(float value)
   {
      std::string oldValue = mInitialProperty->ToString();
      mInitialValue = value;
      ValueNode::OnInitialValueChanged(oldValue);
   }

   //////////////////////////////////////////////////////////////////////////
   float FloatValue::GetInitialValue()
   {
      return mInitialValue;
   }
}
