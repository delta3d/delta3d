#include <dtCore/odeworldwrap.h>
#include <dtCore/transformable.h>
#include <dtCore/physical.h>

#include <ode/objects.h>

//////////////////////////////////////////////////////////////////////////
dtCore::ODEWorldWrap::ODEWorldWrap()
: mWorldID(0)
, mGravity(0.f, 0.f, 0.f)
{
   mWorldID = dWorldCreate();
   SetGravity(0.f, 0.f, -9.81f);
}

//////////////////////////////////////////////////////////////////////////
dtCore::ODEWorldWrap::~ODEWorldWrap()
{
   dWorldDestroy(mWorldID);
}

//////////////////////////////////////////////////////////////////////////
dWorldID dtCore::ODEWorldWrap::GetWorldID() const
{
   return mWorldID;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEWorldWrap::Step(const double &stepSize)
{
   dWorldQuickStep(mWorldID, stepSize);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEWorldWrap::SetGravity(float x, float y, float z)
{
   SetGravity( osg::Vec3(x,y,z) );
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEWorldWrap::GetGravity(osg::Vec3& vec) const
{
   vec = mGravity;
}

//////////////////////////////////////////////////////////////////////////
osg::Vec3 dtCore::ODEWorldWrap::GetGravity() const
{
   return mGravity;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEWorldWrap::GetGravity(float &x, float &y, float &z) const
{
   x = mGravity[0]; y = mGravity[1]; z = mGravity[2];
}

/////////////////////////////////////////////
void dtCore::ODEWorldWrap::SetGravity( const osg::Vec3& gravity )
{
   mGravity.set(gravity);

   dWorldSetGravity(mWorldID, mGravity[0], mGravity[1], mGravity[2]);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEWorldWrap::RegisterCollidable(Transformable* collidable)
{
   // This should probably be some sort of virtual function.
   // Or perhaps RegisterPhysical can stick around and only do
   // this.
   if( Physical* physical = dynamic_cast<Physical*>(collidable) )
   {
      if(physical != NULL)
      {
         physical->SetBodyID( dBodyCreate( mWorldID ) );
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEWorldWrap::UnRegisterCollidable(Transformable* collidable)
{
   if (Physical* physical = dynamic_cast<Physical*>(collidable))
   {
      if (physical != NULL) 
      {
         physical->SetBodyID(0);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEWorldWrap::SetDamping(float linearScale, float angularScale)
{
   dWorldSetDamping(mWorldID, linearScale, angularScale);
}

//////////////////////////////////////////////////////////////////////////
float dtCore::ODEWorldWrap::GetLinearDamping() const
{
   return dWorldGetLinearDamping(mWorldID);
}

//////////////////////////////////////////////////////////////////////////
float dtCore::ODEWorldWrap::GetAngularDamping() const
{
   return dWorldGetAngularDamping(mWorldID);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEWorldWrap::SetLinearDampingThreshold(float linearThreshold)
{
   dWorldSetLinearDampingThreshold(mWorldID, linearThreshold);
}

//////////////////////////////////////////////////////////////////////////
float dtCore::ODEWorldWrap::GetLinearDampingThreshold() const
{
   return dWorldGetLinearDampingThreshold(mWorldID);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEWorldWrap::SetAngularDampingThreshold(float angularThreshold)
{
   dWorldSetAngularDampingThreshold(mWorldID, angularThreshold);
}

//////////////////////////////////////////////////////////////////////////
float dtCore::ODEWorldWrap::GetAngularDampingThreshold() const
{
   return dWorldGetAngularDampingThreshold(mWorldID);
}
