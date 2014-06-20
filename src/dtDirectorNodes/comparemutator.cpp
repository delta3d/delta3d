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
#include <dtDirectorNodes/comparemutator.h>

#include <dtCore/booleanactorproperty.h>
#include <dtCore/doubleactorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CompareMutator::CompareMutator()
      : MutatorNode()
      , mValueA(0.0f)
      , mValueB(0.0f)
      , mResult(false)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   CompareMutator::~CompareMutator()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareMutator::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      MutatorNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CompareMutator::BuildPropertyMap()
   {
      MutatorNode::BuildPropertyMap();

      // Create our value links.
      dtCore::DoubleActorProperty* leftProp = new dtCore::DoubleActorProperty(
         "A", "A",
         dtCore::DoubleActorProperty::SetFuncType(this, &CompareMutator::SetA),
         dtCore::DoubleActorProperty::GetFuncType(this, &CompareMutator::GetA),
         "The Left value.");

      dtCore::DoubleActorProperty* rightProp = new dtCore::DoubleActorProperty(
         "B", "B",
         dtCore::DoubleActorProperty::SetFuncType(this, &CompareMutator::SetB),
         dtCore::DoubleActorProperty::GetFuncType(this, &CompareMutator::GetB),
         "The Right value.");
      AddProperty(leftProp);
      AddProperty(rightProp);

      mProperty = new dtCore::BooleanActorProperty(
         "Result", "Result",
         dtCore::BooleanActorProperty::SetFuncType(this, &CompareMutator::SetResult),
         dtCore::BooleanActorProperty::GetFuncType(this, &CompareMutator::GetResult),
         "The Result value.");
      AddProperty(mProperty);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, leftProp, false, false, false));
      mValues.push_back(ValueLink(this, rightProp, false, false, false));

      OnLinkValueChanged("A");
   }

   //////////////////////////////////////////////////////////////////////////
   void CompareMutator::OnLinkValueChanged(const std::string& linkName)
   {
      MutatorNode::OnLinkValueChanged(linkName);

      dtCore::DataType& leftType = Node::GetPropertyType("A");
      dtCore::DataType& rightType = Node::GetPropertyType("B");

      if (leftType == dtCore::DataType::STRING &&
         rightType == dtCore::DataType::STRING)
      {
         bool result = Compare(GetString("A"), GetString("B"));

         if (GetBoolean("Result") != result)
         {
            SetBoolean(result, "Result");

            OnValueChanged();
         }
         return;
      }

      osg::Vec4 left;
      osg::Vec4 right;
      bool sameType = true;

      if (leftType == dtCore::DataType::VEC2F)
      {
         sameType = rightType == dtCore::DataType::VEC2F;
         osg::Vec2 vec2A = GetVec2("A");
         left.x() = vec2A.x();
         left.y() = vec2A.y();
         left.z() = left.x();
         left.w() = left.x();
      }
      else if (leftType == dtCore::DataType::VEC3F)
      {
         sameType = rightType == dtCore::DataType::VEC3F;
         osg::Vec3 vec3A = GetVec3("A");
         left.x() = vec3A.x();
         left.y() = vec3A.y();
         left.z() = vec3A.z();
         left.w() = left.x();
      }
      else if (leftType == dtCore::DataType::VEC4F)
      {
         sameType = rightType == dtCore::DataType::VEC4F;
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
         sameType = rightType == dtCore::DataType::VEC2F;
         osg::Vec2 vec2A = GetVec2("B");
         right.x() = vec2A.x();
         right.y() = vec2A.y();
         right.z() = right.x();
         right.w() = right.x();
      }
      else if (rightType == dtCore::DataType::VEC3F)
      {
         sameType = rightType == dtCore::DataType::VEC3F;
         osg::Vec3 vec3A = GetVec3("B");
         right.x() = vec3A.x();
         right.y() = vec3A.y();
         right.z() = vec3A.z();
         right.w() = right.x();
      }
      else if (rightType == dtCore::DataType::VEC4F)
      {
         sameType = rightType == dtCore::DataType::VEC4F;
         right = GetVec4("B");
      }
      else
      {
         right.x() = GetDouble("B");
         right.y() = right.x();
         right.z() = right.x();
         right.w() = right.x();
      }

      // We can only properly perform the compare if
      // both values are of the same type.
      bool result = false;
      if (sameType)
      {
         result = Compare(left, right);
      }

      if (GetBoolean("Result") != result)
      {
         SetBoolean(result, "Result");

         OnValueChanged();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool CompareMutator::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (MutatorNode::CanConnectValue(link, value))
      {
         if (link->GetName() == "A" || link->GetName() == "B")
         {
            if (value->CanBeType(dtCore::DataType::STRING)  ||
                value->CanBeType(dtCore::DataType::BOOLEAN) ||
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
}

////////////////////////////////////////////////////////////////////////////////
