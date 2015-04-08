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
#include <dtDirectorNodes/staticmeshvalue.h>

#include <dtCore/resourceactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   StaticMeshValue::StaticMeshValue()
       : ValueNode()
       , mValue(dtCore::ResourceDescriptor::NULL_RESOURCE)
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

      mProperty = new dtCore::ResourceActorProperty(
         dtCore::DataType::STATIC_MESH, "Resource", "Resource",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &StaticMeshValue::SetValue),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &StaticMeshValue::GetValue),
         "The resource.");
      AddProperty(mProperty);

      mInitialProperty = new dtCore::ResourceActorProperty(
         dtCore::DataType::STATIC_MESH, "Initial Resource", "Initial Resource",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &StaticMeshValue::SetInitialValue),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &StaticMeshValue::GetInitialValue),
         "The initial resource.");
      AddProperty(mInitialProperty);
   }

   //////////////////////////////////////////////////////////////////////////
   void StaticMeshValue::SetValue(const dtCore::ResourceDescriptor& value)
   {
      if (mValue != value)
      {
         mValue = value;
         ValueNode::OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor StaticMeshValue::GetValue()
   {
      ValueNode::OnValueRetrieved();
      return mValue;
   }

   //////////////////////////////////////////////////////////////////////////
   void StaticMeshValue::SetInitialValue(const dtCore::ResourceDescriptor& value)
   {
      std::string oldValue = mInitialProperty->ToString();
      mInitialValue = value;
      ValueNode::OnInitialValueChanged(oldValue);
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor StaticMeshValue::GetInitialValue()
   {
      return mInitialValue;
   }
}
