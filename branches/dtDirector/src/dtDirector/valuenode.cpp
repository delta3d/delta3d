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

#include <sstream>
#include <algorithm>

#include <dtDirector/valuenode.h>
#include <dtDirector/valuelink.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ValueNode::ValueNode()
       : Node()
       , mProperty(NULL)
   {
      mName = "Variable";
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ValueNode::~ValueNode()
   {
      Disconnect();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ValueNode::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      Node::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueNode::BuildPropertyMap()
   {
      Node::BuildPropertyMap();

      AddProperty(new dtDAL::StringActorProperty(
         "Name", "Name",
         dtDAL::StringActorProperty::SetFuncType(this, &ValueNode::SetName),
         dtDAL::StringActorProperty::GetFuncType(this, &ValueNode::GetName),
         "The variables name."));
   }

   //////////////////////////////////////////////////////////////////////////
   bool ValueNode::Connect(ValueLink* valueLink)
   {
      if (valueLink) return valueLink->Connect(this);
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNode::Disconnect()
   {
      Node::Disconnect();

      Disconnect(NULL);
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNode::Disconnect(ValueLink* valueLink)
   {
      if (!valueLink)
      {
         while (!mLinks.empty())
         {
            mLinks[0]->Disconnect(this);
         }
      }
      else
      {
         valueLink->Disconnect(this);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNode::OnConnectionChange()
   {

   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* ValueNode::GetProperty()
   {
      return mProperty;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ValueNode::CanBeType(dtDAL::DataType& type)
   {
      if (GetPropertyType() == type)
      {
         return true;
      }
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::DataType& ValueNode::GetPropertyType()
   {
      // By default, use the property of this node.
      if (GetProperty()) return GetProperty()->GetDataType();

      // If there is no property, return an undefined type.
      return dtDAL::DataType::UNKNOWN;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ValueNode::InputsExposed()
   {
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ValueNode::OutputsExposed()
   {
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ValueNode::ValuesExposed()
   {
      return false;
   }
}
