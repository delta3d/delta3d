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

#include <dtDirectorNodes/randomintvalue.h>

#include <dtDAL/intactorproperty.h>

#include <dtUtil/mathdefines.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   RandomIntValue::RandomIntValue()
       : ValueNode()
       , mMinValue(0.0f)
       , mMaxValue(1.0f)
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   RandomIntValue::~RandomIntValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void RandomIntValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RandomIntValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtDAL::IntActorProperty(
         "Value", "Value",
         dtDAL::IntActorProperty::SetFuncType(this, &RandomIntValue::SetValue),
         dtDAL::IntActorProperty::GetFuncType(this, &RandomIntValue::GetValue),
         "The value.");

      dtDAL::IntActorProperty* minProp = new dtDAL::IntActorProperty(
         "Min Value", "Min Value",
         dtDAL::IntActorProperty::SetFuncType(this, &RandomIntValue::SetMinValue),
         dtDAL::IntActorProperty::GetFuncType(this, &RandomIntValue::GetMinValue),
         "The minimum value.");
      AddProperty(minProp);

      dtDAL::IntActorProperty* maxProp = new dtDAL::IntActorProperty(
         "Max Value", "Max Value",
         dtDAL::IntActorProperty::SetFuncType(this, &RandomIntValue::SetMaxValue),
         dtDAL::IntActorProperty::GetFuncType(this, &RandomIntValue::GetMaxValue),
         "The minimum value.");
      AddProperty(maxProp);
   }

   //////////////////////////////////////////////////////////////////////////
   std::string RandomIntValue::GetValueLabel()
   {
      dtDAL::ActorProperty* minProp = Node::GetProperty("Min Value");
      dtDAL::ActorProperty* maxProp = Node::GetProperty("Max Value");

      if (minProp && maxProp)
      {
         return "(" + minProp->GetValueString() + "-" + maxProp->GetValueString() + ")";
      }

      return "()";
   }

   //////////////////////////////////////////////////////////////////////////
   void RandomIntValue::SetMinValue(int value)
   {
      if (mMinValue != value)
      {
         mMinValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   int RandomIntValue::GetMinValue()
   {
      ValueNode::OnValueRetrieved();
      return mMinValue;
   }

   //////////////////////////////////////////////////////////////////////////
   void RandomIntValue::SetMaxValue(int value)
   {
      if (mMaxValue != value)
      {
         mMaxValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   int RandomIntValue::GetMaxValue()
   {
      ValueNode::OnValueRetrieved();
      return mMaxValue;
   }

   //////////////////////////////////////////////////////////////////////////
   void RandomIntValue::SetValue(int value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   int RandomIntValue::GetValue()
   {
      ValueNode::OnValueRetrieved();

      return dtUtil::RandRange(GetInt("Min Value"), GetInt("Max Value"));
   }
}
