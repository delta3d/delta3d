#include <dtAudio/audioactorregistry.h>
#include <dtAudio/soundactor.h>
#include <dtAudio/soundcomponent.h>

namespace dtAudio
{

   ///////////////////////////////////////////////////////////////////////////////
   extern "C" DT_AUDIO_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
   {
      return new AudioActorRegistry;
   }

   ///////////////////////////////////////////////////////////////////////////////
   extern "C" DT_AUDIO_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry* registry)
   {
      if (registry != NULL)
         delete registry;
   }


   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtCore::ActorType> AudioActorRegistry::SOUND_ACTOR_TYPE(new dtCore::ActorType("Sound Actor", "dtcore.Environment","Generic Sound Actor"));

   ///////////////////////////////////////////////////////////////////////////////
   const dtCore::RefPtr<dtCore::SystemComponentType> SoundComponent::TYPE(new dtCore::SystemComponentType("SoundComponent","GMComponents",
         "Sound manager component.  It allows one to register sounds, find them, and play them without needing the manage the objects themselves.",
         dtGame::GMComponent::BaseGMComponentType));
   const dtUtil::RefString SoundComponent::DEFAULT_NAME(TYPE->GetName());


   ///////////////////////////////////////////////////////////////////////////////
   dtAudio::AudioActorRegistry::AudioActorRegistry():
       dtCore::ActorPluginRegistry("CoreActors")
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   void dtAudio::AudioActorRegistry::RegisterActorTypes()
   {
      // sound actor
      mActorFactory->RegisterType<dtAudio::SoundActor>(SOUND_ACTOR_TYPE.get());
   }
}
