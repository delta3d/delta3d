#ifndef labelactorproxy_h__
#define labelactorproxy_h__

#include <dtDAL/plugin_export.h>
#include <dtDAL/transformableactorproxy.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT LabelActorProxy : public dtDAL::TransformableActorProxy
   {
   public:
      typedef dtDAL::TransformableActorProxy BaseClass;

      static const dtUtil::RefString CLASS_NAME;

      LabelActorProxy();

      virtual void CreateActor();

      virtual void BuildPropertyMap();

   protected:
      virtual ~LabelActorProxy();

   private:
   };

}
#endif // labelactorproxy_h__
