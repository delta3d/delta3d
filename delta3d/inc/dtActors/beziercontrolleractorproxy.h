#ifndef __BEZIER_CONTROLLER_ACTOR_PROXY_H__
#define __BEZIER_CONTROLLER_ACTOR_PROXY_H__

#include <dtCore/deltadrawable.h>
#include <dtDAL/plugin_export.h>
#include "motionactionactorproxy.h"


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
* @author Bradley Anderegg
*/

namespace dtActors
{


class DT_PLUGIN_EXPORT BezierControllerActorProxy: public MotionActionActorProxy
{

public:

   /*virtual*/ void BuildPropertyMap();
   void SetActorStartNode(dtCore::DeltaDrawable* pBezierNode);
   dtCore::DeltaDrawable* GetActorStartNode();
   
   /*virtual*/ const ActorProxy::RenderMode& GetRenderMode();

protected:
   /*virtual*/ void CreateActor();

};



}//namespace dtActors

#endif //__BEZIER_CONTROLLER_ACTOR_PROXY_H__

