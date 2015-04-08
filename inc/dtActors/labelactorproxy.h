#ifndef labelactorproxy_h__
#define labelactorproxy_h__

#include <dtCore/plugin_export.h>
#include <dtCore/transformableactorproxy.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT LabelActorProxy : public dtCore::TransformableActorProxy
   {
   public:
      typedef dtCore::TransformableActorProxy BaseClass;

      static const dtUtil::RefString CLASS_NAME;

      LabelActorProxy();

      virtual void CreateDrawable();

      virtual void BuildPropertyMap();

   protected:
      virtual ~LabelActorProxy();

   private:
   };

}
#endif // labelactorproxy_h__
