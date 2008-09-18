#include <dtCore/odecontroller.h>
#include <dtCore/transformable.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h> //due to scene.h
#include <dtCore/keyboardmousehandler.h>//due to scene.h
#include <dtUtil/log.h>
#include <cassert>

/////////////////////////////////////////////
// Replacement message handler for ODE
extern "C" void ODEMessageHandler(int errnum, const char *msg, va_list ap)
{
   dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_INFO, __FILE__, msg, ap);
}
/////////////////////////////////////////////
// Replacement debug handler for ODE
extern "C" void ODEDebugHandler(int errnum, const char *msg, va_list ap)
{
   dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FILE__, msg, ap);

   assert(false);
}
/////////////////////////////////////////////
// Replacement error handler for ODE
extern "C" void ODEErrorHandler(int errnum, const char *msg, va_list ap)
{
   dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FILE__, msg, ap);

   assert(false);
}


// dTriIndex is a macro hack in delta for ODE < 0.10. For 0.10 it's a typedef, so this will only
// exist for 0.10 and later.
#ifndef dTriIndex

#include <ode/odeinit.h>

class ODELifeCycle
{
public:
   ODELifeCycle()
   {
      dInitODE2(0);
   }

   ~ODELifeCycle()
   {
      dCloseODE();
   }
};

// This statically starts and shuts ode down.  This is needed for ODE 0.10.
static ODELifeCycle odeLifeCycle;
#endif


//////////////////////////////////////////////////////////////////////////
dtCore::ODEController::ODEController(dtCore::Base *msgSender):
mMsgSender(msgSender),
mSpaceWrapper(NULL),
mWorldWrapper(new ODEWorldWrap())
{
   mSpaceWrapper = new ODESpaceWrap(mWorldWrapper.get());

   Ctor();
}

//////////////////////////////////////////////////////////////////////////
dtCore::ODEController::ODEController(ODESpaceWrap& spaceWrapper, ODEWorldWrap& worldWrap, dtCore::Base *msgSender):
mMsgSender(msgSender),
mSpaceWrapper(&spaceWrapper),
mWorldWrapper(&worldWrap)
{
   Ctor();
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::Ctor()
{
   //supply our method to be called when geoms actually collide
   mSpaceWrapper->SetDefaultCollisionCBFunc(dtCore::ODESpaceWrap::CollisionCBFunc(this, &dtCore::ODEController::DefaultCBFunc));

   dSetMessageHandler(ODEMessageHandler);
   dSetDebugHandler(ODEDebugHandler);
   dSetErrorHandler(ODEErrorHandler);
}

//////////////////////////////////////////////////////////////////////////
dtCore::ODEController::~ODEController()
{
   // Since we are going to destroy all the bodies in our world with dWorldDestroy,
   // we must remove the references to the bodies associated with their default collision
   // geoms. Otherwise destroying the world will leave the geoms references bad memory.
   // This prevents a crash-on-exit in STAGE.
   for(  TransformableVector::iterator iter = mCollidableContents.begin();
      iter != mCollidableContents.end();
      ++iter )
   {
      mSpaceWrapper->UnRegisterCollidable((*iter));
      mWorldWrapper->UnRegisterCollidable((*iter));
   }

   mCollidableContents.clear();
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::Iterate(double deltaFrameTime)
{
   bool usingDeltaStep = false;

   // if step size is 0.0, use the deltaFrameTime instead
   if( mPhysicsStepSize == 0.0 )
   {
      SetPhysicsStepSize(deltaFrameTime);
      usingDeltaStep = true;
   }

   const int numSteps = int(deltaFrameTime/mPhysicsStepSize);

   TransformableVector::iterator it;

   for (it = mCollidableContents.begin();
        it != mCollidableContents.end();
        it++ )
   {
      (*it)->PrePhysicsStepUpdate();
   }

   for (int i=0; i<numSteps; i++)
   {
      Step(mPhysicsStepSize);
   }

   const double leftOver = deltaFrameTime - (numSteps * mPhysicsStepSize);

   if(leftOver > 0.0)
   {
      Step(leftOver);
   }

   for(it = mCollidableContents.begin();
       it != mCollidableContents.end();
       it++)
   {
      (*it)->PostPhysicsStepUpdate();
   }

   if (usingDeltaStep) //reset physics step size to 0.0 (i.e. use System step size)
   {
      SetPhysicsStepSize(0.0);
   }
}

//////////////////////////////////////////////////////////////////////////
double dtCore::ODEController::GetPhysicsStepSize() const
{
   return mPhysicsStepSize;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::SetPhysicsStepSize(double stepSize)
{
   mPhysicsStepSize = stepSize;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::RegisterCollidable(Transformable* collidable)
{
   if (collidable == NULL) {return;}

   mSpaceWrapper->RegisterCollidable(collidable);
   mWorldWrapper->RegisterCollidable(collidable);

   mCollidableContents.push_back( collidable );
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::UnRegisterCollidable(Transformable* collidable)
{
   if (collidable == NULL) {return;}

   mSpaceWrapper->UnRegisterCollidable(collidable);
   mWorldWrapper->UnRegisterCollidable(collidable);

   for (TransformableVector::iterator it = mCollidableContents.begin();
        it != mCollidableContents.end();
        ++it)
   {
      if (*it == collidable)
      {
         mCollidableContents.erase(it);
         break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////
dtCore::ODEWorldWrap* dtCore::ODEController::GetWorldWrapper() const
{
   return mWorldWrapper.get();
}

//////////////////////////////////////////////////////////////////////////
dtCore::ODESpaceWrap* dtCore::ODEController::GetSpaceWrapper() const
{
   return mSpaceWrapper.get();
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::SetGravity(const osg::Vec3& gravity) const
{
   if (mWorldWrapper.valid())
   {
      mWorldWrapper->SetGravity(gravity);
   }
}

//////////////////////////////////////////////////////////////////////////
osg::Vec3 dtCore::ODEController::GetGravity() const
{
   if (mWorldWrapper.valid())
   {
      return mWorldWrapper->GetGravity();
   }
   else
   {
      return osg::Vec3(0.f, 0.f, 0.f);
   }
}

//////////////////////////////////////////////////////////////////////////
dSpaceID dtCore::ODEController::GetSpaceID() const
{
   if (mSpaceWrapper.valid())
   {
      return mSpaceWrapper->GetSpaceID();
   }
   else
   {
      return 0;
   }
}

//////////////////////////////////////////////////////////////////////////
dWorldID dtCore::ODEController::GetWorldID() const
{
   if (mWorldWrapper.valid())
   {
      return mWorldWrapper->GetWorldID();
   }
   else
   {
      return 0;
   }
}

//////////////////////////////////////////////////////////////////////////
dJointGroupID dtCore::ODEController::GetContactJoinGroupID() const
{
   if (mSpaceWrapper.valid())
   {
      return mSpaceWrapper->GetContactJoinGroupID();
   }
   else
   {
      return 0;
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::SetUserCollisionCallback(dNearCallback* func, void* data) const
{
   if (mSpaceWrapper.valid())
   {
      mSpaceWrapper->SetUserCollisionCallback(func, data);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::Step(double stepSize) const
{
   if (mSpaceWrapper.valid()) { mSpaceWrapper->Collide(); }

   if (mWorldWrapper.valid()) { mWorldWrapper->Step(stepSize); }

   if (mSpaceWrapper.valid()) { mSpaceWrapper->PostCollide(); }
}

void dtCore::ODEController::DefaultCBFunc(const dtCore::ODESpaceWrap::CollisionData &data)
{
   if (mMsgSender.valid())
   {
      //have to convert to Scene::CollisionData for backward compatibility
      dtCore::Scene::CollisionData scd;
      scd.mBodies[0] = data.mBodies[0];
      scd.mBodies[1] = data.mBodies[1];
      scd.mDepth =     data.mDepth;
      scd.mLocation =  data.mLocation;
      scd.mNormal =    data.mNormal;

      //if a collision took place and we have a sender pointer,
      //send out the "collision" message
      mMsgSender->SendMessage("collision", &scd);
   }
}
