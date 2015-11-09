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
      BindActor(UnaryFunctor& func, GMImpl* impl = NULL, GameManager* gm = NULL)
      : mFunc(func)
      , mImpl(impl)
      , mGM(gm)
      {}

      void operator () (pairType thePair)
      {
         dtCore::BaseActorObject* actor = thePair.second;
         if (mImpl && actor->IsGameActor() && !static_cast<GameActorProxy*>(actor)->IsInGM() && !static_cast<GameActorProxy*>(actor)->IsDeleted())
         {
            GameActorProxy* gActor = static_cast<GameActorProxy*>(actor);
            try
            {
               mImpl->AddActorToWorld(*mGM, *gActor);
               mFunc(*actor);
            }
            catch (const dtUtil::Exception& ex)
            {
               // Actors can just decide to not be added by throwing an exception.  If it's an error, the actor
               // should log it as such, but here it's only a warning.
               ex.LogException(dtUtil::Log::LOG_WARNING, *mImpl->mLogger);
            }
         }
         else
         {
            mFunc(*actor);
         }
      }
   private:
      UnaryFunctor& mFunc;
      GMImpl* mImpl;
      GameManager* mGM;

   };

   template <typename FindFunctor>
   class FindFuncWrapper
   {
   public:
      FindFuncWrapper(FindFunctor& ifFunc, dtCore::ActorPtrVector& selectedActors)
      : mFunc(ifFunc)
      , mSelectedActors(selectedActors)
      {}

      void operator () (dtCore::BaseActorObject& actor)
      {
         if (mFunc(actor))
         {
            mSelectedActors.push_back(&actor);
         }
      }

   private:
      FindFunctor& mFunc;
      dtCore::ActorPtrVector& mSelectedActors;
   };

   template <typename UnaryFunctor>
   inline void GameManager::ForEachActor(UnaryFunctor func, bool applyOnlyToGameActors /*= false*/)
   {
      if (!applyOnlyToGameActors)
      {
         BindActor<UnaryFunctor, GMImpl::ActorMap::value_type> actorMapBindFunc(func);
         std::for_each(mGMImpl->mBaseActorObjectMap.begin(), mGMImpl->mBaseActorObjectMap.end(), actorMapBindFunc);
      }
      GMImpl* impl = NULL;
      if (mGMImpl->mBatchData.valid())
      {
         impl = mGMImpl;
      }
      BindActor<UnaryFunctor, GMImpl::GameActorMap::value_type> gameActorMapBindFunc(func, impl, this);
      std::for_each(mGMImpl->mGameActorProxyMap.begin(), mGMImpl->mGameActorProxyMap.end(), gameActorMapBindFunc);
   }

   template <typename UnaryFunctor>
   inline void GameManager::ForEachPrototype(UnaryFunctor func) const
   {
      BindActor<UnaryFunctor, GMImpl::GameActorMap::value_type> gameActorMapBindFunc(func);
      std::for_each(mGMImpl->mPrototypeActors.begin(), mGMImpl->mPrototypeActors.end(), gameActorMapBindFunc);
   }

   template <typename FindFunctor>
   inline void GameManager::FindActorsIf(FindFunctor ifFunc, dtCore::ActorPtrVector& toFill)
   {
      toFill.clear();
      FindFuncWrapper<FindFunctor> findWrapper(ifFunc, toFill);
      ForEachActor(findWrapper);
   }

   template <typename FindFunctor>
   inline void GameManager::FindPrototypesIf(FindFunctor ifFunc, dtCore::ActorPtrVector& toFill) const
   {
      toFill.clear();
      FindFuncWrapper<FindFunctor> findWrapper(ifFunc, toFill);
      ForEachPrototype(findWrapper);
   }
}
#endif
