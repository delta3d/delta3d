#include <dtAudio/audioactorregistry.h>
#include <dtAudio/soundactor.h>

using namespace dtAudio;

extern "C" DT_AUDIO_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
{
   return new AudioActorRegistry;
}

///////////////////////////////////////////////////////////////////////////////
extern "C" DT_AUDIO_EXPORT void DestroyPluginRegistry(
   dtCore::ActorPluginRegistry *registry)
{
   if (registry != NULL)
      delete registry;
}


dtCore::RefPtr<dtCore::ActorType> AudioActorRegistry::SOUND_ACTOR_TYPE(new dtCore::ActorType("Sound Actor", "dtcore.Environment","Generic Sound Actor"));

dtAudio::AudioActorRegistry::AudioActorRegistry():
 dtCore::ActorPluginRegistry("CoreActors")
{

}

void dtAudio::AudioActorRegistry::RegisterActorTypes()
{
   // sound actor
   mActorFactory->RegisterType<dtAudio::SoundActor>(SOUND_ACTOR_TYPE.get());
}
