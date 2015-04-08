/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
#include <dtDirectorNodes/switchaction.h>

#include <dtCore/booleanactorproperty.h>
#include <dtCore/intactorproperty.h>
#include <dtUtil/stringutils.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SwitchAction::SwitchAction()
      : ActionNode()
      , mOutputCount(1)
      , mLooping(false)
      , mAutoIncrement(true)
      , mCurrentIndex(1)
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   SwitchAction::~SwitchAction()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void SwitchAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.clear();
      SetOutputCount(1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SwitchAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::IntActorProperty* outputCountProp = new dtCore::IntActorProperty(
         "OutputCount", "Output Count",
         dtCore::IntActorProperty::SetFuncType(this, &SwitchAction::SetOutputCount),
         dtCore::IntActorProperty::GetFuncType(this, &SwitchAction::GetOutputCount),
         "The number of output links on this node.");
      AddProperty(outputCountProp);

      dtCore::IntActorProperty* currentCountProp = new dtCore::IntActorProperty(
         "CurrentIndex", "Current Index",
         dtCore::IntActorProperty::SetFuncType(this, &SwitchAction::SetCurrentIndex),
         dtCore::IntActorProperty::GetFuncType(this, &SwitchAction::GetCurrentIndex),
         "The index of the output to fire next.");

      dtCore::BooleanActorProperty* loopingProp = new dtCore::BooleanActorProperty(
         "Looping", "Looping",
         dtCore::BooleanActorProperty::SetFuncType(this, &SwitchAction::SetLooping),
         dtCore::BooleanActorProperty::GetFuncType(this, &SwitchAction::GetLooping),
         "Whether this switch should loop or not.");
      AddProperty(loopingProp);

      AddProperty(new dtCore::BooleanActorProperty(
         "AutoIncrement", "Auto Increment",
         dtCore::BooleanActorProperty::SetFuncType(this, &SwitchAction::SetAutoIncrement),
         dtCore::BooleanActorProperty::GetFuncType(this, &SwitchAction::GetAutoIncrement),
         "Whether this switch should auto increment the output-index after activation."));

      mValues.push_back(ValueLink(this, outputCountProp, false, false, true, false));
      mValues.push_back(ValueLink(this, currentCountProp, false, false, false));
      mValues.push_back(ValueLink(this, loopingProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SwitchAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      int currentIndex = GetInt("CurrentIndex");
      if (currentIndex <= mOutputCount)
      {
         // Trigger the current output link
         ActivateOutput("Out " + dtUtil::ToString(currentIndex));
         if (GetBoolean("AutoIncrement"))
         {
            currentIndex++;
         }
      }

      if (mLooping && currentIndex > mOutputCount)
      {
         currentIndex = 1;
      }

      SetInt(currentIndex, "CurrentIndex");

      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SwitchAction::SetOutputCount(int value)
   {
      mOutputCount = value;
      int currentOutCount = mOutputs.size();

      // If we have too many, remove some
      while (currentOutCount > mOutputCount)
      {
         mOutputs.pop_back();
         --currentOutCount;
      }

      // If we don't have enough, add some
      while (currentOutCount < mOutputCount)
      {
         ++currentOutCount;
         mOutputs.push_back(OutputLink(this, "Out " + dtUtil::ToString(currentOutCount),
            "Activated when the switch has been executed " + dtUtil::ToString(currentOutCount) + " time(s)."));
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   int SwitchAction::GetOutputCount() const
   {
      return mOutputCount;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SwitchAction::SetCurrentIndex(int value)
   {
      mCurrentIndex = value;
   }

   ///////////////////////////////////////////////////////////////////////////////
   int SwitchAction::GetCurrentIndex() const
   {
      return mCurrentIndex;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SwitchAction::SetLooping(bool value)
   {
      mLooping = value;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool SwitchAction::GetLooping() const
   {
      return mLooping;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SwitchAction::SetAutoIncrement(bool value)
   {
      mAutoIncrement = value;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool SwitchAction::GetAutoIncrement() const
   {
      return mAutoIncrement;
   }

   /////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
