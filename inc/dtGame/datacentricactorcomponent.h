/* -*-c++-*-
 * Delta3D
 * Copyright 2013, 
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
 * David Guthrie
 */

#ifndef DATACENTRICACTORCOMPONENT_H_
#define DATACENTRICACTORCOMPONENT_H_

#include <dtGame/actorcomponent.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/gameactor.h>
#include <dtUtil/getsetmacros.h>

namespace dtGame
{
   /**
    * Actor component that will auto register and unregister with a DataCentricActorComponent.
    * It is meant to enable multithreaded updating where each aspect of the actor separates its data it is processed in
    * bulk.
    *
    * This is not data centric design per se, it merely facilitates it. If the data is stored in a block on the
    * GM component and this class accesses it via an index or offset pointer, it will be.
    *
    * Any processing that you do not want to run multi-threaded should be updated outside of the "Update" function.
    */
   template <typename GMComponentType, typename ActorComponentType>
   class DataCentricActorComponent: public dtGame::ActorComponent
   {
   public:
      DataCentricActorComponent(dtGame::ActorComponent::ACType type)
      : dtGame::ActorComponent(type)
      , mAutoRegisterWithGMComponent(true)
      {
      }

      DT_DECLARE_ACCESSOR_INLINE(bool, AutoRegisterWithGMComponent);

      virtual void OnEnteredWorld()
      {
         if (mAutoRegisterWithGMComponent)
         {
            RegisterWithGMComponent(false);
         }
      }

      virtual void OnRemovedFromWorld()
      {
         if (mAutoRegisterWithGMComponent)
         {
            RegisterWithGMComponent(true);
         }
      }

      virtual void Update(float dt) = 0;
   protected:
      virtual ~DataCentricActorComponent() {}

      void RegisterWithGMComponent(bool unregister)
      {
         GMComponentType* gmComp = NULL;

         dtGame::GameActorProxy* act;
         GetOwner(act);

         act->GetGameManager()->
                  GetComponentByName(GMComponentType::DEFAULT_NAME, gmComp);

         if (gmComp != NULL)
         {
            if (!unregister)
            {
               gmComp->RegisterActor(*act, *static_cast<ActorComponentType*>(this));
            }
            else
            {
               gmComp->UnregisterActor(act->GetId());
            }
         }
         else
         {
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                     "Actor \"%s\"\"%s\" unable to find component: %s",
                     act->GetName().c_str(), act->GetId().ToString().c_str(), GMComponentType::DEFAULT_NAME.c_str());
         }
      }
   };
}
#endif /* DATACENTRICACTORCOMPONENT_H_ */
