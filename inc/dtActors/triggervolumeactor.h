/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2008, MOVES Institute
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* @author MG
*/
#ifndef TRIGGER_VOLUME_H
#define TRIGGER_VOLUME_H

#include <dtCore/plugin_export.h>

#include <dtGame/gameactor.h>
#include <dtCore/observerptr.h>
#include <dtCore/functor.h>

#include <osg/NodeVisitor>

namespace dtActors
{
   class TriggerVolumeActorProxy;

   class DT_PLUGIN_EXPORT TriggerVolumeActor : public dtGame::GameActor
   {
      DECLARE_MANAGEMENT_LAYER(TriggerVolumeActor)

   public:
      enum TriggerEventType
      {
         ENTER_EVENT,
         LEAVE_EVENT,
      };

      typedef dtUtil::Functor<void, TYPELIST_2(dtCore::Transformable*, TriggerEventType)> EventFuncType;

      TriggerVolumeActor(dtActors::TriggerVolumeActorProxy& proxy, const std::string& name = "TriggerVolumeActor");

      void SetMaxTriggerCount(int maxTriggercount) { mMaxTriggerCount = maxTriggercount; }
      int GetMaxTriggerCount() const               { return mMaxTriggerCount; }

      virtual void OnSystem(const dtUtil::RefString& str, double, double)
;


      ///From DeltaDrawable
      virtual void AddedToScene(dtCore::Scene* scene);

      ///From DeltaDrawable
      virtual void RemovedFromScene(dtCore::Scene* scene);


      /**
      * Registers a listener for trigger events caused by this volume.
      *
      * @param[in]  receiver       The receiver of the events.
      * @param[in]  eventCallback  The event callback functor to call.
      *
      * @param[in]  Returns false if the receiver was already registered.
      */
      bool RegisterListener(void* receiver, EventFuncType eventCallback);

      /**
      * Unregisters a listener from this trigger.
      *
      * @param[in]  receiver  The receiver of the events.
      *
      * @return     Returns false if the receiver was not registered.
      */
      bool UnregisterListener(void* receiver);


      /**
      * Instant check if another transformable/actor is in the volume.
      *
      * @param[in]  actor The collider.
      *
      * @return     Returns false if the actor is outside the volume.
      */
      bool IsActorInVolume(dtCore::Transformable* actor);


      /**
       *	Retrieves the occupancy list.
       */
      const std::set<dtCore::ObserverPtr<dtCore::Transformable> >& GetOccupants() const;

   protected:

      virtual ~TriggerVolumeActor() {}

      struct RegisterData
      {
         void*          receiver;
         EventFuncType  onEvent;
      };

      typedef std::set<dtCore::ObserverPtr<dtCore::Transformable> > CollidableContainer;
      CollidableContainer mOccupancyList; ///<list of previous collided objects
      CollidableContainer mNewCollisions; ///<list of this-frame collided objects

      std::vector<RegisterData> mListenerList;

      int mMaxTriggerCount;
      int mTriggerCount;

      bool IsActorAnOccupant(dtCore::Transformable* actor);

      void TriggerEvent(dtCore::Transformable* instigator, TriggerEventType eventType);
   };
}

#endif //TRIGGER_VOLUME_H
