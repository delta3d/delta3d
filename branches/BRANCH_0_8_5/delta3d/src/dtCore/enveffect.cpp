#include "dtCore/enveffect.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(EnvEffect)

EnvEffect::EnvEffect(const std::string name)
{
   RegisterInstance(this);
   SetName(name);
}

EnvEffect::~EnvEffect(void)
{
   DeregisterInstance(this);
}
