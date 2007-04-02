#ifndef _plugin_entity_id_compare_h_
#define _plugin_entity_id_compare_h_

#include <dtDIS/dtdisexport.h>    // for library export symbols

namespace DIS
{
   class EntityID;
}

namespace dtDIS
{
   namespace details
   {
      /// useful for sorting rather than using a '<' operator.
      struct DT_DIS_EXPORT EntityIDCompare
      {
         bool operator ()(const DIS::EntityID& lhs, const DIS::EntityID& rhs) const;
      };
   }
}

#endif  // _plugin_entity_id_compare_h_
