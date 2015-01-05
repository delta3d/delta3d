/* -*-c++-*-
 * Delta3D
 * Copyright 2014, David Guthrie
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

#include <dtGame/gmcomponent.h>
#include <dtGame/baseinputcomponent.h>
#include <dtGame/defaultnetworkpublishingcomponent.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/logcontroller.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/taskcomponent.h>
#include <dtGame/gamestatecomponent.h>


#include <dtGame/actorcomponent.h>
#include <dtGame/cascadingdeleteactorcomponent.h>
#include <dtGame/shaderactorcomponent.h>
#include <dtGame/deadreckoninghelper.h>
#include <dtGame/drpublishingactcomp.h>


namespace dtGame
{
   const dtCore::RefPtr<dtCore::SystemComponentType> GMComponent::BaseGMComponentType(new dtCore::SystemComponentType("Base",
         "GMComponents", "A base type for all gm component types to set as a parent"));

   const dtCore::RefPtr<dtCore::SystemComponentType> BaseInputComponent::DEFAULT_TYPE(new dtCore::SystemComponentType("BaseInputComponent", "GMComponents",
         "Tank Target input", dtGame::GMComponent::BaseGMComponentType));
   const std::string BaseInputComponent::DEFAULT_NAME(BaseInputComponent::DEFAULT_TYPE->GetName());

   const dtCore::RefPtr<dtCore::SystemComponentType> DefaultMessageProcessor::TYPE(new dtCore::SystemComponentType("DefaultMessageProcessor", "GMComponents",
         "This handles base system messages that come in remotely such as actor created, deleted, and updated plus a few other system level things.  You almost always need this component.", dtGame::GMComponent::BaseGMComponentType));
   const std::string DefaultMessageProcessor::DEFAULT_NAME(DefaultMessageProcessor::TYPE->GetName());

   const dtCore::RefPtr<dtCore::SystemComponentType> DefaultNetworkPublishingComponent::TYPE(new dtCore::SystemComponentType("DefaultNetworkPublishingComponent","GMComponents",
         "Takes messages from published actors and other configurable messages and pushes them to the network queue.", dtGame::GMComponent::BaseGMComponentType));
   const std::string DefaultNetworkPublishingComponent::DEFAULT_NAME(DefaultNetworkPublishingComponent::TYPE->GetName());

   const dtCore::RefPtr<dtCore::SystemComponentType> LogController::TYPE(new dtCore::SystemComponentType("LogController","GMComponents",
         "Controls a ServerLoggerComponent either locally or on a remote system.", dtGame::GMComponent::BaseGMComponentType));
   const std::string LogController::DEFAULT_NAME(LogController::TYPE->GetName());

   const dtCore::RefPtr<dtCore::SystemComponentType> ServerLoggerComponent::TYPE(new dtCore::SystemComponentType("ServerLoggerComponent","GMComponents",
         "Logs messages to disk and allows for playback of recorded simulations.",
         dtGame::GMComponent::BaseGMComponentType));
   const std::string ServerLoggerComponent::DEFAULT_NAME(ServerLoggerComponent::TYPE->GetName());

   const dtCore::RefPtr<dtCore::SystemComponentType> TaskComponent::TYPE(new dtCore::SystemComponentType("TaskComponent","GMComponents",
         "Manages Task Actors.", dtGame::GMComponent::BaseGMComponentType));
   const std::string TaskComponent::DEFAULT_NAME(TaskComponent::TYPE->GetName());

   const dtCore::RefPtr<dtCore::SystemComponentType> GameStateComponent::TYPE(new dtCore::SystemComponentType("GameStateComponent","GMComponents",
         "Saves and transitions the game state.", dtGame::GMComponent::BaseGMComponentType));

   const std::string GameStateComponent::DEFAULT_NAME(TYPE->GetName());



   const ActorComponent::ACType ActorComponent::BaseActorComponentType(new dtCore::ActorType("Base", "ActorComponents", "A base type so that all actor component types should set as a parent"));

   const ActorComponent::ACType CascadingDeleteActorComponent::TYPE(new dtCore::ActorType("CascadingDeleteActorComponent", "ActorComponents",
         "Deletes associated actors when the owner is deleted.", dtGame::ActorComponent::BaseActorComponentType));

   const ActorComponent::ACType ShaderActorComponent::TYPE(new dtCore::ActorType("ShaderActorComponent", "ActorComponents",
         "Moves, rotates, and ground-clamps actors based on dead-reckoning data.  It also receives the updates from the network.",
         ActorComponent::BaseActorComponentType));

   const ActorComponent::ACType DeadReckoningActorComponent::TYPE(new dtCore::ActorType("DeadReckoningActComp", "ActorComponents",
         "Moves, rotates, and ground-clamps actors based on dead-reckoning data.  It also receives the updates from the network.",
         ActorComponent::BaseActorComponentType));

   const ActorComponent::ACType DRPublishingActComp::TYPE(new dtCore::ActorType("DRPublishingActComp", "ActorComponents",
         "Times and publishes network updates based on velocity, acceleration, and other dead-reckoning related features.  It works best with a Dead-Reckoning Actor Component",
         ActorComponent::BaseActorComponentType));


}




