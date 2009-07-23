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
#include <string>
#include <dtUtil/refstring.h>

namespace dtDIS
{
   struct DT_DIS_EXPORT EntityPropertyName
   {
      /// Contains the official ActorProperty names for DIS::EntityStatePdu attributes.

      // entityid stuff
      //static const char ENTITYID[];

      //// physical stuff
      //static const char LINEAR_VELOCITY[];

      static dtUtil::RefString APPEARANCE;

      // entitytype stuff
      static dtUtil::RefString ENTITY_TYPE;
   } ;

   struct DT_DIS_EXPORT EnginePropertyName
   {
      // Contains the official ActorProperty names that supported engine components respond to.


      static dtUtil::RefString ENTITY_MARKING;
      ///Outgoing DIS packets read from this Property for the entity's XYZ translation
      static dtUtil::RefString ENTITY_LOCATION; 
      
      ///Outgoing DIS packets read from this Property for the entity's XYZ axis rotation (not HPR)
      static dtUtil::RefString ENTITY_ORIENTATION;

      /**Incoming DIS packets apply entity XYZ translation to this Property.  This is used
       * by the dtGame::DeadReckoningComponent to update the Delta3D actor's position.  If
       * no DeadReckoningComponent is used, set this variable to be the same as ENTITY_LOCATION.
       */
      static dtUtil::RefString LAST_KNOWN_LOCATION;

      /**Incoming DIS packets apply entity orientation to this Property.  This is used
      * by the dtGame::DeadReckoningComponent to update the Delta3D actor's rotation.  If
      * no DeadReckoningComponent is used, set this variable to be the same as ENTITY_ORIENTATION.
      */
      static dtUtil::RefString LAST_KNOWN_ORIENTATION;

      static dtUtil::RefString ENTITY_LINEARY_VELOCITY;
      //static dtUtil::RefString ACCELERATION;

      static dtUtil::RefString DEAD_RECKONING_ALGORITHM;
      static dtUtil::RefString GROUND_CLAMP;

      static dtUtil::RefString RESOURCE_DAMAGE_OFF;
      static dtUtil::RefString RESOURCE_DAMAGE_ON;
      static dtUtil::RefString RESOURCE_DAMAGE_DESTROYED;

      static dtUtil::RefString ARTICULATION;
      static dtUtil::RefString DOF_NODE_NAME;
   } ;

   //struct DT_DIS_EXPORT HLABaseEntityPropertyName
   //{
      // HLA based actor properties from BMH's SimCore.

      // Contains the official ActorProperty names that SimCore 
      // supported engine components respond to, namely that actor library.
      // Additionally, these are the base entity class properties only.

      //static const std::string PROPERTY_ACCELERATION_VECTOR        ;
      //static const std::string PROPERTY_ANGULAR_VELOCITY_VECTOR    ;
      //static const std::string PROPERTY_ENGINE_SMOKE_POSITION      ;
      //static const std::string PROPERTY_ENGINE_SMOKE_ON            ;
      //static const std::string PROPERTY_FLAMES_PRESENT             ;
      //static const std::string PROPERTY_SMOKE_PLUME_PRESENT        ;
      //static const std::string PROPERTY_ENGINE_POSITION            ;
      //static const std::string PROPERTY_FLYING                     ;
      //static const std::string PROPERTY_DAMAGE_STATE               ;
      //static const std::string PROPERTY_DEFAULT_SCALE              ;
      //static const std::string PROPERTY_SCALE_MAGNIFICATION_FACTOR ;
      //static const std::string PROPERTY_MODEL_SCALE                ;
      //static const std::string PROPERTY_MODEL_ROTATION             ;
   //} ;
}

#endif  // __DELTA_DTDIS_PROPERTY_NAME_H__

