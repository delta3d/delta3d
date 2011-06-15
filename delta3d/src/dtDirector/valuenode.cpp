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
#include <dtDirector/director.h>

#include <dtDAL/actorproperty.h>
#include <dtDAL/stringactorproperty.h>

#include <dtDirector/valuelink.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ValueNode::ValueNode()
       : Node()
       , mProperty(NULL)
       , mInitialProperty(NULL)
       , mHasInitialValue(false)
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
   void ValueNode::OnFinishedLoading()
   {
      // If we do not have an initial value yet, then copy the contents
      // of our current value into the initial.
      if (mInitialProperty.valid())
      {
         if (!mHasInitialValue)
         {
            mInitialProperty->FromString(mProperty->ToString());
         }

         if (!GetDirector()->GetNotifier())
         {
            // As soon as we finish loading the script, remove the initial
            // property from the node.
            dtDAL::PropertyContainer::RemoveProperty(mInitialProperty->GetName());
         }
      }
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

   ////////////////////////////////////////////////////////////////////////////////
   bool ValueNode::IsPropertyDefault(const dtDAL::ActorProperty& prop) const
   {
      if (GetDirector()->GetNotifier())
      {
         if (mInitialProperty && &prop == mProperty)
         {
            if (mProperty->ToString() == mInitialProperty->ToString())
            {
               return true;
            }
            return false;
         }
      }

      return Node::IsPropertyDefault(prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueNode::ResetProperty(dtDAL::ActorProperty& prop)
   {
      if (GetDirector()->GetNotifier())
      {
         if (mInitialProperty && &prop == mProperty)
         {
            mProperty->FromString(mInitialProperty->ToString());
            return;
         }
      }

      Node::ResetProperty(prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ValueNode::ShouldPropertySave(const dtDAL::ActorProperty& prop) const
   {
      if (GetDirector()->GetNotifier())
      {
         // Initial properties get saved out regardless of whether
         // they are set to their defaults.
         if (&prop == mInitialProperty)
         {
            return true;
         }
      }

      return Node::ShouldPropertySave(prop);
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

      if (GetDirector()->GetNotifier())
      {
         GetDirector()->GetNotifier()->OnValueChanged(this);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueNode::OnInitialValueChanged(const std::string& oldValue)
   {
      mHasInitialValue = true;

      // If we have not started to run the script or our current value
      // is equal to the initial value before it was just changed,
      // copy the contents of the initial value to the current.
      if (mProperty.valid() &&
         (!GetDirector()->HasStarted() ||
         mProperty->ToString() == oldValue))
      {
         mProperty->FromString(mInitialProperty->ToString());
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
   void ValueNode::ExposeInitialValue()
   {
      if (!mInitialProperty)
      {
         return;
      }

      if (dtDAL::PropertyContainer::GetProperty(mInitialProperty->GetName()))
      {
         return;
      }

      AddProperty(mInitialProperty);
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
