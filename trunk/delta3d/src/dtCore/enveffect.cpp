#include "dtCore/enveffect.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(EnvEffect)

EnvEffect::EnvEffect(const std::string name)
{
   SetName(name);
}

EnvEffect::~EnvEffect(void)
{
   DeregisterInstance(this);
}
