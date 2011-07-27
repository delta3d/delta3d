#include <dtAudio/audioactorregistry.h>
#include <dtAudio/soundactorproxy.h>

using namespace dtAudio;

extern "C" DT_AUDIO_EXPORT dtDAL::ActorPluginRegistry* CreatePluginRegistry()
{
   return new AudioActorRegistry;
}

///////////////////////////////////////////////////////////////////////////////
extern "C" DT_AUDIO_EXPORT void DestroyPluginRegistry(
   dtDAL::ActorPluginRegistry *registry)
{
   if (registry != NULL)
      delete registry;
}


dtCore::RefPtr<dtDAL::ActorType> AudioActorRegistry::SOUND_ACTOR_TYPE(new dtDAL::ActorType("Sound Actor", "dtcore.Environment","Generic Sound Actor"));

dtAudio::AudioActorRegistry::AudioActorRegistry():
 dtDAL::ActorPluginRegistry("CoreActors")
{

}

void dtAudio::AudioActorRegistry::RegisterActorTypes()
{
   // sound actor
   mActorFactory->RegisterType<dtAudio::SoundActorProxy>(SOUND_ACTOR_TYPE.get());
}
