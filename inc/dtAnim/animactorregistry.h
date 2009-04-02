#ifndef animactorregistry_h__
#define animactorregistry_h__
#include <dtDAL/actorpluginregistry.h>
#include <dtAnim/export.h>

namespace dtAnim
{

   class DT_ANIM_EXPORT AnimActorRegistry : public dtDAL::ActorPluginRegistry
   {
   public:
      static dtCore::RefPtr<dtDAL::ActorType> CAL3D_ACTOR_TYPE;       
      static dtCore::RefPtr<dtDAL::ActorType> ANIMATION_ACTOR_TYPE;

      AnimActorRegistry();

      virtual void RegisterActorTypes();

   protected:

   private:
   };
}
#endif // animactorregistry_h__
