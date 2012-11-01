#include <dtActors/triggervolumeactor.h>
#include <dtActors/triggervolumeactorproxy.h>

#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/collisioncategorydefaults.h>
#include <dtCore/odecontroller.h>

#include <ode/collision.h>

#include <assert.h>
#include <iostream>
#include <iterator>
using namespace dtActors;

IMPLEMENT_MANAGEMENT_LAYER(TriggerVolumeActor)

////////////////////////////////////////////////////////////////////////////////
TriggerVolumeActor::TriggerVolumeActor(dtActors::TriggerVolumeActorProxy& proxy,
                                       const std::string& name)
   : dtGame::GameActor(proxy, name)
   , mMaxTriggerCount(0)
   , mTriggerCount(0)
{
   RegisterInstance(this);

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
   if (IsGameActorProxyValid() && GetGameActorProxy().IsInSTAGE())
   {
      return;
   }

   if (data->message == dtCore::ODEController::MESSAGE_PHYSICS_STEP)
   {
      mNewCollisions.clear(); //ensure we start with no new collisions
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
   if (IsGameActorProxyValid() && GetGameActorProxy().IsInSTAGE())
   {
      return false;
   }

   //store this collision
   mNewCollisions.insert(collider);

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
      dContactGeom contactGeoms[1];

      int numContacts = dCollide(GetGeomID(), actor->GetGeomID(), 1, contactGeoms, sizeof(dContactGeom));

      inVolume = numContacts > 0;
   }

   return inVolume;
}

////////////////////////////////////////////////////////////////////////////////
bool TriggerVolumeActor::IsActorAnOccupant(dtCore::Transformable* actor)
{
   // If we have the actor in the list (the volume), it's an occupant
   CollidableContainer::iterator found = mOccupancyList.find(actor);
   if (found != mOccupancyList.end())
   {
      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
const std::set<dtCore::ObserverPtr<dtCore::Transformable> >& TriggerVolumeActor::GetOccupants() const
{
   return mOccupancyList;
}

////////////////////////////////////////////////////////////////////////////////
void TriggerVolumeActor::TriggerEvent(dtCore::Transformable* instigator, TriggerEventType eventType)
{
   // Increment the trigger count whenever an occupant has entered the volume.
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


////////////////////////////////////////////////////////////////////////////////
void dtActors::TriggerVolumeActor::PostPhysicsStepUpdate()
{

   //what's in the new that isn't in the old: these are collidables that just entered the volume
   {
      CollidableContainer result;

      CollidableContainer::iterator itr;
      std::set_difference(mNewCollisions.begin(), mNewCollisions.end(),
                          mOccupancyList.begin(), mOccupancyList.end(), std::inserter(result, result.begin()));

      CollidableContainer::iterator enteredItr = result.begin();
      while (enteredItr != result.end())
      {
         dtCore::Transformable* occupant = (*enteredItr).get();
         if (occupant)
         {
            TriggerEvent(occupant, ENTER_EVENT);
         }
         ++enteredItr;
      }

   }

   //what's in the old that isn't in the new: these are collidables that just left the volume
   {
      CollidableContainer result;
      std::set_difference(mOccupancyList.begin(), mOccupancyList.end(),
                          mNewCollisions.begin(), mNewCollisions.end(),std::inserter(result, result.begin()));

      CollidableContainer::iterator leftItr = result.begin();
      while (leftItr != result.end())
      {
         dtCore::Transformable* occupant = (*leftItr).get();
         if (occupant)
         {
            TriggerEvent(occupant, LEAVE_EVENT);
            CollidableContainer::iterator foundItr = mOccupancyList.find(occupant);
            if (foundItr != mOccupancyList.end())
            {
               mOccupancyList.erase(foundItr);
            }
         }
         ++leftItr;
      }
   }

   //Add the newly collided to the old
   mOccupancyList.insert(mNewCollisions.begin(), mNewCollisions.end());

   dtGame::GameActor::PostPhysicsStepUpdate();
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::TriggerVolumeActor::AddedToScene(dtCore::Scene* scene)
{
   if (scene)
   {
      AddSender(scene);
   }

   dtGame::GameActor::AddedToScene(scene);
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::TriggerVolumeActor::RemovedFromScene(dtCore::Scene* scene)
{
   if (scene)
   {
      RemoveSender(scene);
   }

   dtGame::GameActor::RemovedFromScene(scene);
}
