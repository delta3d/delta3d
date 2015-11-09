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
 * William E. Johnson II, David Guthrie, Curtiss Murphy
 */

#include <prefix/dtgameprefix.h>
#include <dtGame/gameactor.h>
#include <dtGame/basemessages.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/shaderactorcomponent.h>

#include <dtCore/shadergroup.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/shadermanager.h>

#include <dtUtil/log.h>
#include <dtUtil/exception.h>

namespace dtGame
{

   ///////////////////////////////////////////
   // Actor code
   ///////////////////////////////////////////
   const std::string GameActor::NULL_PROXY_ERROR("The ActorObject for a game drawable is NULL.  This usually happens if the drawable is held in a RefPtr, but not the actor object is not.");

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   GameActor::GameActor(GameActorProxy& owner, const std::string& name)
      : BaseClass(name)
      , mOwner(&owner)
      , mLogger(dtUtil::Log::GetInstance("gameactor.cpp"))
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   GameActor::GameActor(GameActorProxy& owner, TransformableNode& node, const std::string& name)
      : BaseClass(node, name)
      , mOwner(&owner)
      , mLogger(dtUtil::Log::GetInstance("gameactor.cpp"))
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   GameActor::~GameActor()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   /*override*/ void GameActor::SetName(const std::string& name)
   {
      BaseClass::SetName(name);
      static bool settingName = false;
      if (settingName) return;
      settingName = true;
      GetGameActorProxy().SetName(name);
      settingName = false;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   GameActorProxy& GameActor::GetGameActorProxy()
   {
      if (!mOwner.valid())
      {
         throw dtGame::InvalidActorStateException(
                  NULL_PROXY_ERROR,
                  __FILE__, __LINE__);
      }
      return *mOwner;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   const GameActorProxy& GameActor::GetGameActorProxy() const
   {
      if (!mOwner.valid())
      {
         throw dtGame::InvalidActorStateException(
                  NULL_PROXY_ERROR,
                  __FILE__, __LINE__);
      }
      return *mOwner;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GameActor::IsGameActorProxyValid() const
   {
      return mOwner.valid();
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::BuildActorComponents()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::OnTickLocal(const TickMessage& tickMessage)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::OnTickRemote(const TickMessage& tickMessage)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::ProcessMessage(const Message& message)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameActor::RemoveAllComponentsOfType(ActorComponent::ACType type)
   {
      GetGameActorProxy().RemoveAllComponentsOfType(type);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameActor::RemoveAllComponents()
   {
      GetGameActorProxy().RemoveAllComponents();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameActor::CallOnEnteredWorldForActorComponents()
   {
      GetGameActorProxy().CallOnEnteredWorldForActorComponents();
   }
   ////////////////////////////////////////////////////////////////////////////////
   void GameActor::CallOnRemovedFromWorldForActorComponents()
   {
      GetGameActorProxy().CallOnRemovedFromWorldForActorComponents();
   }
   ////////////////////////////////////////////////////////////////////////////////
   void GameActor::BuildComponentPropertyMaps()
   {
      GetGameActorProxy().BuildComponentPropertyMaps();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<ActorComponent*> GameActor::GetComponents(ActorComponent::ACType type) const
   {
      return GetGameActorProxy().GetComponents(type);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameActor::GetComponents(ActorComponent::ACType type, ActorComponentVector& toFill) const
   {
      GetGameActorProxy().GetComponents(type, toFill);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameActor::GetAllComponents(std::vector<ActorComponent*>& toFill)
   {
      GetGameActorProxy().GetAllComponents(toFill);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameActor::GetAllComponents(ActorComponentVectorConst& toFill) const
   {
      GetGameActorProxy().GetAllComponents(toFill);
   }


   ////////////////////////////////////////////////////////////////////////////////
   bool GameActor::HasComponent(ActorComponent::ACType type) const
   {
      return GetGameActorProxy().HasComponent(type);
   }


   ////////////////////////////////////////////////////////////////////////////////
   void GameActor::AddComponent(ActorComponent& component)
   {
      GetGameActorProxy().AddComponent(component);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameActor::RemoveComponent(ActorComponent& component)
   {
      GetGameActorProxy().RemoveComponent(component);
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string GameActor::GetPrototypeName() const
   {
      std::string result;

      if (GetGameActorProxy().GetPrototype() != NULL)
         result = GetGameActorProxy().GetPrototype()->GetName();
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId GameActor::GetPrototypeID() const
   {
      dtCore::UniqueId result(false);

      if (GetGameActorProxy().GetPrototype() != NULL)
         result = GetGameActorProxy().GetPrototype()->GetId();
      return result;
   }


   ////////////////////////////////////////////////////////////////////////////////
   bool GameActor::IsRemote() const
   {
      return GetGameActorProxy().IsRemote();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GameActor::IsPublished() const
   {
      return GetGameActorProxy().IsPublished();
   }


}

