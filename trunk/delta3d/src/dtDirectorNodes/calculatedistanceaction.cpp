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
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/calculatedistanceaction.h>

#include <dtDirector/director.h>

#include <dtDAL/doubleactorproperty.h>
#include <dtDAL/vectoractorproperties.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CalculateDistanceAction::CalculateDistanceAction()
      : ActionNode()
      , mValueA(osg::Vec3())
      , mValueB(osg::Vec3())
      , mResult(0.0f)
      , mResultProp(NULL)
   {
      AddAuthor("Eric R. Heine");
   }

   ////////////////////////////////////////////////////////////////////////////////
   CalculateDistanceAction::~CalculateDistanceAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CalculateDistanceAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CalculateDistanceAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::Vec3ActorProperty* leftProp = new dtDAL::Vec3ActorProperty(
         "A", "A",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &CalculateDistanceAction::SetA),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &CalculateDistanceAction::GetA),
         "The Left value.");

      dtDAL::Vec3ActorProperty* rightProp = new dtDAL::Vec3ActorProperty(
         "B", "B",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &CalculateDistanceAction::SetB),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &CalculateDistanceAction::GetB),
         "The Right value.");

      mResultProp = new dtDAL::DoubleActorProperty(
         "Result", "Result",
         dtDAL::DoubleActorProperty::SetFuncType(this, &CalculateDistanceAction::SetResult),
         dtDAL::DoubleActorProperty::GetFuncType(this, &CalculateDistanceAction::GetResult),
         "The resulting output value.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, leftProp, false, false, false));
      mValues.push_back(ValueLink(this, rightProp, false, false, false));
      mValues.push_back(ValueLink(this, mResultProp.get(), true, true));
   }

   //////////////////////////////////////////////////////////////////////////
   bool CalculateDistanceAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtDAL::DataType& leftType = GetPropertyType("A");
      dtDAL::DataType& rightType = GetPropertyType("B");

      if (leftType != rightType)
      {
         LOG_ERROR("Mismatching vector types in CalculateDistanceAction node");
         return false;
      }

      double result;
      if (leftType == dtDAL::DataType::VEC2F)
      {
         result = (GetVec2("B") - GetVec2("A")).length();
      }
      else if (leftType == dtDAL::DataType::VEC3F)
      {
         result = (GetVec3("B") - GetVec3("A")).length();
      }
      else if (leftType == dtDAL::DataType::VEC4F)
      {
         result = (GetVec4("B") - GetVec4("A")).length();
      }

      int count = GetPropertyCount("Result");
      for (int index = 0; index < count; index++)
      {
         SetDouble(result, "Result", index);
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   //////////////////////////////////////////////////////////////////////////
   bool CalculateDistanceAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (ActionNode::CanConnectValue(link, value))
      {
         if (link->GetName() == "A" || link->GetName() == "B")
         {
            if (value->CanBeType(dtDAL::DataType::VEC2F) ||
                value->CanBeType(dtDAL::DataType::VEC3F) ||
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
   void CalculateDistanceAction::SetA(const osg::Vec3& value)
   {
      mValueA = value;
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec3 CalculateDistanceAction::GetA()
   {
      return mValueA;
   }

   //////////////////////////////////////////////////////////////////////////
   void CalculateDistanceAction::SetB(const osg::Vec3& value)
   {
      mValueB = value;
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec3 CalculateDistanceAction::GetB()
   {
      return mValueB;
   }

   //////////////////////////////////////////////////////////////////////////
   void CalculateDistanceAction::SetResult(double value)
   {
      mResult = value;
   }

   //////////////////////////////////////////////////////////////////////////
   double CalculateDistanceAction::GetResult()
   {
      return mResult;
   }
}

////////////////////////////////////////////////////////////////////////////////
