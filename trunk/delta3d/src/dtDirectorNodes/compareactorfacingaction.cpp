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
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/stringactorproperty.h>

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
      mOutputs.push_back(OutputLink(this, "True"));
      mOutputs.push_back(OutputLink(this, "False"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void CompareActorFacingAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &CompareActorFacingAction::SetCurrentActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &CompareActorFacingAction::GetCurrentActor),
         "", "The actor.");
      AddProperty(actorProp);

      dtDAL::ActorIDActorProperty* targetProp = new dtDAL::ActorIDActorProperty(
         "Target", "Target",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &CompareActorFacingAction::SetTargetActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &CompareActorFacingAction::GetTargetActor),
         "", "The target to face.");
      AddProperty(targetProp);

      dtDAL::FloatActorProperty* sizeProp = new dtDAL::FloatActorProperty(
         "Size", "Size",
         dtDAL::FloatActorProperty::SetFuncType(this, &CompareActorFacingAction::SetSize),
         dtDAL::FloatActorProperty::GetFuncType(this, &CompareActorFacingAction::GetSize),
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

      dtDAL::TransformableActorProxy* actor = 
         dynamic_cast<dtDAL::TransformableActorProxy*>(GetActor("Actor"));
      if (actor)
      {
         dtCore::Transformable* transformable = NULL;
         actor->GetActor(transformable);
         if (transformable)
         {
            transformable->GetTransform(transform);
         }
      }

      actor = 
         dynamic_cast<dtDAL::TransformableActorProxy*>(GetActor("Target"));
      if (!actor)
      {
         targetPos = GetVec3("Target");
      }
      else
      {
         dtDAL::Vec3ActorProperty* vecProp = 
            dynamic_cast<dtDAL::Vec3ActorProperty*>(actor->GetProperty("Translation"));
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
            TriggerOutput("True");
            return false;
         }
      }

      TriggerOutput("False");
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CompareActorFacingAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (ActionNode::CanConnectValue(link, value))
      {
         if (link->GetName() == "Target")
         {
            if (value->CanBeType(dtDAL::DataType::ACTOR) ||
               value->CanBeType(dtDAL::DataType::VEC3))
            {
               return true;
            }
         }
         else if (link->GetName() == "Size")
         {
            if (value->CanBeType(dtDAL::DataType::INT) ||
               value->CanBeType(dtDAL::DataType::FLOAT) ||
               value->CanBeType(dtDAL::DataType::DOUBLE))
            {
               return true;
            }
         }
         else
         {
            return true;
         }
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
