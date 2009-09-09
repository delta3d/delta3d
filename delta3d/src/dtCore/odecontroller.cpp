#include <dtCore/odecontroller.h>
#include <dtCore/transformable.h>
#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include <cassert>
#include <ode/odeinit.h>

/////////////////////////////////////////////
// Replacement message handler for ODE
extern "C" void ODEMessageHandler(int errnum, const char* msg, va_list ap)
{
   dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_INFO, __FILE__, __LINE__, msg, ap);
}
/////////////////////////////////////////////
// Replacement debug handler for ODE
extern "C" void ODEDebugHandler(int errnum, const char* msg, va_list ap)
{
   dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FILE__, __LINE__, msg, ap);

   assert(false);
}
/////////////////////////////////////////////
// Replacement error handler for ODE
extern "C" void ODEErrorHandler(int errnum, const char* msg, va_list ap)
{
   dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FILE__, msg, ap);

   assert(false);
}

namespace dtCore
{
   static OpenThreads::Atomic ODERefCount;

   static void RefODE()
   {
      if (++ODERefCount == 1)
      {
         if (!dInitODE2(0))
         {
            LOG_ERROR("ODE failed to initialize.");
         }
      }
   }

   static void DerefODE()
   {
      if (--ODERefCount == 0)
      {
         dCloseODE();
      }
   }
}

const dtUtil::RefString dtCore::ODEController::MESSAGE_COLLISION("collision");
const dtUtil::RefString dtCore::ODEController::MESSAGE_PHYSICS_STEP("physics_step");


//////////////////////////////////////////////////////////////////////////
dtCore::ODEController::ODEController(dtCore::Base* msgSender):
mSpaceWrapper(NULL),
mWorldWrapper(new ODEWorldWrap()),
mPhysicsStepSize(0.0),
mMsgSender(msgSender)
{
   mSpaceWrapper = new ODESpaceWrap(mWorldWrapper.get());

   Ctor();
}

//////////////////////////////////////////////////////////////////////////
dtCore::ODEController::ODEController(ODESpaceWrap& spaceWrapper, ODEWorldWrap& worldWrap, dtCore::Base* msgSender):
mSpaceWrapper(&spaceWrapper),
mWorldWrapper(&worldWrap),
mPhysicsStepSize(0.0),
mMsgSender(msgSender)
{
   Ctor();
}

//////////////////////////////////////////////////////////////////////////
unsigned dtCore::ODEController::GetODERefCount()
{
   return ODERefCount;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::Ctor()
{
   RefODE();
   //supply our method to be called when geoms actually collide
   mSpaceWrapper->SetDefaultCollisionCBFunc(dtCore::ODESpaceWrap::CollisionCBFunc(this, &ODEController::DefaultCBFunc));

   dSetMessageHandler(ODEMessageHandler);
   dSetDebugHandler(ODEDebugHandler);
   dSetErrorHandler(ODEErrorHandler);
}


//////////////////////////////////////////////////////////////////////////
dtCore::ODEController::~ODEController()
{
   DerefODE();

   // Since we are going to destroy all the bodies in our world with dWorldDestroy,
   // we must remove the references to the bodies associated with their default collision
   // geoms. Otherwise destroying the world will leave the geoms references bad memory.
   // This prevents a crash-on-exit in STAGE.
   for (TransformableVector::iterator iter = mCollidableContents.begin();
        iter != mCollidableContents.end();
        ++iter)
   {
      mSpaceWrapper->UnRegisterCollidable((*iter));
      mWorldWrapper->UnRegisterCollidable((*iter));
   }

   mCollidableContents.clear();
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::Iterate(double deltaFrameTime)
{
   double stepSize = deltaFrameTime;

   // if step size is set, use it instead of the delta frame time
   if (GetPhysicsStepSize() > 0.0)
   {
      stepSize = GetPhysicsStepSize();
   }

   //calc the number of steps to take
   const int numSteps = int(deltaFrameTime/stepSize);

   TransformableVector::const_iterator it;

   for (it = GetRegisteredCollidables().begin();
        it != GetRegisteredCollidables().end();
        ++it)
   {
      (*it)->PrePhysicsStepUpdate();
   }

   for (int i=0; i<numSteps; ++i)
   {
      Step(stepSize);
   }

   const double leftOver = deltaFrameTime - (numSteps * stepSize);

   if (leftOver > 0.0)
   {
      Step(leftOver);
   }

   for (it = GetRegisteredCollidables().begin();
        it != GetRegisteredCollidables().end();
        ++it)
   {
      (*it)->PostPhysicsStepUpdate();
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

   mCollidableContents.push_back(collidable);
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
dJointGroupID dtCore::ODEController::GetContactJointGroupID() const
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
dNearCallback* dtCore::ODEController::GetUserCollisionCallback() const
{
   return mSpaceWrapper.valid() ? mSpaceWrapper->GetUserCollisionCallback() : NULL;
}

//////////////////////////////////////////////////////////////////////////
void* dtCore::ODEController::GetUserCollisionData()
{
   return mSpaceWrapper.valid() ? mSpaceWrapper->GetUserCollisionData() : NULL;
}

//////////////////////////////////////////////////////////////////////////
const void* dtCore::ODEController::GetUserCollisionData() const
{
   return mSpaceWrapper.valid() ? mSpaceWrapper->GetUserCollisionData() : NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::Step(double stepSize)
{
   if (mMsgSender.valid())
   {
      mMsgSender->SendMessage(ODEController::MESSAGE_PHYSICS_STEP, &stepSize);
   }

   if (mSpaceWrapper.valid()) { mSpaceWrapper->Collide(); }

   if (mWorldWrapper.valid()) { mWorldWrapper->Step(stepSize); }

   if (mSpaceWrapper.valid()) { mSpaceWrapper->PostCollide(); }
}

void dtCore::ODEController::DefaultCBFunc(const dtCore::ODESpaceWrap::CollisionData& data)
{
   if (mMsgSender.valid())
   {
      //have to convert to Scene::CollisionData for backward compatibility
      dtCore::Scene::CollisionData scd;
      scd.mBodies[0] = data.mBodies[0];
      scd.mBodies[1] = data.mBodies[1];
      scd.mDepth     = data.mDepth;
      scd.mLocation  = data.mLocation;
      scd.mNormal    = data.mNormal;

      //if a collision took place and we have a sender pointer,
      //send out the "collision" message
      mMsgSender->SendMessage(ODEController::MESSAGE_COLLISION, &scd);
   }
}

//////////////////////////////////////////////////////////////////////////
const dtCore::ODEController::TransformableVector& dtCore::ODEController::GetRegisteredCollidables() const
{
   return mCollidableContents;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEController::SetMessageSender(dtCore::Base* msgSender)
{
   mMsgSender = msgSender;
}
