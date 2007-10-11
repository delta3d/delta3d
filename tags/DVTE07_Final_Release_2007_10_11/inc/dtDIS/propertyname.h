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

#ifndef __DELTA_DTDIS_PROPERTY_NAME_H__
#define __DELTA_DTDIS_PROPERTY_NAME_H__

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

#endif  // __DELTA_DTDIS_PROPERTY_NAME_H__

