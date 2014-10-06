/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004-2005 MOVES Institute 
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
* Bradley Anderegg
*/

#ifndef __BEZIER_CONTROLLER_ACTOR_COMPONENT_H__
#define __BEZIER_CONTROLLER_ACTOR_COMPONENT_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtCore/deltadrawable.h>
#include <dtCore/gameevent.h>
#include <dtCore/plugin_export.h>
#include <dtGame/basemessages.h>
#include <dtABC/beziercontroller.h>
#include <dtABC/beziernode.h>
#include <dtUtil/refstring.h>
#include "motionactionactorproxy.h"



namespace dtGame
{
   class GameEventMessage;
}

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT BezierControllerActorComponent : public dtGame::ActorComponent
   {
   public:
      typedef dtGame::ActorComponent BaseClass;
      static const dtGame::ActorComponent::ACType TYPE;

      static const dtUtil::RefString CLASS_NAME;

      static const dtUtil::RefString PROPERTY_EVENT_TO_START;
      static const dtUtil::RefString PROPERTY_EVENT_TO_PAUSE;
      static const dtUtil::RefString PROPERTY_EVENT_TO_UNPAUSE;
      static const dtUtil::RefString PROPERTY_EVENT_ON_START;
      static const dtUtil::RefString PROPERTY_EVENT_ON_PAUSE;
      static const dtUtil::RefString PROPERTY_EVENT_ON_UNPAUSE;
      static const dtUtil::RefString PROPERTY_EVENT_ON_NEXT_STEP;
      static const dtUtil::RefString PROPERTY_EVENT_ON_RESTART;
      static const dtUtil::RefString PROPERTY_EVENT_ON_END;

      BezierControllerActorComponent(const ACType& type = TYPE);

      dtABC::BezierController* GetController();

      /*virtual*/ void BuildPropertyMap();
      /*virtual*/ void OnEnteredWorld();

      void SetEventToStart(dtCore::GameEvent* gameEvent);
      dtCore::GameEvent* GetEventToStart() const;

      void SetEventToPause(dtCore::GameEvent* gameEvent);
      dtCore::GameEvent* GetEventToPause() const;

      void SetEventToUnPause(dtCore::GameEvent* gameEvent);
      dtCore::GameEvent* GetEventToUnPause() const;

      void SetEventOnStart(dtCore::GameEvent* gameEvent);
      dtCore::GameEvent* GetEventOnStart() const;

      void SetEventOnPause(dtCore::GameEvent* gameEvent);
      dtCore::GameEvent* GetEventOnPause() const;

      void SetEventOnUnPause(dtCore::GameEvent* gameEvent);
      dtCore::GameEvent* GetEventOnUnPause() const;

      void SetEventOnNextStep(dtCore::GameEvent* gameEvent);
      dtCore::GameEvent* GetEventOnNextStep() const;

      void SetEventOnRestart(dtCore::GameEvent* gameEvent);
      dtCore::GameEvent* GetEventOnRestart() const;

      void SetEventOnEnd(dtCore::GameEvent* gameEvent);
      dtCore::GameEvent* GetEventOnEnd() const;

      virtual void OnNextStep(dtABC::BezierController& controller);
      virtual void OnStart(dtABC::BezierController& controller);
      virtual void OnPause(dtABC::BezierController& controller);
      virtual void OnUnPause(dtABC::BezierController& controller);
      virtual void OnRestart(dtABC::BezierController& controller);
      virtual void OnEnd(dtABC::BezierController& controller);

      void ProcessGameEvent(const dtGame::GameEventMessage& gameEventMessage);
      
   protected:
      /*virtual*/ ~BezierControllerActorComponent();

      void SendGameEvent(dtCore::GameEvent* gameEvent);

      // Events received to trigger behavior.
      dtCore::RefPtr<dtCore::GameEvent> mEventToStart;
      dtCore::RefPtr<dtCore::GameEvent> mEventToPause;
      dtCore::RefPtr<dtCore::GameEvent> mEventToUnPause;

      // Events to send when something is happens.
      dtCore::RefPtr<dtCore::GameEvent> mEventOnStart;
      dtCore::RefPtr<dtCore::GameEvent> mEventOnPause;
      dtCore::RefPtr<dtCore::GameEvent> mEventOnUnPause;
      dtCore::RefPtr<dtCore::GameEvent> mEventOnNextStep;
      dtCore::RefPtr<dtCore::GameEvent> mEventOnRestart;
      dtCore::RefPtr<dtCore::GameEvent> mEventOnEnd;
   };



}//namespace dtActors

#endif //__BEZIER_CONTROLLER_ACTOR_PROXY_H__

