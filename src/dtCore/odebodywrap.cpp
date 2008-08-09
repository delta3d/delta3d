#include <dtCore/odebodywrap.h>
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

   if(GetBodyID() != 0)
   {
      if(enable)
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

   if(GetBodyID() != 0)
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

   if(GetBodyID() != 0)
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
void dtCore::ODEBodyWrap::SetPosition( const osg::Vec3& xyz )
{
   if (GetBodyID() != 0)
   {
      dBodySetPosition(GetBodyID(), xyz.x(), xyz.y(), xyz.z());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEBodyWrap::SetRotation( const osg::Matrix& rot )
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
void dtCore::ODEBodyWrap::SetCenterOfGravity( const osg::Vec3& centerOfGravity )
{
   mMass.c[0] = centerOfGravity[0];
   mMass.c[1] = centerOfGravity[1];
   mMass.c[2] = centerOfGravity[2];

   if(GetBodyID() != 0)
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
void dtCore::ODEBodyWrap::SetInertiaTensor( const osg::Matrix& inertiaTensor )
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

   if(GetBodyID() != 0)
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
osg::Vec3 dtCore::ODEBodyWrap::GetLinearVelocity() const
{
   osg::Vec3 velVec;

   if (GetBodyID() != 0)
   {
      const dReal *vel = dBodyGetLinearVel(GetBodyID());
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
      const dReal *vel = dBodyGetAngularVel(GetBodyID());
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
