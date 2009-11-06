#include <dtDIS/entityidcompare.h>
#include <DIS/EntityID.h>

using namespace dtDIS::details;

bool EntityIDCompare::operator ()(const DIS::EntityID& lhs, const DIS::EntityID& rhs) const
{
   if (lhs.getApplication() != rhs.getApplication())
      return lhs.getApplication() < rhs.getApplication();
   else if (lhs.getSite() != rhs.getSite())
      return lhs.getSite() < rhs.getSite();
   else if (lhs.getEntity() != rhs.getEntity())
      return lhs.getEntity() < rhs.getEntity();
   else
      return false;
}
