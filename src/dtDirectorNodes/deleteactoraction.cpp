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
#include <dtDirectorNodes/deleteactoraction.h>

#include <dtCore/actoridactorproperty.h>
#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   DeleteActorAction::DeleteActorAction()
      : ActionNode()
      , mActor(dtCore::UniqueId(""))
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   DeleteActorAction::~DeleteActorAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeleteActorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Actor", "Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &DeleteActorAction::SetDeletedActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &DeleteActorAction::GetDeletedActor),
         "", "The actor to Delete.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DeleteActorAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtCore::BaseActorObject* actor = GetActor("Actor");
      if (actor != NULL)
      {
         dtGame::GameManager* gm = GetDirector()->GetGameManager();
         if (gm != NULL)
         {
            gm->DeleteActor(*actor);
         }
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeleteActorAction::SetDeletedActor(const dtCore::UniqueId& value)
   {
      mActor = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId DeleteActorAction::GetDeletedActor()
   {
      return mActor;
   }
}

////////////////////////////////////////////////////////////////////////////////
