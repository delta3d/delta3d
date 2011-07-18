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
 * Author: Eric R. Heine
 */

#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/getactororientationaction.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/vectoractorproperties.h>
#include <dtDirector/director.h>
#include <dtGame/gamemanager.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   GetActorOrientationAction::GetActorOrientationAction()
      : ActionNode()
      , mActorID(dtCore::UniqueId(""))
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   GetActorOrientationAction::~GetActorOrientationAction()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void GetActorOrientationAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetActorOrientationAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &GetActorOrientationAction::SetActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &GetActorOrientationAction::GetActor),
         "dtCore::Transformable",
         "The actor whose orientation we want.");
      AddProperty(actorProp);

      mpForwardProp = new dtDAL::Vec3ActorProperty(
         "Forward", "Forward",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &GetActorOrientationAction::SetEmptyVec),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &GetActorOrientationAction::GetEmptyVec),
         "The actor's forward vector.");
      mpUpProp = new dtDAL::Vec3ActorProperty(
         "Up", "Up",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &GetActorOrientationAction::SetEmptyVec),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &GetActorOrientationAction::GetEmptyVec),
         "The actor's up vector.");
      mpRightProp = new dtDAL::Vec3ActorProperty(
         "Right", "Right",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &GetActorOrientationAction::SetEmptyVec),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &GetActorOrientationAction::GetEmptyVec),
         "The actor's right vector.");

      mValues.push_back(ValueLink(this, actorProp));
      mValues.push_back(ValueLink(this, mpForwardProp.get(), true, true));
      mValues.push_back(ValueLink(this, mpUpProp.get(), true, true));
      mValues.push_back(ValueLink(this, mpRightProp.get(), true, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GetActorOrientationAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtCore::UniqueId actorID = GetActorID("Actor");
      dtDAL::BaseActorObject* proxy = GetDirector()->GetGameManager()->FindActorById(actorID);
      if (proxy != NULL)
      {
         dtCore::Transformable* actor = NULL;
         proxy->GetActor(actor);
         if (actor != NULL)
         {
            dtCore::Transform transform;
            actor->GetTransform(transform);

            SetVec3(transform.GetForwardVector(), "Forward");
            SetVec3(transform.GetUpVector(), "Up");
            SetVec3(transform.GetRightVector(), "Right");
         }
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GetActorOrientationAction::SetActor(const dtCore::UniqueId& value)
   {
      mActorID = value;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId GetActorOrientationAction::GetActor() const
   {
      return mActorID;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GetActorOrientationAction::SetEmptyVec(const osg::Vec3& value)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 GetActorOrientationAction::GetEmptyVec() const
   {
      return osg::Vec3();
   }

   /////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
