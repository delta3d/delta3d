/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005, MOVES Institute
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
 * @author William E. Johnson II
 */
#include "dtActors/playerstartactorproxy.h"
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>

namespace dtActors
{
   ///////////////////////////////////////////////////
   // Actor proxy code
   ///////////////////////////////////////////////////
   PlayerStartActorProxy::PlayerStartActorProxy()
   {

   }

   PlayerStartActorProxy::~PlayerStartActorProxy()
   {

   }

   void PlayerStartActorProxy::BuildPropertyMap()
   {
      dtDAL::TransformableActorProxy::BuildPropertyMap();
   }

   dtDAL::ActorProxyIcon* PlayerStartActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
         mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::STATICMESH);

      return mBillBoardIcon.get();
   }

   ///////////////////////////////////////////////////
   // Actor code
   ///////////////////////////////////////////////////
   PlayerStartActor::PlayerStartActor()
   {

   }

   PlayerStartActor::~PlayerStartActor()
   {

   }
}
