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
#include <dtDirectorNodes/compareboolaction.h>

#include <dtCore/booleanactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CompareBoolAction::CompareBoolAction()
      : ActionNode()
      , mValueA(0)
      , mValueB(0)
   {
      AddAuthor("Eric R. Heine");
   }

   ////////////////////////////////////////////////////////////////////////////////
   CompareBoolAction::~CompareBoolAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareBoolAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "A == B", "Activates when value A and value B are equal."));
      mOutputs.push_back(OutputLink(this, "A != B", "Activates when value A and value B are not equal."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareBoolAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::BooleanActorProperty* leftProp = new dtCore::BooleanActorProperty(
         "A", "A",
         dtCore::BooleanActorProperty::SetFuncType(this, &CompareBoolAction::SetA),
         dtCore::BooleanActorProperty::GetFuncType(this, &CompareBoolAction::GetA),
         "Value A.");

      dtCore::BooleanActorProperty* rightProp = new dtCore::BooleanActorProperty(
         "B", "B",
         dtCore::BooleanActorProperty::SetFuncType(this, &CompareBoolAction::SetB),
         dtCore::BooleanActorProperty::GetFuncType(this, &CompareBoolAction::GetB),
         "Value B.");

      AddProperty(leftProp);
      AddProperty(rightProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, leftProp));
      mValues.push_back(ValueLink(this, rightProp));
   }

   //////////////////////////////////////////////////////////////////////////
   bool CompareBoolAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      bool valueA = GetBoolean("A");
      bool valueB = GetBoolean("B");

      OutputLink* link = NULL;
      if (valueA == valueB)
      {
         link = GetOutputLink("A == B");
      }
      else
      {
         link = GetOutputLink("A != B");
      }

      if (link) link->Activate();
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void CompareBoolAction::SetA(bool value)
   {
      mValueA = value;
   }

   //////////////////////////////////////////////////////////////////////////
   bool CompareBoolAction::GetA() const
   {
      return mValueA;
   }

   //////////////////////////////////////////////////////////////////////////
   void CompareBoolAction::SetB(bool value)
   {
      mValueB = value;
   }

   //////////////////////////////////////////////////////////////////////////
   bool CompareBoolAction::GetB() const
   {
      return mValueB;
   }
}

////////////////////////////////////////////////////////////////////////////////
