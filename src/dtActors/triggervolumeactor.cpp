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
   // Do not send events in STAGE.
   if (GetGameActorProxy().IsInSTAGE())
   {
      return;
   }

   if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
   {
      //double dt = *static_cast<double*>(data->userData);

      // Check to see if any occupants have left the volume
      for (size_t actorIndex = 0; actorIndex < mOccupancyList.size(); ++actorIndex)
      {
         dtCore::Transformable* occupant = mOccupancyList[actorIndex].get();
         if (!IsActorInVolume(occupant))
         {
            mOccupancyList.erase(mOccupancyList.begin() + actorIndex);
            actorIndex--;

            TriggerEvent(occupant, LEAVE_EVENT);
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool TriggerVolumeActor::FilterContact(dContact* contact, Transformable* collider)
{
   // Do not trigger this event if we have reached our trigger limit.
   if (mMaxTriggerCount != 0 && mTriggerCount >= mMaxTriggerCount)
   {
      return false;
   }

   // Do not send events in STAGE.
   if (GetGameActorProxy().IsInSTAGE())
   {
      return false;
   }

   // Is this actor in the volume for the first time?
   if (!IsActorAnOccupant(collider))
   {
      mOccupancyList.push_back(collider);

      TriggerEvent(collider, ENTER_EVENT);
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool TriggerVolumeActor::RegisterListener(void* receiver, EventFuncType eventCallback)
{
   // Make sure the listener is not already registered.
   int count = (int)mListenerList.size();
   for (int index = 0; index < count; index++)
   {
      RegisterData& data = mListenerList[index];
      if (data.receiver == receiver)
      {
         data.onEvent = eventCallback;
         return false;
      }
   }

   // Register the listener.
   RegisterData data;
   data.receiver = receiver;
   data.onEvent = eventCallback;
   mListenerList.push_back(data);

   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool TriggerVolumeActor::UnregisterListener(void* receiver)
{
   // Make sure the listener is not already registered.
   int count = (int)mListenerList.size();
   for (int index = 0; index < count; index++)
   {
      RegisterData& data = mListenerList[index];
      if (data.receiver == receiver)
      {
         mListenerList.erase(mListenerList.begin() + index);
         return true;
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
void TriggerVolumeActor::TriggerEvent(dtCore::Transformable* instigator, TriggerEventType eventType)
{
   // Increment the trigger cound whenever an occupant has entered the volume.
   if (eventType == ENTER_EVENT)
   {
      ++mTriggerCount;
   }

   // Notify all registered listeners of the triggered event.
   int count = (int)mListenerList.size();
   for (int index = 0; index < count; index++)
   {
      RegisterData& data = mListenerList[index];
      data.onEvent(instigator, eventType);
   }

   // If an occupant is leaving, we have exceeded our maximum trigger count,
   // and there are no more occupants, de-register this volume from receiving
   // messages.
   if (eventType == LEAVE_EVENT)
   {
      // If this trigger has expired, it no longer needs update messages
      if (mMaxTriggerCount != 0 && mTriggerCount >= mMaxTriggerCount)
      {
         if (mOccupancyList.empty())
         {
            DeregisterInstance(this);
         }
      }
   }
}
