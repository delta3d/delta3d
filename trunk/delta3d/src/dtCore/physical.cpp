// physical.cpp: Implementation of the Physical class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/scene.h>
#include <dtCore/object.h>
#include <dtUtil/log.h>
#include <dtUtil/polardecomp.h>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Physical)

Physical::Physical( const std::string& name )
   :  Transformable(name),
      mBodyID(0),
      mDynamicsEnabled(false)
{
   RegisterInstance(this);

   dMassSetSphere(&mMass, 1.0f, 1.0f);

   // Default collision category = 7
   SetCollisionCategoryBits( UNSIGNED_BIT(7) );
}

Physical::~Physical()
{
   if( mBodyID != 0 )
   {
      dBodyDestroy(mBodyID);
   }
   
   DeregisterInstance(this);
}

/**
 * Sets the ODE body identifier associated with this object.  Should
 * only be called by Scene.
 *
 * @param bodyID the new body identifier
 */
void Physical::SetBodyID( dBodyID bodyID )
{
   if( mBodyID != 0 )
   {
      dBodyDestroy(mBodyID);
   }

   mBodyID = bodyID;

   EnableDynamics(mDynamicsEnabled);

   SetMass(&mMass);

   if( mBodyID != 0 )
   {
      // Copy position and rotation of geometry over to 
      const dReal* position = dGeomGetPosition( GetGeomID() );
      dBodySetPosition( mBodyID, position[0], position[1], position[2] );
      const dReal* rotation = dGeomGetRotation( GetGeomID() );
      dBodySetRotation( mBodyID, rotation );
   }

   // This wipes out all previous transforms on the geom, so
   // that's why we had to copy them into the body first.
   dGeomSetBody( GetGeomID(), mBodyID );
}

/**
 * Returns the ODE body identifier associated with this
 * object.
 *
 * @return the object's body identifier
 */
dBodyID Physical::GetBodyID() const
{
   return mBodyID;
}

/**
 * Enables or disables dynamics for this object.
 *
 * @param enable true to enable dynamics, false to disable
 */
void Physical::EnableDynamics(bool enable)
{
   mDynamicsEnabled = enable;

   if(mBodyID != 0)
   {
      if(enable)
      {
         dBodyEnable(mBodyID);
      }
      else
      {
         dBodyDisable(mBodyID);
      }
   }
}

/**
 * Checks whether or not dynamics are enabled for
 * this object.
 *
 * @return true if dynamics are enabled, false otherwise
 */
bool Physical::DynamicsEnabled() const
{
   return mDynamicsEnabled;
}

/**
 * Sets the ODE mass parameters of this object.
 *
 * @param mass a pointer to the mass structure to copy
 */
void Physical::SetMass(const dMass* mass)
{
   mMass = *mass;

   if(mBodyID != 0)
   {
      dBodySetMass(mBodyID, &mMass);
   }
}

/**
 * Retrieves the ODE mass parameters of this object.
 *
 * @param mass a pointer to the mass structure to fill
 */
void Physical::GetMass(dMass* mass) const
{
   *mass = mMass;
}

/**
 * Sets the mass of this object.
 *
 * @param mass the new mass, in kilograms
 */
void Physical::SetMass(float mass)
{
   mMass.mass = mass;

   if(mBodyID != 0)
   {
      dBodySetMass(mBodyID, &mMass);
   }
}

/**
 * Returns the mass of this object.
 *
 * @return the current mass
 */
float Physical::GetMass() const
{
   return mMass.mass;
}

/**
 * Sets this object's center of gravity.
 *
 * @param centerOfGravity the new center of gravity
 */
void Physical::SetCenterOfGravity(const osg::Vec3& centerOfGravity)
{
   mMass.c[0] = centerOfGravity[0];
   mMass.c[1] = centerOfGravity[1];
   mMass.c[2] = centerOfGravity[2];

   if(mBodyID != 0)
   {
      dBodySetMass(mBodyID, &mMass);
   }
}

/**
 * Retrieves this object's center of gravity.
 *
 * @param dest the vector in which to place the center
 * of gravity
 */
void Physical::GetCenterOfGravity(osg::Vec3& dest) const
{
   dest[0] = mMass.c[0];
   dest[1] = mMass.c[1];
   dest[2] = mMass.c[2];
}

/**
 * Sets this object's inertia tensor.
 *
 * @param inertiaTensor the new inertia tensor, only uses the rotation part of the transform matrix
 */

void Physical::SetInertiaTensor(const osg::Matrix& inertiaTensor)
{
   mMass.I[0] = inertiaTensor(0,0);
   mMass.I[1] = inertiaTensor(1,0);
   mMass.I[2] = inertiaTensor(2,0);

   mMass.I[4] = inertiaTensor(0,1);
   mMass.I[5] = inertiaTensor(1,1);
   mMass.I[6] = inertiaTensor(2,1);

   mMass.I[8] = inertiaTensor(0,2);
   mMass.I[9] = inertiaTensor(1,2);
   mMass.I[10] = inertiaTensor(2,2);

   if(mBodyID != 0)
   {
      dBodySetMass(mBodyID, &mMass);
   }
}

/**
 * Retrieves this object's inertia tensor.
 *
 * @param dest the matrix in which to place the inertia
 * tensor, uses only rotation part of the transform matrix
 */
void Physical::GetInertiaTensor(osg::Matrix& dest) const
{
   dest(0,0) = mMass.I[0];
   dest(1,0) = mMass.I[1];
   dest(2,0) = mMass.I[2];

   dest(0,1) = mMass.I[4];
   dest(1,1) = mMass.I[5];
   dest(2,1) = mMass.I[6];

   dest(0,2) = mMass.I[8];
   dest(1,2) = mMass.I[9];
   dest(2,2) = mMass.I[10];
}

/**
 * Updates the state of this object just after a physical
 * simulation step.  Should only be called by dtCore::Scene.
 * The default implementation copies the new object
 * position into the user-accessible transformation.
 */
void Physical::PostPhysicsStepUpdate()
{
   if( DynamicsEnabled() )
   {
      const dReal* position = dGeomGetPosition(GetGeomID());
      const dReal* rotation = dGeomGetRotation(GetGeomID());

      osg::Matrix newRotation;

      newRotation(0,0) = rotation[0];
      newRotation(1,0) = rotation[1];
      newRotation(2,0) = rotation[2];

      newRotation(0,1) = rotation[4];
      newRotation(1,1) = rotation[5];
      newRotation(2,1) = rotation[6];

      newRotation(0,2) = rotation[8];
      newRotation(1,2) = rotation[9];
      newRotation(2,2) = rotation[10];

      mGeomTransform.SetTranslation( position[0], position[1], position[2] );
      mGeomTransform.SetRotation(newRotation);

      this->SetTransform(&mGeomTransform);
   }
}
