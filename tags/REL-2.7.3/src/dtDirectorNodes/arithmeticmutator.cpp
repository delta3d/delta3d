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
#include <dtDirectorNodes/arithmeticmutator.h>

#include <dtDirector/director.h>

#include <dtCore/stringactorproperty.h>
#include <dtCore/doubleactorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   ArithmeticMutator::ArithmeticMutator()
      : MutatorNode()
      , mValueA(0.0f)
      , mValueB(0.0f)
      , mResult("")
      , mResultType(NULL)
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
   void ArithmeticMutator::OnFinishedLoading()
   {
      MutatorNode::OnFinishedLoading();

      OnLinkValueChanged("Result");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArithmeticMutator::BuildPropertyMap()
   {
      MutatorNode::BuildPropertyMap();

      // Create our value links.
      dtCore::DoubleActorProperty* leftProp = new dtCore::DoubleActorProperty(
         "A", "A",
         dtCore::DoubleActorProperty::SetFuncType(this, &ArithmeticMutator::SetA),
         dtCore::DoubleActorProperty::GetFuncType(this, &ArithmeticMutator::GetA),
         "The Left value.");

      dtCore::DoubleActorProperty* rightProp = new dtCore::DoubleActorProperty(
         "B", "B",
         dtCore::DoubleActorProperty::SetFuncType(this, &ArithmeticMutator::SetB),
         dtCore::DoubleActorProperty::GetFuncType(this, &ArithmeticMutator::GetB),
         "The Right value.");
      AddProperty(leftProp);
      AddProperty(rightProp);

      mProperty = new dtCore::StringActorProperty(
         "Result", "Result",
         dtCore::StringActorProperty::SetFuncType(this, &ArithmeticMutator::SetResult),
         dtCore::StringActorProperty::GetFuncType(this, &ArithmeticMutator::GetResult),
         "The Result value.");
      //AddProperty(mProperty);

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

      // Skip this calculation while we are in the loading process or we
      // are not changing one of our input values.
      if (GetDirector()->IsLoading() ||
         (linkName != mProperty->GetName() &&
         linkName != "A" && linkName != "B"))
      {
         return;
      }

      dtCore::DataType& leftType = Node::GetPropertyType("A");
      dtCore::DataType& rightType = Node::GetPropertyType("B");
      dtCore::DataType& desiredType = GetStrongerType(leftType, rightType);
      mResultType = &desiredType;

      osg::Vec4 left;
      osg::Vec4 right;
      osg::Vec4 result;

      if (leftType == dtCore::DataType::VEC2F)
      {
         osg::Vec2 vec2A = GetVec2("A");
         left.x() = vec2A.x();
         left.y() = vec2A.y();
         left.z() = left.x();
         left.w() = left.x();
      }
      else if (leftType == dtCore::DataType::VEC3F)
      {
         osg::Vec3 vec3A = GetVec3("A");
         left.x() = vec3A.x();
         left.y() = vec3A.y();
         left.z() = vec3A.z();
         left.w() = left.x();
      }
      else if (leftType == dtCore::DataType::VEC4F)
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

      if (rightType == dtCore::DataType::VEC2F)
      {
         osg::Vec2 vec2A = GetVec2("B");
         right.x() = vec2A.x();
         right.y() = vec2A.y();
         right.z() = right.x();
         right.w() = right.x();
      }
      else if (rightType == dtCore::DataType::VEC3F)
      {
         osg::Vec3 vec3A = GetVec3("B");
         right.x() = vec3A.x();
         right.y() = vec3A.y();
         right.z() = vec3A.z();
         right.w() = right.x();
      }
      else if (rightType == dtCore::DataType::VEC4F)
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
      switch (desiredType.GetTypeId())
      {
      case dtCore::DataType::BOOLEAN_ID:
      case dtCore::DataType::INT_ID:
      case dtCore::DataType::FLOAT_ID:
      case dtCore::DataType::DOUBLE_ID:
         SetDouble(result.x(), "Result");
         break;

      case dtCore::DataType::VEC2F_ID:
         SetVec2(osg::Vec2(result.x(), result.y()), "Result");
         break;

      case dtCore::DataType::VEC3F_ID:
         SetVec3(osg::Vec3(result.x(), result.y(), result.z()), "Result");
         break;

      case dtCore::DataType::VEC4F_ID:
         SetVec4(result, "Result");
         break;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool ArithmeticMutator::CanBeType(dtCore::DataType& type)
   {
      switch (type.GetTypeId())
      {
      case dtCore::DataType::BOOLEAN_ID:
      case dtCore::DataType::INT_ID:
      case dtCore::DataType::FLOAT_ID:
      case dtCore::DataType::DOUBLE_ID:
      case dtCore::DataType::VEC2F_ID:
      case dtCore::DataType::VEC3F_ID:
      case dtCore::DataType::VEC4F_ID:
      case dtCore::DataType::UNKNOWN_ID:
         return true;

      default:
         return false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::DataType& ArithmeticMutator::GetPropertyType()
   {
      if (mResultType) return *mResultType;

      // If there is no property, return an undefined type.
      return dtCore::DataType::UNKNOWN;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ArithmeticMutator::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (MutatorNode::CanConnectValue(link, value))
      {
         if (link->GetName() == "A" || link->GetName() == "B")
         {
            if (value->CanBeType(dtCore::DataType::BOOLEAN) ||
                value->CanBeType(dtCore::DataType::INT)     ||
                value->CanBeType(dtCore::DataType::FLOAT)   ||
                value->CanBeType(dtCore::DataType::DOUBLE)  ||
                value->CanBeType(dtCore::DataType::VEC2F)   ||
                value->CanBeType(dtCore::DataType::VEC3F)   ||
                value->CanBeType(dtCore::DataType::VEC4F))
            {
               return true;
            }
            return false;
         }
         return true;
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::DataType& ArithmeticMutator::GetStrongerType(dtCore::DataType& type1, dtCore::DataType& type2)
   {
      int type1Strength = 0;
      int type2Strength = 0;

      switch (type1.GetTypeId())
      {
      case dtCore::DataType::BOOLEAN_ID:   type1Strength = 0;   break;
      case dtCore::DataType::INT_ID:       type1Strength = 1;   break;
      case dtCore::DataType::FLOAT_ID:     type1Strength = 2;   break;
      case dtCore::DataType::DOUBLE_ID:    type1Strength = 3;   break;
      case dtCore::DataType::VEC2F_ID:     type1Strength = 4;   break;
      case dtCore::DataType::VEC3F_ID:     type1Strength = 5;   break;
      case dtCore::DataType::VEC4F_ID:     type1Strength = 6;   break;
      }

      switch (type2.GetTypeId())
      {
      case dtCore::DataType::BOOLEAN_ID:   type2Strength = 0;   break;
      case dtCore::DataType::INT_ID:       type2Strength = 1;   break;
      case dtCore::DataType::FLOAT_ID:     type2Strength = 2;   break;
      case dtCore::DataType::DOUBLE_ID:    type2Strength = 3;   break;
      case dtCore::DataType::VEC2F_ID:     type2Strength = 4;   break;
      case dtCore::DataType::VEC3F_ID:     type2Strength = 5;   break;
      case dtCore::DataType::VEC4F_ID:     type2Strength = 6;   break;
      }

      if (type1Strength >= type2Strength)
      {
         return type1;
      }

      return type2;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArithmeticMutator::SetA(double value)
   {
      if (mValueA != value)
      {
         mValueA = value;
         OnValueChanged();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   double ArithmeticMutator::GetA() const
   {
      return mValueA;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArithmeticMutator::SetB(double value)
   {
      if (mValueB != value)
      {
         mValueB = value;
         OnValueChanged();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   double ArithmeticMutator::GetB() const
   {
      return mValueB;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArithmeticMutator::SetResult(const std::string& value)
   {
      if (mResult != value)
      {
         mResult = value;
         OnValueChanged();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& ArithmeticMutator::GetResult() const
   {
      return mResult;
   }
}

////////////////////////////////////////////////////////////////////////////////
