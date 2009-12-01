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
   void OperationAction::Init(const NodeType& nodeType)
   {
      ActionNode::Init(nodeType);

      // Create multiple inputs for different operations.
      mInputs.clear();
      //mInputs.push_back(InputLink(this, "Some Really Long Input Name for Testing"));
      mInputs.push_back(InputLink(this, "Add"));
      mInputs.push_back(InputLink(this, "Subtract"));
      mInputs.push_back(InputLink(this, "Multiply"));
      mInputs.push_back(InputLink(this, "Divide"));

      mOutputs.push_back(OutputLink(this, "Some Really Long Output Name for Testing"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OperationAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::IntActorProperty* leftProp = new dtDAL::IntActorProperty(
         "A", "A",
         dtDAL::IntActorProperty::SetFuncType(this, &OperationAction::SetA),
         dtDAL::IntActorProperty::GetFuncType(this, &OperationAction::GetA),
         "The Left value.", "Data");

      dtDAL::IntActorProperty* rightProp = new dtDAL::IntActorProperty(
         "B", "B",
         dtDAL::IntActorProperty::SetFuncType(this, &OperationAction::SetB),
         dtDAL::IntActorProperty::GetFuncType(this, &OperationAction::GetB),
         "The Right value.", "Data");

      dtDAL::IntActorProperty* resultProp = new dtDAL::IntActorProperty(
         "Result", "Result Output Val",
         dtDAL::IntActorProperty::SetFuncType(this, &OperationAction::SetResult),
         dtDAL::IntActorProperty::GetFuncType(this, &OperationAction::GetResult),
         "The Right value.", "Data");

      AddProperty(leftProp);
      AddProperty(rightProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, leftProp, false, false, false));
      mValues.push_back(ValueLink(this, rightProp, false, false, false));
      mValues.push_back(ValueLink(this, resultProp, true, true, false));
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
   void OperationAction::SetA(int value)
   {
      mValueA = value;
   }

   //////////////////////////////////////////////////////////////////////////
   int OperationAction::GetA()
   {
      return mValueA;
   }

   //////////////////////////////////////////////////////////////////////////
   void OperationAction::SetB(int value)
   {
      mValueB = value;
   }

   //////////////////////////////////////////////////////////////////////////
   int OperationAction::GetB()
   {
      return mValueB;
   }

   //////////////////////////////////////////////////////////////////////////
   void OperationAction::SetResult(int value)
   {
      mResult = value;
   }

   //////////////////////////////////////////////////////////////////////////
   int OperationAction::GetResult()
   {
      return mResult;
   }
}

////////////////////////////////////////////////////////////////////////////////
