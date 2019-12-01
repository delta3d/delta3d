#ifndef animactorregistry_h__
#define animactorregistry_h__
#include <dtCore/actorpluginregistry.h>
#include <dtAnim/export.h>

namespace dtAnim
{

   class DT_ANIM_EXPORT AnimActorRegistry : public dtCore::ActorPluginRegistry
   {
   public:
      static dtCore::RefPtr<dtCore::ActorType> CAL3D_ACTOR_TYPE;       
      static dtCore::RefPtr<dtCore::ActorType> ANIMATION_ACTOR_TYPE;

      AnimActorRegistry();

      virtual void RegisterActorTypes();

   protected:

   private:
   };
}
#endif // animactorregistry_h__
