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
   struct DT_DIS_EXPORT EntityPropertyName
   {
      /// Contains the official ActorProperty names for DIS::EntityStatePdu attributes.

      // entityid stuff
      static const char ENTITYID[];

      //// physical stuff
      //static const char LINEAR_VELOCITY[];

      static const char APPEARANCE[];
      // entitytype stuff
   } ;

   struct DT_DIS_EXPORT EnginePropertyName
   {
      // Contains the official ActorProperty names that supported engine components respond to.

      static const char ROTATION                  [] ;
      static const char TRANSLATION               [] ;
      static const char LAST_KNOWN_TRANSLATION    [] ;
      static const char LAST_KNOWN_ROTATION       [] ;

      static const char VELOCITY                  [] ;
      static const char ACCELERATION              [] ;

      static const char DEAD_RECKONING_ALGORITHM  [] ;
      static const char GROUND_CLAMP              [] ;

      static const char RESOURCE_DAMAGE_OFF       [] ;
      static const char RESOURCE_DAMAGE_ON        [] ;
      static const char RESOURCE_DAMAGE_DESTROYED [] ;

      static const char ARTICULATION              [] ;
      static const char DOF_NODE_NAME             [] ;
   } ;

   struct DT_DIS_EXPORT HLABaseEntityPropertyName
   {
      // HLA based actor properties from BMH's SimCore.

      // Contains the official ActorProperty names that SimCore 
      // supported engine components respond to, namely that actor library.
      // Additionally, these are the base entity class properties only.

      static const char PROPERTY_LAST_KNOWN_TRANSLATION     [] ;
      static const char PROPERTY_LAST_KNOWN_ROTATION        [] ;
      static const char PROPERTY_VELOCITY_VECTOR            [] ;
      static const char PROPERTY_ACCELERATION_VECTOR        [] ;

      static const char PROPERTY_ANGULAR_VELOCITY_VECTOR    [] ;
      static const char PROPERTY_ENGINE_SMOKE_POSITION      [] ;
      static const char PROPERTY_ENGINE_SMOKE_ON            [] ;
      static const char PROPERTY_FLAMES_PRESENT             [] ;
      static const char PROPERTY_SMOKE_PLUME_PRESENT        [] ;
      static const char PROPERTY_ENGINE_POSITION            [] ;
      static const char PROPERTY_FLYING                     [] ;
      static const char PROPERTY_DAMAGE_STATE               [] ;
      static const char PROPERTY_DEFAULT_SCALE              [] ;
      static const char PROPERTY_SCALE_MAGNIFICATION_FACTOR [] ;
      static const char PROPERTY_MODEL_SCALE                [] ;
      static const char PROPERTY_MODEL_ROTATION             [] ;
   } ;
}

#endif  // __DELTA_DTDIS_PROPERTY_NAME_H__

