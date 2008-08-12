/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_ARTICULATION_CONSTANTS_H__
#define __DELTA_DTDIS_ARTICULATION_CONSTANTS_H__

#include <string>                        // for parameter type
#include <dtDIS/dtdisexport.h>           // for library export definitions


namespace dtDIS
{
   /// the scope for articulation specific constants, lacking in the DIS dependency, and other tools.
   namespace Articulation
   {
      enum ParameterTypeDesignator
      {
         ARTICULATED_PART=0,
         ATTACHED_PART=1
      };

      enum PartType
      {
         PART_PRIMARY_TURRET = 4096,
         PART_PRIMARY_GUN = 4416,
         PART_SECONDARY_GUN = 6016
      };

      enum MotionType
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

#endif // __DELTA_DTDIS_ARTICULATION_CONSTANTS_H__
