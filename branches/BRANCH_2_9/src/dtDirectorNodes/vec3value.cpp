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
#include <dtDirectorNodes/vec3value.h>
#include <dtDirector/colors.h>

#include <dtCore/vectoractorproperties.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   Vec3Value::Vec3Value()
       : ValueNode()
   {
      AddAuthor("Jeff P. Houde");
      //SetColorRGB(Colors::RED); 
  }

   ///////////////////////////////////////////////////////////////////////////////////////
   Vec3Value::~Vec3Value()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void Vec3Value::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec3Value::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtCore::Vec3ActorProperty(
         "Value", "Value",
         dtCore::Vec3ActorProperty::SetFuncType(this, &Vec3Value::SetValue),
         dtCore::Vec3ActorProperty::GetFuncType(this, &Vec3Value::GetValue),
         "The value.");
      AddProperty(mProperty);

      mInitialProperty = new dtCore::Vec3ActorProperty(
         "Initial Value", "Initial Value",
         dtCore::Vec3ActorProperty::SetFuncType(this, &Vec3Value::SetInitialValue),
         dtCore::Vec3ActorProperty::GetFuncType(this, &Vec3Value::GetInitialValue),
         "The initial value.");
      AddProperty(mInitialProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void Vec3Value::SetValue(const osg::Vec3& value)
   {
      if (mValue != value)
      {
         mValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   const osg::Vec3& Vec3Value::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }

   //////////////////////////////////////////////////////////////////////////
   void Vec3Value::SetInitialValue(const osg::Vec3& value)
   {
      std::string oldValue = mInitialProperty->ToString();
      mInitialValue = value;
      ValueNode::OnInitialValueChanged(oldValue);
   }

   //////////////////////////////////////////////////////////////////////////
   const osg::Vec3& Vec3Value::GetInitialValue()
   {
      return mInitialValue;
   }
}
