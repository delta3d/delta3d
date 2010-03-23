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
       , mMaxTriggerCount(0)
       , mTriggerCount(0)
   {
      mInstigator = "";
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   EventNode::~EventNode()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void EventNode::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      Node::Init(nodeType, graph);

      // Create our default output.
      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Out"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EventNode::BuildPropertyMap()
   {
      Node::BuildPropertyMap();
      mValues.clear();

      dtDAL::IntActorProperty* triggerCountProp =
         new dtDAL::IntActorProperty("TriggerCount", "Max Trigger Count",
         dtDAL::IntActorProperty::SetFuncType(this, &EventNode::SetTriggerCount),
         dtDAL::IntActorProperty::GetFuncType(this, &EventNode::GetTriggerCount),
         "The maximum number of times this event can be triggered before it is disabled (Zero = no limit).");
      AddProperty(triggerCountProp);

      // Create the instigator property.
      if (UsesInstigator())
      {
         dtDAL::ActorIDActorProperty* instigatorProp =
            new dtDAL::ActorIDActorProperty("Instigator", "Instigator", 
            dtDAL::ActorIDActorProperty::SetFuncType(this, &EventNode::SetInstigator),
            dtDAL::ActorIDActorProperty::GetFuncType(this, &EventNode::GetInstigator),
            "", "The Instigator that can trigger this event.");
         AddProperty(instigatorProp);

         mValues.push_back(ValueLink(this, instigatorProp, false, true, true));
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void EventNode::Trigger(const std::string& outputName, const dtCore::UniqueId* instigator, bool countTrigger, bool immediate)
   {
      // Can't trigger a disabled event.
      if (!GetEnabled()) return;

      if (Test(outputName, instigator))
      {
         // If this event has a trigger count limit,
         // disable the event when that limit is met.
         if (countTrigger && mMaxTriggerCount > 0)
         {
            if (mTriggerCount >= mMaxTriggerCount)
            {
               return;
            }
            mTriggerCount++;
         }

         OutputLink* link = GetOutputLink(outputName);
         if (link) link->Activate();

         // Begin a new thread.
         GetDirector()->BeginThread(this, 0, immediate);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool EventNode::Test(const std::string& outputName, const dtCore::UniqueId* instigator)
   {
      OutputLink* link = GetOutputLink(outputName);
      if (link)
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
                  if (!instigator || instigator->ToString().empty()) break;

                  if (*instigator == id)
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
   bool EventNode::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      return false;
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

   //////////////////////////////////////////////////////////////////////////
   void EventNode::SetInstigator(const dtCore::UniqueId& id)
   {
      mInstigator = id;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId EventNode::GetInstigator()
   {
      return mInstigator;
   }
}
