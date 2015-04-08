/*
 * MeshObjectActor.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: david
 */

#include "meshobjectactor.h"
#include "testapputils.h"

#include <dtPhysics/physicsactcomp.h>
#include <dtPhysics/physicsobject.h>

#include <dtGame/drpublishingactcomp.h>
#include <dtGame/deadreckoninghelper.h>

namespace dtExample
{

   MeshObjectActor::MeshObjectActor()
   {
      SetHideDTCorePhysicsProps(true);

   }

   MeshObjectActor::~MeshObjectActor()
   {
   }

   void MeshObjectActor::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();
      TestAppUtils util;
      util.GenerateTangentsForObject(*this);
   }

   void MeshObjectActor::BuildActorComponents()
   {
      BaseClass::BuildActorComponents();
      dtCore::RefPtr<dtPhysics::PhysicsActComp> pac = new dtPhysics::PhysicsActComp;
      dtCore::RefPtr<dtPhysics::PhysicsObject> phyObj = new dtPhysics::PhysicsObject("Main");
      phyObj->SetMechanicsType(dtPhysics::MechanicsType::STATIC);
      phyObj->SetPrimitiveType(dtPhysics::PrimitiveType::CONVEX_HULL);
      pac->AddPhysicsObject(*phyObj);
      pac->SetAutoCreateOnEnteringWorld(true);
      AddComponent(*pac);

      AddComponent(*new dtGame::DeadReckoningHelper);
      AddComponent(*new dtGame::DRPublishingActComp);
   }


} /* namespace dtExample */
