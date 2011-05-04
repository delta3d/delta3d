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

#include <dtDirector/valuenode.h>
#include <dtDirector/colors.h>

#include <dtDAL/actorproperty.h>
#include <dtDAL/stringactorproperty.h>

#include <dtDirector/valuelink.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ValueNode::ValueNode()
       : Node()
       , mProperty(NULL)
   {
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
   ValueNode* ValueNode::AsValueNode()
   {
      return this;
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
   std::string ValueNode::GetValueLabel()
   {
      std::string label = "";
      if (mProperty) label = "(" + mProperty->GetValueString() + ")";
      return label;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string ValueNode::GetFormattedValue()
   {
      if (mProperty)
      {
         return mProperty->ToString();
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueNode::SetFormattedValue(const std::string& value)
   {
      if (mProperty)
      {
         mProperty->FromString(value);
      }
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
      std::vector<ValueLink*> links = mLinks;
      int count = (int)links.size();
      for (int index = 0; index < count; index++)
      {
         ValueLink* link = links[index];
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

   ////////////////////////////////////////////////////////////////////////////////
   void ValueNode::SetName(const std::string& name)
   {
      Node::SetName(name);

      // Notify any reference nodes that reference this value.
      if (!mLinks.empty())
      {
         std::vector<ValueLink*> links = mLinks;
         int count = (int)links.size();
         for (int index = 0; index < count; index++)
         {
            ValueLink* link = links[index];
            if (link)
            {
               ValueNode* node = link->GetOwner()->AsValueNode();
               if (node)
               {
                  node->SetString(name, "Reference");
               }
            }
         }
      }
   }
}
