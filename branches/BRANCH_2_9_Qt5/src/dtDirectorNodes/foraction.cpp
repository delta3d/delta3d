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
 * Author: Eric R. Heine
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/foraction.h>

#include <dtCore/actorfactory.h>

#include <dtDirector/director.h>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   ForAction::ForAction()
      : ActionNode()
      , mStartIndex(0)
      , mEndIndex(0)
      , mCurrentIndex(0)
   {
      AddAuthor("Eric R. Heine");
   }

   ////////////////////////////////////////////////////////////////////////////////
   ForAction::~ForAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ForAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Out", "Activates once for each index of the loop."));
      mOutputs.push_back(OutputLink(this, "Finished",  "Activates after the loop is complete."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ForAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::IntActorProperty* startProp = new dtCore::IntActorProperty(
         "Start Index", "Start Index",
         dtCore::IntActorProperty::SetFuncType(this, &ForAction::SetStartIndex),
         dtCore::IntActorProperty::GetFuncType(this, &ForAction::GetStartIndex),
         "The starting index of the loop.");
      AddProperty(startProp);

      dtCore::IntActorProperty* endProp = new dtCore::IntActorProperty(
         "End Index", "End Index",
         dtCore::IntActorProperty::SetFuncType(this, &ForAction::SetEndIndex),
         dtCore::IntActorProperty::GetFuncType(this, &ForAction::GetEndIndex),
         "The ending index of the loop.");
      AddProperty(endProp);

      mIndexProp = new dtCore::IntActorProperty(
         "Current Index", "Current Index",
         dtCore::IntActorProperty::SetFuncType(this, &ForAction::SetCurrentIndex),
         dtCore::IntActorProperty::GetFuncType(this, &ForAction::GetCurrentIndex),
         "The currently iterated index.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, startProp));
      mValues.push_back(ValueLink(this, endProp));
      mValues.push_back(ValueLink(this, mIndexProp, true));
   }

   //////////////////////////////////////////////////////////////////////////
   bool ForAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      // On the first update, find all the actors we want to iterate through.
      if (input == 0)
      {
         // Iterate through each found actor.
         GetDirector()->PushStack(this, 11);

         mCurrentIndex = GetInt("Start Index");
         int endIndex = GetInt("End Index");
         for (int index = mCurrentIndex; index <= endIndex; ++index)
         {
            // We push a stack so that we can execute the entire "For"
            // output chain before we return back to this node.
            GetDirector()->PushStack(this, 10);
         }

         return false;
      }
      // If the input is 10, it means we are iterating through an actor.
      else if (input == 10)
      {
         int endIndex = GetInt("End Index");
         if (mCurrentIndex <= endIndex)
         {
            SetInt(mCurrentIndex, "Current Index");

            ActivateOutput("Out");

            ++mCurrentIndex;
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
   void ForAction::SetStartIndex(int value)
   {
      mStartIndex = value;
   }

   //////////////////////////////////////////////////////////////////////////
   int ForAction::GetStartIndex() const
   {
      return mStartIndex;
   }

   //////////////////////////////////////////////////////////////////////////
   void ForAction::SetEndIndex(int value)
   {
      mEndIndex = value;
   }

   //////////////////////////////////////////////////////////////////////////
   int ForAction::GetEndIndex() const
   {
      return mEndIndex;
   }

   //////////////////////////////////////////////////////////////////////////
   void ForAction::SetCurrentIndex(int value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   int ForAction::GetCurrentIndex() const
   {
      return 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
