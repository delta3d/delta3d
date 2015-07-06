/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * William E. Johnson II
 */
#include <fireFighter/entityactorregistry.h>
#include <fireFighter/flysequenceactor.h>
#include <fireFighter/fireactor.h>
#include <fireFighter/ddgactor.h>
#include <fireFighter/playeractor.h>
#include <fireFighter/gamelevelactor.h>
#include <fireFighter/firehoseactor.h>
#include <fireFighter/firesuitactor.h>
#include <fireFighter/scbaactor.h>
#include <fireFighter/halonactors.h>
#include <fireFighter/hatchactor.h>
#include <fireFighter/fuelvalveactor.h>

dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_FLY_SEQUENCE(new dtCore::ActorType("FlySequence", "FlySequence", "The actor runs the intro movie"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_FIRE_ACTOR(new dtCore::ActorType("FireActor", "FireActor"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_DDG_ACTOR(new dtCore::ActorType("DDGActor", "DDGActor"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_PLAYER_ACTOR(new dtCore::ActorType("PlayerActor", "PlayerActor"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_GAME_LEVEL_ACTOR(new dtCore::ActorType("GameLevelActor", "GameLevelActor"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_FIRE_HOSE_ACTOR(new dtCore::ActorType("FireHoseActor", "FireHoseActor"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_FIRE_SUIT_ACTOR(new dtCore::ActorType("FireSuitActor", "FireSuitActor"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_SCBA_ACTOR(new dtCore::ActorType("SCBAActor", "SCBAActor"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_PRIMARY_HALON_ACTOR(new dtCore::ActorType("PrimaryHalonActor", "PrimaryHalonActor"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_SECONDARY_HALON_ACTOR(new dtCore::ActorType("SecondaryHalonActor", "SecondaryHalonActor"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_HATCH_ACTOR(new dtCore::ActorType("HatchActor", "HatchActor"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_FUEL_VALVE_ACTOR(new dtCore::ActorType("FuelValveActor", "FuelValveActor"));
dtCore::RefPtr<dtCore::ActorType> EntityActorRegistry::TYPE_COLLIDABLE_ACTOR(new dtCore::ActorType("CollidableActor", "CollidableActor"));

extern "C" FIRE_FIGHTER_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
{
   return new EntityActorRegistry;
}

///////////////////////////////////////////////////////////////////////////////
extern "C" FIRE_FIGHTER_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry *registry)
{
   delete registry;
}

///////////////////////////////////////////////////////////////////////////////
EntityActorRegistry::EntityActorRegistry() : dtCore::ActorPluginRegistry("FireFighterActors")
{
   
}

///////////////////////////////////////////////////////////////////////////////
void EntityActorRegistry::RegisterActorTypes()
{
   mActorFactory->RegisterType<FlySequenceActorProxy>(TYPE_FLY_SEQUENCE.get());
   mActorFactory->RegisterType<FireActor>(TYPE_FIRE_ACTOR.get());
   mActorFactory->RegisterType<DDGActorProxy>(TYPE_DDG_ACTOR.get());
   mActorFactory->RegisterType<PlayerActorProxy>(TYPE_PLAYER_ACTOR.get());
   mActorFactory->RegisterType<GameLevelActorProxy>(TYPE_GAME_LEVEL_ACTOR.get());
   mActorFactory->RegisterType<FireHoseActor>(TYPE_FIRE_HOSE_ACTOR.get());
   mActorFactory->RegisterType<FireSuitActor>(TYPE_FIRE_SUIT_ACTOR.get());
   mActorFactory->RegisterType<SCBAActor>(TYPE_SCBA_ACTOR.get());
   mActorFactory->RegisterType<PrimaryHalonActor>(TYPE_PRIMARY_HALON_ACTOR.get());
   mActorFactory->RegisterType<SecondaryHalonActor>(TYPE_SECONDARY_HALON_ACTOR.get());
   mActorFactory->RegisterType<HatchActor>(TYPE_HATCH_ACTOR.get());
   mActorFactory->RegisterType<FuelValveActor>(TYPE_FUEL_VALVE_ACTOR.get());
   mActorFactory->RegisterType<FuelValveActor>(TYPE_COLLIDABLE_ACTOR.get());
}
