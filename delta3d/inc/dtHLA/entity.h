/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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
*/

#ifndef DELTA_ENTITY
#define DELTA_ENTITY

// entity.h: Declaration of the Entity class.
//
//////////////////////////////////////////////////////////////////////


#include <string>

#include <dtHLA/dis_types.h>
#include <dtCore/object.h>

namespace dtHLA
{
   /**
    * A DIS/RPR-FOM entity.
    */
   class DT_HLA_EXPORT Entity : public dtCore::Object
   {
      DECLARE_MANAGEMENT_LAYER(Entity)

      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         Entity(const std::string& name = "Entity");

         /**
          * Destructor.
          */
         virtual ~Entity();

         /**
          * Sets this entity's DIS/RPR-FOM entity identifier.
          *
          * @param entityIdentifier the entity identifier to copy
          */
         void SetEntityIdentifier(const EntityIdentifier& entityIdentifier);

         /**
          * Retrieves this entity's DIS/RPR-FOM entity identifier.
          *
          * @return the entity identitifier
          */
         const EntityIdentifier& GetEntityIdentifier();

         /**
          * Sets this entity's DIS/RPR-FOM entity type.
          *
          * @param entityType the entity type to copy
          */
         void SetEntityType(const EntityType& entityType);

         /**
          * Retrieves this entity's DIS/RPR-FOM entity type.
          *
          * @return the entity type
          */
         const EntityType& GetEntityType();
         
         /**
          * Sets this entity's DIS/RPR-FOM world location.
          *
          * @param worldLocation the world location to copy
          */
         void SetWorldLocation(const WorldCoordinate& worldLocation);

         /**
          * Retrieves this entity's DIS/RPR-FOM world location.
          *
          * @return the world location
          */
         const WorldCoordinate& GetWorldLocation();
         
         /**
          * Sets this entity's DIS/RPR-FOM orientation.
          *
          * @param orientation the orientation to copy
          */
         void SetOrientation(const EulerAngles& orientation);

         /**
          * Retrieves this entity's DIS/RPR-FOM orientation.
          *
          * @return the orientation
          */
         const EulerAngles& GetOrientation();
         
         /**
          * Sets this entity's DIS/RPR-FOM velocity vector.
          *
          * @param velocityVector the velocity vector to copy
          */
         void SetVelocityVector(const VelocityVector& velocityVector);

         /**
          * Retrieves this entity's DIS/RPR-FOM velocity vector.
          *
          * @return the velocity vector
          */
         const VelocityVector& GetVelocityVector();
         
         /**
          * Sets this entity's DIS/RPR-FOM acceleration vector.
          *
          * @param accelerationVector the acceleration vector to copy
          */
         void SetAccelerationVector(const VelocityVector& accelerationVector);

         /**
          * Retrieves this entity's DIS/RPR-FOM acceleration vector.
          *
          * @return the acceleration vector
          */
         const VelocityVector& GetAccelerationVector();
         
         /**
          * Sets this entity's DIS/RPR-FOM angular velocity vector.
          *
          * @param angularVelocityVector the angular velocity vector to copy
          */
         void SetAngularVelocityVector(const VelocityVector& angularVelocityVector);

         /**
          * Retrieves this entity's DIS/RPR-FOM angular velocity vector.
          *
          * @return the angular velocity vector
          */
         const VelocityVector& GetAngularVelocityVector();

         /**
          * Sets this entity's DIS/RPR-FOM articulated parameters array.
          *
          * @param array the articulated parameter array to copy
          */
         void SetArticulatedParametersArray(const std::vector<ArticulatedParameter>& array);
         
         /**
          * Retrieves this entity's DIS/RPR-FOM articulated parameters array.
          *
          * @return the articulated parameter array
          */
         const std::vector<ArticulatedParameter>& GetArticulatedParametersArray();
         
         /**
          * Sets this entity's DIS/RPR-FOM damage state.
          *
          * @param damageState the damage state
          */
         void SetDamageState(DamageState damageState);
         
         /**
          * Returns this entity's DIS/RPR-FOM damage state.
          *
          * @return the damage state
          */
         DamageState GetDamageState();


      private:

         /**
          * The entity's DIS/RPR-FOM entity identifier.
          */
         EntityIdentifier mEntityIdentifier;

         /**
          * The entity's DIS/RPR-FOM entity type.
          */
         EntityType mEntityType;
         
         /**
          * The entity's DIS/RPR-FOM world location.
          */
         WorldCoordinate mWorldLocation;
         
         /**
          * The entity's DIS/RPR-FOM orientation.
          */
         EulerAngles mOrientation;
         
         /**
          * The entity's DIS/RPR-FOM velocity vector.
          */
         VelocityVector mVelocityVector;
         
         /**
          * The entity's DIS/RPR-FOM acceleration vector.
          */
         VelocityVector mAccelerationVector;
         
         /**
          * The entity's DIS/RPR-FOM angular velocity vector.
          */
         VelocityVector mAngularVelocityVector;
         
         /**
          * The entity's DIS/RPR-FOM articulated parameter array.
          */
         std::vector<ArticulatedParameter> mArticulatedParametersArray;
         
         /**
          * The entity's DIS/RPR-FOM damage state.
          */
         DamageState mDamageState;
   };
};


#endif // DELTA_ENTITY
