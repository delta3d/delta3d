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
#include <dtDirectorNodes/attachcameraaction.h>

#include <dtABC/application.h>

#include <dtCore/camera.h>
#include <dtCore/deltadrawable.h>

#include <dtDAL/actoridactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   AttachCameraAction::AttachCameraAction()
      : ActionNode()
   {
      mActor = "";

      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   AttachCameraAction::~AttachCameraAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void AttachCameraAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void AttachCameraAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &AttachCameraAction::SetCurrentActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &AttachCameraAction::GetCurrentActor),
         "", "The actor to attach the camera to.");
      AddProperty(actorProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool AttachCameraAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtDAL::ActorProxy* proxy = GetActor("Actor");
      dtABC::Application* app = dtABC::Application::GetInstance(0);
      if (proxy && app)
      {
         dtCore::Camera* camera = app->GetCamera();
         dtCore::DeltaDrawable* actor = proxy->GetActor();
         if (camera && actor)
         {
            actor->AddChild(camera);
            return ActionNode::Update(simDelta, delta, input, firstUpdate);
         }
      }

      ActivateOutput("Failed");
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void AttachCameraAction::SetCurrentActor(const dtCore::UniqueId& value)
   {
      mActor = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId AttachCameraAction::GetCurrentActor()
   {
      return mActor;
   }
}

////////////////////////////////////////////////////////////////////////////////
