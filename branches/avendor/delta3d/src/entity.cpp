// entity.cpp: Implementation of the Entity class.
//
//////////////////////////////////////////////////////////////////////

#include "entity.h"
#include "notify.h"

using namespace dtCore;
using namespace dtHLA;
using namespace std;


IMPLEMENT_MANAGEMENT_LAYER(Entity)


/**
 * Constructor.
 *
 * @param name the instance name
 */
Entity::Entity(string name)
   : Object(name)
{}

/**
 * Destructor.
 */
Entity::~Entity()
{}

/**
 * Sets this entity's DIS/RPR-FOM entity identifier.
 *
 * @param entityIdentifier the entity identifier to copy
 */
void Entity::SetEntityIdentifier(const EntityIdentifier& entityIdentifier)
{
   mEntityIdentifier = entityIdentifier;
}

/**
 * Retrieves this entity's DIS/RPR-FOM entity identifier.
 *
 * @return the entity identitifier
 */
const EntityIdentifier& Entity::GetEntityIdentifier()
{
   return mEntityIdentifier;
}

/**
 * Sets this entity's DIS/RPR-FOM entity type.
 *
 * @param entityType the entity type to copy
 */
void Entity::SetEntityType(const EntityType& entityType)
{
   mEntityType = entityType;
}

/**
 * Retrieves this entity's DIS/RPR-FOM entity type.
 *
 * @return the entity type
 */
const EntityType& Entity::GetEntityType()
{
   return mEntityType;
}

/**
 * Sets this entity's DIS/RPR-FOM world location.
 *
 * @param worldLocation the world location to copy
 */
void Entity::SetWorldLocation(const WorldCoordinate& worldLocation)
{
   mWorldLocation = worldLocation;
}

/**
 * Retrieves this entity's DIS/RPR-FOM world location.
 *
 * @return the world location
 */
const WorldCoordinate& Entity::GetWorldLocation()
{
   return mWorldLocation;
}

/**
 * Sets this entity's DIS/RPR-FOM orientation.
 *
 * @param orientation the orientation to copy
 */
void Entity::SetOrientation(const EulerAngles& orientation)
{
   mOrientation = orientation;
}

/**
 * Retrieves this entity's DIS/RPR-FOM orientation.
 *
 * @return the orientation
 */
const EulerAngles& Entity::GetOrientation()
{
   return mOrientation;
}

/**
 * Sets this entity's DIS/RPR-FOM velocity vector.
 *
 * @param velocityVector the velocity vector to copy
 */
void Entity::SetVelocityVector(const VelocityVector& velocityVector)
{
   mVelocityVector = velocityVector;
}

/**
 * Retrieves this entity's DIS/RPR-FOM velocity vector.
 *
 * @return the velocity vector
 */
const VelocityVector& Entity::GetVelocityVector()
{
   return mVelocityVector;
}

/**
 * Sets this entity's DIS/RPR-FOM acceleration vector.
 *
 * @param accelerationVector the acceleration vector to copy
 */
void Entity::SetAccelerationVector(const VelocityVector& accelerationVector)
{
   mAccelerationVector = accelerationVector;
}

/**
 * Retrieves this entity's DIS/RPR-FOM acceleration vector.
 *
 * @return the acceleration vector
 */
const VelocityVector& Entity::GetAccelerationVector()
{
   return mAccelerationVector;
}

/**
 * Sets this entity's DIS/RPR-FOM angular velocity vector.
 *
 * @param angularVelocityVector the angular velocity vector to copy
 */
void Entity::SetAngularVelocityVector(const VelocityVector& angularVelocityVector)
{
   mAngularVelocityVector = angularVelocityVector;
}

/**
 * Retrieves this entity's DIS/RPR-FOM angular velocity vector.
 *
 * @return the angular velocity vector
 */
const VelocityVector& Entity::GetAngularVelocityVector()
{
   return mAngularVelocityVector;
}