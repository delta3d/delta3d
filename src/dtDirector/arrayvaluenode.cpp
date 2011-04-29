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

#include <dtDirector/arrayvaluenode.h>

#include <dtDAL/actorproperty.h>

#include <dtDirector/valuelink.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ArrayValueNode::ArrayValueNode()
       : ValueNode()
       , mPropertyIndex(0)
       , mArrayProperty(NULL)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ArrayValueNode::~ArrayValueNode()
   {
      Disconnect();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ArrayValueNode::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ArrayValueNode::GetValueLabel()
   {
      std::string label = "";
      int count = GetPropertyCount();
      if (count > 0)
      {
         label = "[" + dtUtil::ToString(count) + "]";
      }
      return label;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* ArrayValueNode::GetProperty(int index, ValueNode **outNode)
   {
      mPropertyIndex = index;
      return ValueNode::GetProperty(index, outNode);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int ArrayValueNode::GetPropertyCount(const std::string& name)
   {
      if (mArrayProperty)
      {
         return mArrayProperty->GetArraySize();
      }

      return 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayValueNode::SetPropertyIndex(int index)
   {
      mPropertyIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ArrayActorPropertyBase* ArrayValueNode::GetArrayProperty()
   {
      return mArrayProperty.get();
   }
}
