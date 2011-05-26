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

#include <dtDirectorNodes/staticmeshvalue.h>

#include <dtDAL/resourceactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   StaticMeshValue::StaticMeshValue()
       : ValueNode()
       , mValue(dtDAL::ResourceDescriptor::NULL_RESOURCE)
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   StaticMeshValue::~StaticMeshValue()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void StaticMeshValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StaticMeshValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      mProperty = new dtDAL::ResourceActorProperty(
         dtDAL::DataType::STATIC_MESH, "Resource", "Resource",
         dtDAL::ResourceActorProperty::SetDescFuncType(this, &StaticMeshValue::SetValue),
         dtDAL::ResourceActorProperty::GetDescFuncType(this, &StaticMeshValue::GetValue),
         "The resource.");
      AddProperty(mProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void StaticMeshValue::SetValue(const dtDAL::ResourceDescriptor& value)
   {
      if (mValue != value)
      {
         mValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor StaticMeshValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }
}
