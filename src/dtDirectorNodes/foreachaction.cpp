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
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/foreachaction.h>

#include <dtCore/actorfactory.h>

#include <dtDirector/director.h>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   ForEachAction::ForEachAction()
      : ActionNode()
      , mCurrentItem(0)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   ForEachAction::~ForEachAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ForEachAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Each Item", "Activates once for each item in the given array."));
      mOutputs.push_back(OutputLink(this, "Finished",  "Activates after all items have been iterated through."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ForEachAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      mItemArrayProp = new dtCore::IntActorProperty(
         "Item List", "Item List",
         dtCore::IntActorProperty::SetFuncType(this, &ForEachAction::SetItemArray),
         dtCore::IntActorProperty::GetFuncType(this, &ForEachAction::GetItemArray),
         "The list of items to iterate through.");

      mItemProp = new dtCore::IntActorProperty(
         "Current Item", "Current Item",
         dtCore::IntActorProperty::SetFuncType(this, &ForEachAction::SetCurrentItem),
         dtCore::IntActorProperty::GetFuncType(this, &ForEachAction::GetCurrentItem),
         "", "The currently iterated item.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, mItemArrayProp, false, true, false, true));
      mValues.push_back(ValueLink(this, mItemProp, true, true, false, true));
   }

   //////////////////////////////////////////////////////////////////////////
   bool ForEachAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      // On the first update, find all the actors we want to iterate through.
      if (input == 0)
      {
         mCurrentItem = 0;

         // Iterate through each found actor.
         GetDirector()->PushStack(this, 11);

         int count = GetPropertyCount("Item List");
         for (int index = 0; index < count; ++index)
         {
            // We push a stack so that we can execute the entire "For Each"
            // output chain before we return back to this node.
            GetDirector()->PushStack(this, 10);
         }

         return false;
      }
      // If the input is 10, it means we are iterating through an actor.
      else if (input == 10)
      {
         int count = GetPropertyCount("Item List");
         if (mCurrentItem < count)
         {
            std::string itemValue = GetString("Item List", mCurrentItem);
            SetString(itemValue, "Current Item");

            ActivateOutput("Each Item");

            mCurrentItem++;
         }
         return false;
      }
      else if (input == 11)
      {
         ActivateOutput("Finished");
         return false;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void ForEachAction::SetItemArray(int value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   int ForEachAction::GetItemArray() const
   {
      return 0;
   }

   //////////////////////////////////////////////////////////////////////////
   void ForEachAction::SetCurrentItem(int value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   int ForEachAction::GetCurrentItem() const
   {
      return 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
