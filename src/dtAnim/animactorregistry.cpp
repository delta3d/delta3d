#include <dtAnim/animactorregistry.h>
#include <dtAnim/animationgameactor.h>
#include <dtAnim/cal3dgameactor.h>


using namespace dtAnim;

extern "C" DT_ANIM_EXPORT dtDAL::ActorPluginRegistry* CreatePluginRegistry()
{
   return new AnimActorRegistry;
}

///////////////////////////////////////////////////////////////////////////////
extern "C" DT_ANIM_EXPORT void DestroyPluginRegistry(dtDAL::ActorPluginRegistry* registry)
{
   if (registry != NULL)
   {
      delete registry;
   }
}

dtCore::RefPtr<dtDAL::ActorType> AnimActorRegistry::CAL3D_ACTOR_TYPE(new dtDAL::ActorType("Cal3DGameActor", "dtanim", "Cal3DGameActor"));
dtCore::RefPtr<dtDAL::ActorType> AnimActorRegistry::ANIMATION_ACTOR_TYPE(new dtDAL::ActorType("AnimationGameActor", "dtanim", "AnimationGameActor"));

AnimActorRegistry::AnimActorRegistry()
   : dtDAL::ActorPluginRegistry("CoreActors")
{

}

void dtAnim::AnimActorRegistry::RegisterActorTypes()
{
   mActorFactory->RegisterType<Cal3DGameActorProxy>(CAL3D_ACTOR_TYPE.get());
   mActorFactory->RegisterType<AnimationGameActorProxy>(ANIMATION_ACTOR_TYPE.get());
}
