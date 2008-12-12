/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * Matthew W. Campbell
 */
#include <prefix/dtdalprefix-src.h>
#include "dtDAL/actorpluginregistry.h"
#include <dtCore/scene.h>

namespace dtDAL
{

   ///////////////////////////////////////////////////////////////////////////////////
   void ActorPluginRegistry::GetSupportedActorTypes(std::vector<dtCore::RefPtr<const ActorType> > &actors)
   {
      mActorFactory->GetSupportedTypes(actors);
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool ActorPluginRegistry::IsActorTypeSupported(dtCore::RefPtr<const ActorType> type)
   {
      return mActorFactory->IsTypeSupported(type);
   }

   ///////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ActorProxy> ActorPluginRegistry::CreateActorProxy(const ActorType& type)
   {
      dtCore::RefPtr<ActorProxy> proxy = mActorFactory->CreateObject(dtCore::RefPtr<const ActorType>(&type));
      proxy->Init(type);
      return proxy;
   }
}
