#ifndef singletonmanager_h__
#define singletonmanager_h__

#include <dtCore/export.h>

namespace dtCore
{
   ///Used as a single point of control for Singletons
   class DT_CORE_EXPORT SingletonManager
   {
   public:

      ///Destroy all the Singletons that SingletonManager knows about.
      static void Destroy();

   protected:
      SingletonManager();
      ~SingletonManager();
   };
}
#endif // singletonmanager_h__
