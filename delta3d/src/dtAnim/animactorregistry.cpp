#include <dtAnim/animactorregistry.h>
#include <dtAnim/animationgameactor.h>

#include <dtAnim/animationcomponent.h>

using namespace dtAnim;

extern "C" DT_ANIM_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
{
   return new AnimActorRegistry;
}

///////////////////////////////////////////////////////////////////////////////
extern "C" DT_ANIM_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry* registry)
{
   if (registry != NULL)
   {
      delete registry;
   }
}

dtCore::RefPtr<dtCore::ActorType> AnimActorRegistry::ANIMATION_ACTOR_TYPE(new dtCore::ActorType("AnimationGameActor", "dtanim", "AnimationGameActor"));

const dtCore::RefPtr<dtCore::SystemComponentType> AnimationComponent::TYPE(new dtCore::SystemComponentType("AnimationComponent","GMComponents",
      "Controls the updating of character animations.  It has a collection of Animation Actor Components.", dtGame::GMComponent::BaseGMComponentType));
/////////////////////////////////////////////////////////////
const std::string AnimationComponent::DEFAULT_NAME(TYPE->GetName());

AnimActorRegistry::AnimActorRegistry()
   : dtCore::ActorPluginRegistry("dtAnim Core Actors")
{

}

void dtAnim::AnimActorRegistry::RegisterActorTypes()
{
   mActorFactory->RegisterType<AnimationGameActorProxy>(ANIMATION_ACTOR_TYPE.get());
   mActorFactory->RegisterType<AnimationComponent>();
}
