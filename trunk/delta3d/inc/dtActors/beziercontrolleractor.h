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

#ifndef __BEZIER_CONTROLLER_ACTOR_H__
#define __BEZIER_CONTROLLER_ACTOR_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtCore/deltadrawable.h>
#include <dtCore/gameevent.h>
#include <dtCore/plugin_export.h>
#include <dtABC/beziercontroller.h>
#include <dtABC/beziernode.h>
#include <dtActors/beziercontrolleractorcomponent.h>
#include <dtActors/motionactionactorproxy.h>
#include <dtGame/basemessages.h>
#include <dtUtil/refstring.h>



namespace dtGame
{
   class GameEventMessage;
}

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT BezierControllerActor: public MotionActionActorProxy
   {
   public:
      typedef MotionActionActorProxy BaseClass;

      static const dtUtil::RefString CLASS_NAME;

      static const dtUtil::RefString INVOKABLE_PROCESS_GAME_EVENT;

      static const dtUtil::RefString PROPERTY_START_NODE;
      static const dtUtil::RefString PROPERTY_RENDER_PATH;
      static const dtUtil::RefString PROPERTY_FOLLOW_PATH;
      static const dtUtil::RefString PROPERTY_SET_LOOPING;

      BezierControllerActor();

      /*virtual*/ bool IsPlaceable() const;

      /*virtual*/ void BuildInvokables();
      /*virtual*/ void BuildPropertyMap();
      /*virtual*/ void BuildActorComponents();
      /*virtual*/ void OnEnteredWorld();

      dtCore::DeltaDrawable* GetActorStartNode();
      void SetActorStartNode(dtCore::BaseActorObject* node);

      void ProcessGameEvent(const dtGame::GameEventMessage& gameEventMessage);

   protected:
      /*virtual*/ ~BezierControllerActor();

      /*virtual*/ void CreateDrawable();
      static int mNumControllers;

      dtCore::RefPtr<BezierControllerActorComponent> mComp;
   };



}//namespace dtActors

#endif //__BEZIER_CONTROLLER_ACTOR_PROXY_H__

