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

#include <dtDirectorNodes/vecvalue.h>
#include <dtDirector/colors.h>

#include <dtDAL/vectoractorproperties.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   VecValue::VecValue()
       : ValueNode()
   {
      AddAuthor("Jeff P. Houde");
      //SetColorRGB(Colors::RED); 
  }

   ///////////////////////////////////////////////////////////////////////////////////////
   VecValue::~VecValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void VecValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void VecValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtDAL::Vec4ActorProperty(
         "Value", "Value",
         dtDAL::Vec4ActorProperty::SetFuncType(this, &VecValue::SetValue),
         dtDAL::Vec4ActorProperty::GetFuncType(this, &VecValue::GetValue),
         "The value.");
      AddProperty(mProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void VecValue::SetValue(const osg::Vec4& value)
   {
      mValue = value;
      ValueNode::OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   const osg::Vec4& VecValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }
}
