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
#include <dtDirectorNodes/randomfloatvalue.h>

#include <dtCore/floatactorproperty.h>

#include <dtUtil/mathdefines.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   RandomFloatValue::RandomFloatValue()
       : ValueNode()
       , mMinValue(0.0f)
       , mMaxValue(1.0f)
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   RandomFloatValue::~RandomFloatValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void RandomFloatValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RandomFloatValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtCore::FloatActorProperty(
         "Value", "Value",
         dtCore::FloatActorProperty::SetFuncType(this, &RandomFloatValue::SetValue),
         dtCore::FloatActorProperty::GetFuncType(this, &RandomFloatValue::GetValue),
         "The value.");

      dtCore::FloatActorProperty* minProp = new dtCore::FloatActorProperty(
         "Min Value", "Min Value",
         dtCore::FloatActorProperty::SetFuncType(this, &RandomFloatValue::SetMinValue),
         dtCore::FloatActorProperty::GetFuncType(this, &RandomFloatValue::GetMinValue),
         "The minimum value.");
      AddProperty(minProp);

      dtCore::FloatActorProperty* maxProp = new dtCore::FloatActorProperty(
         "Max Value", "Max Value",
         dtCore::FloatActorProperty::SetFuncType(this, &RandomFloatValue::SetMaxValue),
         dtCore::FloatActorProperty::GetFuncType(this, &RandomFloatValue::GetMaxValue),
         "The minimum value.");
      AddProperty(maxProp);
   }

   //////////////////////////////////////////////////////////////////////////
   std::string RandomFloatValue::GetValueLabel()
   {
      dtCore::ActorProperty* minProp = Node::GetProperty("Min Value");
      dtCore::ActorProperty* maxProp = Node::GetProperty("Max Value");

      if (minProp && maxProp)
      {
         return "(" + minProp->GetValueString() + "-" + maxProp->GetValueString() + ")";
      }

      return "()";
   }

   //////////////////////////////////////////////////////////////////////////
   void RandomFloatValue::SetValue(float value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   float RandomFloatValue::GetValue()
   {
      ValueNode::OnValueRetrieved();

      return dtUtil::RandFloat(GetFloat("Min Value"), GetFloat("Max Value"));
   }

   //////////////////////////////////////////////////////////////////////////
   void RandomFloatValue::SetMinValue(float value)
   {
      if (mMinValue != value)
      {
         mMinValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   float RandomFloatValue::GetMinValue()
   {
      ValueNode::OnValueRetrieved();
      return mMinValue;
   }

   //////////////////////////////////////////////////////////////////////////
   void RandomFloatValue::SetMaxValue(float value)
   {
      if (mMaxValue != value)
      {
         mMaxValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   float RandomFloatValue::GetMaxValue()
   {
      ValueNode::OnValueRetrieved();
      return mMaxValue;
   }
}
