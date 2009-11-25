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

#include <dtDirector/eventnode.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxy.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   EventNode::EventNode()
       : Node()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   EventNode::~EventNode()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void EventNode::Init(const NodeType& nodeType)
   {
      Node::Init(nodeType);

      // Create our default output.
      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Out"));
   }

   //////////////////////////////////////////////////////////////////////////
   void EventNode::Trigger(int outputIndex, const dtDAL::ActorProxy* instigator)
   {
      // Can't trigger a disabled event.
      if (GetDisabled()) return;

      if (outputIndex < (int)mOutputs.size())
      {
         // TODO: Check the instigator.

         mOutputs[outputIndex].Activate();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EventNode::BuildPropertyMap()
   {
      Node::BuildPropertyMap();
   }

   //////////////////////////////////////////////////////////////////////////
   void EventNode::Update(float simDelta, float delta)
   {
      Node::Update(simDelta, delta);
   }

   //////////////////////////////////////////////////////////////////////////
   int EventNode::GetPropertyCount(const std::string& name)
   {
      // First iterate through all value links to see if this property
      // is redirected.
      for (int valueIndex = 0; valueIndex < (int)mValues.size(); valueIndex++)
      {
         dtDAL::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
         if (prop && prop->GetName() == name)
         {
            return mValues[valueIndex].GetPropertyCount();
         }
      }

      // Did not find any overrides, so return the default.
      return Node::GetPropertyCount(name);
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* EventNode::GetProperty(const std::string& name, int index)
   {
      // First iterate through all value links to see if this property
      // is redirected.
      for (int valueIndex = 0; valueIndex < (int)mValues.size(); valueIndex++)
      {
         dtDAL::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
         if (prop && prop->GetName() == name)
         {
            return mValues[valueIndex].GetProperty(index);
         }
      }

      // Did not find any overrides, so return the default.
      return Node::GetProperty(name, index);
   }
}
