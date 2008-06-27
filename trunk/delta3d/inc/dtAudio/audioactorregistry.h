#ifndef audioactorregistry_h__
#define audioactorregistry_h__

#include <dtAudio/export.h>
#include <dtDAL/actorpluginregistry.h>

namespace dtAudio
{
   class DT_AUDIO_EXPORT AudioActorRegistry : public dtDAL::ActorPluginRegistry
   {
   public:
      static dtCore::RefPtr<dtDAL::ActorType> SOUND_ACTOR_TYPE;

      AudioActorRegistry();

      virtual void RegisterActorTypes();

   protected:

   private:
   };

}
#endif // audioactorregistry_h__