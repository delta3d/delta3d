#include "samplegameactorproxy.h"

using namespace dtTest;

SampleGameActorProxy::SampleGameActorProxy()
{
}

SampleGameActorProxy::~SampleGameActorProxy()
{
}

void SampleGameActorProxy::CreateActor()
{
   SetActor( *(new dtGame::GameActor(*this)) );
}

void SampleGameActorProxy::BuildPropertyMap()
{
}

void SampleGameActorProxy::BuildInvokables()
{
}
