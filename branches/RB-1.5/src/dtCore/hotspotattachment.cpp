#include <prefix/dtcoreprefix-src.h>
#include <dtCore/hotspotattachment.h>
#include <dtCore/transformable.h>
#include <cstddef>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(HotSpotAttachment)

HotSpotAttachment::HotSpotAttachment(const dtUtil::HotSpotDefinition& definition)
   : dtCore::Transformable( definition.mName )
   , mDefinition(definition)
{
}

HotSpotAttachment::~HotSpotAttachment()
{
}

const dtUtil::HotSpotDefinition& HotSpotAttachment::GetDefinition() const
{
   return mDefinition;
}
