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
#include <prefix/dtcoreprefix.h>
#include "dtCore/actorpluginregistry.h"
#include <dtCore/scene.h>

namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////
   ActorPluginRegistry::ActorPluginRegistry(const std::string& name, const std::string& desc)
      : mName(name)
      , mDescription(desc)
   {
      mActorFactory = new dtUtil::ObjectFactory<dtCore::RefPtr<const ActorType>, BaseActorObject, ActorType::RefPtrComp>;
   }

   /////////////////////////////////////////////////////////////////////////////
   ActorPluginRegistry::~ActorPluginRegistry() { }

   /////////////////////////////////////////////////////////////////////////////
   void ActorPluginRegistry::GetSupportedActorTypes(std::vector<dtCore::RefPtr<const ActorType> >& actors)
   {
      mActorFactory->GetSupportedTypes(actors);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool ActorPluginRegistry::IsActorTypeSupported(const ActorType& type) const
   {
      return mActorFactory->IsTypeSupported(&type);
   }

   /////////////////////////////////////////////////////////////////////////////
   const ActorType* ActorPluginRegistry::GetActorType(const std::string& category, const std::string& name) const
   {
      ActorTypePtr type = new ActorType(name, category);
      FactoryType::ObjectMap::const_iterator itr = mActorFactory->GetMap().find(type);
      if (itr != mActorFactory->GetMap().end())
      {
         return itr->first.get();
      }
      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseActorObject> ActorPluginRegistry::CreateActor(const ActorType& type)
   {
      dtCore::RefPtr<BaseActorObject> proxy = mActorFactory->CreateObject(dtCore::RefPtr<const ActorType>(&type));
      proxy->Init(type);
      proxy->InitDefaults();
      return proxy;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActorPluginRegistry::SetName(const std::string& name) { mName = name; }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& ActorPluginRegistry::GetName() const { return mName; }

   /////////////////////////////////////////////////////////////////////////////
   void ActorPluginRegistry::SetDescription(const std::string& desc) { mDescription = desc; }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& ActorPluginRegistry::GetDescription() const { return mDescription; }


   /////////////////////////////////////////////////////////////////////////////
   void ActorPluginRegistry::GetReplacementActorTypes(ActorPluginRegistry::ActorTypeReplacements& replacements) const
   {
   }
}
