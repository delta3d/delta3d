/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation & MOVES Institute
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
 * @author Curtiss Murphy
 * @author Chris Osborn
 */
#include "ActorsRegistry.h"
#include "TankActor.h"
#include <dtCore/shadermanager.h>

dtCore::RefPtr<dtDAL::ActorType> ActorsRegistry::TANK_ACTOR_TYPE( new dtDAL::ActorType( "Tank","MyActors.Tanks","A really cool tank!." ) );

///////////////////////////////////////////////////////////////////////////////
extern "C" TUTORIAL_TANK_EXPORT dtDAL::ActorPluginRegistry* CreatePluginRegistry()
{
   return new ActorsRegistry(); 
}

///////////////////////////////////////////////////////////////////////////////
extern "C" TUTORIAL_TANK_EXPORT void DestroyPluginRegistry(dtDAL::ActorPluginRegistry *registry)
{
   if (registry != NULL)
      delete registry;
}

//////////////////////////////////////////////////////////////////////////
ActorsRegistry::ActorsRegistry() :
   dtDAL::ActorPluginRegistry("TutorialActors")
{
   SetDescription("This is a library of actors used by the Game Manager Tutorials.");
}

//////////////////////////////////////////////////////////////////////////
void ActorsRegistry::RegisterActorTypes()
{
   mActorFactory->RegisterType<TankActorProxy>( TANK_ACTOR_TYPE.get() );
}