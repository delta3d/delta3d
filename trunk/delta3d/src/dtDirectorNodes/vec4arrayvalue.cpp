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
#include <dtDirectorNodes/vec4arrayvalue.h>

#include <dtDAL/vectoractorproperties.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/arrayactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   Vec4ArrayValue::Vec4ArrayValue()
       : ArrayValueNode()
   {
      AddAuthor("Eric R. Heine");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   Vec4ArrayValue::~Vec4ArrayValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void Vec4ArrayValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ArrayValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec4ArrayValue::BuildPropertyMap()
   {
      ArrayValueNode::BuildPropertyMap();

      mProperty = new dtDAL::Vec4ActorProperty(
         "Value", "Value",
         dtDAL::Vec4ActorProperty::SetFuncType(this, &Vec4ArrayValue::SetValue),
         dtDAL::Vec4ActorProperty::GetFuncType(this, &Vec4ArrayValue::GetValue),
         "", "The value.");

      mInitialProperty = new dtDAL::Vec4ActorProperty(
         "Value", "Value",
         dtDAL::Vec4ActorProperty::SetFuncType(this, &Vec4ArrayValue::SetInitialValue),
         dtDAL::Vec4ActorProperty::GetFuncType(this, &Vec4ArrayValue::GetInitialValue),
         "", "The initial value.");

      mArrayProperty = new dtDAL::ArrayActorProperty<osg::Vec4>(
         "ValueList", "Value List", "All values contained in this array.",
         dtDAL::ArrayActorProperty<osg::Vec4>::SetIndexFuncType(this, &ArrayValueNode::SetPropertyIndex),
         dtDAL::ArrayActorProperty<osg::Vec4>::GetDefaultFuncType(this, &Vec4ArrayValue::GetDefaultValue),
         dtDAL::ArrayActorProperty<osg::Vec4>::GetArrayFuncType(this, &Vec4ArrayValue::GetArray),
         dtDAL::ArrayActorProperty<osg::Vec4>::SetArrayFuncType(this, &Vec4ArrayValue::SetArray),
         mProperty, "");
      AddProperty(mArrayProperty);

      mInitialArrayProperty = new dtDAL::ArrayActorProperty<osg::Vec4>(
         "InitialValueList", "Initial Value List", "All initial values contained in this array.",
         dtDAL::ArrayActorProperty<osg::Vec4>::SetIndexFuncType(this, &ArrayValueNode::SetInitialPropertyIndex),
         dtDAL::ArrayActorProperty<osg::Vec4>::GetDefaultFuncType(this, &Vec4ArrayValue::GetDefaultValue),
         dtDAL::ArrayActorProperty<osg::Vec4>::GetArrayFuncType(this, &Vec4ArrayValue::GetInitialArray),
         dtDAL::ArrayActorProperty<osg::Vec4>::SetArrayFuncType(this, &Vec4ArrayValue::SetInitialArray),
         mInitialProperty, "");
      AddProperty(mInitialArrayProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void Vec4ArrayValue::SetValue(const osg::Vec4& value)
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
   osg::Vec4 Vec4ArrayValue::GetValue()
   {
      ArrayValueNode::OnValueRetrieved();

      if (mPropertyIndex < (int)mValues.size())
      {
         return mValues[mPropertyIndex];
      }

      return GetDefaultValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec4 Vec4ArrayValue::GetDefaultValue()
   {
      return osg::Vec4();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<osg::Vec4> Vec4ArrayValue::GetArray()
   {
      return mValues;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec4ArrayValue::SetArray(const std::vector<osg::Vec4>& value)
   {
      mValues = value;
   }

   //////////////////////////////////////////////////////////////////////////
   void Vec4ArrayValue::SetInitialValue(const osg::Vec4& value)
   {
      if (mInitialPropertyIndex < (int)mInitialValues.size())
      {
         if (mInitialValues[mInitialPropertyIndex] != value)
         {
            std::string oldValue = mInitialArrayProperty->ToString();

            mInitialValues[mInitialPropertyIndex] = value;

            ArrayValueNode::OnInitialValueChanged(oldValue);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec4 Vec4ArrayValue::GetInitialValue()
   {
      if (mInitialPropertyIndex < (int)mInitialValues.size())
      {
         return mInitialValues[mInitialPropertyIndex];
      }

      return GetDefaultValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<osg::Vec4> Vec4ArrayValue::GetInitialArray()
   {
      return mInitialValues;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec4ArrayValue::SetInitialArray(const std::vector<osg::Vec4>& value)
   {
      if (mInitialValues != value)
      {
         std::string oldValue = mInitialProperty->ToString();
         mInitialValues = value;
         ArrayValueNode::OnInitialValueChanged(oldValue);
      }
   }
}
