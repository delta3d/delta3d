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
#include <dtDirector/director.h>

#include <dtCore/actorproperty.h>

#include <dtDirector/valuelink.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ArrayValueNode::ArrayValueNode()
       : ValueNode()
       , mPropertyIndex(0)
       , mInitialPropertyIndex(0)
       , mArrayProperty(NULL)
       , mInitialArrayProperty(NULL)
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
      Node::Init(nodeType, graph);

      // Creating default values will erroneously set hasInitialValue to true so we undo it here.
      mHasInitialValue = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayValueNode::OnFinishedLoading()
   {
      // If we do not have an initial value yet, then copy the contents
      // of our current value into the initial.
      if (mInitialArrayProperty.valid())
      {
         if (!mHasInitialValue)
         {
            mInitialArrayProperty->FromString(mArrayProperty->ToString());
         }
         else
         {
            mArrayProperty->FromString(mInitialArrayProperty->ToString());
         }

         if (!GetDirector()->GetNotifier())
         {
            // As soon as we finish loading the script, remove the initial
            // property from the node.
            dtCore::PropertyContainer::RemoveProperty(mInitialArrayProperty->GetName());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ArrayValueNode::IsPropertyDefault(const dtCore::ActorProperty& prop) const
   {
      if (GetDirector()->GetNotifier())
      {
         if (mInitialArrayProperty && &prop == mArrayProperty)
         {
            if (mArrayProperty->ToString() == mInitialArrayProperty->ToString())
            {
               return true;
            }
            return false;
         }
      }

      return Node::IsPropertyDefault(prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayValueNode::ResetProperty(dtCore::ActorProperty& prop)
   {
      if (GetDirector()->GetNotifier())
      {
         if (mInitialArrayProperty && &prop == mArrayProperty)
         {
            mArrayProperty->FromString(mInitialArrayProperty->ToString());
            return;
         }
      }

      Node::ResetProperty(prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ArrayValueNode::ShouldPropertySave(const dtCore::ActorProperty& prop) const
   {
      if (GetDirector()->GetNotifier())
      {
         // Initial properties get saved out regardless of whether
         // they are set to their defaults.
         if (&prop == mInitialArrayProperty)
         {
            return true;
         }

         if (&prop == mArrayProperty)
         {
            return false;
         }
      }

      return Node::ShouldPropertySave(prop);
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

   ////////////////////////////////////////////////////////////////////////////////
   std::string ArrayValueNode::GetFormattedValue()
   {
      if (mArrayProperty.valid())
      {
         return mArrayProperty->ToString();
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayValueNode::SetFormattedValue(const std::string& value)
   {
      if (mArrayProperty.valid())
      {
         mArrayProperty->FromString(value);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string ArrayValueNode::GetFormattedInitialValue()
   {
      if (mInitialArrayProperty)
      {
         return mInitialArrayProperty->ToString();
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayValueNode::SetFormattedInitialValue(const std::string& value)
   {
      if (mInitialArrayProperty)
      {
         mInitialArrayProperty->FromString(value);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProperty* ArrayValueNode::GetProperty(int index, ValueNode **outNode)
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
   void ArrayValueNode::OnInitialValueChanged(const std::string& oldValue)
   {
      mHasInitialValue = true;

      GetDirector()->OnInitialValueChanged(this);

      // If we have not started to run the script or our current value
      // is equal to the initial value before it was just changed,
      // copy the contents of the initial value to the current.
      if (mInitialArrayProperty.valid() &&
         (!GetDirector()->HasStarted() ||
         mArrayProperty->ToString() == oldValue))
      {
         mArrayProperty->FromString(mInitialArrayProperty->ToString());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayValueNode::ExposeInitialValue()
   {
      if (!mInitialArrayProperty)
      {
         return;
      }

      if (dtCore::PropertyContainer::GetProperty(mInitialArrayProperty->GetName()))
      {
         return;
      }

      AddProperty(mInitialArrayProperty);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayValueNode::SetPropertyIndex(int index)
   {
      mPropertyIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayValueNode::SetInitialPropertyIndex(int index)
   {
      mInitialPropertyIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ArrayActorPropertyBase* ArrayValueNode::GetArrayProperty()
   {
      return mArrayProperty.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ArrayActorPropertyBase* ArrayValueNode::GetInitialArrayProperty()
   {
      return mInitialArrayProperty.get();
   }
}
