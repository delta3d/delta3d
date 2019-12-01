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
#include <dtDirectorNodes/actorarrayvalue.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/actorproperty.h>
#include <dtCore/arrayactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ActorArrayValue::ActorArrayValue()
       : ArrayValueNode()
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActorArrayValue::~ActorArrayValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActorArrayValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ArrayValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorArrayValue::BuildPropertyMap()
   {
      ArrayValueNode::BuildPropertyMap();

      mProperty = new dtCore::ActorIDActorProperty(
         "Value", "Value",
         dtCore::ActorIDActorProperty::SetFuncType(this, &ActorArrayValue::SetValue),
         dtCore::ActorIDActorProperty::GetFuncType(this, &ActorArrayValue::GetValue),
         "", "The value.");

      mInitialProperty = new dtCore::ActorIDActorProperty(
         "Value", "Value",
         dtCore::ActorIDActorProperty::SetFuncType(this, &ActorArrayValue::SetInitialValue),
         dtCore::ActorIDActorProperty::GetFuncType(this, &ActorArrayValue::GetInitialValue),
         "", "The initial value.");

      mArrayProperty = new dtCore::ArrayActorProperty<dtCore::UniqueId>(
         "ValueList", "Value List", "All values contained in this array.",
         dtCore::ArrayActorProperty<dtCore::UniqueId>::SetIndexFuncType(this, &ArrayValueNode::SetPropertyIndex),
         dtCore::ArrayActorProperty<dtCore::UniqueId>::GetDefaultFuncType(this, &ActorArrayValue::GetDefaultValue),
         dtCore::ArrayActorProperty<dtCore::UniqueId>::GetArrayFuncType(this, &ActorArrayValue::GetArray),
         dtCore::ArrayActorProperty<dtCore::UniqueId>::SetArrayFuncType(this, &ActorArrayValue::SetArray),
         mProperty, "");
      AddProperty(mArrayProperty);

      mInitialArrayProperty = new dtCore::ArrayActorProperty<dtCore::UniqueId>(
         "InitialValueList", "Initial Value List", "All initial values contained in this array.",
         dtCore::ArrayActorProperty<dtCore::UniqueId>::SetIndexFuncType(this, &ArrayValueNode::SetInitialPropertyIndex),
         dtCore::ArrayActorProperty<dtCore::UniqueId>::GetDefaultFuncType(this, &ActorArrayValue::GetDefaultValue),
         dtCore::ArrayActorProperty<dtCore::UniqueId>::GetArrayFuncType(this, &ActorArrayValue::GetInitialArray),
         dtCore::ArrayActorProperty<dtCore::UniqueId>::SetArrayFuncType(this, &ActorArrayValue::SetInitialArray),
         mInitialProperty, "");
      AddProperty(mInitialArrayProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorArrayValue::SetValue(const dtCore::UniqueId& value)
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
   dtCore::UniqueId ActorArrayValue::GetValue()
   {
      ArrayValueNode::OnValueRetrieved();

      if (mPropertyIndex < (int)mValues.size())
      {
         return mValues[mPropertyIndex];
      }

      return GetDefaultValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId ActorArrayValue::GetDefaultValue()
   {
      dtCore::UniqueId id;
      id = "";
      return id;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::UniqueId> ActorArrayValue::GetArray()
   {
      return mValues;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorArrayValue::SetArray(const std::vector<dtCore::UniqueId>& value)
   {
      mValues = value;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorArrayValue::SetInitialValue(const dtCore::UniqueId& value)
   {
      if (mInitialPropertyIndex < (int)mInitialValues.size())
      {
         int index = mInitialPropertyIndex;
         std::string oldValue = mInitialArrayProperty->ToString();

         mInitialValues[index] = value;

         OnInitialValueChanged(oldValue);
         mInitialPropertyIndex = index;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId ActorArrayValue::GetInitialValue()
   {
      if (mInitialPropertyIndex < (int)mInitialValues.size())
      {
         return mInitialValues[mInitialPropertyIndex];
      }

      return GetDefaultValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::UniqueId> ActorArrayValue::GetInitialArray()
   {
      return mInitialValues;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorArrayValue::SetInitialArray(const std::vector<dtCore::UniqueId>& value)
   {
      int index = mInitialPropertyIndex;
      std::string oldValue = mInitialProperty->ToString();

      mInitialValues = value;
      OnInitialValueChanged(oldValue);
      mInitialPropertyIndex = index;
   }
}
