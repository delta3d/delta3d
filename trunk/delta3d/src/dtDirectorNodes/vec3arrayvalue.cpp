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

#include <dtDirectorNodes/vec3arrayvalue.h>

#include <dtDAL/vectoractorproperties.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/arrayactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   Vec3ArrayValue::Vec3ArrayValue()
       : ArrayValueNode()
   {
      AddAuthor("Eric R. Heine");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   Vec3ArrayValue::~Vec3ArrayValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void Vec3ArrayValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ArrayValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec3ArrayValue::BuildPropertyMap()
   {
      ArrayValueNode::BuildPropertyMap();

      mProperty = new dtDAL::Vec3ActorProperty(
         "Value", "Value",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &Vec3ArrayValue::SetValue),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &Vec3ArrayValue::GetValue),
         "The value.");

      mArrayProperty = new dtDAL::ArrayActorProperty<osg::Vec3>(
         "ValueList", "Value List", "All values contained in this array.",
         dtDAL::ArrayActorProperty<osg::Vec3>::SetIndexFuncType(this, &ArrayValueNode::SetPropertyIndex),
         dtDAL::ArrayActorProperty<osg::Vec3>::GetDefaultFuncType(this, &Vec3ArrayValue::GetDefaultValue),
         dtDAL::ArrayActorProperty<osg::Vec3>::GetArrayFuncType(this, &Vec3ArrayValue::GetArray),
         dtDAL::ArrayActorProperty<osg::Vec3>::SetArrayFuncType(this, &Vec3ArrayValue::SetArray),
         mProperty, "");
      AddProperty(mArrayProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void Vec3ArrayValue::SetValue(const osg::Vec3& value)
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
   osg::Vec3 Vec3ArrayValue::GetValue()
   {
      ArrayValueNode::OnValueRetrieved();

      if (mPropertyIndex < (int)mValues.size())
      {
         return mValues[mPropertyIndex];
      }

      return GetDefaultValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Vec3ArrayValue::GetDefaultValue()
   {
      return osg::Vec3();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<osg::Vec3> Vec3ArrayValue::GetArray()
   {
      return mValues;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec3ArrayValue::SetArray(const std::vector<osg::Vec3>& value)
   {
      mValues = value;
   }
}
