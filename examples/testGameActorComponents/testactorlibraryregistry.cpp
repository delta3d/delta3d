#include "testactorlibraryregistry.h"
#include "componentgameactor.h"
#include <dtCore/actorpluginregistry.h>
#include <dtCore/actortype.h>

dtCore::RefPtr<dtCore::ActorType> TestActorLibraryRegistry::COMPONENT_GAME_ACTOR_TYPE(new dtCore::ActorType("ComponentGameActor", "TestActors"));

////////////////////////////////////////////////////////////////////////////////
extern "C" TEST_GA_COMPONENTS_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
{
   return new TestActorLibraryRegistry;
}

////////////////////////////////////////////////////////////////////////////////
extern "C" TEST_GA_COMPONENTS_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry* registry)
{
   delete registry;
}

////////////////////////////////////////////////////////////////////////////////
TestActorLibraryRegistry::TestActorLibraryRegistry() 
   : dtCore::ActorPluginRegistry("Component Actors")
{
}

////////////////////////////////////////////////////////////////////////////////
void TestActorLibraryRegistry::RegisterActorTypes()
{
   mActorFactory->RegisterType<ComponentGameActor>(COMPONENT_GAME_ACTOR_TYPE.get());
}
