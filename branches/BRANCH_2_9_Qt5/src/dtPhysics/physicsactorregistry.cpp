/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2007, Alion Science and Technology, BMH Operation
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
* Allen Danklefsen
* David Guthrie
*/
#include <dtPhysics/physicsactorregistry.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/physicsactcomp.h>
// For the autoreg
#include <dtCore/actorfactory.h>

namespace dtPhysics
{
   dtCore::RefPtr<dtCore::ActorType> PhysicsActorRegistry::PHYSICS_MATERIAL_ACTOR_TYPE(new dtCore::ActorType
      ("PhysicsMaterial", "dtPhysics", "Friction, restitution, and other physics material properties - assign this to other actors to get material properties."));

   const dtGame::ActorComponent::ACType PhysicsActComp::TYPE(new dtCore::ActorType("PhysicsActComp", "ActorComponents",
         "Physics subsystem actor component.  Requires a GM level PhysicsComponent",
         dtGame::ActorComponent::BaseActorComponentType));

   // Must be after the types or it will crash.
   //dtCore::AutoLibraryRegister<PhysicsActorRegistry> gAutoReg("dtPhysics");

   ////////////////////////////////////////////////////////////////////////////
   extern "C" DT_PHYSICS_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
   {
      return new dtPhysics::PhysicsActorRegistry;
   }

   ////////////////////////////////////////////////////////////////////////////
   extern "C" DT_PHYSICS_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry* registry)
   {
      delete registry;
   }

   ////////////////////////////////////////////////////////////////////////////
   PhysicsActorRegistry::PhysicsActorRegistry() : dtCore::ActorPluginRegistry("dtPhysics", "This library will store Physics Material actors")
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysicsActorRegistry::RegisterActorTypes()
   {
      mActorFactory->RegisterType<MaterialActor>(PHYSICS_MATERIAL_ACTOR_TYPE.get());
      mActorFactory->RegisterType<PhysicsActComp>();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysicsActorRegistry::GetReplacementActorTypes(ActorTypeReplacements& replacements) const
   {
       replacements.push_back(std::make_pair("Physics Actors.Physics Material", "dtPhysics.PhysicsMaterial"));
   }

}
