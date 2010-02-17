#include <dtCore/odespacewrap.h>
#include <dtCore/physical.h>
#include <dtCore/odeworldwrap.h>
#include <ode/collision_space.h>
#include <ode/collision.h>
#include <ode/objects.h>
#include <dtCore/transformable.h>

//////////////////////////////////////////////////////////////////////////
dtCore::ODESpaceWrap::ODESpaceWrap(ODEWorldWrap* worldWrapper)
   : mSpaceID(0)
   , mUserNearCallback(NULL)
   , mContactJointGroupID(0)
   , mWorldWrapper(worldWrapper)
{
   mSpaceID = dHashSpaceCreate(0);
   dSpaceSetCleanup(mSpaceID, 0);

   mContactJointGroupID = dJointGroupCreate(0);
}

//////////////////////////////////////////////////////////////////////////
dtCore::ODESpaceWrap::~ODESpaceWrap()
{
   dSpaceDestroy(mSpaceID);
   dJointGroupDestroy(mContactJointGroupID);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODESpaceWrap::RegisterCollidable(Transformable* collidable)
{
   if (collidable == NULL) { return; }

   dSpaceAdd(mSpaceID, collidable->GetGeomID());

   dGeomSetData(collidable->GetGeomID(), collidable);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODESpaceWrap::UnRegisterCollidable(Transformable* collidable)
{
   if (collidable == NULL) { return; }

   dSpaceRemove(mSpaceID, collidable->GetGeomID());
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODESpaceWrap::Collide()
{
   if (mUserNearCallback)
   {
      dSpaceCollide(mSpaceID, mUserNearCallbackData, mUserNearCallback);
   }
   else
   {
      dSpaceCollide(mSpaceID, this, DefaultNearCallback);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODESpaceWrap::SetUserCollisionCallback(dNearCallback* func, void* data)
{
   mUserNearCallback = func;
   mUserNearCallbackData = data;
}

/// static
void dtCore::ODESpaceWrap::DefaultNearCallback(void* data, dGeomID o1, dGeomID o2)
{
   if (data == 0 || o1 == 0 || o2 == 0)
   {
      return;
   }

   ODESpaceWrap* spaceWrap = static_cast<ODESpaceWrap*>(data);

   Transformable* c1 = static_cast<Transformable*>(dGeomGetData(o1));
   Transformable* c2 = static_cast<Transformable*>(dGeomGetData(o2));

   dContactGeom contactGeoms[8];

   int numContacts = dCollide(o1, o2, 8, contactGeoms, sizeof(dContactGeom));

   if (numContacts > 0 && c1 != 0 && c2 != 0)
   {
      CollisionData cd;

      cd.mBodies[0] = c1;
      cd.mBodies[1] = c2;

      cd.mLocation.set(
         contactGeoms[0].pos[0], contactGeoms[0].pos[1], contactGeoms[0].pos[2]
      );

      cd.mNormal.set(
         contactGeoms[0].normal[0], contactGeoms[0].normal[1], contactGeoms[0].normal[2]
      );

      cd.mDepth = contactGeoms[0].depth;

      if (spaceWrap->mCollisionCBFunc.valid())
      {
         spaceWrap->mCollisionCBFunc(cd);
      }

      if (c1 != 0 || c2 != 0)
      {
         dContact contact;

         for (int i = 0; i < numContacts; ++i)
         {
            contact.surface.mode       = dContactBounce;
            contact.surface.mu         = (dReal)1000.0;
            contact.surface.bounce     = (dReal)0.75;
            contact.surface.bounce_vel = (dReal)0.001;

            contact.geom = contactGeoms[i];

            // Make sure to call these both, because in the case of
            // Trigger, meaningful stuff happens even if the return
            // is false.
            bool contactResult1 = c1->FilterContact(&contact, c2);
            bool contactResult2 = c2->FilterContact(&contact, c1);

            if (contactResult1 && contactResult2)
            {
               // All this also should be in a virtual function.
               Physical* p1 = dynamic_cast<Physical*>(c1);
               Physical* p2 = dynamic_cast<Physical*>(c2);

               if (p1 != 0 || p2 != 0)
               {
                  dJointID joint = dJointCreateContact(spaceWrap->mWorldWrapper->GetWorldID(),
                                                       spaceWrap->mContactJointGroupID,
                                                       &contact);

                  dJointAttach(joint,
                     p1 != 0 && p1->DynamicsEnabled() ? p1->GetBodyID() : 0,
                     p2 != 0 && p2->DynamicsEnabled() ? p2->GetBodyID() : 0);
               }
            }
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
dNearCallback* dtCore::ODESpaceWrap::GetUserCollisionCallback() const
{
   return mUserNearCallback;
}

//////////////////////////////////////////////////////////////////////////
void* dtCore::ODESpaceWrap::GetUserCollisionData()
{
   return mUserNearCallbackData;
}

//////////////////////////////////////////////////////////////////////////
const void* dtCore::ODESpaceWrap::GetUserCollisionData() const
{
   return mUserNearCallbackData;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODESpaceWrap::PostCollide()
{
   dJointGroupEmpty(mContactJointGroupID);
}

//////////////////////////////////////////////////////////////////////////
dJointGroupID dtCore::ODESpaceWrap::GetContactJoinGroupID() const
{
   return mContactJointGroupID;
}

//////////////////////////////////////////////////////////////////////////
dSpaceID dtCore::ODESpaceWrap::GetSpaceID() const
{
   return mSpaceID;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODESpaceWrap::SetDefaultCollisionCBFunc(const CollisionCBFunc& func)
{
   mCollisionCBFunc = func;
}
