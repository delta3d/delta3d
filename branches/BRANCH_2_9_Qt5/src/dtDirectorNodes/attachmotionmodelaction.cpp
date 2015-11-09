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
#include <dtDirectorNodes/attachmotionmodelaction.h>

#include <dtCore/motionmodel.h>
#include <dtCore/deltadrawable.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/stringactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   AttachMotionModelAction::AttachMotionModelAction()
      : ActionNode()
   {
      mActor = "";
      mModelName = "";

      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   AttachMotionModelAction::~AttachMotionModelAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void AttachMotionModelAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed", "Activated if the motion model could not be found."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void AttachMotionModelAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::StringActorProperty* nameProp = new dtCore::StringActorProperty(
         "Motion Model", "Motion Model",
         dtCore::StringActorProperty::SetFuncType(this, &AttachMotionModelAction::SetModelName),
         dtCore::StringActorProperty::GetFuncType(this, &AttachMotionModelAction::GetModelName),
         "The name of the motion model to attach.");
      AddProperty(nameProp);

      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Actor", "Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &AttachMotionModelAction::SetCurrentActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &AttachMotionModelAction::GetCurrentActor),
         "", "The actor to attach the motion model to.");
      AddProperty(actorProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, nameProp, false, false, true, false));
      mValues.push_back(ValueLink(this, actorProp));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool AttachMotionModelAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtCore::BaseActorObject* actor = GetActor("Actor");
      dtCore::Transformable* txable = NULL;

      if (actor)
      {
         actor->GetDrawable(txable);
      }

      std::string modelName = GetString("Motion Model");
      if (!modelName.empty())
      {
         dtCore::MotionModel* model = dtCore::MotionModel::GetInstance(modelName);
         if (model)
         {
            model->SetTarget(txable);
            return ActionNode::Update(simDelta, delta, input, firstUpdate);
         }
      }

      ActivateOutput("Failed");
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AttachMotionModelAction::SetModelName(const std::string& value)
   {
      mModelName = value;
      mName = mModelName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string AttachMotionModelAction::GetModelName() const
   {
      return mModelName;
   }

   /////////////////////////////////////////////////////////////////////////////
   void AttachMotionModelAction::SetCurrentActor(const dtCore::UniqueId& value)
   {
      mActor = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId AttachMotionModelAction::GetCurrentActor()
   {
      return mActor;
   }
}

////////////////////////////////////////////////////////////////////////////////
