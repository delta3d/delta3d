#include <dtActors/triggervolumeactor.h>
#include <dtActors/triggervolumeactorproxy.h>

#include <dtCore/scene.h>
#include <dtCore/system.h>

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

}

////////////////////////////////////////////////////////////////////////////////
void TriggerVolumeActor::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{
   // Do not send events in STAGE.
   if (IsGameActorProxyValid() && GetGameActorProxy().IsInSTAGE())
   {
      return;
   }

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
      int numContacts = 0;

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
void dtActors::TriggerVolumeActor::AddedToScene(dtCore::Scene* scene)
{
   if (scene)
   {
      //AddSender(scene);
   }

   dtGame::GameActor::AddedToScene(scene);
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::TriggerVolumeActor::RemovedFromScene(dtCore::Scene* scene)
{
   if (scene)
   {
      //RemoveSender(scene);
   }

   dtGame::GameActor::RemovedFromScene(scene);
}
