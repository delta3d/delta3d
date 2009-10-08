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

#include <dtAI/baseaicomponent.h>
#include <dtCore/scene.h>

#include <dtABC/application.h>
#include <dtGame/basemessages.h>

#include <dtAI/aiactorregistry.h>
#include <dtAI/aiinterfaceactor.h>
#include <dtAI/aiplugininterface.h>
#include <dtAI/aidebugdrawable.h>

namespace dtAI
{
   const std::string BaseAIComponent::DEFAULT_NAME = "BaseAIComponent";


   BaseAIComponent::BaseAIComponent(const std::string& name)
      : dtGame::GMComponent(name)
   {

   }
   /////////////////////////////////////////////////////////////
   void BaseAIComponent::OnAddedToGM()
   {

   }

   /////////////////////////////////////////////////////////////
   void BaseAIComponent::OnRemovedFromGM()
   {
      CleanUp();
   }

   /////////////////////////////////////////////////////////////
   BaseAIComponent::~BaseAIComponent()
   {
      //
   }

   /////////////////////////////////////////////////////////////
   void BaseAIComponent::CleanUp()
   {
      mAIInterfaceProxy = NULL;
   }

   /////////////////////////////////////////////////////////////
   dtAI::AIPluginInterface* BaseAIComponent::GetAIPluginInterface()
   {
      dtAI::AIPluginInterface* interface = mAIInterfaceProxy.valid() ? mAIInterfaceProxy->GetAIInterface() : NULL;
      return interface;
   }

   /////////////////////////////////////////////////////////////
   const dtAI::AIPluginInterface* BaseAIComponent::GetAIPluginInterface() const
   {
      const dtAI::AIPluginInterface* interface = mAIInterfaceProxy.valid() ? mAIInterfaceProxy->GetAIInterface() : NULL;
      return interface;
   }

   /////////////////////////////////////////////////////////////
   void BaseAIComponent::ProcessMessage(const dtGame::Message &message)
   {
      if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
      {
         //float dt = float(static_cast<const dtGame::TickMessage&>(message).GetDeltaSimTime());
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_GAME_EVENT)
      {
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
      {
         // when the map is loaded we must look for an instance of an AIInterfaceActor and add it's debug drawable
         // to the scene, this will make the waypoints visible in the map
         dtAI::AIInterfaceActorProxy* aiInterface = NULL;
         GetGameManager()->FindActorByType(*dtAI::AIActorRegistry::AI_INTERFACE_ACTOR_TYPE, aiInterface);
         mAIInterfaceProxy = aiInterface;
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_UNLOADED)
      {
         CleanUp();
      }
   }
   
} // namespace dtAI
