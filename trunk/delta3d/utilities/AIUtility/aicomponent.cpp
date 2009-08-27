/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, Alion Science and Technology, BMH Operation
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
 * Bradley Anderegg
 */


#include "aicomponent.h"

#include <dtCore/scene.h>

#include <dtABC/application.h>
#include <dtGame/basemessages.h>

#include <dtAI/aiactorregistry.h>
#include <dtAI/aiinterfaceactor.h>
#include <dtAI/aiplugininterface.h>
#include <dtAI/aidebugdrawable.h>

#include "aiutilityapp.h"

const std::string AIComponent::DEFAULT_NAME("AIComponent");

/////////////////////////////////////////////////////////////
AIComponent::AIComponent(const std::string& name)
   : dtGame::GMComponent(name)
{

}

/////////////////////////////////////////////////////////////
AIComponent::~AIComponent()
{

}

/////////////////////////////////////////////////////////////
void AIComponent::OnAddedToGM()
{

}

/////////////////////////////////////////////////////////////
void AIComponent::OnRemovedFromGM()
{
   CleanUp();
}

/////////////////////////////////////////////////////////////
void AIComponent::CleanUp()
{
   if (mAIInterfaceProxy.valid())
   {
      GetGameManager()->GetScene().RemoveDrawable(GetAIPluginInterface()->GetDebugDrawable());
      mAIInterfaceProxy = NULL;
   }
}

/////////////////////////////////////////////////////////////
dtAI::AIPluginInterface* AIComponent::GetAIPluginInterface()
{
   if (mAIInterfaceProxy.valid())
   {
      return mAIInterfaceProxy->GetAIInterface();
   }
   return NULL;
}


/////////////////////////////////////////////////////////////
void AIComponent::ProcessMessage(const dtGame::Message& message)
{
   if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
   {
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_GAME_EVENT)
   {
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      //when the map is loaded we must look for an instance of an AIInterfaceActor and add it's debug drawable
      //to the scene, this will make the waypoints visible in the map
      dtAI::AIInterfaceActorProxy* aiInterface = NULL;
      GetGameManager()->FindActorByType(*dtAI::AIActorRegistry::AI_INTERFACE_ACTOR_TYPE, aiInterface);
      mAIInterfaceProxy = aiInterface;

      if (mAIInterfaceProxy.valid())
      {
         GetGameManager()->GetScene().AddDrawable(GetAIPluginInterface()->GetDebugDrawable());
      }

      AIUtilityApp& aiApp = dynamic_cast<AIUtilityApp&>(GetGameManager()->GetApplication());
      aiApp.SetAIPluginInterface(mAIInterfaceProxy->GetAIInterface());
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_UNLOADED)
   {
      CleanUp();
   }
}


