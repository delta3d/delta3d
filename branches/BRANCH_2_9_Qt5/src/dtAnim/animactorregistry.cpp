#include <dtAnim/animactorregistry.h>
#include <dtAnim/animationgameactor.h>

#include <dtAnim/animationcomponent.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/animationtransitionplanner.h>

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
const dtGame::ActorComponent::ACType AnimationHelper::TYPE(new dtCore::ActorType("AnimationActorComponent", "ActorComponents",
       "Encapsulates skeletal mesh support and control.",
       dtGame::ActorComponent::BaseActorComponentType));
const dtGame::ActorComponent::ACType AnimationTransitionPlanner::TYPE(new dtCore::ActorType("AnimationStancePlanner", "ActorComponents",
      "An AI planner for chaining animation transitions together for stances and weapons.",
      dtGame::ActorComponent::BaseActorComponentType));

/////////////////////////////////////////////////////////////
const std::string AnimationComponent::DEFAULT_NAME(TYPE->GetName());

AnimActorRegistry::AnimActorRegistry()
   : dtCore::ActorPluginRegistry("dtAnim Core Actors")
{

}

void dtAnim::AnimActorRegistry::RegisterActorTypes()
{
   mActorFactory->RegisterType<AnimationGameActor>(ANIMATION_ACTOR_TYPE.get());
   mActorFactory->RegisterType<AnimationComponent>();
   mActorFactory->RegisterType<AnimationHelper>();
   mActorFactory->RegisterType<AnimationTransitionPlanner>();
}
