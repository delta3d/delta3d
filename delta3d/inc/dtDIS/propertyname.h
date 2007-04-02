#ifndef _dt_dis_property_name_h_
#define _dt_dis_property_name_h_

#include <dtDIS/dtdisexport.h>      // for export symbols

namespace dtDIS
{
   /// Contains the official ActorProperty names for DIS::EntityStatePdu attributes.
   struct DT_DIS_EXPORT EntityPropertyName
   {
      // entityid stuff
      static const char ENTITYID[];

      //// physical stuff
      //static const char LINEAR_VELOCITY[];

      static const char APPEARANCE[];
      // entitytype stuff
   };

   /// Contains the official ActorProperty names that supported engine components respond to.
   struct DT_DIS_EXPORT EnginePropertyName
   {
      static const char ROTATION[];
      static const char TRANSLATION[];
      static const char LAST_KNOWN_TRANSLATION[];
      static const char LAST_KNOWN_ROTATION[];

      static const char VELOCITY[];
      static const char ACCELERATION[];

      static const char DEAD_RECKONING_ALGORITHM[];
      static const char GROUND_CLAMP[];

      static const char RESOURCE_DAMAGE_OFF[];
      static const char RESOURCE_DAMAGE_ON[];
      static const char RESOURCE_DAMAGE_DESTROYED[];

      static const char ARTICULATION[];
      static const char DOF_NODE_NAME[];
   };
}

#endif  // _dt_dis_property_name_h_

