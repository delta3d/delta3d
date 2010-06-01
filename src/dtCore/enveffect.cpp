#include <prefix/dtcoreprefix.h>
#include <dtCore/enveffect.h>

namespace dtCore
{
   IMPLEMENT_MANAGEMENT_LAYER(EnvEffect)

   EnvEffect::EnvEffect( const std::string& name )
   {
      RegisterInstance(this);
      SetName(name);
   }

   EnvEffect::~EnvEffect()
   {
      DeregisterInstance(this);
   }
}
