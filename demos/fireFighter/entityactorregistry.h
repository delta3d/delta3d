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
#ifndef DELTA_FIRE_FIGHTER_ENTITY_ACTOR_REGISTRY
#define DELTA_FIRE_FIGHTER_ENTITY_ACTOR_REGISTRY

#include <dtCore/actorpluginregistry.h>
#include <fireFighter/export.h>

class FIRE_FIGHTER_EXPORT EntityActorRegistry : public dtCore::ActorPluginRegistry
{
   public:

      static dtCore::RefPtr<dtCore::ActorType> TYPE_FLY_SEQUENCE;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_FIRE_ACTOR;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_DDG_ACTOR;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_PLAYER_ACTOR;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_GAME_LEVEL_ACTOR;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_FIRE_HOSE_ACTOR;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_FIRE_SUIT_ACTOR;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_SCBA_ACTOR;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_PRIMARY_HALON_ACTOR;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_SECONDARY_HALON_ACTOR;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_HATCH_ACTOR;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_FUEL_VALVE_ACTOR;
      static dtCore::RefPtr<dtCore::ActorType> TYPE_COLLIDABLE_ACTOR;

     /**
      * Constructs the engine actor registry.
      */
      EntityActorRegistry();

     /**
      * Registers the actor types representing the core engine objects.
      */
      void RegisterActorTypes();
};

#endif
