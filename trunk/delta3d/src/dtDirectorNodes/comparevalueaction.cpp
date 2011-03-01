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

#include <dtDirectorNodes/comparevalueaction.h>

#include <dtDAL/doubleactorproperty.h>

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
      mOutputs.push_back(OutputLink(this, "A > B"));
      mOutputs.push_back(OutputLink(this, "A = B"));
      mOutputs.push_back(OutputLink(this, "B > A"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareValueAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::DoubleActorProperty* leftProp = new dtDAL::DoubleActorProperty(
         "A", "A",
         dtDAL::DoubleActorProperty::SetFuncType(this, &CompareValueAction::SetA),
         dtDAL::DoubleActorProperty::GetFuncType(this, &CompareValueAction::GetA),
         "Value A.");

      dtDAL::DoubleActorProperty* rightProp = new dtDAL::DoubleActorProperty(
         "B", "B",
         dtDAL::DoubleActorProperty::SetFuncType(this, &CompareValueAction::SetB),
         dtDAL::DoubleActorProperty::GetFuncType(this, &CompareValueAction::GetB),
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
      ValueNode* nodeA = NULL;
      ValueNode* nodeB = NULL;

      GetProperty("A", 0, &nodeA);
      GetProperty("B", 0, &nodeB);

      if (nodeA->GetPropertyType() == dtDAL::DataType::STRING ||
          nodeB->GetPropertyType() == dtDAL::DataType::STRING)
      {
         CompareAsStrings(*nodeA, *nodeB);
      }
      else
      {
         CompareAsNumbers(*nodeA, *nodeB);
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool CompareValueAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (Node::CanConnectValue(link, value))
      {
         dtDAL::DataType& type = value->GetPropertyType();
         switch (type.GetTypeId())
         {
            case dtDAL::DataType::INT_ID:
            case dtDAL::DataType::FLOAT_ID:
            case dtDAL::DataType::DOUBLE_ID:
            case dtDAL::DataType::STRING_ID:
               return true;
            default:
               return false;
         }
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
   void CompareValueAction::CompareAsStrings(ValueNode& nodeA, ValueNode& nodeB)
   {
      std::string valueA = GetString("A");
      std::string valueB = GetString("B");

      OutputLink* link = NULL;
      if (valueA == valueB)
      {
         link = GetOutputLink("A = B");
      }
      else if (valueA < valueB)
      {
         link = GetOutputLink("B > A");
      }
      else if (valueA > valueB)
      {
         link = GetOutputLink("A > B");
      }

      if (link)
      {
         link->Activate();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareValueAction::CompareAsNumbers(ValueNode& nodeA, ValueNode& nodeB)
   {
      double valueA = GetDouble("A");
      double valueB = GetDouble("B");

      OutputLink* link = NULL;
      if (valueA == valueB)
      {
         link = GetOutputLink("A = B");
      }
      else if (valueA < valueB)
      {
         link = GetOutputLink("B > A");
      }
      else if (valueA > valueB)
      {
         link = GetOutputLink("A > B");
      }

      if (link)
      {
         link->Activate();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
