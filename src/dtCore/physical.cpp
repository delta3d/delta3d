// physical.cpp: Implementation of the Physical class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/physical.h>
#include <dtCore/odebodywrap.h>
#include <dtCore/collisioncategorydefaults.h>
#include <dtCore/transform.h>
#include <ode/collision.h>
#include <ode/objects.h>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Physical)

Physical::Physical( const std::string& name )
   :  Transformable(name),
      mBodyWrap(new ODEBodyWrap())
{
   Ctor();
}

Physical::Physical( TransformableNode &node, const std::string &name )
   : Transformable(node, name),
      mBodyWrap(new ODEBodyWrap())
{
   Ctor();
}

Physical::~Physical()
{
   mBodyWrap = NULL;

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
   mBodyWrap->SetBodyID(bodyID);

   if( bodyID != 0 )
   {
      // Copy position and rotation of geometry over to
      const dReal* position = dGeomGetPosition( GetGeomID() );
      mBodyWrap->SetPosition(osg::Vec3(position[0], position[1], position[2]));

      const dReal* rotation = dGeomGetRotation( GetGeomID() );
      osg::Matrix mat;
      mat(0,0) = rotation[0];
      mat(1,0) = rotation[1];
      mat(2,0) = rotation[2];

      mat(0,1) = rotation[4];
      mat(1,1) = rotation[5];
      mat(2,1) = rotation[6];

      mat(0,2) = rotation[8];
      mat(1,2) = rotation[9];
      mat(2,2) = rotation[10];
      mBodyWrap->SetRotation(mat);
   }

   // This wipes out all previous transforms on the geom, so
   // that's why we had to copy them into the body first.
   dGeomSetBody(GetGeomID(), bodyID);
}

/**
 * Returns the ODE body identifier associated with this
 * object.
 *
 * @return the object's body identifier
 */
dBodyID Physical::GetBodyID() const
{
   return mBodyWrap->GetBodyID();
}

/**
 * Enables or disables dynamics for this object.
 *
 * @param enable true to enable dynamics, false to disable
 */
void Physical::EnableDynamics(bool enable)
{
   mBodyWrap->EnableDynamics(enable);
}

/**
 * Checks whether or not dynamics are enabled for
 * this object.
 *
 * @return true if dynamics are enabled, false otherwise
 */
bool Physical::DynamicsEnabled() const
{
   return mBodyWrap->DynamicsEnabled();
}

/**
 * Sets the ODE mass parameters of this object.
 *
 * @param mass a pointer to the mass structure to copy
 */
void Physical::SetMass(const dMass* mass)
{
   mBodyWrap->SetMass(mass);
}

/**
 * Retrieves the ODE mass parameters of this object.
 *
 * @param mass a pointer to the mass structure to fill
 */
void Physical::GetMass(dMass* mass) const
{
   mBodyWrap->GetMass(mass);
}

/**
 * Sets the mass of this object.
 *
 * @param mass the new mass, in kilograms
 */
void Physical::SetMass(float mass)
{
   mBodyWrap->SetMass(mass);
}

/**
 * Returns the mass of this object.
 *
 * @return the current mass
 */
float Physical::GetMass() const
{
   return mBodyWrap->GetMass();
}

/**
 * Sets this object's center of gravity.
 *
 * @param centerOfGravity the new center of gravity
 */
void Physical::SetCenterOfGravity(const osg::Vec3& centerOfGravity)
{
   mBodyWrap->SetCenterOfGravity(centerOfGravity);
}

/**
 * Retrieves this object's center of gravity.
 *
 * @param dest the vector in which to place the center
 * of gravity
 */
void Physical::GetCenterOfGravity(osg::Vec3& dest) const
{
   mBodyWrap->GetCenterOfGravity(dest);
}

/**
 * Sets this object's inertia tensor.
 *
 * @param inertiaTensor the new inertia tensor, only uses the rotation part of the transform matrix
 */

void Physical::SetInertiaTensor(const osg::Matrix& inertiaTensor)
{
   mBodyWrap->SetInertiaTensor(inertiaTensor);
}

/**
 * Retrieves this object's inertia tensor.
 *
 * @param dest the matrix in which to place the inertia
 * tensor, uses only rotation part of the transform matrix
 */
void Physical::GetInertiaTensor(osg::Matrix& dest) const
{
   mBodyWrap->GetInertiaTensor(dest);
}

//////////////////////////////////////////////////////////////////////////
void Physical::PrePhysicsStepUpdate()
{
   //Check to see if we need to update the ODE body to match
   //our current Transform.  This is in case the user set the Transform
   //manually.  If the dynamics aren't enabled, then pass the call to
   //Transform in case it wants to do something.

   if (mBodyWrap->DynamicsEnabled() &&
       mBodyWrap->GetBodyID())
   {
      Transform transform;

      this->GetTransform(transform, Transformable::ABS_CS);

      //update the body with our current Transform
      mBodyWrap->UpdateBodyTransform(transform);
   }
   else
   {
      //otherwise, collision detection might be enabled so let
      //Transformable do it's thing.
      Transformable::PrePhysicsStepUpdate();
   }
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
      const dReal* position = dBodyGetPosition(mBodyWrap->GetBodyID());
      const dReal* rotation = dBodyGetRotation(mBodyWrap->GetBodyID());

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

      Transform newTransform;
      newTransform.SetTranslation(position[0], position[1], position[2]);
      newTransform.SetRotation(newRotation);

      this->SetTransform(newTransform);
   }
}


void Physical::Ctor()
{
   RegisterInstance(this);

   SetCollisionCategoryBits(COLLISION_CATEGORY_MASK_PHYSICAL);
}

