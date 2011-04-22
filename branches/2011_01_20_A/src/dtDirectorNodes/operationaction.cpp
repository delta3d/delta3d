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

#include <dtDAL/doubleactorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   OperationAction::OperationAction()
      : ActionNode()
      , mValueA(0.0f)
      , mValueB(0.0f)
      , mResult(0.0f)
      , mResultProp(NULL)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   OperationAction::~OperationAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OperationAction::Init(const NodeType& nodeType, DirectorGraph* graph)
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
      ActionNode::BuildPropertyMap();

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

      mResultProp = new dtDAL::DoubleActorProperty(
         "Result", "Result",
         dtDAL::DoubleActorProperty::SetFuncType(this, &OperationAction::SetResult),
         dtDAL::DoubleActorProperty::GetFuncType(this, &OperationAction::GetResult),
         "The resulting output value.");

      AddProperty(leftProp);
      AddProperty(rightProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, leftProp, false, false, false));
      mValues.push_back(ValueLink(this, rightProp, false, false, false));
      mValues.push_back(ValueLink(this, mResultProp.get(), true, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool OperationAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtDAL::DataType& leftType = GetPropertyType("A");
      dtDAL::DataType& rightType = GetPropertyType("B");

      osg::Vec4 left;
      osg::Vec4 right;
      osg::Vec4 result;

      if (leftType == dtDAL::DataType::VEC4F)
      {
         left = GetVec("A");
      }
      else
      {
         left.x() = GetDouble("A");
         left.y() = left.x();
         left.z() = left.x();
         left.w() = left.x();
      }

      if (rightType == dtDAL::DataType::VEC4F)
      {
         right = GetVec("B");
      }
      else
      {
         right.x() = GetDouble("B");
         right.y() = right.x();
         right.z() = right.x();
         right.w() = right.x();
      }

      // Perform math!
      switch (input)
      {
      case INPUT_ADD:
         result = left + right;
         break;
      case INPUT_SUB:
         result = left - right;
         break;
      case INPUT_MUL:
         result.x() = left.x() * right.x();
         result.y() = left.y() * right.y();
         result.z() = left.z() * right.z();
         result.w() = left.w() * right.w();
         break;
      case INPUT_DIV:
         if (right.x() != 0) result = left / right.x();
         break;
      }

      int count = GetPropertyCount("Result");
      for (int index = 0; index < count; index++)
      {
         dtDAL::DataType& resultType = GetPropertyType("Result", index);
         
         if (resultType == dtDAL::DataType::VEC4F)
         {
            SetVec(result, "Result", index);
         }
         else
         {
            SetDouble(result.x(), "Result", index);
         }
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   //////////////////////////////////////////////////////////////////////////
   bool OperationAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (Node::CanConnectValue(link, value))
      {
         dtDAL::DataType& type = value->GetPropertyType();
         switch (type.GetTypeId())
         {
         case dtDAL::DataType::BOOLEAN_ID:
         case dtDAL::DataType::INT_ID:
         case dtDAL::DataType::FLOAT_ID:
         case dtDAL::DataType::DOUBLE_ID:
         case dtDAL::DataType::VEC4F_ID:
         case dtDAL::DataType::UNKNOWN_ID:
            return true;

         default:
            return false;
         }
      }

      return false;
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