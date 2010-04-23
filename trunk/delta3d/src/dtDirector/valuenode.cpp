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

      SetValueName(mName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueNode::BuildPropertyMap()
   {
      Node::BuildPropertyMap();

      AddProperty(new dtDAL::StringActorProperty(
         "Name", "Name",
         dtDAL::StringActorProperty::SetFuncType(this, &ValueNode::SetValueName),
         dtDAL::StringActorProperty::GetFuncType(this, &ValueNode::GetValueName),
         "The variables name."));
   }

   //////////////////////////////////////////////////////////////////////////
   bool ValueNode::Connect(ValueLink* valueLink)
   {
      if (valueLink) return valueLink->Connect(this);
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ValueNode::Disconnect()
   {
      bool result = false;
      result |= Node::Disconnect();

      result |= Disconnect(NULL);

      return result;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ValueNode::Disconnect(ValueLink* valueLink)
   {
      if (!valueLink)
      {
         bool result = false;
         while (!mLinks.empty())
         {
            result |= mLinks[0]->Disconnect(this);
         }

         return result;
      }
      else
      {
         return valueLink->Disconnect(this);
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNode::OnConnectionChange()
   {
      // Notify all new connections that this value has changed, just in case
      // they need to initialize on this node's current value.
      OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNode::SetValueName(const std::string& name)
   {
      mName = name;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& ValueNode::GetValueName()
   {
      return mName;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& ValueNode::GetName()
   {
      //mLabel = mName + GetValueLabel();
      return mName;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ValueNode::GetValueLabel()
   {
      std::string label = "";
      if (mProperty) label = "(" + mProperty->GetValueString() + ")";
      return label;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* ValueNode::GetProperty(ValueNode** outNode)
   {
      return GetProperty(0, outNode);
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* ValueNode::GetProperty(int index, ValueNode** outNode)
   {
      if (outNode) *outNode = this;

      return mProperty;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int ValueNode::GetPropertyCount(const std::string& name)
   {
      return 1;
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
   void ValueNode::OnValueChanged()
   {
      int count = (int)mLinks.size();
      for (int index = 0; index < count; index++)
      {
         ValueLink* link = mLinks[index];
         if (link) link->GetOwner()->OnLinkValueChanged(link->GetName());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNode::OnValueRetrieved()
   {
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
