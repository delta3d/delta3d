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
#include <dtDAL/actortype.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/stringactorproperty.h>
#include <dtDAL/stringselectoractorproperty.h>
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

      dtDAL::StringSelectorActorProperty* classProp = new dtDAL::StringSelectorActorProperty(
         "Actor Type", "Actor Type",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &GetActorAction::SetClassType),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &GetActorAction::GetClassType),
         dtDAL::StringSelectorActorProperty::GetListFuncType(&dtDAL::LibraryManager::GetInstance(),
         &dtDAL::LibraryManager::GetClassTypes),
         "The type of the actor to get(optional).", "", false);
      AddProperty(classProp);

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

      dtDAL::BaseActorObject* actorToGet = NULL;

      // If an actual actor instance was specified, use it
      if (!mActor.ToString().empty())
      {
         actorToGet = gm->FindActorById(mActor);
      }
      else if (!mActorName.empty())
      {
         // If we have a name, filter by it first
         std::vector<dtDAL::BaseActorObject*> baseList;
         gm->FindActorsByName(mActorName, baseList);

         // If we have a type, filter by it second
         if (!mClassType.empty())
         {
            for (size_t actorIndex = 0; actorIndex < baseList.size(); ++actorIndex)
            {
               if (baseList[actorIndex]->GetActorType().GetFullName() == mClassType)
               {
                  actorToGet = baseList[actorIndex];
               }
            }
         }
         else if (!baseList.empty())
         {
            // If we only have a name, return the first found
            actorToGet = baseList[0];
         }
      }
      else if (!mClassType.empty())
      {
         std::vector<dtDAL::BaseActorObject*> baseList;
         gm->GetAllActors(baseList);

         // Find the first actor of the given type
         for (size_t actorIndex = 0; actorIndex < baseList.size(); ++actorIndex)
         {
            if (baseList[actorIndex]->GetActorType().GetFullName() == mClassType)
            {
               actorToGet = baseList[actorIndex];
               break;
            }
         }
      }

      if (actorToGet)
      {
         // Call director's set actor which will give precedence to links
         SetActorID(actorToGet->GetId(), "Actor");

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

   ////////////////////////////////////////////////////////////////////////////////
   void GetActorAction::SetClassType(const std::string& value)
   {
      mClassType = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& GetActorAction::GetClassType() const
   {
      return mClassType;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string& GetActorAction::GetName()
   {
      return (mActorName.empty() ? mClassType: mActorName);
   }
}

////////////////////////////////////////////////////////////////////////////////
