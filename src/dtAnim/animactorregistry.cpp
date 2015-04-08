#include <dtAnim/animactorregistry.h>
#include <dtAnim/animationgameactor.h>
#include <dtAnim/cal3dgameactor.h>


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

dtCore::RefPtr<dtCore::ActorType> AnimActorRegistry::CAL3D_ACTOR_TYPE(new dtCore::ActorType("Cal3DGameActor", "dtanim", "Cal3DGameActor"));
dtCore::RefPtr<dtCore::ActorType> AnimActorRegistry::ANIMATION_ACTOR_TYPE(new dtCore::ActorType("AnimationGameActor", "dtanim", "AnimationGameActor"));

AnimActorRegistry::AnimActorRegistry()
   : dtCore::ActorPluginRegistry("CoreActors")
{

}

void dtAnim::AnimActorRegistry::RegisterActorTypes()
{
   mActorFactory->RegisterType<Cal3DGameActorProxy>(CAL3D_ACTOR_TYPE.get());
   mActorFactory->RegisterType<AnimationGameActor>(ANIMATION_ACTOR_TYPE.get());
}
