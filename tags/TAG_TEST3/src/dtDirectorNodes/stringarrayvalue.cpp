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

#include <dtDirectorNodes/stringarrayvalue.h>

#include <dtDAL/arrayactorproperty.h>
#include <dtDAL/stringactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   StringArrayValue::StringArrayValue()
       : ArrayValueNode()
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   StringArrayValue::~StringArrayValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void StringArrayValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ArrayValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringArrayValue::BuildPropertyMap()
   {
      ArrayValueNode::BuildPropertyMap();

      mProperty = new dtDAL::StringActorProperty(
         "Value", "Value",
         dtDAL::StringActorProperty::SetFuncType(this, &StringArrayValue::SetValue),
         dtDAL::StringActorProperty::GetFuncType(this, &StringArrayValue::GetValue),
         "The value.");

      mArrayProperty = new dtDAL::ArrayActorProperty<std::string>(
         "ValueList", "Value List", "All values contained in this array.",
         dtDAL::ArrayActorProperty<std::string>::SetIndexFuncType(this, &ArrayValueNode::SetPropertyIndex),
         dtDAL::ArrayActorProperty<std::string>::GetDefaultFuncType(this, &StringArrayValue::GetDefaultValue),
         dtDAL::ArrayActorProperty<std::string>::GetArrayFuncType(this, &StringArrayValue::GetArray),
         dtDAL::ArrayActorProperty<std::string>::SetArrayFuncType(this, &StringArrayValue::SetArray),
         mProperty, "");
      AddProperty(mArrayProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void StringArrayValue::SetValue(const std::string& value)
   {
      if (mPropertyIndex < (int)mValues.size())
      {
         mValues[mPropertyIndex] = value;
      }

      ArrayValueNode::OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   std::string StringArrayValue::GetValue()
   {
      ArrayValueNode::OnValueRetrieved();

      if (mPropertyIndex < (int)mValues.size())
      {
         return mValues[mPropertyIndex];
      }
      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string StringArrayValue::GetDefaultValue()
   {
      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> StringArrayValue::GetArray()
   {
      return mValues;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringArrayValue::SetArray(const std::vector<std::string>& value)
   {
      mValues = value;
   }
}
