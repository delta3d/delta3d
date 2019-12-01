/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
#include <dtDirectorNodes/compareactorfacingaction.h>

#include <dtCore/transform.h>

#include <dtCore/transformable.h>
#include <dtCore/transformableactorproxy.h>
#include <dtCore/actoridactorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtDirector/director.h>

#include <dtUtil/mathdefines.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   CompareActorFacingAction::CompareActorFacingAction()
      : ActionNode()
   {
      mActor = "";
      mTarget = "";
      mSize = 1.0f;

      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   CompareActorFacingAction::~CompareActorFacingAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void CompareActorFacingAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "True", "Activates if the Actor is facing towards the Target."));
      mOutputs.push_back(OutputLink(this, "False", "Activates if the Actor is not facing towards the Target."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void CompareActorFacingAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Actor", "Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &CompareActorFacingAction::SetCurrentActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &CompareActorFacingAction::GetCurrentActor),
         "", "The actor.");
      AddProperty(actorProp);

      dtCore::ActorIDActorProperty* targetProp = new dtCore::ActorIDActorProperty(
         "Target", "Target",
         dtCore::ActorIDActorProperty::SetFuncType(this, &CompareActorFacingAction::SetTargetActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &CompareActorFacingAction::GetTargetActor),
         "", "The target to face.");
      AddProperty(targetProp);

      dtCore::FloatActorProperty* sizeProp = new dtCore::FloatActorProperty(
         "Size", "Size",
         dtCore::FloatActorProperty::SetFuncType(this, &CompareActorFacingAction::SetSize),
         dtCore::FloatActorProperty::GetFuncType(this, &CompareActorFacingAction::GetSize),
         "The size of the target.");
      AddProperty(sizeProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp));
      mValues.push_back(ValueLink(this, targetProp, false, false, false));
      mValues.push_back(ValueLink(this, sizeProp, false, false, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool CompareActorFacingAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      float threshold = GetFloat("Size");

      dtCore::Transform transform;
      osg::Vec3 targetPos;

      dtCore::TransformableActorProxy* actor =
         dynamic_cast<dtCore::TransformableActorProxy*>(GetActor("Actor"));
      if (actor)
      {
         dtCore::Transformable* transformable = NULL;
         actor->GetDrawable(transformable);
         if (transformable)
         {
            transformable->GetTransform(transform);
         }
      }

      actor =
         dynamic_cast<dtCore::TransformableActorProxy*>(GetActor("Target"));
      if (!actor)
      {
         targetPos = GetVec3("Target");
      }
      else
      {
         dtCore::Vec3ActorProperty* vecProp =
            dynamic_cast<dtCore::Vec3ActorProperty*>(actor->GetProperty("Translation"));
         if (vecProp)
         {
            targetPos = vecProp->GetValue();
         }
      }

      osg::Vec3 vecToTarget = targetPos - transform.GetTranslation();
      float targetDist = vecToTarget.length();
      vecToTarget.normalize();

      osg::Vec3 facingVec = transform.GetForwardVector();

      float dot = vecToTarget * facingVec;
      if (dot > 0.0f)
      {
         osg::Vec3 closestPoint = transform.GetTranslation() + (facingVec * targetDist * dot);

         if ((targetPos - closestPoint).length() <= threshold)
         {
            ActivateOutput("True");
            return false;
         }
      }

      ActivateOutput("False");
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CompareActorFacingAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (ActionNode::CanConnectValue(link, value))
      {
         if (link->GetName() == "Target")
         {
            if (value->CanBeType(dtCore::DataType::ACTOR) ||
                value->CanBeType(dtCore::DataType::VEC3))
            {
               return true;
            }
            return false;
         }
         else if (link->GetName() == "Size")
         {
            if (value->CanBeType(dtCore::DataType::INT)   ||
                value->CanBeType(dtCore::DataType::FLOAT) ||
                value->CanBeType(dtCore::DataType::DOUBLE))
            {
               return true;
            }
            return false;
         }
         return true;
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void CompareActorFacingAction::SetCurrentActor(const dtCore::UniqueId& value)
   {
      mActor = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId CompareActorFacingAction::GetCurrentActor()
   {
      return mActor;
   }

   /////////////////////////////////////////////////////////////////////////////
   void CompareActorFacingAction::SetTargetActor(const dtCore::UniqueId& value)
   {
      mTarget = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId CompareActorFacingAction::GetTargetActor()
   {
      return mTarget;
   }

   //////////////////////////////////////////////////////////////////////////
   void CompareActorFacingAction::SetSize(const float& value)
   {
      mSize = value;
   }

   //////////////////////////////////////////////////////////////////////////
   float CompareActorFacingAction::GetSize() const
   {
      return mSize;
   }
}

////////////////////////////////////////////////////////////////////////////////
