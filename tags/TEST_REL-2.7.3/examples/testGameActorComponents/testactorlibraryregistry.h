#ifndef TEST_ACTOR_LIBRARY_ACTOR_REGISTRY_H_
#define TEST_ACTOR_LIBRARY_ACTOR_REGISTRY_H_

#include <dtCore/actorpluginregistry.h>

#include "testexport.h"

class TEST_GA_COMPONENTS_EXPORT TestActorLibraryRegistry : public dtCore::ActorPluginRegistry
{
public:
   static dtCore::RefPtr<dtCore::ActorType> COMPONENT_GAME_ACTOR_TYPE;

   TestActorLibraryRegistry();

   void RegisterActorTypes();
};


#endif // TEST_ACTOR_LIBRARY_ACTOR_REGISTRY_H_
