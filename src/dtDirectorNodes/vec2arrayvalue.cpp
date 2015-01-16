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
#include <dtDirectorNodes/vec2arrayvalue.h>

#include <dtCore/vectoractorproperties.h>
#include <dtCore/actorproperty.h>
#include <dtCore/arrayactorproperty.h>

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

      mProperty = new dtCore::Vec2ActorProperty(
         "Value", "Value",
         dtCore::Vec2ActorProperty::SetFuncType(this, &Vec2ArrayValue::SetValue),
         dtCore::Vec2ActorProperty::GetFuncType(this, &Vec2ArrayValue::GetValue),
         "", "The value.");

      mInitialProperty = new dtCore::Vec2ActorProperty(
         "Value", "Value",
         dtCore::Vec2ActorProperty::SetFuncType(this, &Vec2ArrayValue::SetInitialValue),
         dtCore::Vec2ActorProperty::GetFuncType(this, &Vec2ArrayValue::GetInitialValue),
         "", "The initial value.");

      mArrayProperty = new dtCore::ArrayActorProperty<osg::Vec2>(
         "ValueList", "Value List", "All values contained in this array.",
         dtCore::ArrayActorProperty<osg::Vec2>::SetIndexFuncType(this, &ArrayValueNode::SetPropertyIndex),
         dtCore::ArrayActorProperty<osg::Vec2>::GetDefaultFuncType(this, &Vec2ArrayValue::GetDefaultValue),
         dtCore::ArrayActorProperty<osg::Vec2>::GetArrayFuncType(this, &Vec2ArrayValue::GetArray),
         dtCore::ArrayActorProperty<osg::Vec2>::SetArrayFuncType(this, &Vec2ArrayValue::SetArray),
         mProperty, "");
      AddProperty(mArrayProperty);

      mInitialArrayProperty = new dtCore::ArrayActorProperty<osg::Vec2>(
         "InitialValueList", "Initial Value List", "All initial values contained in this array.",
         dtCore::ArrayActorProperty<osg::Vec2>::SetIndexFuncType(this, &ArrayValueNode::SetInitialPropertyIndex),
         dtCore::ArrayActorProperty<osg::Vec2>::GetDefaultFuncType(this, &Vec2ArrayValue::GetDefaultValue),
         dtCore::ArrayActorProperty<osg::Vec2>::GetArrayFuncType(this, &Vec2ArrayValue::GetInitialArray),
         dtCore::ArrayActorProperty<osg::Vec2>::SetArrayFuncType(this, &Vec2ArrayValue::SetInitialArray),
         mInitialProperty, "");
      AddProperty(mInitialArrayProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void Vec2ArrayValue::SetValue(const osg::Vec2& value)
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

   //////////////////////////////////////////////////////////////////////////
   void Vec2ArrayValue::SetInitialValue(const osg::Vec2& value)
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
   osg::Vec2 Vec2ArrayValue::GetInitialValue()
   {
      if (mInitialPropertyIndex < (int)mValues.size())
      {
         return mInitialValues[mInitialPropertyIndex];
      }

      return GetDefaultValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<osg::Vec2> Vec2ArrayValue::GetInitialArray()
   {
      return mInitialValues;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec2ArrayValue::SetInitialArray(const std::vector<osg::Vec2>& value)
   {
      int index = mInitialPropertyIndex;
      std::string oldValue = mInitialProperty->ToString();

      mInitialValues = value;
      OnInitialValueChanged(oldValue);
      mInitialPropertyIndex = index;
   }
}
