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

#include <dtActors/prefabactor.h>
#include <dtCore/datatype.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/mapxml.h>
#include <dtCore/project.h>
#include <dtUtil/exception.h>
#include <dtCore/functor.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString PrefabActor::CLASS_NAME("dtActors::PrefabActor");

   /////////////////////////////////////////////////////////////////////////////
   PrefabActor::PrefabActor()
      : BaseClass()
   {
      SetClassName(CLASS_NAME.Get());
   }

   /////////////////////////////////////////////////////////////////////////////
   PrefabActor::~PrefabActor()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void PrefabActor::CreateDrawable()
   {
      SetDrawable(*new dtCore::Transformable);
      SetName("Prefab");
   }

   /////////////////////////////////////////////////////////////////////////////
   void PrefabActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::PREFAB,
         "PrefabResource", "PrefabResource",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &PrefabActor::SetPrefabResource),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &PrefabActor::GetPrefabResource),
         "Defines the Prefab resource to use.", "PrefabActor"));
   }

   ///////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR_WITH_STATEMENT(PrefabActor, dtCore::ResourceDescriptor, PrefabResource, LoadPrefab(value);)

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabActor::LoadPrefab(const dtCore::ResourceDescriptor& value)
   {
      if (!mPrefabResource.IsEmpty())
         DetachChildActors(true);

      if (!value.IsEmpty())
      {
         dtCore::ActorRefPtrVector actors;
         dtCore::Project::GetInstance().LoadPrefab(value, actors);
         std::for_each(actors.begin(), actors.end(),
               [this](dtCore::ActorPtr& baseActor)
               {
                  if (baseActor->IsGameActor())
                  {
                     dtGame::GameActorProxy* gameActor = static_cast<dtGame::GameActorProxy*>(baseActor.get());
                     gameActor->SetParentActor(this);
                  }
                  else
                  {
                     LOGN_ERROR("prefabactor.cpp", "The prefab actor will now only accept actors as children that extend GameActorProxy.  Other children are ignored.");
                  }
               });
      }
   }
}
