#include <prefix/unittestprefix.h>
#include "samplegameactorproxy.h"

using namespace dtTest;

SampleGameActorProxy::SampleGameActorProxy()
{
}

SampleGameActorProxy::~SampleGameActorProxy()
{
}

void SampleGameActorProxy::CreateDrawable()
{
   SetDrawable( *(new dtGame::GameActor(*this)) );
}

void SampleGameActorProxy::BuildPropertyMap()
{
}

void SampleGameActorProxy::BuildInvokables()
{
}
