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
#include <dtCore/datatype.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/mapxml.h>
#include <dtCore/project.h>
#include <dtUtil/exception.h>
#include <dtCore/functor.h>

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
   void PrefabActorProxy::CreateDrawable()
   {
      SetDrawable(*new PrefabActor());
   }

   /////////////////////////////////////////////////////////////////////////////
   void PrefabActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      PrefabActor* actor = NULL;
      GetDrawable(actor);

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::PREFAB,
         "PrefabResource", "Prefab",
         dtCore::ResourceActorProperty::SetFuncType(this, &PrefabActorProxy::SetPrefab),
         "Defines the Prefab resource to use.", "Prefab"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PrefabActorProxy::SetMap(dtCore::Map* map)
   {
      mMap = map;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabActorProxy::SetPrefab(const std::string& fileName)
   {
      PrefabActor* drawable = NULL;
      GetDrawable(drawable);

      if (drawable == NULL)
      {
         return;
      }

      // First remove the current actors from any previous Prefabs...
      for (int proxyIndex = 0; proxyIndex < (int)mActors.size(); proxyIndex++)
      {
         dtCore::BaseActorObject* actor = mActors[proxyIndex].get();
         if (actor)
         {
            dtCore::DeltaDrawable* childDrawable = NULL;
            actor->GetDrawable(childDrawable);

            if (childDrawable)
            {
               drawable->RemoveChild(childDrawable);
            }
         }
      }

      mActors.clear();

      if (!fileName.empty())
      {
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.PushDirectory(dtCore::Project::GetInstance().GetContext());
         try
         {
            dtCore::RefPtr<dtCore::MapParser> parser = new dtCore::MapParser;
            parser->ParsePrefab(fileName, mActors, mMap.get());

            for (int idx = 0; idx < (int)mActors.size(); idx++)
            {
               dtCore::BaseActorObject* actor = mActors[idx].get();

               dtCore::DeltaDrawable* childDrawable = NULL;
               actor->GetDrawable(childDrawable);

               if (childDrawable)
               {
                  drawable->AddChild(childDrawable);
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
   std::vector<dtCore::RefPtr<dtCore::BaseActorObject> >& PrefabActorProxy::GetPrefabProxies()
   {
      return mActors;
   }
}
