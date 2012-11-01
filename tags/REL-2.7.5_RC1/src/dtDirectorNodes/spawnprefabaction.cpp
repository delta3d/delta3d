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
#include <dtDirectorNodes/spawnprefabaction.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/baseactorobject.h>

#include <dtCore/resourceactorproperty.h>
#include <dtCore/vectoractorproperties.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SpawnPrefabAction::SpawnPrefabAction()
      : ActionNode()
      , mPrefab("")
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   SpawnPrefabAction::~SpawnPrefabAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SpawnPrefabAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed", "Activated if the prefab has failed to spawn."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SpawnPrefabAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::ResourceActorProperty* prefabProp = new dtCore::ResourceActorProperty(
         dtCore::DataType::PREFAB, "Prefab", "Prefab",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &SpawnPrefabAction::SetPrefab),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &SpawnPrefabAction::GetPrefab),
         "The prefab resource to spawn.");
      AddProperty(prefabProp);

      dtCore::Vec3ActorProperty* spawnPosProp = new dtCore::Vec3ActorProperty(
         "Spawn Location", "Spawn Location",
         dtCore::Vec3ActorProperty::SetFuncType(this, &SpawnPrefabAction::SetSpawnLocation),
         dtCore::Vec3ActorProperty::GetFuncType(this, &SpawnPrefabAction::GetSpawnLocation),
         "The location to spawn the new actor.");
      AddProperty(spawnPosProp);

      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
         "Out Actor", "Out Actor",
         dtCore::ActorIDActorProperty::SetFuncType(this, &SpawnPrefabAction::SetNewActor),
         dtCore::ActorIDActorProperty::GetFuncType(this, &SpawnPrefabAction::GetNewActor),
         "", "The actor that was spawned.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, prefabProp, false, false, true, false));
      mValues.push_back(ValueLink(this, spawnPosProp, false, false, false));
      mValues.push_back(ValueLink(this, actorProp, true, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SpawnPrefabAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtGame::GameManager* gm = GetDirector()->GetGameManager();
      dtCore::ResourceDescriptor prefab = GetResource("Prefab");
      osg::Vec3 spawnLocation = GetVec3("Spawn Location");
      osg::Vec3 spawnRotation;
      dtCore::BaseActorObject* locationActor = GetActor("Spawn Location");
      if (locationActor)
      {
         dtCore::Vec3ActorProperty* transProp =
            dynamic_cast<dtCore::Vec3ActorProperty*>(locationActor->GetProperty("Translation"));
         dtCore::Vec3ActorProperty* rotProp =
            dynamic_cast<dtCore::Vec3ActorProperty*>(locationActor->GetProperty("Rotation"));

         if (transProp)
         {
            spawnLocation = transProp->GetValue();
         }
         if (rotProp)
         {
            spawnRotation = rotProp->GetValue();
         }
      }

      if (gm && !prefab.IsEmpty())
      {
         dtCore::RefPtr<dtCore::BaseActorObject> proxy = gm->CreateActor("dtActors", "Prefab");
         if (proxy.valid())
         {
            dtCore::Vec3ActorProperty* transProp =
               dynamic_cast<dtCore::Vec3ActorProperty*>(proxy->GetProperty("Translation"));
            dtCore::Vec3ActorProperty* rotProp =
               dynamic_cast<dtCore::Vec3ActorProperty*>(proxy->GetProperty("Rotation"));

            if (transProp)
            {
               transProp->SetValue(spawnLocation);
            }
            if (rotProp)
            {
               rotProp->SetValue(spawnRotation);
            }

            gm->AddActor(*proxy);

            dtCore::ResourceActorProperty* resourceProp = NULL;
            resourceProp = dynamic_cast<dtCore::ResourceActorProperty*>(proxy->GetProperty("PrefabResource"));
            if (resourceProp)
            {
               resourceProp->SetValue(prefab);
            }

            SetActorID(proxy->GetId(), "Out Actor");
            return ActionNode::Update(simDelta, delta, input, firstUpdate);
         }
      }

      ActivateOutput("Failed");
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool SpawnPrefabAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (ActionNode::CanConnectValue(link, value))
      {
         if (link->GetName() == "Spawn Location")
         {
            if (value->CanBeType(dtCore::DataType::ACTOR) ||
                value->CanBeType(dtCore::DataType::VEC3))
            {
               return true;
            }
            return false;
         }
         return true;
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnPrefabAction::SetPrefab(const dtCore::ResourceDescriptor& value)
   {
      mPrefab = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor SpawnPrefabAction::GetPrefab() const
   {
      return mPrefab;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnPrefabAction::SetSpawnLocation(const osg::Vec3& value)
   {
      mSpawnLocation = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 SpawnPrefabAction::GetSpawnLocation() const
   {
      return mSpawnLocation;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SpawnPrefabAction::SetNewActor(const dtCore::UniqueId& value)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId SpawnPrefabAction::GetNewActor()
   {
      dtCore::UniqueId id;
      id = "";
      return id;
   }
}

////////////////////////////////////////////////////////////////////////////////
