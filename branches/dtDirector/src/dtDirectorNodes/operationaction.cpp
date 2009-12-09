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

#include <dtDirectorNodes/operationaction.h>

#include <dtDirector/director.h>

#include <dtDAL/enginepropertytypes.h>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   OperationAction::OperationAction()
      : ActionNode()
      , mValueA(0.0f)
      , mValueB(0.0f)
      , mResult(0.0f)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   OperationAction::~OperationAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OperationAction::Init(const NodeType& nodeType, DirectorGraphData* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      //mInputs.push_back(InputLink(this, "Some Really Long Input Name for Testing"));
      mInputs.push_back(InputLink(this, "Add"));
      mInputs.push_back(InputLink(this, "Subtract"));
      mInputs.push_back(InputLink(this, "Multiply"));
      mInputs.push_back(InputLink(this, "Divide"));

      //mOutputs.push_back(OutputLink(this, "Some Really Long Output Name for Testing"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OperationAction::BuildPropertyMap()
   {
      // Create our value links.
      dtDAL::DoubleActorProperty* leftProp = new dtDAL::DoubleActorProperty(
         "A", "A",
         dtDAL::DoubleActorProperty::SetFuncType(this, &OperationAction::SetA),
         dtDAL::DoubleActorProperty::GetFuncType(this, &OperationAction::GetA),
         "The Left value.");

      dtDAL::DoubleActorProperty* rightProp = new dtDAL::DoubleActorProperty(
         "B", "B",
         dtDAL::DoubleActorProperty::SetFuncType(this, &OperationAction::SetB),
         dtDAL::DoubleActorProperty::GetFuncType(this, &OperationAction::GetB),
         "The Right value.");

      dtDAL::DoubleActorProperty* resultProp = new dtDAL::DoubleActorProperty(
         "Result", "Result",
         dtDAL::DoubleActorProperty::SetFuncType(this, &OperationAction::SetResult),
         dtDAL::DoubleActorProperty::GetFuncType(this, &OperationAction::GetResult),
         "The Right value.");

      dtDAL::FloatActorProperty* floatProp = new dtDAL::FloatActorProperty(
         "Float", "Float",
         dtDAL::FloatActorProperty::SetFuncType(this, &OperationAction::SetFloatTest),
         dtDAL::FloatActorProperty::GetFuncType(this, &OperationAction::GetFloatTest));

      dtDAL::DoubleActorProperty* doubleProp = new dtDAL::DoubleActorProperty(
         "Double", "Double",
         dtDAL::DoubleActorProperty::SetFuncType(this, &OperationAction::SetDoubleTest),
         dtDAL::DoubleActorProperty::GetFuncType(this, &OperationAction::GetDoubleTest));

      dtDAL::BooleanActorProperty* boolProp = new dtDAL::BooleanActorProperty(
         "Bool", "Bool",
         dtDAL::BooleanActorProperty::SetFuncType(this, &OperationAction::SetBoolTest),
         dtDAL::BooleanActorProperty::GetFuncType(this, &OperationAction::GetBoolTest));

      dtDAL::StringActorProperty* stringProp = new dtDAL::StringActorProperty(
         "String", "String",
         dtDAL::StringActorProperty::SetFuncType(this, &OperationAction::SetStringTest),
         dtDAL::StringActorProperty::GetFuncType(this, &OperationAction::GetStringTest));

      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &OperationAction::SetActorTest),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &OperationAction::GetActorTest),
         dtDAL::ActorIDActorProperty::GetMapType(GetDirector(), &Director::GetMap));

      AddProperty(leftProp);
      AddProperty(rightProp);

      AddProperty(floatProp);
      AddProperty(doubleProp);
      AddProperty(boolProp);
      AddProperty(stringProp);
      AddProperty(actorProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, leftProp, false, false, false));
      mValues.push_back(ValueLink(this, rightProp, false, false, false));
      mValues.push_back(ValueLink(this, resultProp, true, true, false));

      mValues.push_back(ValueLink(this, floatProp, true, true, true));
      mValues.push_back(ValueLink(this, doubleProp, true, true, true));
      mValues.push_back(ValueLink(this, boolProp, true, true, true));
      mValues.push_back(ValueLink(this, stringProp, true, true, true));
      mValues.push_back(ValueLink(this, actorProp, true, true, true));

      ActionNode::BuildPropertyMap();
   }

   //////////////////////////////////////////////////////////////////////////
   void OperationAction::OnInputActivated(int inputIndex)
   {
      ActionNode::OnInputActivated(inputIndex);

      // Perform math!
      double left = GetDouble("A");
      double right = GetDouble("B");
      double result = 0;

      switch (inputIndex)
      {
      case 0: // Add
         result = left + right;
         break;
      case 1: // Subtract
         result = left - right;
         break;
      case 2: // Multiply
         result = left * right;
         break;
      case 3: // Divide
         if (right != 0) result = left / right;
         break;
      }

      SetDouble(result, "Result");
   }

   //////////////////////////////////////////////////////////////////////////
   void OperationAction::SetA(double value)
   {
      mValueA = value;
   }

   //////////////////////////////////////////////////////////////////////////
   double OperationAction::GetA()
   {
      return mValueA;
   }

   //////////////////////////////////////////////////////////////////////////
   void OperationAction::SetB(double value)
   {
      mValueB = value;
   }

   //////////////////////////////////////////////////////////////////////////
   double OperationAction::GetB()
   {
      return mValueB;
   }

   //////////////////////////////////////////////////////////////////////////
   void OperationAction::SetResult(double value)
   {
      mResult = value;
   }

   //////////////////////////////////////////////////////////////////////////
   double OperationAction::GetResult()
   {
      return mResult;
   }
}

////////////////////////////////////////////////////////////////////////////////
