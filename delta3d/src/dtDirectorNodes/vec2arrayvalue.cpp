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

#include <dtDirectorNodes/vec2arrayvalue.h>

#include <dtDAL/vectoractorproperties.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/arrayactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   Vec2ArrayValue::Vec2ArrayValue()
       : ArrayValueNode()
   {
      AddAuthor("Eric R. Heine");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   Vec2ArrayValue::~Vec2ArrayValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void Vec2ArrayValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ArrayValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec2ArrayValue::BuildPropertyMap()
   {
      ArrayValueNode::BuildPropertyMap();

      mProperty = new dtDAL::Vec2ActorProperty(
         "Value", "Value",
         dtDAL::Vec2ActorProperty::SetFuncType(this, &Vec2ArrayValue::SetValue),
         dtDAL::Vec2ActorProperty::GetFuncType(this, &Vec2ArrayValue::GetValue),
         "", "The value.");

      mArrayProperty = new dtDAL::ArrayActorProperty<osg::Vec2>(
         "ValueList", "Value List", "All values contained in this array.",
         dtDAL::ArrayActorProperty<osg::Vec2>::SetIndexFuncType(this, &ArrayValueNode::SetPropertyIndex),
         dtDAL::ArrayActorProperty<osg::Vec2>::GetDefaultFuncType(this, &Vec2ArrayValue::GetDefaultValue),
         dtDAL::ArrayActorProperty<osg::Vec2>::GetArrayFuncType(this, &Vec2ArrayValue::GetArray),
         dtDAL::ArrayActorProperty<osg::Vec2>::SetArrayFuncType(this, &Vec2ArrayValue::SetArray),
         mProperty, "");
      AddProperty(mArrayProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void Vec2ArrayValue::SetValue(const osg::Vec2& value)
   {
      if (mPropertyIndex < (int)mValues.size())
      {
         mValues[mPropertyIndex] = value;
      }

      ArrayValueNode::OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec2 Vec2ArrayValue::GetValue()
   {
      ArrayValueNode::OnValueRetrieved();

      if (mPropertyIndex < (int)mValues.size())
      {
         return mValues[mPropertyIndex];
      }

      return GetDefaultValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2 Vec2ArrayValue::GetDefaultValue()
   {
      return osg::Vec2();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<osg::Vec2> Vec2ArrayValue::GetArray()
   {
      return mValues;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec2ArrayValue::SetArray(const std::vector<osg::Vec2>& value)
   {
      mValues = value;
   }
}
