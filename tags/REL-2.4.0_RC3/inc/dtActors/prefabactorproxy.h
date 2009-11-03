#ifndef prefabactorproxy_h__
#define prefabactorproxy_h__

#include <dtDAL/plugin_export.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtABC/export.h>
#include <dtDAL/actorproperty.h>
#include <osgText/Text>
#include <dtCore/transformable.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT PrefabActor : public dtCore::Transformable
   {
   public:
      typedef dtCore::Transformable BaseClass;

      PrefabActor(const std::string& name = "PrefabActor");

   protected:
      virtual ~PrefabActor();

   private:
   };

   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT PrefabActorProxy : public dtDAL::TransformableActorProxy
   {
   public:
      typedef dtDAL::TransformableActorProxy BaseClass;

      static const dtUtil::RefString CLASS_NAME;

      PrefabActorProxy();

      virtual void CreateActor();

      virtual void BuildPropertyMap();

      void SetPrefab(const std::string& fileName);

      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> >& GetPrefabProxies();

   protected:
      virtual ~PrefabActorProxy();

   private:
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > mProxies;
   };
}
#endif // prefabactorproxy_h__
