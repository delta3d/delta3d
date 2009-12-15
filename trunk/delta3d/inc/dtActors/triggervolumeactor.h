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

#include <dtDAL/plugin_export.h>

#include <dtGame/gameactor.h>
#include <dtCore/observerptr.h>
#include <dtABC/action.h>

#include <osg/NodeVisitor>

namespace dtActors
{
   class TriggerVolumeActorProxy;

   class DT_PLUGIN_EXPORT TriggerVolumeActor : public dtGame::GameActor
   {
      DECLARE_MANAGEMENT_LAYER(TriggerVolumeActor)

   public:

      TriggerVolumeActor(dtActors::TriggerVolumeActorProxy& proxy);

      void SetEnterAction(dtABC::Action* action) { mEnterAction = action; }
      void SetExitAction(dtABC::Action* action)  { mExitAction = action; }

      void SetMaxTriggerCount(int maxTriggercount) { mMaxTriggerCount = maxTriggercount; }
      int GetMaxTriggerCount() const               { return mTriggerCount; }

      dtABC::Action* GetEnterAction()             { return mEnterAction.get(); }
      const dtABC::Action* GetEnterAction() const { return mEnterAction.get(); }

      dtABC::Action* GetExitAction()             { return mExitAction.get(); }
      const dtABC::Action* GetExitAction() const { return mExitAction.get(); }

      virtual void OnMessage(dtCore::Base::MessageData* data);

      /**
      * Callback from Scene when a contact occurs. This normally is used to 
      * filter out Transformables that you do not want to perform expensive
      * physics calculations on, but here we use it to fire our Trigger.
      */
      virtual bool FilterContact(dContact* contact, Transformable* collider);

   protected:

       virtual ~TriggerVolumeActor() {}

   private: 

      std::vector<dtCore::ObserverPtr<dtCore::Transformable> > mOccupancyList;

      dtCore::RefPtr<dtABC::Action> mEnterAction;
      dtCore::RefPtr<dtABC::Action> mExitAction;

      int mMaxTriggerCount;
      int mTriggerCount;

      bool IsActorInVolume(dtCore::Transformable* actor);
      bool IsActorAnOccupant(dtCore::Transformable* actor);

      void TriggerAction(dtABC::Action& actionToFire);
   };
}

#endif //TRIGGER_VOLUME_H
