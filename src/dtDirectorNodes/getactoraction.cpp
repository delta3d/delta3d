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
#include <dtDirectorNodes/getactoraction.h>

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/stringactorproperty.h>
#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   GetActorAction::GetActorAction()
      : ActionNode()
   {
      mActor = "";
      AddAuthor("MG");
   }

   /////////////////////////////////////////////////////////////////////////////
   GetActorAction::~GetActorAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetActorAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);
      mOutputs.push_back(OutputLink(this, "Failed", "Activated when the actor could not be found."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetActorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::StringActorProperty* nameProp = new dtDAL::StringActorProperty("ActorName", "Actor Name",
         dtDAL::StringActorProperty::SetFuncType(this, &GetActorAction::SetActorName),
         dtDAL::StringActorProperty::GetFuncType(this, &GetActorAction::GetActorName),
         "Actor name to get.");
      AddProperty(nameProp);

      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &GetActorAction::SetActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &GetActorAction::GetActor),
         "", "The actor to retrieve.");
      AddProperty(actorProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp, true, false, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GetActorAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtGame::GameManager* gm = GetDirector()->GetGameManager();

      dtDAL::BaseActorObject* base = NULL;
      gm->FindActorByName(mActorName, base);

      if (base)
      {
         // Call director's set actor which will give precedence to links
         SetActorID(base->GetId(), "Actor");

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
   void GetActorAction::SetActor(const dtCore::UniqueId& value)
   {
      mActor = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId GetActorAction::GetActor() const
   {
      return mActor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetActorAction::SetActorName(const std::string& name)
   {
      mActorName = name;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string GetActorAction::GetActorName() const
   {
      return mActorName;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string& GetActorAction::GetName()
   {
      return mActorName;
   }
}

////////////////////////////////////////////////////////////////////////////////
