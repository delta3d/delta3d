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
#include <dtDirector/director.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxy.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   EventNode::EventNode()
       : Node()
       , mInstigator("")
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   EventNode::~EventNode()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void EventNode::Init(const NodeType& nodeType, DirectorGraphData* graph)
   {
      Node::Init(nodeType, graph);

      // Create our default output.
      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Out"));
   }

   //////////////////////////////////////////////////////////////////////////
   bool EventNode::Test(int outputIndex, const dtDAL::ActorProxy* instigator)
   {
      // Can't trigger a disabled event.
      if (!GetEnabled()) return false;

      if (outputIndex < (int)mOutputs.size())
      {
         bool bFound = false;
         bool bValidValue = false;

         // Check the instigator.
         if (UsesInstigator())
         {
            int count = GetPropertyCount("Instigator");
            for (int index = 0; index < count; index++)
            {
               dtCore::UniqueId id = GetActorID("Instigator", index);
               if (id.ToString() != "")
               {
                  // The test is valid if we have valid connections
                  // to the instigator link.
                  bValidValue = true;

                  // Can't do proper matching if we have no instigator.
                  if (!instigator) break;

                  if (instigator->GetId() == id)
                  {
                     bFound = true;
                     break;
                  }
               }
            }
         }

         // Only activate the event if there is no instigator connected
         // to the node that we care to test with, or if the instigating
         // actor matched an instigator connected to this node.
         if (!bValidValue || bFound)
         {
            return true;
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void EventNode::Trigger(int outputIndex)
   {
      if (outputIndex < (int)mOutputs.size())
      {
         mOutputs[outputIndex].Activate();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EventNode::BuildPropertyMap()
   {
      Node::BuildPropertyMap();
      mValues.clear();

      // Create the instigator property.
      if (UsesInstigator())
      {
         dtDAL::ActorIDActorProperty* instigatorProp =
            new dtDAL::ActorIDActorProperty("Instigator", "Instigator", 
            dtDAL::ActorIDActorProperty::SetFuncType(this, &EventNode::SetInstigator),
            dtDAL::ActorIDActorProperty::GetFuncType(this, &EventNode::GetInstigator),
            dtDAL::ActorIDActorProperty::GetMapType(GetDirector(), &Director::GetMap),
            "", "The Instigator that can trigger this event.");
         AddProperty(instigatorProp);

         mValues.push_back(ValueLink(this, instigatorProp, false, true, true));
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void EventNode::Update(float simDelta, float delta)
   {
      Node::Update(simDelta, delta);
   }

   //////////////////////////////////////////////////////////////////////////
   bool EventNode::InputsExposed()
   {
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool EventNode::UsesInstigator()
   {
      return true;
   }
}
