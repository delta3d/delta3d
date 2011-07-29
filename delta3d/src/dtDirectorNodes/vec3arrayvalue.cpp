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
#include <dtDirectorNodes/vec3arrayvalue.h>

#include <dtCore/vectoractorproperties.h>
#include <dtCore/actorproperty.h>
#include <dtCore/arrayactorproperty.h>

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

      mProperty = new dtCore::Vec3ActorProperty(
         "Value", "Value",
         dtCore::Vec3ActorProperty::SetFuncType(this, &Vec3ArrayValue::SetValue),
         dtCore::Vec3ActorProperty::GetFuncType(this, &Vec3ArrayValue::GetValue),
         "The value.");

      mInitialProperty = new dtCore::Vec3ActorProperty(
         "Value", "Value",
         dtCore::Vec3ActorProperty::SetFuncType(this, &Vec3ArrayValue::SetInitialValue),
         dtCore::Vec3ActorProperty::GetFuncType(this, &Vec3ArrayValue::GetInitialValue),
         "The value.");

      mArrayProperty = new dtCore::ArrayActorProperty<osg::Vec3>(
         "ValueList", "Value List", "All values contained in this array.",
         dtCore::ArrayActorProperty<osg::Vec3>::SetIndexFuncType(this, &ArrayValueNode::SetPropertyIndex),
         dtCore::ArrayActorProperty<osg::Vec3>::GetDefaultFuncType(this, &Vec3ArrayValue::GetDefaultValue),
         dtCore::ArrayActorProperty<osg::Vec3>::GetArrayFuncType(this, &Vec3ArrayValue::GetArray),
         dtCore::ArrayActorProperty<osg::Vec3>::SetArrayFuncType(this, &Vec3ArrayValue::SetArray),
         mProperty, "");
      AddProperty(mArrayProperty);

      mInitialArrayProperty = new dtCore::ArrayActorProperty<osg::Vec3>(
         "InitialValueList", "Initial Value List", "All initial values contained in this array.",
         dtCore::ArrayActorProperty<osg::Vec3>::SetIndexFuncType(this, &ArrayValueNode::SetInitialPropertyIndex),
         dtCore::ArrayActorProperty<osg::Vec3>::GetDefaultFuncType(this, &Vec3ArrayValue::GetDefaultValue),
         dtCore::ArrayActorProperty<osg::Vec3>::GetArrayFuncType(this, &Vec3ArrayValue::GetInitialArray),
         dtCore::ArrayActorProperty<osg::Vec3>::SetArrayFuncType(this, &Vec3ArrayValue::SetInitialArray),
         mInitialProperty, "");
      AddProperty(mInitialArrayProperty);
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

   //////////////////////////////////////////////////////////////////////////
   void Vec3ArrayValue::SetInitialValue(const osg::Vec3& value)
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
   osg::Vec3 Vec3ArrayValue::GetInitialValue()
   {
      if (mInitialPropertyIndex < (int)mInitialValues.size())
      {
         return mInitialValues[mInitialPropertyIndex];
      }

      return GetDefaultValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<osg::Vec3> Vec3ArrayValue::GetInitialArray()
   {
      return mInitialValues;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec3ArrayValue::SetInitialArray(const std::vector<osg::Vec3>& value)
   {
      int index = mInitialPropertyIndex;
      std::string oldValue = mInitialProperty->ToString();

      mInitialValues = value;
      OnInitialValueChanged(oldValue);
      mInitialPropertyIndex = index;
   }
}
