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

#include <dtDirector/eventnode.h>
#include <dtDirector/colors.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/actorproperty.h>
#include <dtCore/actorproxy.h>
#include <dtCore/intactorproperty.h>

#include <dtDirector/director.h>
#include <dtDirector/messagegmcomponent.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   EventNode::EventNode()
       : Node()
       , mMaxTriggerCount(0)
       , mTriggerCount(0)
       , mActorFilter(dtCore::UniqueId(""))
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
      mOutputs.push_back(OutputLink(this, "Out", "Activated when this node is activated."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   EventNode* EventNode::AsEventNode()
   {
      return this;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EventNode::BuildPropertyMap()
   {
      Node::BuildPropertyMap();

      dtCore::IntActorProperty* triggerCountProp =
         new dtCore::IntActorProperty("TriggerCount", "Max Trigger Count",
         dtCore::IntActorProperty::SetFuncType(this, &EventNode::SetTriggerCount),
         dtCore::IntActorProperty::GetFuncType(this, &EventNode::GetTriggerCount),
         "The maximum number of times this event can be triggered before it is disabled (Zero = no limit).");
      AddProperty(triggerCountProp);
      mValues.push_back(ValueLink(this, triggerCountProp, false, false, true, false));

      mInstigatorProp = new dtCore::ActorIDActorProperty(
         "Instigator", "Instigator",
         dtCore::ActorIDActorProperty::SetFuncType(this, &EventNode::SetInstigator),
         dtCore::ActorIDActorProperty::GetFuncType(this, &EventNode::GetInstigator),
         "", "The Instigator that can trigger this event.");
      mValues.push_back(ValueLink(this, mInstigatorProp.get(), true, true, true, false));

      // Create the actor filters property.
      if (UsesActorFilters())
      {
         dtCore::ActorIDActorProperty* filterProp = new dtCore::ActorIDActorProperty(
            "ActorFilters", "Actor Filters",
            dtCore::ActorIDActorProperty::SetFuncType(this, &EventNode::SetActorFilters),
            dtCore::ActorIDActorProperty::GetFuncType(this, &EventNode::GetActorFilters),
            "", "The actors that need to cause this event to fire the output.");

         mValues.push_back(ValueLink(this, filterProp, false, true, true));
      }
   }

   //////////////////////////////////////////////////////////////////////////
   int EventNode::Trigger(const std::string& outputName, const dtCore::UniqueId* instigator, bool countTrigger)
   {
      // Can't trigger a disabled event.
      if (!IsEnabled()) return -1;

      if (instigator != NULL)
      {
         SetActorID(*instigator, "Instigator");
      }
      if (Test(outputName, instigator, countTrigger))
      {
         ActivateOutput(outputName);

         // Begin a new thread.
         return GetDirector()->BeginThread(this, 0);
      }

      return -1;
   }

   //////////////////////////////////////////////////////////////////////////
   bool EventNode::Test(const std::string& outputName, const dtCore::UniqueId* instigator, bool countTrigger)
   {
      // If this node is part of a cached instance, then we don't want to trigger.
      if (GetDirector() && GetDirector()->IsCachedInstance())
      {
         return false;
      }

      // If this event has a trigger count limit,
      // disable the event when that limit is met.
      if (mMaxTriggerCount > 0)
      {
         if (mTriggerCount >= mMaxTriggerCount)
         {
            return false;
         }

         if (countTrigger)
         {
            mTriggerCount++;
         }
      }

      bool success = false;
      OutputLink* link = GetOutputLink(outputName);
      if (link)
      {
         // Check the instigator.
         if (UsesActorFilters())
         {
            int count = GetPropertyCount("ActorFilters");

            if (count != 0)
            {
               // If our count is 0 then we are connected to an empty array,
               // If our count is greater than 0 then we are connected to multiple
               // values.  Either case, the instigator will be valid.
               if (GetValueLink("ActorFilters")->GetLinks().size() == 0)
               {
                  success = true;
               }
               if (instigator != NULL && !instigator->ToString().empty())
               {
                  for (int index = 0; index < count; index++)
                  {
                     dtCore::UniqueId id = GetActorID("ActorFilters", index);
                     if (id.ToString() != "")
                     {
                        if (*instigator == id)
                        {
                           success = true;
                           break;
                        }
                     }
                  }
               }
            }
            else
            {
               LOG_ERROR("Node: " + GetName() + " has UsesActorFilters set but the link does not exist.");
            }
         }
         else
         {
            success = true;
         }
      }

      return success;
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
   bool EventNode::UsesActorFilters()
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

   //////////////////////////////////////////////////////////////////////////
   void EventNode::SetActorFilters(const dtCore::UniqueId& id)
   {
      mActorFilter = id;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId EventNode::GetActorFilters()
   {
      return mActorFilter;
   }
}
