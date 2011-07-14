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
      mInputs.push_back(InputLink(this, "Add", "Performs an add operation between values A and B."));
      mInputs.push_back(InputLink(this, "Subtract", "Performs a subtraction operation between values A and B."));
      mInputs.push_back(InputLink(this, "Multiply", "Performs a multiply operation between values A and B."));
      mInputs.push_back(InputLink(this, "Divide", "Performs a divide operation between values A and B."));
      mInputs.push_back(InputLink(this, "Mod", "Performs a modulus operation between values A and B."));
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

      if (leftType == dtDAL::DataType::VEC2F)
      {
         osg::Vec2 vec2A = GetVec2("A");
         left.x() = vec2A.x();
         left.y() = vec2A.y();
         left.z() = left.x();
         left.w() = left.x();
      }
      else if (leftType == dtDAL::DataType::VEC3F)
      {
         osg::Vec3 vec3A = GetVec3("A");
         left.x() = vec3A.x();
         left.y() = vec3A.y();
         left.z() = vec3A.z();
         left.w() = left.x();
      }
      else if (leftType == dtDAL::DataType::VEC4F)
      {
         left = GetVec4("A");
      }
      else
      {
         left.x() = GetDouble("A");
         left.y() = left.x();
         left.z() = left.x();
         left.w() = left.x();
      }

      if (rightType == dtDAL::DataType::VEC2F)
      {
         osg::Vec2 vec2A = GetVec2("B");
         right.x() = vec2A.x();
         right.y() = vec2A.y();
         right.z() = right.x();
         right.w() = right.x();
      }
      else if (rightType == dtDAL::DataType::VEC3F)
      {
         osg::Vec3 vec3A = GetVec3("B");
         right.x() = vec3A.x();
         right.y() = vec3A.y();
         right.z() = vec3A.z();
         right.w() = right.x();
      }
      else if (rightType == dtDAL::DataType::VEC4F)
      {
         right = GetVec4("B");
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
      case INPUT_MOD:
         result.x() = Mod(left.x(), right.x());
         result.y() = Mod(left.y(), right.y());
         result.z() = Mod(left.z(), right.z());
         result.w() = Mod(left.w(), right.w());
         break;
      }

      int count = GetPropertyCount("Result");
      for (int index = 0; index < count; index++)
      {
         dtDAL::DataType& resultType = GetPropertyType("Result", index);

         if (resultType == dtDAL::DataType::VEC2F)
         {
            SetVec2(osg::Vec2(result.x(), result.y()), "Result", index);
         }
         else if (resultType == dtDAL::DataType::VEC3F)
         {
            SetVec3(osg::Vec3(result.x(), result.y(), result.z()), "Result", index);
         }
         else if (resultType == dtDAL::DataType::VEC4F)
         {
            SetVec4(result, "Result", index);
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
      if (ActionNode::CanConnectValue(link, value))
      {
         if (link->GetName() == "A" || link->GetName() == "B")
         {
            if (value->CanBeType(dtDAL::DataType::BOOLEAN) ||
                value->CanBeType(dtDAL::DataType::INT)     ||
                value->CanBeType(dtDAL::DataType::FLOAT)   ||
                value->CanBeType(dtDAL::DataType::DOUBLE)  ||
                value->CanBeType(dtDAL::DataType::VEC2F)   ||
                value->CanBeType(dtDAL::DataType::VEC3F)   ||
                value->CanBeType(dtDAL::DataType::VEC4F))
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

   ///////////////////////////////////////////////////////////////////////////////
   int OperationAction::Mod(int left, int right)
   {
      return left % right;
   }
}

////////////////////////////////////////////////////////////////////////////////
