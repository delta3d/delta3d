/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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

#include <fireFighter/halonactors.h>
#include <dtDAL/gameeventmanager.h>
#include <dtGame/gamemanager.h>
#include <dtGame/basemessages.h>

using dtCore::RefPtr;

////////////////////////////////////////////////////////
PrimaryHalonActorProxy::PrimaryHalonActorProxy()
{

}

PrimaryHalonActorProxy::~PrimaryHalonActorProxy()
{

}

void PrimaryHalonActorProxy::BuildPropertyMap()
{
   GameItemActorProxy::BuildPropertyMap();
}

void PrimaryHalonActorProxy::BuildInvokables()
{
   GameItemActorProxy::BuildInvokables();
}

////////////////////////////////////////////////////////
PrimaryHalonActor::PrimaryHalonActor(dtGame::GameActorProxy& proxy)
   : GameItemActor(proxy)
{

}

PrimaryHalonActor::~PrimaryHalonActor()
{

}

void PrimaryHalonActor::Activate(bool enable)
{
   GameItemActor::Activate(enable);

   const std::string& name = "ThrowPrimaryHalon";

   // No event, peace out
   if (!IsActivated())
   {
      return;
   }

   dtDAL::GameEvent* event = dtDAL::GameEventManager::GetInstance().FindEvent(name);
   if (event == NULL)
   {
      throw dtUtil::Exception("Failed to find the game event: " + name, __FILE__, __LINE__);
   }

   dtGame::GameManager& mgr = *GetGameActorProxy().GetGameManager();
   RefPtr<dtGame::Message> msg =
      mgr.GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT);

   dtGame::GameEventMessage& gem = static_cast<dtGame::GameEventMessage&>(*msg);
   gem.SetGameEvent(*event);
   mgr.SendMessage(gem);
}

////////////////////////////////////////////////////////
SecondaryHalonActorProxy::SecondaryHalonActorProxy()
{

}

SecondaryHalonActorProxy::~SecondaryHalonActorProxy()
{

}

void SecondaryHalonActorProxy::BuildPropertyMap()
{
   GameItemActorProxy::BuildPropertyMap();
}

void SecondaryHalonActorProxy::BuildInvokables()
{
   GameItemActorProxy::BuildInvokables();
}

////////////////////////////////////////////////////////
SecondaryHalonActor::SecondaryHalonActor(dtGame::GameActorProxy& proxy) :
   GameItemActor(proxy)
{

}

SecondaryHalonActor::~SecondaryHalonActor()
{

}

void SecondaryHalonActor::Activate(bool enable)
{
   GameItemActor::Activate(enable);

   const std::string& name = "ThrowSecondaryHalon";

   // No event, peace out
   if (!IsActivated())
   {
      return;
   }

   dtDAL::GameEvent* event = dtDAL::GameEventManager::GetInstance().FindEvent(name);
   if (event == NULL)
   {
      throw dtUtil::Exception("Failed to find the game event: " + name, __FILE__, __LINE__);
   }

   dtGame::GameManager& mgr = *GetGameActorProxy().GetGameManager();
   RefPtr<dtGame::Message> msg =
      mgr.GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT);

   dtGame::GameEventMessage& gem = static_cast<dtGame::GameEventMessage&>(*msg);
   gem.SetGameEvent(*event);
   mgr.SendMessage(gem);
}
