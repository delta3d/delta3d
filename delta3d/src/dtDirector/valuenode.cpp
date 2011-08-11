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

#include <dtCore/actorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/stringactorproperty.h>

#include <dtDirector/valuelink.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ValueNode::ValueNode()
       : Node()
       , mProperty(NULL)
       , mInitialProperty(NULL)
       , mHasInitialValue(false)
       , mIsGlobal(false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ValueNode::~ValueNode()
   {
      if (mIsGlobal)
      {
         GetDirector()->UnRegisterGlobalValue(this);
      }
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
            dtCore::PropertyContainer::RemoveProperty(mInitialProperty->GetName());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueNode::BuildPropertyMap()
   {
      Node::BuildPropertyMap();

      AddProperty(new dtCore::StringActorProperty(
         "Name", "Name",
         dtCore::StringActorProperty::SetFuncType(this, &ValueNode::SetName),
         dtCore::StringActorProperty::GetFuncType(this, &ValueNode::GetName),
         "The variables name."));

      AddProperty(new dtCore::BooleanActorProperty(
         "Global", "Global",
         dtCore::BooleanActorProperty::SetFuncType(this, &ValueNode::SetGlobal),
         dtCore::BooleanActorProperty::GetFuncType(this, &ValueNode::GetGlobal),
         "True if this value node should share it's value with other value \
         nodes from all scripts of the same name and type."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ValueNode::IsPropertyDefault(const dtCore::ActorProperty& prop) const
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
   void ValueNode::ResetProperty(dtCore::ActorProperty& prop)
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
   bool ValueNode::ShouldPropertySave(const dtCore::ActorProperty& prop) const
   {
      if (GetDirector()->GetNotifier())
      {
         // Initial properties get saved out regardless of whether
         // they are set to their defaults.
         if (&prop == mInitialProperty)
         {
            return true;
         }

         if (&prop == mProperty)
         {
            return false;
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
   void ValueNode::SetEnabled(bool enabled)
   {
      Node::SetEnabled(enabled);
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
   std::string ValueNode::GetFormattedInitialValue()
   {
      if (mInitialProperty)
      {
         return mInitialProperty->ToString();
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueNode::SetFormattedInitialValue(const std::string& value)
   {
      if (mInitialProperty)
      {
         mInitialProperty->FromString(value);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ActorProperty* ValueNode::GetProperty(ValueNode** outNode)
   {
      return GetProperty(0, outNode);
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProperty* ValueNode::GetProperty(int index, ValueNode** outNode)
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
   bool ValueNode::CanBeType(dtCore::DataType& type)
   {
      if (GetPropertyType() == type)
      {
         return true;
      }
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::DataType& ValueNode::GetPropertyType()
   {
      // By default, use the property of this node.
      if (GetProperty()) return GetProperty()->GetDataType();

      // If there is no property, return an undefined type.
      return dtCore::DataType::UNKNOWN;
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

      GetDirector()->OnValueChanged(this);
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNode::OnInitialValueChanged(const std::string& oldValue)
   {
      mHasInitialValue = true;

      GetDirector()->OnInitialValueChanged(this);

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

      if (dtCore::PropertyContainer::GetProperty(mInitialProperty->GetName()))
      {
         return;
      }

      AddProperty(mInitialProperty);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueNode::SetName(const std::string& name)
   {
      if (mIsGlobal)
      {
         GetDirector()->UnRegisterGlobalValue(this);
      }

      Node::SetName(name);

      if (mIsGlobal)
      {
         GetDirector()->RegisterGlobalValue(this);
      }

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

   ////////////////////////////////////////////////////////////////////////////////
   void ValueNode::SetGlobal(bool value)
   {
      if (mIsGlobal)
      {
         GetDirector()->UnRegisterGlobalValue(this);
      }

      mIsGlobal = value;

      if (mIsGlobal)
      {
         GetDirector()->RegisterGlobalValue(this);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ValueNode::GetGlobal() const
   {
      return mIsGlobal;
   }
}
