#include "testactorlibraryregistry.h"
#include "componentgameactor.h"
#include <dtDAL/actorpluginregistry.h>
#include <dtDAL/actortype.h>

dtCore::RefPtr<dtDAL::ActorType> TestActorLibraryRegistry::COMPONENT_GAME_ACTOR_TYPE(new dtDAL::ActorType("ComponentGameActor", "TestActors"));

//////////////////////////////////////////////////////////////////////////
extern "C" TEST_GA_COMPONENTS_EXPORT dtDAL::ActorPluginRegistry* CreatePluginRegistry()
{
   return new TestActorLibraryRegistry;
}

//////////////////////////////////////////////////////////////////////////
extern "C" TEST_GA_COMPONENTS_EXPORT void DestroyPluginRegistry(dtDAL::ActorPluginRegistry* registry)
{
   delete registry;
}

//////////////////////////////////////////////////////////////////////////
TestActorLibraryRegistry::TestActorLibraryRegistry() 
   : dtDAL::ActorPluginRegistry("Component Actors")
{
}

//////////////////////////////////////////////////////////////////////////
void TestActorLibraryRegistry::RegisterActorTypes()
{
   mActorFactory->RegisterType<ComponentGameActorProxy>(COMPONENT_GAME_ACTOR_TYPE.get());
}
