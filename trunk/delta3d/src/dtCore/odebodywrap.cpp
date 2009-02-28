#include <dtCore/odebodywrap.h>
#include <dtCore/transform.h>
#include <ode/objects.h>

dtCore::ODEBodyWrap::ODEBodyWrap()
  : mBodyID(0)
  , mDynamicsEnabled(false)
{
   dMassSetSphere(&mMass, 1.f, 1.f);
   SetMass(&mMass);
}

//////////////////////////////////////////////////////////////////////////
dtCore::ODEBodyWrap::~ODEBodyWrap()
{
   if (GetBodyID() != 0)
   {
      dBodyDestroy(GetBodyID());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetBodyID(dBodyID bodyID)
{
   if (GetBodyID() != 0)
   {
      dBodyDestroy(GetBodyID());
   }

   mBodyID = bodyID;

   EnableDynamics(mDynamicsEnabled);

   SetMass(&mMass);
}

//////////////////////////////////////////////////////////////////////////
dBodyID dtCore::ODEBodyWrap::GetBodyID() const
{
   return mBodyID;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::EnableDynamics(bool enable)
{
   mDynamicsEnabled = enable;

   if (GetBodyID() != 0)
   {
      if (enable)
      {
         dBodyEnable(GetBodyID());
      }
      else
      {
         dBodyDisable(GetBodyID());
      }
   }
}

//////////////////////////////////////////////////////////////////////////
bool dtCore::ODEBodyWrap::DynamicsEnabled() const
{
   return mDynamicsEnabled;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetMass(const dMass* mass)
{
   mMass = *mass;

   if (GetBodyID() != 0)
   {
      dBodySetMass(GetBodyID(), &mMass);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::GetMass(dMass* mass) const
{
    *mass = mMass;
}

void dtCore::ODEBodyWrap::SetMass(float mass)
{
   mMass.mass = mass;

   if (GetBodyID() != 0)
   {
      dBodySetMass(GetBodyID(), &mMass);
   }
}

//////////////////////////////////////////////////////////////////////////
float dtCore::ODEBodyWrap::GetMass() const
{
   return mMass.mass;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetPosition(const osg::Vec3& xyz)
{
   if (GetBodyID() != 0)
   {
      dBodySetPosition(GetBodyID(), xyz.x(), xyz.y(), xyz.z());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetRotation(const osg::Matrix& rot)
{
   if (GetBodyID() != 0)
   {
      dMatrix3 dRot;
      dRot[0] = rot(0,0);
      dRot[1] = rot(1,0);
      dRot[2] = rot(2,0);

      dRot[4] = rot(0,1);
      dRot[5] = rot(1,1);
      dRot[6] = rot(2,1);

      dRot[8] = rot(0,2);
      dRot[9] = rot(1,2);
      dRot[10] = rot(2,2);

      dBodySetRotation(GetBodyID(), dRot);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetCenterOfGravity(const osg::Vec3& centerOfGravity)
{
   mMass.c[0] = centerOfGravity[0];
   mMass.c[1] = centerOfGravity[1];
   mMass.c[2] = centerOfGravity[2];

   if (GetBodyID() != 0)
   {
      dBodySetMass(GetBodyID(), &mMass);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::GetCenterOfGravity(osg::Vec3& dest) const
{
   dest[0] = mMass.c[0];
   dest[1] = mMass.c[1];
   dest[2] = mMass.c[2];
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetInertiaTensor(const osg::Matrix& inertiaTensor)
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

   if (GetBodyID() != 0)
   {
      dBodySetMass(GetBodyID(), &mMass);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::GetInertiaTensor(osg::Matrix& dest) const
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

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetLinearVelocity(const osg::Vec3& newVelocity)
{
   if (GetBodyID() != 0)
   {
      dBodySetLinearVel(GetBodyID(), newVelocity.x(), newVelocity.y(), newVelocity.z());
   }
}

//////////////////////////////////////////////////////////////////////////
osg::Vec3 dtCore::ODEBodyWrap::GetLinearVelocity() const
{
   osg::Vec3 velVec;

   if (GetBodyID() != 0)
   {
      const dReal* vel = dBodyGetLinearVel(GetBodyID());
      velVec.set(vel[0], vel[1], vel[2]);
   }

   return velVec;
}

//////////////////////////////////////////////////////////////////////////
osg::Vec3 dtCore::ODEBodyWrap::GetAngularVelocity() const
{
   osg::Vec3 angVec;

   if (GetBodyID() != 0)
   {
      const dReal* vel = dBodyGetAngularVel(GetBodyID());
      angVec.set(vel[0], vel[1], vel[2]);
   }

   return angVec;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::ApplyForce(const osg::Vec3& force)
{
   if (GetBodyID() != 0)
   {
      dBodyAddForce(GetBodyID(), force.x(), force.y(), force.z());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::ApplyForceAtPos(const osg::Vec3& force, const osg::Vec3& position)
{
   if (GetBodyID() != 0)
   {
      dBodyAddForceAtPos(GetBodyID(), force.x(), force.y(), force.z(),
                         position.x(), position.y(), position.z());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::ApplyForceAtRelPos(const osg::Vec3& force, const osg::Vec3& relPosition)
{
   if (GetBodyID() != 0)
   {
      dBodyAddForceAtRelPos(GetBodyID(), force.x(), force.y(), force.z(),
                           relPosition.x(), relPosition.y(), relPosition.z());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::ApplyRelForce(const osg::Vec3& relForce)
{
   if (GetBodyID() != 0)
   {
      dBodyAddRelForce(GetBodyID(), relForce.x(), relForce.y(), relForce.z());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::ApplyRelForceAtRelPos(const osg::Vec3& relForce,
                                                const osg::Vec3& relPosition)
{
   if (GetBodyID() != 0)
   {
      dBodyAddRelForceAtRelPos(GetBodyID(), relForce.x(), relForce.y(), relForce.z(),
                               relPosition.x(), relPosition.y(), relPosition.z());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::ApplyTorque(const osg::Vec3& torque)
{
   if (GetBodyID() != 0)
   {
      dBodyAddTorque(GetBodyID(), torque.x(), torque.y(), torque.z());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::ApplyRelTorque(const osg::Vec3& torque)
{
   if (GetBodyID() != 0)
   {
      dBodyAddRelTorque(GetBodyID(), torque.x(), torque.y(), torque.z());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::GetBodyTransform(dtCore::Transform& xform) const
{
   const dReal* position = dBodyGetPosition(GetBodyID());
   const dReal* rotation = dBodyGetRotation(GetBodyID());

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

   xform.SetTranslation(position[0], position[1], position[2]);
   xform.SetRotation(newRotation);

}

////////////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::UpdateBodyTransform(const dtCore::Transform& newTransform)
{
   if (DynamicsEnabled() == false) {return;}

   dtCore::Transform odeTransform;
   GetBodyTransform(odeTransform);

   if (!newTransform.EpsilonEquals(odeTransform))
   {
      osg::Matrix rotation;
      osg::Vec3 position;

      newTransform.GetTranslation(position);
      newTransform.GetRotation(rotation);

      // Set translation
      dBodySetPosition(GetBodyID(), position[0], position[1], position[2]);

      // Set rotation
      dMatrix3 dRot;

      dRot[ 0] = rotation(0,0);
      dRot[ 1] = rotation(1,0);
      dRot[ 2] = rotation(2,0);

      dRot[ 4] = rotation(0,1);
      dRot[ 5] = rotation(1,1);
      dRot[ 6] = rotation(2,1);

      dRot[ 8] = rotation(0,2);
      dRot[ 9] = rotation(1,2);
      dRot[10] = rotation(2,2);

      dBodySetRotation(GetBodyID(), dRot);
   }
}

////////////////////////////////////////////////////////////////////////////////
bool dtCore::ODEBodyWrap::GetAutoDisableFlag() const
{
   if (GetBodyID() != 0)
   {
      return dBodyGetAutoDisableFlag(GetBodyID()) ? true : false;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetAutoDisableFlag(bool auto_disable)
{
   if (GetBodyID() != 0)
   {
      dBodySetAutoDisableFlag(GetBodyID(), auto_disable);
   }
}

////////////////////////////////////////////////////////////////////////////////
float dtCore::ODEBodyWrap::GetAutoDisableLinearThreshold() const
{
   if (GetBodyID() != 0)
   {
      return dBodyGetAutoDisableLinearThreshold(GetBodyID());
   }

   return -1.0f;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetAutoDisableLinearThreshold(float threshold)
{
   if (GetBodyID() != 0)
   {
      dBodySetAutoDisableLinearThreshold(GetBodyID(), threshold);
   }
}

////////////////////////////////////////////////////////////////////////////////
float dtCore::ODEBodyWrap::GetAutoDisableAngularThreshold() const
{
   if (GetBodyID() != 0)
   {
      return dBodyGetAutoDisableAngularThreshold(GetBodyID());
   }

   return -1.0f;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetAutoDisableAngularThreshold(float threshold)
{
   if (GetBodyID() != 0)
   {
      dBodySetAutoDisableAngularThreshold(GetBodyID(), threshold);
   }
}

////////////////////////////////////////////////////////////////////////////////
int dtCore::ODEBodyWrap::GetAutoDisableSteps() const
{
   if (GetBodyID() != 0)
   {
      return dBodyGetAutoDisableSteps(GetBodyID());
   }

   return 0;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetAutoDisableSteps(int numSteps)
{
   if (GetBodyID() != 0)
   {
      dBodySetAutoDisableSteps(GetBodyID(), numSteps);
   }
}

////////////////////////////////////////////////////////////////////////////////
float dtCore::ODEBodyWrap::GetAutoDisableTime() const
{
   if (GetBodyID() != 0)
   {
      return dBodyGetAutoDisableTime(GetBodyID());
   }

   return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetAutoDisableTime(float numSeconds)
{
   if (GetBodyID() != 0)
   {
      dBodySetAutoDisableTime(GetBodyID(), numSeconds);
   }
}

////////////////////////////////////////////////////////////////////////////////
float dtCore::ODEBodyWrap::GetLinearDamping() const
{
   if (GetBodyID() != 0)
   {
      return dBodyGetLinearDamping(GetBodyID());
   }

   return -1.0f;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetLinearDamping(float scale)
{
   if (GetBodyID() != 0)
   {
      dBodySetLinearDamping(GetBodyID(), scale);
   }
}

////////////////////////////////////////////////////////////////////////////////
float dtCore::ODEBodyWrap::GetAngularDamping() const
{
   if (GetBodyID() != 0)
   {
      return dBodyGetAngularDamping(GetBodyID());
   }

   return -1.0f;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetAngularDamping(float scale)
{
   if (GetBodyID() != 0)
   {
      dBodySetAngularDamping(GetBodyID(), scale);
   }
}

////////////////////////////////////////////////////////////////////////////////


