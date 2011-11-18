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
 * William E. Johnson II
 */
#include <prefix/dtgameprefix.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/message.h>

namespace dtGame
{
   //////////////////////////////////////////////
   GMComponent::GMComponent(const std::string& name) : dtCore::Base(name), mParent(NULL),
      mPriority(&GameManager::ComponentPriority::NORMAL)
   {
   }

   //////////////////////////////////////////////
   GMComponent::~GMComponent()
   {
   }

   //////////////////////////////////////////////
   void GMComponent::DispatchNetworkMessage(const Message& message)
   {
   }

   //////////////////////////////////////////////
   void GMComponent::ProcessMessage(const Message& message)
   {
   }

   //////////////////////////////////////////////
   const GameManager::ComponentPriority& GMComponent::GetComponentPriority() const
   {
      return *mPriority;
   }

   //////////////////////////////////////////////
   void GMComponent::OnAddedToGM() {}

   //////////////////////////////////////////////
   void GMComponent::OnRemovedFromGM() {}

   //////////////////////////////////////////////
   void GMComponent::SetGameManager(GameManager* gameManager)
   {
      mParent = gameManager;
   }

   //////////////////////////////////////////////
   void GMComponent::SetComponentPriority(const GameManager::ComponentPriority& newPriority)
   {
      mPriority = &newPriority;
   }

   //////////////////////////////////////////////
   GMComponent::GMComponent(const GMComponent&)
   {
   }

   //////////////////////////////////////////////
   GMComponent& GMComponent::operator=(const GMComponent&)
   {
      return *this;
   }

}
