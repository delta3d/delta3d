#ifndef _dtdis_articulation_constants_h_
#define _dtdis_articulation_constants_h_

#include <string>                        // for parameter type
#include <dtDIS/dtdisexport.h>           // for library export definitions


namespace dtDIS
{
   /// the scope for articulation specific constants, lacking in the DIS dependency, and other tools.
   namespace Articulation
   {
      enum DT_DIS_EXPORT ParameterTypeDesignator
      {
         ARTICULATED_PART=0,
         ATTACHED_PART=1
      };

      enum DT_DIS_EXPORT PartType
      {
         PART_PRIMARY_TURRET = 4096,
         PART_PRIMARY_GUN = 4416,
         PART_SECONDARY_GUN = 6016
      };

      enum DT_DIS_EXPORT MotionType
      {
         MOTION_AZIMUTH = 11,
         MOTION_AZIMUTH_RATE = 12,
         MOTION_ELEVATION = 13
      };

      /// a static definition of the scene graph node names to be used for articulated parts.
      struct DT_DIS_EXPORT NodeName
      {
      public:
         static const char NODE_PRIMARY_TURRET[];
         static const char NODE_PRIMARY_GUN[];
         static const char NODE_SECONDARY_GUN[];
      };
   }
}

#endif // _dtdis_articulation_constants_h_
