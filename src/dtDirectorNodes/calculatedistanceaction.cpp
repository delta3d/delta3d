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
 * Author: Rudy T. Alfaro
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/calculatedistanceaction.h>

#include <dtDirector/director.h>

#include <dtCore/transformable.h>

#include <dtCore/doubleactorproperty.h>
#include <dtCore/vectoractorproperties.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   CalculateDistanceAction::CalculateDistanceAction()
      : ActionNode()
      , mValueA(osg::Vec3())
      , mValueB(osg::Vec3())
      , mSquaredResult(false)
      , mResult(0.0f)
      , mResultProp(NULL)
   {
      AddAuthor("Eric R. Heine");
      AddAuthor("Rudy T. Alfaro");
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
      dtCore::Vec3ActorProperty* leftProp = new dtCore::Vec3ActorProperty(
         "A", "A",
         dtCore::Vec3ActorProperty::SetFuncType(this, &CalculateDistanceAction::SetA),
         dtCore::Vec3ActorProperty::GetFuncType(this, &CalculateDistanceAction::GetA),
         "The Left value.");

      dtCore::Vec3ActorProperty* rightProp = new dtCore::Vec3ActorProperty(
         "B", "B",
         dtCore::Vec3ActorProperty::SetFuncType(this, &CalculateDistanceAction::SetB),
         dtCore::Vec3ActorProperty::GetFuncType(this, &CalculateDistanceAction::GetB),
         "The Right value.");

      dtCore::BooleanActorProperty* squaredResult = new dtCore::BooleanActorProperty(
         "SquaredResult", "Squared Result",
         dtCore::BooleanActorProperty::SetFuncType(this, &CalculateDistanceAction::SetSquaredResult),
         dtCore::BooleanActorProperty::GetFuncType(this, &CalculateDistanceAction::GetSquaredResult),
         "Whether the result is to be squared or not.");
    

      mResultProp = new dtCore::DoubleActorProperty(
         "Result", "Result",
         dtCore::DoubleActorProperty::SetFuncType(this, &CalculateDistanceAction::SetResult),
         dtCore::DoubleActorProperty::GetFuncType(this, &CalculateDistanceAction::GetResult),
         "The resulting output value.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, leftProp, false, false, false));
      mValues.push_back(ValueLink(this, rightProp, false, false, false));
      mValues.push_back(ValueLink(this, squaredResult, false, false, false));
      mValues.push_back(ValueLink(this, mResultProp.get(), true, true));

      AddProperty(squaredResult);
   }

   //////////////////////////////////////////////////////////////////////////
   bool CalculateDistanceAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtCore::DataType& leftType = GetPropertyType("A");
      dtCore::DataType& rightType = GetPropertyType("B");

      if (leftType != rightType)
      {
         LOG_ERROR("Mismatching vector types in CalculateDistanceAction node");
         return false;
      }

      double result;
      if (leftType == dtCore::DataType::VEC2F)
      {
         const osg::Vec2 diff = GetVec2("B") - GetVec2("A");

         if(mSquaredResult)
         {
            result = diff.length2();
         }
         else
         {
            result = diff.length();
         }
      }
      else if (leftType == dtCore::DataType::VEC3F)
      {
         const osg::Vec3f diff = GetVec3("B") - GetVec3("A");

         if(mSquaredResult)
         {
            result = diff.length2();
         }
         else
         {
            result = diff.length();
         }
      }
      else if (leftType == dtCore::DataType::VEC4F)
      {
         const osg::Vec4f diff = GetVec4("B") - GetVec4("A");

         if(mSquaredResult)
         {
            result = diff.length2();
         }
         else
         {
            result = diff.length();
         }
      }
      else if (leftType == dtCore::DataType::ACTOR)
      {         
         //Get the transforms of the actors
         dtCore::Transform firstTransform;
         dtCore::Transform secondTransform;
         GetActor("A")->GetDrawable()->AsTransformable()->GetTransform(firstTransform);
         GetActor("B")->GetDrawable()->AsTransformable()->GetTransform(secondTransform);

         //Get the translations
         osg::Vec3f firstTranslation;
         osg::Vec3f secondTranslation;
         firstTransform.GetTranslation(firstTranslation);
         secondTransform.GetTranslation(secondTranslation);   

         if(mSquaredResult)
         {
            //Result gets the distance squared.
            result = (firstTranslation - secondTranslation).length2();
         }
         else
         {
            result = (firstTranslation - secondTranslation).length();
         }
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
      if (GetDirector()->IsLoading())
      {
         return true;
      }

      if (ActionNode::CanConnectValue(link, value))
      {
         if (link->GetName() == "A" || link->GetName() == "B")
         {
            if (value->CanBeType(dtCore::DataType::VEC2F) ||
                value->CanBeType(dtCore::DataType::VEC3F) ||
                value->CanBeType(dtCore::DataType::VEC4F) ||
                value->CanBeType(dtCore::DataType::ACTOR))
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
   void CalculateDistanceAction::SetSquaredResult(bool value)
   {
      mSquaredResult = value;
   }

   //////////////////////////////////////////////////////////////////////////
   bool CalculateDistanceAction::GetSquaredResult()
   {
      return mSquaredResult;
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
