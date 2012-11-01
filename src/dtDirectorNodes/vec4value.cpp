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
#include <dtDirectorNodes/vec4value.h>
#include <dtDirector/colors.h>

#include <dtCore/vectoractorproperties.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   Vec4Value::Vec4Value()
       : ValueNode()
   {
      AddAuthor("Jeff P. Houde");
      //SetColorRGB(Colors::RED); 
  }

   ///////////////////////////////////////////////////////////////////////////////////////
   Vec4Value::~Vec4Value()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void Vec4Value::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec4Value::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtCore::Vec4ActorProperty(
         "Value", "Value",
         dtCore::Vec4ActorProperty::SetFuncType(this, &Vec4Value::SetValue),
         dtCore::Vec4ActorProperty::GetFuncType(this, &Vec4Value::GetValue),
         "The value.");
      AddProperty(mProperty);

      mInitialProperty = new dtCore::Vec4ActorProperty(
         "Initial Value", "Initial Value",
         dtCore::Vec4ActorProperty::SetFuncType(this, &Vec4Value::SetInitialValue),
         dtCore::Vec4ActorProperty::GetFuncType(this, &Vec4Value::GetInitialValue),
         "The initial value.");
      AddProperty(mInitialProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void Vec4Value::SetValue(const osg::Vec4& value)
   {
      if (mValue != value)
      {
         mValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   const osg::Vec4& Vec4Value::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }

   //////////////////////////////////////////////////////////////////////////
   void Vec4Value::SetInitialValue(const osg::Vec4& value)
   {
      std::string oldValue = mInitialProperty->ToString();
      mInitialValue = value;
      ValueNode::OnInitialValueChanged(oldValue);
   }

   //////////////////////////////////////////////////////////////////////////
   const osg::Vec4& Vec4Value::GetInitialValue()
   {
      return mInitialValue;
   }
}
