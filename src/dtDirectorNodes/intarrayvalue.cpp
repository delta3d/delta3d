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
 * Author: Eric R. Heine
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/intarrayvalue.h>

#include <dtCore/arrayactorproperty.h>
#include <dtCore/intactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   IntArrayValue::IntArrayValue()
       : ArrayValueNode()
   {
      AddAuthor("Eric R. Heine");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   IntArrayValue::~IntArrayValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void IntArrayValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ArrayValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IntArrayValue::BuildPropertyMap()
   {
      ArrayValueNode::BuildPropertyMap();

      mProperty = new dtCore::IntActorProperty(
         "Value", "Value",
         dtCore::IntActorProperty::SetFuncType(this, &IntArrayValue::SetValue),
         dtCore::IntActorProperty::GetFuncType(this, &IntArrayValue::GetValue),
         "The value.");

      mInitialProperty = new dtCore::IntActorProperty(
         "Initial Value", "Initial Value",
         dtCore::IntActorProperty::SetFuncType(this, &IntArrayValue::SetInitialValue),
         dtCore::IntActorProperty::GetFuncType(this, &IntArrayValue::GetInitialValue),
         "The initial value.");

      mArrayProperty = new dtCore::ArrayActorProperty<int>(
         "ValueList", "Value List", "All values contained in this array.",
         dtCore::ArrayActorProperty<int>::SetIndexFuncType(this, &ArrayValueNode::SetPropertyIndex),
         dtCore::ArrayActorProperty<int>::GetDefaultFuncType(this, &IntArrayValue::GetDefaultValue),
         dtCore::ArrayActorProperty<int>::GetArrayFuncType(this, &IntArrayValue::GetArray),
         dtCore::ArrayActorProperty<int>::SetArrayFuncType(this, &IntArrayValue::SetArray),
         mProperty, "");
      AddProperty(mArrayProperty);

      mInitialArrayProperty = new dtCore::ArrayActorProperty<int>(
         "InitialValueList", "Initial Value List", "All initial values contained in this array.",
         dtCore::ArrayActorProperty<int>::SetIndexFuncType(this, &ArrayValueNode::SetInitialPropertyIndex),
         dtCore::ArrayActorProperty<int>::GetDefaultFuncType(this, &IntArrayValue::GetDefaultValue),
         dtCore::ArrayActorProperty<int>::GetArrayFuncType(this, &IntArrayValue::GetInitialArray),
         dtCore::ArrayActorProperty<int>::SetArrayFuncType(this, &IntArrayValue::SetInitialArray),
         mInitialProperty, "");
      AddProperty(mInitialArrayProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void IntArrayValue::SetValue(int value)
   {
      if (mPropertyIndex < (int)mValues.size())
      {
         if (mValues[mPropertyIndex] != value)
         {
            mValues[mPropertyIndex] = value;

            ArrayValueNode::OnValueChanged();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   int IntArrayValue::GetValue()
   {
      ArrayValueNode::OnValueRetrieved();

      if (mPropertyIndex < (int)mValues.size())
      {
         return mValues[mPropertyIndex];
      }
      return 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int IntArrayValue::GetDefaultValue()
   {
      return 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<int> IntArrayValue::GetArray()
   {
      return mValues;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IntArrayValue::SetArray(const std::vector<int>& value)
   {
      mValues = value;
   }

   //////////////////////////////////////////////////////////////////////////
   void IntArrayValue::SetInitialValue(int value)
   {
      if (mInitialPropertyIndex < (int)mInitialValues.size())
      {
         int index = mInitialPropertyIndex;
         std::string oldValue = mInitialArrayProperty->ToString();

         mInitialValues[index] = value;

         ArrayValueNode::OnInitialValueChanged(oldValue);
         mInitialPropertyIndex = index;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   int IntArrayValue::GetInitialValue()
   {
      if (mInitialPropertyIndex < (int)mInitialValues.size())
      {
         return mInitialValues[mInitialPropertyIndex];
      }
      return 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<int> IntArrayValue::GetInitialArray()
   {
      return mInitialValues;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IntArrayValue::SetInitialArray(const std::vector<int>& value)
   {
      int index = mInitialPropertyIndex;
      std::string oldValue = mInitialProperty->ToString();

      mInitialValues = value;
      OnInitialValueChanged(oldValue);
      mInitialPropertyIndex = index;

      // Clear initial value flag if list is empty
      if (value.empty())
      {
         mHasInitialValue = false;
      }
   }
}
