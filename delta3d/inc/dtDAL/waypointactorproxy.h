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

#ifndef __DELTA_WAYPOINTACTORPROXY_H__
#define __DELTA_WAYPOINTACTORPROXY_H__

#include <dtDAL/export.h>
#include <dtDAL/transformableactorproxy.h>

namespace dtDAL
{
   /**
    * An actor proxy for Waypoint
    */
   class DT_DAL_EXPORT WaypointActorProxy : public dtDAL::TransformableActorProxy
   {
      public:
   
         WaypointActorProxy();
      protected:
         virtual ~WaypointActorProxy();
      
      public:

         /*virtual*/ void CreateActor();
         /*virtual*/ void BuildPropertyMap();

         /*virtual*/ bool IsPlaceable() const;

         /*virtual*/ dtDAL::ActorProxyIcon * GetBillBoardIcon();

         /*virtual*/ const dtDAL::ActorProxy::RenderMode& GetRenderMode();
   

        /*virtual*/ void OnTranslation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue);

        /*virtual*/ bool IsGhostProxy() const;

      private:
      
         static unsigned mNumWaypoints;
   };
}//namespace 

#endif // __DELTA_WAYPOINTACTORPROXY_H__
