/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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
 * @author Bradley Anderegg 06/07/2006
 */

#include <dtDAL/waypointactorproxy.h>
#include <dtAI/waypointmanager.h>
#include <dtAI/waypointactor.h>

#include <dtDAL/actorproxyicon.h>
#include <dtDAL/enginepropertytypes.h>

using namespace dtAI;

namespace dtDAL
{
   unsigned WaypointActorProxy::mNumWaypoints = 0;

   WaypointActorProxy::WaypointActorProxy()
   {
      SetClassName("WaypointActorProxy");
   }
   
   WaypointActorProxy::~WaypointActorProxy()
   {      
      
   }

   void WaypointActorProxy::CreateActor()
   {
      mActor = new WaypointActor;      
      WaypointManager::GetInstance()->AddWaypoint(static_cast<WaypointActor*>(GetActor()));    

      std::ostringstream ss;
      ss << "Waypoint" << mNumWaypoints++;
      SetName(ss.str());
   }  


   void WaypointActorProxy::BuildPropertyMap()
   {
      dtDAL::TransformableActorProxy::BuildPropertyMap();
   }

   const dtDAL::ActorProxy::RenderMode& WaypointActorProxy::GetRenderMode()
   {
      return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
   }


   dtDAL::ActorProxyIcon *WaypointActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         mBillBoardIcon =
            new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::GENERIC);
      }

      return mBillBoardIcon.get();
   }


   void WaypointActorProxy::OnTranslation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      WaypointActor *pWaypointActor = static_cast<WaypointActor*> (mActor.get());
      WaypointManager::GetInstance()->MoveWaypoint(pWaypointActor->GetIndex(), newValue);
   }

   bool WaypointActorProxy::IsPlaceable() const
   {
      return false;
   }
   
   bool WaypointActorProxy::IsGhostProxy() const
   {
      return true;
   }

   void WaypointActorProxy::RemoveFromWaypointManager() const
   {
      WaypointManager::GetInstance()->RemoveWaypoint(static_cast<const WaypointActor*> (mActor.get()));
   }


}//namespace 
