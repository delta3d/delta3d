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

#include <dtDirectorNodes/arithmeticmutator.h>

#include <dtDirector/director.h>

#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/intactorproperty.h>
#include <dtDAL/floatactorproperty.h>
#include <dtDAL/doubleactorproperty.h>
#include <dtDAL/vectoractorproperties.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   ArithmeticMutator::ArithmeticMutator()
      : MutatorNode()
      , mValueA(0.0f)
      , mValueB(0.0f)
      , mResultBool(false)
      , mResultInt(0)
      , mResultFloat(0.0f)
      , mResultDouble(0.0)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   ArithmeticMutator::~ArithmeticMutator()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArithmeticMutator::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      MutatorNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArithmeticMutator::BuildPropertyMap()
   {
      MutatorNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::DoubleActorProperty* leftProp = new dtDAL::DoubleActorProperty(
         "A", "A",
         dtDAL::DoubleActorProperty::SetFuncType(this, &ArithmeticMutator::SetA),
         dtDAL::DoubleActorProperty::GetFuncType(this, &ArithmeticMutator::GetA),
         "The Left value.");

      dtDAL::DoubleActorProperty* rightProp = new dtDAL::DoubleActorProperty(
         "B", "B",
         dtDAL::DoubleActorProperty::SetFuncType(this, &ArithmeticMutator::SetB),
         dtDAL::DoubleActorProperty::GetFuncType(this, &ArithmeticMutator::GetB),
         "The Right value.");
      AddProperty(leftProp);
      AddProperty(rightProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, leftProp, false, false, false));
      mValues.push_back(ValueLink(this, rightProp, false, false, false));

      OnLinkValueChanged("A");
   }

   //////////////////////////////////////////////////////////////////////////
   void ArithmeticMutator::OnLinkValueChanged(const std::string& linkName)
   {
      MutatorNode::OnLinkValueChanged(linkName);

      dtDAL::DataType& leftType = Node::GetPropertyType("A");
      dtDAL::DataType& rightType = Node::GetPropertyType("B");
      dtDAL::DataType& desiredType = GetStrongerType(leftType, rightType);

      // If our result property does not match the proper value type,
      // create one that does.
      if (!mProperty || desiredType != mProperty->GetPropertyType())
      {
         RemoveProperty("Result");

         switch (desiredType.GetTypeId())
         {
         case dtDAL::DataType::BOOLEAN_ID:
            mProperty = new dtDAL::BooleanActorProperty(
               "Result", "Result",
               dtDAL::BooleanActorProperty::SetFuncType(this, &ArithmeticMutator::SetResultBool),
               dtDAL::BooleanActorProperty::GetFuncType(this, &ArithmeticMutator::GetResultBool),
               "The Result value.");
            break;

         case dtDAL::DataType::INT_ID:
            mProperty = new dtDAL::IntActorProperty(
               "Result", "Result",
               dtDAL::IntActorProperty::SetFuncType(this, &ArithmeticMutator::SetResultInt),
               dtDAL::IntActorProperty::GetFuncType(this, &ArithmeticMutator::GetResultInt),
               "The Result value.");
            break;

         case dtDAL::DataType::FLOAT_ID:
            mProperty = new dtDAL::FloatActorProperty(
               "Result", "Result",
               dtDAL::FloatActorProperty::SetFuncType(this, &ArithmeticMutator::SetResultFloat),
               dtDAL::FloatActorProperty::GetFuncType(this, &ArithmeticMutator::GetResultFloat),
               "The Result value.");
            break;

         case dtDAL::DataType::DOUBLE_ID:
            mProperty = new dtDAL::DoubleActorProperty(
               "Result", "Result",
               dtDAL::DoubleActorProperty::SetFuncType(this, &ArithmeticMutator::SetResultDouble),
               dtDAL::DoubleActorProperty::GetFuncType(this, &ArithmeticMutator::GetResultDouble),
               "The Result value.");
            break;

         case dtDAL::DataType::VEC2F_ID:
            mProperty = new dtDAL::Vec2ActorProperty(
               "Result", "Result",
               dtDAL::Vec2ActorProperty::SetFuncType(this, &ArithmeticMutator::SetResultVec2),
               dtDAL::Vec2ActorProperty::GetFuncType(this, &ArithmeticMutator::GetResultVec2),
               "The Result value.");
            break;

         case dtDAL::DataType::VEC3F_ID:
            mProperty = new dtDAL::Vec3ActorProperty(
               "Result", "Result",
               dtDAL::Vec3ActorProperty::SetFuncType(this, &ArithmeticMutator::SetResultVec3),
               dtDAL::Vec3ActorProperty::GetFuncType(this, &ArithmeticMutator::GetResultVec3),
               "The Result value.");
            break;

         case dtDAL::DataType::VEC4F_ID:
            mProperty = new dtDAL::Vec4ActorProperty(
               "Result", "Result",
               dtDAL::Vec4ActorProperty::SetFuncType(this, &ArithmeticMutator::SetResultVec4),
               dtDAL::Vec4ActorProperty::GetFuncType(this, &ArithmeticMutator::GetResultVec4),
               "The Result value.");
            break;
         }

         AddProperty(mProperty);
      }

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

      result = PerformOperation(left, right);

      // Store the result.
      switch (mProperty->GetPropertyType().GetTypeId())
      {
      case dtDAL::DataType::BOOLEAN_ID:
      case dtDAL::DataType::INT_ID:
      case dtDAL::DataType::FLOAT_ID:
      case dtDAL::DataType::DOUBLE_ID:
         SetDouble(result.x(), "Result");
         break;

      case dtDAL::DataType::VEC2F_ID:
         SetVec2(osg::Vec2(result.x(), result.y()), "Result");
         break;

      case dtDAL::DataType::VEC3F_ID:
         SetVec3(osg::Vec3(result.x(), result.y(), result.z()), "Result");
         break;

      case dtDAL::DataType::VEC4F_ID:
         SetVec4(result, "Result");
         break;
      }

      OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   bool ArithmeticMutator::CanBeType(dtDAL::DataType& type)
   {
      switch (type.GetTypeId())
      {
      case dtDAL::DataType::BOOLEAN_ID:
      case dtDAL::DataType::INT_ID:
      case dtDAL::DataType::FLOAT_ID:
      case dtDAL::DataType::DOUBLE_ID:
      case dtDAL::DataType::VEC2F_ID:
      case dtDAL::DataType::VEC3F_ID:
      case dtDAL::DataType::VEC4F_ID:
      case dtDAL::DataType::UNKNOWN_ID:
         return true;

      default:
         return false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool ArithmeticMutator::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (ValueNode::CanConnectValue(link, value))
      {
         dtDAL::DataType& type = value->GetPropertyType();
         switch (type.GetTypeId())
         {
         case dtDAL::DataType::BOOLEAN_ID:
         case dtDAL::DataType::INT_ID:
         case dtDAL::DataType::FLOAT_ID:
         case dtDAL::DataType::DOUBLE_ID:
         case dtDAL::DataType::VEC2F_ID:
         case dtDAL::DataType::VEC3F_ID:
         case dtDAL::DataType::VEC4F_ID:
         case dtDAL::DataType::UNKNOWN_ID:
            return true;

         default:
            return false;
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::DataType& ArithmeticMutator::GetStrongerType(dtDAL::DataType& type1, dtDAL::DataType& type2)
   {
      int type1Strength = 0;
      int type2Strength = 0;

      switch (type1.GetTypeId())
      {
      case dtDAL::DataType::BOOLEAN_ID:   type1Strength = 0;   break;
      case dtDAL::DataType::INT_ID:       type1Strength = 1;   break;
      case dtDAL::DataType::FLOAT_ID:     type1Strength = 2;   break;
      case dtDAL::DataType::DOUBLE_ID:    type1Strength = 3;   break;
      case dtDAL::DataType::VEC2F_ID:     type1Strength = 4;   break;
      case dtDAL::DataType::VEC3F_ID:     type1Strength = 5;   break;
      case dtDAL::DataType::VEC4F_ID:     type1Strength = 6;   break;
      }

      switch (type2.GetTypeId())
      {
      case dtDAL::DataType::BOOLEAN_ID:   type2Strength = 0;   break;
      case dtDAL::DataType::INT_ID:       type2Strength = 1;   break;
      case dtDAL::DataType::FLOAT_ID:     type2Strength = 2;   break;
      case dtDAL::DataType::DOUBLE_ID:    type2Strength = 3;   break;
      case dtDAL::DataType::VEC2F_ID:     type2Strength = 4;   break;
      case dtDAL::DataType::VEC3F_ID:     type2Strength = 5;   break;
      case dtDAL::DataType::VEC4F_ID:     type2Strength = 6;   break;
      }

      if (type1Strength >= type2Strength)
      {
         return type1;
      }

      return type2;
   }
}

////////////////////////////////////////////////////////////////////////////////
