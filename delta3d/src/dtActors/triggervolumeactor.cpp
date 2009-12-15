#include <dtActors/triggervolumeactor.h>
#include <dtActors/triggervolumeactorproxy.h>

#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/collisioncategorydefaults.h>

#include <ode/collision.h>

#include <assert.h>
#include <iostream>

using namespace dtActors;

IMPLEMENT_MANAGEMENT_LAYER(TriggerVolumeActor)

////////////////////////////////////////////////////////////////////////////////
TriggerVolumeActor::TriggerVolumeActor(dtActors::TriggerVolumeActorProxy& proxy)
   : dtGame::GameActor(proxy)
   , mMaxTriggerCount(0)
   , mTriggerCount(0)
{
   RegisterInstance(this);

   AddSender(&dtCore::System::GetInstance());

   // Give a default collision shape and size.
   SetCollisionSphere(5.0f);

   // By default, we want the trigger itself to collide with anything
   SetCollisionCollideBits(COLLISION_CATEGORY_MASK_ALL);
   SetCollisionCategoryBits(COLLISION_CATEGORY_MASK_PROXIMITYTRIGGER);
}

////////////////////////////////////////////////////////////////////////////////
void TriggerVolumeActor::OnMessage(dtCore::Base::MessageData* data)
{
   if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
   {
      double dt = *static_cast<double*>(data->userData);

      // Check to see if any occupants have left the volume
      for (size_t actorIndex = 0; actorIndex < mOccupancyList.size(); ++actorIndex)
      {
         if (!IsActorInVolume(mOccupancyList[actorIndex].get()))
         {  
            // Trigger the exit action
            if (mExitAction.valid())
            {
               TriggerAction(*mExitAction);
            }

            mOccupancyList.erase(mOccupancyList.begin() + actorIndex);
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool TriggerVolumeActor::FilterContact(dContact* contact, Transformable* collider)
{
   // Is this actor in the volume for the first time?
   if (!IsActorAnOccupant(collider))
   {
      mOccupancyList.push_back(collider);

      // Trigger the "Enter" action
      if (mEnterAction.valid())
      {
         TriggerAction(*mEnterAction);
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool TriggerVolumeActor::IsActorInVolume(dtCore::Transformable* actor)
{
   bool inVolume = false;

   if (actor)
   {
      dGeomID volumeID = GetGeomID();
      dGeomID actorID  = actor->GetGeomID();

      dContactGeom contactGeoms[8];

      int numContacts = dCollide(volumeID, actorID, 8, contactGeoms, sizeof(dContactGeom));

      inVolume = numContacts > 0;
   }

   return inVolume;
}

////////////////////////////////////////////////////////////////////////////////
bool TriggerVolumeActor::IsActorAnOccupant(dtCore::Transformable* actor)
{
   // If we've have the actor in the list (the volume), it's an occupant
   for (size_t actorIndex = 0; actorIndex < mOccupancyList.size(); ++actorIndex)
   {
      if (actor == mOccupancyList[actorIndex].get())
      {
         return true;
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
void TriggerVolumeActor::TriggerAction(dtABC::Action& actionToFire)
{
   if (actionToFire.CanStart())
   {
      actionToFire.Start();
      ++mTriggerCount;

      // If this trigger has expired, it no longer needs update messages
      if (mMaxTriggerCount != 0 && mTriggerCount >= mMaxTriggerCount)
      {
         DeregisterInstance(this);
      }
   }
}
