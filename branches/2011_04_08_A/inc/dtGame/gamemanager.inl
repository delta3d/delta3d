/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008, Alion Science and Technology
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

#ifndef DELTA_GAMEMANAGER_INL
#define DELTA_GAMEMANAGER_INL

#include <algorithm>
#include <dtGame/gmimpl.h>

namespace dtGame
{
   template <typename UnaryFunctor, typename pairType>
   class BindActor
   {
   public:
      BindActor(UnaryFunctor func)
      : mFunc(func)
      {}

      void operator () (pairType thePair)
      {
         mFunc(*thePair.second);
      }
   private:
      UnaryFunctor mFunc;
   };

   template <typename FindFunctor>
   class FindFuncWrapper
   {
   public:
      FindFuncWrapper(FindFunctor ifFunc, std::vector<dtDAL::BaseActorObject*>& selectedActors)
      : mFunc(ifFunc)
      , mSelectedActors(selectedActors)
      {}

      void operator () (dtDAL::BaseActorObject& proxy)
      {
         if (mFunc(proxy))
         {
            mSelectedActors.push_back(&proxy);
         }
      }

   private:
      FindFunctor mFunc;
      std::vector<dtDAL::BaseActorObject*>& mSelectedActors;
   };

   template <typename UnaryFunctor>
   inline void GameManager::ForEachActor(UnaryFunctor func) const
   {
      BindActor<UnaryFunctor, GMImpl::ActorMap::value_type> actorMapBindFunc(func);
      std::for_each(mGMImpl->mBaseActorObjectMap.begin(), mGMImpl->mBaseActorObjectMap.end(), actorMapBindFunc);
      BindActor<UnaryFunctor, GMImpl::GameActorMap::value_type> gameActorMapBindFunc(func);
      std::for_each(mGMImpl->mGameActorProxyMap.begin(), mGMImpl->mGameActorProxyMap.end(), gameActorMapBindFunc);
   }

   template <typename UnaryFunctor>
   inline void GameManager::ForEachPrototype(UnaryFunctor func) const
   {
      BindActor<UnaryFunctor, GMImpl::GameActorMap::value_type> gameActorMapBindFunc(func);
      std::for_each(mGMImpl->mPrototypeActors.begin(), mGMImpl->mPrototypeActors.end(), gameActorMapBindFunc);
   }

   template <typename FindFunctor>
   inline void GameManager::FindActorsIf(FindFunctor ifFunc, std::vector<dtDAL::BaseActorObject*>& toFill) const
   {
      toFill.clear();
      FindFuncWrapper<FindFunctor> findWrapper(ifFunc, toFill);
      ForEachActor(findWrapper);
   }

   template <typename FindFunctor>
   inline void GameManager::FindPrototypesIf(FindFunctor ifFunc, std::vector<dtDAL::BaseActorObject*>& toFill) const
   {
      toFill.clear();
      FindFuncWrapper<FindFunctor> findWrapper(ifFunc, toFill);
      ForEachPrototype(findWrapper);
   }
}
#endif
