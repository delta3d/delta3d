/*
 * CivilianActor.cpp
 *
 *  Created on: Aug 27, 2014
 *      Author: david
 */

#include "civilianactor.h"
#include "civilianaiactorcomponent.h"
#include "dtAnim/animationtransitionplanner.h"
#include <dtPhysics/physicsactcomp.h>

namespace dtExample
{

   ////////////////////////////////////////////////
   CivilianActor::CivilianActor()
   {
      SetHideDTCorePhysicsProps(true);
   }

   ////////////////////////////////////////////////
   CivilianActor::~CivilianActor()
   {
   }

   ////////////////////////////////////////////////
   void CivilianActor::BuildActorComponents()
   {
      BaseClass::BuildActorComponents();
      AddComponent(*new CivilianAIActorComponent);
      if (!HasComponent(dtPhysics::PhysicsActComp::TYPE))
      {
         dtCore::RefPtr<dtPhysics::PhysicsActComp> physAC = new dtPhysics::PhysicsActComp();

         dtCore::RefPtr<dtPhysics::PhysicsObject> physicsObject = new dtPhysics::PhysicsObject("Body");
         physicsObject->SetPrimitiveType(dtPhysics::PrimitiveType::CYLINDER);
         physicsObject->SetMechanicsType(dtPhysics::MechanicsType::KINEMATIC);
         physicsObject->SetCollisionGroup(6);
         physicsObject->SetMass(100.0f);
         physicsObject->SetExtents(osg::Vec3(1.8f, 0.5f, 0.0f));
         physAC->AddPhysicsObject(*physicsObject);
         physAC->SetAutoCreateOnEnteringWorld(true);

         AddComponent(*physAC);
      }

   }


} /* namespace dtExample */
