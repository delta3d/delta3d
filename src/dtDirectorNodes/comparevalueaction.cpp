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
#include <dtDirectorNodes/comparevalueaction.h>

#include <dtCore/doubleactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CompareValueAction::CompareValueAction()
      : ActionNode()
      , mValueA(0)
      , mValueB(0)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   CompareValueAction::~CompareValueAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareValueAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "A > B", "Activates when value A is greater than value B."));
      mOutputs.push_back(OutputLink(this, "A = B", "Activates when value A and value B are equal."));
      mOutputs.push_back(OutputLink(this, "A != B", "Activates when value A and value B are not equal."));
      mOutputs.push_back(OutputLink(this, "B > A", "Activates when value A is less than value B."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareValueAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::DoubleActorProperty* leftProp = new dtCore::DoubleActorProperty(
         "A", "A",
         dtCore::DoubleActorProperty::SetFuncType(this, &CompareValueAction::SetA),
         dtCore::DoubleActorProperty::GetFuncType(this, &CompareValueAction::GetA),
         "Value A.");

      dtCore::DoubleActorProperty* rightProp = new dtCore::DoubleActorProperty(
         "B", "B",
         dtCore::DoubleActorProperty::SetFuncType(this, &CompareValueAction::SetB),
         dtCore::DoubleActorProperty::GetFuncType(this, &CompareValueAction::GetB),
         "Value B.");

      AddProperty(leftProp);
      AddProperty(rightProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, leftProp, false, false, false));
      mValues.push_back(ValueLink(this, rightProp, false, false, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool CompareValueAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtCore::DataType& aType = GetPropertyType("A");
      dtCore::DataType& bType = GetPropertyType("B");
      if (aType == dtCore::DataType::STRING ||
         bType == dtCore::DataType::STRING)
      {
         CompareAsStrings();
      }
      else
      {
         CompareAsNumbers();
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool CompareValueAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (ActionNode::CanConnectValue(link, value))
      {
         if (link->GetName() == "A" || link->GetName() == "B")
         {
            if (value->CanBeType(dtCore::DataType::STRING)  ||
                value->CanBeType(dtCore::DataType::INT)     ||
                value->CanBeType(dtCore::DataType::FLOAT)   ||
                value->CanBeType(dtCore::DataType::DOUBLE)  ||
                value->CanBeType(dtCore::DataType::STRING))
            {
               return true;
            }
            return false;
         }
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void CompareValueAction::SetA(double value)
   {
      mValueA = value;
   }

   //////////////////////////////////////////////////////////////////////////
   double CompareValueAction::GetA()
   {
      return mValueA;
   }

   //////////////////////////////////////////////////////////////////////////
   void CompareValueAction::SetB(double value)
   {
      mValueB = value;
   }

   //////////////////////////////////////////////////////////////////////////
   double CompareValueAction::GetB()
   {
      return mValueB;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareValueAction::CompareAsStrings()
   {
      const std::string& valueA = GetString("A");
      const std::string& valueB = GetString("B");

      OutputLink* link = NULL;
      OutputLink* inequalityLink = NULL;

      if (valueA == valueB)
      {
         link = GetOutputLink("A = B");
      }
      else if (valueA < valueB)
      {
         link = GetOutputLink("B > A");
         inequalityLink = GetOutputLink("A != B");
      }
      else if (valueA > valueB)
      {
         link = GetOutputLink("A > B");
         inequalityLink = GetOutputLink("A != B");
      }

      if (link != NULL)
      {
         link->Activate();
      }

      if (inequalityLink != NULL)
      {
         inequalityLink->Activate();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareValueAction::CompareAsNumbers()
   {
      double valueA = GetDouble("A");
      double valueB = GetDouble("B");

      OutputLink* link = NULL;
      OutputLink* inequalityLink = NULL;

      if (valueA == valueB)
      {
         link = GetOutputLink("A = B");
      }
      else if (valueA < valueB)
      {
         link = GetOutputLink("B > A");
         inequalityLink = GetOutputLink("A != B");
      }
      else if (valueA > valueB)
      {
         link = GetOutputLink("A > B");
         inequalityLink = GetOutputLink("A != B");
      }

      if (link)
      {
         link->Activate();
      }

      if (inequalityLink)
      {
         inequalityLink->Activate();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
