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
 * Michael Guerrero
 */

#include "testactorlibraryregistry.h"
#include "proceduralanimationactor.h"

#include <dtCore/actorpluginregistry.h>
#include <dtCore/actortype.h>

dtCore::RefPtr<dtCore::ActorType> TestActorLibraryRegistry::IK_ACTOR_TYPE(new dtCore::ActorType("IKActor", "TestActors"));

//////////////////////////////////////////////////////////////////////////
extern "C" TEST_ANIM_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
{
   return new TestActorLibraryRegistry;
}

//////////////////////////////////////////////////////////////////////////
extern "C" TEST_ANIM_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry* registry)
{
   delete registry;
}

//////////////////////////////////////////////////////////////////////////
TestActorLibraryRegistry::TestActorLibraryRegistry()
   : dtCore::ActorPluginRegistry("Procedural Animation Actors")
{
}

//////////////////////////////////////////////////////////////////////////
void TestActorLibraryRegistry::RegisterActorTypes()
{
   mActorFactory->RegisterType<ProceduralAnimationActorProxy>(IK_ACTOR_TYPE.get());
}
