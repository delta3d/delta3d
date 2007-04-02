#include <dtDIS/entityidcompare.h>
#include <DIS/EntityID.h>

using namespace dtDIS::details;

bool EntityIDCompare::operator ()(const DIS::EntityID& lhs, const DIS::EntityID& rhs) const
{
   return( lhs.getEntity()      < rhs.getEntity()
        || lhs.getSite()        < rhs.getSite()
        || lhs.getApplication() < rhs.getApplication() );
}
