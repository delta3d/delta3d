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
 * Author: MG
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/getparentactoraction.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/actortype.h>
#include <dtCore/actorfactory.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/stringselectoractorproperty.h>
#include <dtCore/deltadrawable.h>
#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   GetParentActorAction::GetParentActorAction()
      : ActionNode()
   {
      mActor = "";
      mParent = "";
      AddAuthor("MG");
   }

   /////////////////////////////////////////////////////////////////////////////
   GetParentActorAction::~GetParentActorAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetParentActorAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);
      mOutputs.push_back(OutputLink(this, "Failed", "Activated when a parent could not be found."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetParentActorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Actor", "Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &GetParentActorAction::SetActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &GetParentActorAction::GetActor),
         "", "The actor to retrieve.");
      AddProperty(actorProp);

      dtCore::ActorIDActorProperty* parentActorProp = new dtCore::ActorIDActorProperty(
         "Parent Actor", "Parent Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &GetParentActorAction::SetParentActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &GetParentActorAction::GetParentActor),
         "", "The parent actor to retrieve.");
      AddProperty(parentActorProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp, false, false, true));
      mValues.push_back(ValueLink(this, parentActorProp, true, false, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GetParentActorAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtGame::GameManager* gm = GetDirector()->GetGameManager();

      // Get the actor that corresponds to this id
      dtGame::GameActorProxy* actorToGet = NULL, * parentActor = NULL;
      gm->FindGameActorById(GetActorID("Actor"), actorToGet);

      // If we've found the actor, try to get its parent
      if (actorToGet != NULL)
      {
         parentActor = actorToGet->GetParentActor();
      }

      // Set the parent if we found it
      if (parentActor != NULL)
      {
         SetActorID(parentActor->GetId(), "Parent Actor");

         // Fire the "out"
         ActionNode::Update(simDelta, delta, input, firstUpdate);
      }
      else
      {
         ActivateOutput("Failed");
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetParentActorAction::SetActor(const dtCore::UniqueId& value)
   {
      mActor = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId GetParentActorAction::GetActor() const
   {
      return mActor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetParentActorAction::SetParentActor(const dtCore::UniqueId& value)
   {
      mParent = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId GetParentActorAction::GetParentActor() const
   {
      return mParent;
   }
}

////////////////////////////////////////////////////////////////////////////////
