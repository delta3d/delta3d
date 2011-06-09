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

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/stringactorproperty.h>
#include <dtDAL/baseactorobject.h>

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

      mOutputs.push_back(OutputLink(this, "Failed"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SpawnPrefabAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::ResourceActorProperty* prefabProp = new dtDAL::ResourceActorProperty(
         dtDAL::DataType::PREFAB, "Prefab", "Prefab",
         dtDAL::ResourceActorProperty::SetDescFuncType(this, &SpawnPrefabAction::SetPrefab),
         dtDAL::ResourceActorProperty::GetDescFuncType(this, &SpawnPrefabAction::GetPrefab),
         "The prefab resource to spawn.");
      AddProperty(prefabProp);

      dtDAL::Vec3ActorProperty* spawnPosProp = new dtDAL::Vec3ActorProperty(
         "Spawn Location", "Spawn Location",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &SpawnPrefabAction::SetSpawnLocation),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &SpawnPrefabAction::GetSpawnLocation),
         "The location to spawn the new actor.");
      AddProperty(spawnPosProp);

      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Spawned", "Spawned",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &SpawnPrefabAction::SetSpawned),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &SpawnPrefabAction::GetSpawned),
         "", "The actor that was spawned.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, prefabProp));
      mValues.push_back(ValueLink(this, spawnPosProp, false, false, false));
      mValues.push_back(ValueLink(this, actorProp, true, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SpawnPrefabAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtGame::GameManager* gm = GetDirector()->GetGameManager();
      dtDAL::ResourceDescriptor prefab = GetResource("Prefab");
      osg::Vec3 spawnLocation = GetVec3("Spawn Location");
      osg::Vec3 spawnRotation;
      dtDAL::BaseActorObject* locationActor = GetActor("Spawn Location");
      if (locationActor)
      {
         dtDAL::Vec3ActorProperty* transProp = 
            dynamic_cast<dtDAL::Vec3ActorProperty*>(locationActor->GetProperty("Translation"));
         dtDAL::Vec3ActorProperty* rotProp =
            dynamic_cast<dtDAL::Vec3ActorProperty*>(locationActor->GetProperty("Rotation"));

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
         dtCore::RefPtr<dtDAL::BaseActorObject> proxy = gm->CreateActor("dtActors", "Prefab");
         if (proxy.valid())
         {
            dtDAL::Vec3ActorProperty* transProp =
               dynamic_cast<dtDAL::Vec3ActorProperty*>(proxy->GetProperty("Translation"));
            dtDAL::Vec3ActorProperty* rotProp =
               dynamic_cast<dtDAL::Vec3ActorProperty*>(proxy->GetProperty("Rotation"));

            if (transProp)
            {
               transProp->SetValue(spawnLocation);
            }
            if (rotProp)
            {
               rotProp->SetValue(spawnRotation);
            }

            gm->AddActor(*proxy);

            dtDAL::ResourceActorProperty* resourceProp = NULL;
            resourceProp = dynamic_cast<dtDAL::ResourceActorProperty*>(proxy->GetProperty("PrefabResource"));
            if (resourceProp)
            {
               resourceProp->SetValue(prefab);
            }

            SetActorID(proxy->GetId());
            return ActionNode::Update(simDelta, delta, input, firstUpdate);
         }
      }

      ActivateOutput("Failed");
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool SpawnPrefabAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (Node::CanConnectValue(link, value))
      {
         if (link->GetName() == "Spawn Location")
         {
            if (value->CanBeType(dtDAL::DataType::ACTOR) ||
               value->CanBeType(dtDAL::DataType::VEC3))
            {
               return true;
            }
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnPrefabAction::SetPrefab(const dtDAL::ResourceDescriptor& value)
   {
      mPrefab = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor SpawnPrefabAction::GetPrefab() const
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
   void SpawnPrefabAction::SetSpawned(const dtCore::UniqueId& value)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId SpawnPrefabAction::GetSpawned()
   {
      dtCore::UniqueId id;
      id = "";
      return id;
   }
}

////////////////////////////////////////////////////////////////////////////////
