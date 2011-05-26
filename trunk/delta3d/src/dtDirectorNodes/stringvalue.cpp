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

#include <dtDirectorNodes/stringvalue.h>
#include <dtDirector/colors.h>

#include <dtDAL/stringactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   StringValue::StringValue()
       : ValueNode()
       , mValue("")
   {
      AddAuthor("Jeff P. Houde");
      //SetColorRGB(Colors::RED); 
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   StringValue::~StringValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void StringValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtDAL::StringActorProperty(
         "Value", "Value",
         dtDAL::StringActorProperty::SetFuncType(this, &StringValue::SetValue),
         dtDAL::StringActorProperty::GetFuncType(this, &StringValue::GetValue),
         "The value.");
      AddProperty(mProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void StringValue::SetValue(const std::string& value)
   {
      if (mValue != value)
      {
         mValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& StringValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }
}
