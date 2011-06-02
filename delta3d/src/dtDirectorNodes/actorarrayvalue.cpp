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

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/arrayactorproperty.h>

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

      mProperty = new dtDAL::ActorIDActorProperty(
         "Value", "Value",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &ActorArrayValue::SetValue),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &ActorArrayValue::GetValue),
         "", "The value.");

      mInitialProperty = new dtDAL::ActorIDActorProperty(
         "Value", "Value",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &ActorArrayValue::SetInitialValue),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &ActorArrayValue::GetInitialValue),
         "", "The initial value.");

      mArrayProperty = new dtDAL::ArrayActorProperty<dtCore::UniqueId>(
         "ValueList", "Value List", "All values contained in this array.",
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::SetIndexFuncType(this, &ArrayValueNode::SetPropertyIndex),
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::GetDefaultFuncType(this, &ActorArrayValue::GetDefaultValue),
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::GetArrayFuncType(this, &ActorArrayValue::GetArray),
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::SetArrayFuncType(this, &ActorArrayValue::SetArray),
         mProperty, "");
      AddProperty(mArrayProperty);

      mInitialArrayProperty = new dtDAL::ArrayActorProperty<dtCore::UniqueId>(
         "InitialValueList", "Initial Value List", "All initial values contained in this array.",
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::SetIndexFuncType(this, &ArrayValueNode::SetInitialPropertyIndex),
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::GetDefaultFuncType(this, &ActorArrayValue::GetDefaultValue),
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::GetArrayFuncType(this, &ActorArrayValue::GetInitialArray),
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::SetArrayFuncType(this, &ActorArrayValue::SetInitialArray),
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
         if (mInitialValues[mInitialPropertyIndex] != value)
         {
            std::string oldValue = mInitialArrayProperty->ToString();

            mInitialValues[mInitialPropertyIndex] = value;

            OnInitialValueChanged(oldValue);
         }
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
      if (mInitialValues != value)
      {
         std::string oldValue = mInitialProperty->ToString();
         mInitialValues = value;
         OnInitialValueChanged(oldValue);
      }
   }
}
