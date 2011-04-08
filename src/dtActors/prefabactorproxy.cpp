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
 * Jeff P. Houde
 */

#include <dtActors/prefabactorproxy.h>
#include <dtDAL/datatype.h>
#include <dtDAL/resourceactorproperty.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/project.h>
#include <dtUtil/exception.h>
#include <dtDAL/functor.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   PrefabActor::PrefabActor(const std::string& name)
      : BaseClass(name)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   PrefabActor::~PrefabActor()
   {
   }


   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString PrefabActorProxy::CLASS_NAME("dtActors::PrefabActor");

   /////////////////////////////////////////////////////////////////////////////
   PrefabActorProxy::PrefabActorProxy()
      : BaseClass()
   {
      SetClassName(CLASS_NAME.Get());
   }

   /////////////////////////////////////////////////////////////////////////////
   PrefabActorProxy::~PrefabActorProxy()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void PrefabActorProxy::CreateActor()
   {
      SetActor(*new PrefabActor());
   }

   /////////////////////////////////////////////////////////////////////////////
   void PrefabActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      PrefabActor* actor = NULL;
      GetActor(actor);

      AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::PREFAB,
         "PrefabResource", "Prefab",
         dtDAL::ResourceActorProperty::SetFuncType(this, &PrefabActorProxy::SetPrefab),
         "Defines the Prefab resource to use.", "Prefab"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PrefabActorProxy::SetMap(dtDAL::Map* map)
   {
      mMap = map;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabActorProxy::SetPrefab(const std::string& fileName)
   {
      PrefabActor* actor = NULL;
      GetActor(actor);

      if (!actor)
      {
         return;
      }

      // First remove the current actors from any previous Prefabs...
      for (int proxyIndex = 0; proxyIndex < (int)mProxies.size(); proxyIndex++)
      {
         dtDAL::BaseActorObject* proxy = mProxies[proxyIndex].get();
         if (proxy)
         {
            dtCore::DeltaDrawable* proxyActor = NULL;
            proxy->GetActor(proxyActor);

            if (proxyActor)
            {
               actor->RemoveChild(proxyActor);
            }
         }
      }

      mProxies.clear();

      if (!fileName.empty())
      {
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());
         try
         {
            dtCore::RefPtr<dtDAL::MapParser> parser = new dtDAL::MapParser;
            parser->ParsePrefab(fileName, mProxies, mMap.get());

            for (int proxyIndex = 0; proxyIndex < (int)mProxies.size(); proxyIndex++)
            {
               dtDAL::BaseActorObject* proxy = mProxies[proxyIndex].get();

               dtCore::DeltaDrawable* proxyActor = NULL;
               proxy->GetActor(proxyActor);

               if (proxyActor)
               {
                  actor->AddChild(proxyActor);
               }
            }
         }
         catch (const dtUtil::Exception& e)
         {
            LOG_ERROR(e.What());
         }
         fileUtils.PopDirectory();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> >& PrefabActorProxy::GetPrefabProxies()
   {
      return mProxies;
   }
}
