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
#include <dtCore/transform.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtDirector/director.h>

namespace dtDirector
{
   DT_IMPLEMENT_ACCESSOR(AttachCameraAction, osg::Vec3, Offset);

   /////////////////////////////////////////////////////////////////////////////
   AttachCameraAction::AttachCameraAction()
      : ActionNode()
      , mOffset(0.0f, 0.0f, 0.0f)
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

      mOutputs.push_back(OutputLink(this, "Failed", "Activated if the camera or the actor to attach the camera to could not be found."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void AttachCameraAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Actor", "Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &AttachCameraAction::SetCurrentActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &AttachCameraAction::GetCurrentActor),
         "", "The actor to attach the camera to.");
      AddProperty(actorProp);

      dtCore::Vec3ActorProperty* offsetProp = new dtCore::Vec3ActorProperty(
         "Offset", "Parent Offset",
         dtCore::Vec3ActorProperty::SetFuncType(this, &AttachCameraAction::SetOffset),
         dtCore::Vec3ActorProperty::GetFuncType(this, &AttachCameraAction::GetOffset),
         "Translational offset from the parent actor.", "");
      AddProperty(offsetProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp));
      mValues.push_back(ValueLink(this, offsetProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool AttachCameraAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtCore::ActorProxy* proxy = GetActor("Actor");
      dtABC::Application* app = dtABC::Application::GetInstance(0);
      if (proxy && app)
      {
         dtCore::Camera* camera = app->GetCamera();
         dtCore::DeltaDrawable* actor = proxy->GetDrawable();
         if (camera && actor)
         {
            camera->SetTransform(dtCore::Transform(), dtCore::Transformable::REL_CS);

            dtCore::DeltaDrawable* parent = camera->GetParent();
            if (parent)
            {
               parent->RemoveChild(camera);
            }
            actor->AddChild(camera);
            
            dtCore::Transform offset;

            offset.SetTranslation(GetVec3("Offset"));
            camera->SetTransform(offset, dtCore::Transformable::REL_CS);
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
