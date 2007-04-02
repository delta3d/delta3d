#ifndef _dtdis_entity_type_constants_h_
#define _dtdis_entity_type_constants_h_

#include <dtDIS/dtdisexport.h>           // for library export definitions

namespace dtDIS
{
   enum DT_DIS_EXPORT DomainType
   {
      DOMAIN_LAND = 1,
      DOMAIN_AIR = 2,
      DOMAIN_SURFACE = 3,
      DOMAIN_SUBSURFACE = 4,
      DOMAIN_SPACE = 5,
   };
}

#endif // _dtdis_entity_type_constants_h_
