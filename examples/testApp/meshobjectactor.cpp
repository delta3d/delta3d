/*
 * MeshObjectActor.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: david
 */

#include "meshobjectactor.h"

#include <dtPhysics/physicsactcomp.h>
#include <dtPhysics/physicsobject.h>

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
      if (GetMeshResource() != dtCore::ResourceDescriptor::NULL_RESOURCE)
      {
         dtActors::GameMeshDrawable* drawable = NULL;
         GetDrawable(drawable);
         if (drawable->GetMeshNode() != NULL)
         {
            GetComponent<dtPhysics::PhysicsActComp>()->GetMainPhysicsObject()->
                  CreateFromProperties(drawable->GetMeshNode(), false, GetMeshResource().GetResourceIdentifier());
         }
      }
   }

   void MeshObjectActor::BuildActorComponents()
   {
      BaseClass::BuildActorComponents();
      dtCore::RefPtr<dtPhysics::PhysicsActComp> pac = new dtPhysics::PhysicsActComp;
      dtCore::RefPtr<dtPhysics::PhysicsObject> phyObj = new dtPhysics::PhysicsObject("Main");
      phyObj->SetMechanicsType(dtPhysics::MechanicsType::STATIC);
      phyObj->SetPrimitiveType(dtPhysics::PrimitiveType::CONVEX_HULL);
      pac->AddPhysicsObject(*phyObj);
      pac->SetAutoCreateOnEnteringWorld(false);
      AddComponent(*pac);
   }


} /* namespace dtExample */
