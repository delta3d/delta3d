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

#include <dtDirectorNodes/compareactoraction.h>

#include <dtDAL/actoridactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CompareActorAction::CompareActorAction()
      : ActionNode()
      , mValueA("")
      , mValueB("")
   {
      AddAuthor("Eric R. Heine");
   }

   ////////////////////////////////////////////////////////////////////////////////
   CompareActorAction::~CompareActorAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareActorAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "A == B"));
      mOutputs.push_back(OutputLink(this, "A != B"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareActorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::ActorIDActorProperty* leftProp = new dtDAL::ActorIDActorProperty(
         "A", "A",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &CompareActorAction::SetA),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &CompareActorAction::GetA),
         "Value A.");

      dtDAL::ActorIDActorProperty* rightProp = new dtDAL::ActorIDActorProperty(
         "B", "B",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &CompareActorAction::SetB),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &CompareActorAction::GetB),
         "Value B.");

      AddProperty(leftProp);
      AddProperty(rightProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, leftProp, false, false, false));
      mValues.push_back(ValueLink(this, rightProp, false, false, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool CompareActorAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtCore::UniqueId valueA = GetActorID("A");
      dtCore::UniqueId valueB = GetActorID("B");

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
   bool CompareActorAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (Node::CanConnectValue(link, value))
      {
         if (value->CanBeType(dtDAL::DataType::ACTOR) ||
            value->CanBeType(dtDAL::DataType::STRING))
         {
            return true;
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void CompareActorAction::SetA(const dtCore::UniqueId& value)
   {
      mValueA = value;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId CompareActorAction::GetA()
   {
      return mValueA;
   }

   //////////////////////////////////////////////////////////////////////////
   void CompareActorAction::SetB(const dtCore::UniqueId& value)
   {
      mValueB = value;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId CompareActorAction::GetB()
   {
      return mValueB;
   }
}

////////////////////////////////////////////////////////////////////////////////
