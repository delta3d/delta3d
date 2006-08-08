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
 * @author William E. Johnson II
 */

#include "dtGame/defaultmessageprocessor.h"
#include "dtGame/gamemanager.h"
#include "dtGame/message.h"
#include "dtGame/actorupdatemessage.h"
#include "dtGame/basemessages.h"
#include "dtGame/messagetype.h"
#include "dtGame/gameactor.h"
#include "dtGame/exceptionenum.h"
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/log.h>

#include <iostream>

namespace dtGame
{
   DefaultMessageProcessor::DefaultMessageProcessor(const std::string& name) : GMComponent(name)
   {

   }

   DefaultMessageProcessor::~DefaultMessageProcessor()
   {

   }

   void DefaultMessageProcessor::ProcessMessage(const Message &msg)
   {
      //bool remote = false;
      if (GetGameManager() == NULL)
      {
         LOG_ERROR("This component is not assigned to a GameManager, but received a message.  It will be ignored.");         
         return;
      }  
             
      if (msg.GetDestination() != NULL && GetGameManager()->GetMachineInfo() != *msg.GetDestination())
      {
         if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            LOG_DEBUG("Received message has a destination set to a different machine that this one. It will be ignored.");         
            return;
         }
      }
      
      if (msg.GetMessageType() == MessageType::INFO_ACTOR_CREATED)
         ProcessCreateActor(static_cast<const ActorUpdateMessage&>(msg));
      else if (msg.GetMessageType() == MessageType::INFO_ACTOR_UPDATED)
         ProcessUpdateActor(static_cast<const ActorUpdateMessage&>(msg));
      else if (msg.GetMessageType() == MessageType::INFO_ACTOR_DELETED)
         ProcessDeleteActor(static_cast<const ActorDeletedMessage&>(msg));
      else if (msg.GetMessageType() == MessageType::INFO_PLAYER_ENTERED_WORLD)
         ProcessPlayerEnteredWorldMessage(msg);
      else if (msg.GetMessageType() == MessageType::TICK_LOCAL ||
            msg.GetMessageType() == MessageType::TICK_REMOTE)
         ProcessTick(static_cast<const TickMessage&>(msg));
      else 
      {
         //remote = GetGameManager()->GetMachineInfo() != msg.GetSource();
         
         if (msg.GetMessageType() == MessageType::COMMAND_PAUSE)
            ProcessPauseCommand(msg);
         else if (msg.GetMessageType() == MessageType::COMMAND_RESUME)
            ProcessResumeCommand(msg);
         else if (msg.GetMessageType() == MessageType::COMMAND_RESTART)
            ProcessRestartCommand(static_cast<const RestartMessage&>(msg));
         else if (msg.GetMessageType() == MessageType::COMMAND_SET_TIME)
            ProcessTimeChangeCommand(static_cast<const TimeChangeMessage&>(msg));
         else 
         {
            LOG_DEBUG("The default message component is processing an unhandled local message");
            ProcessUnhandledLocalMessage(msg);
         }
      }
   }

   dtCore::RefPtr<GameActorProxy> DefaultMessageProcessor::ProcessRemoteCreateActor(const ActorUpdateMessage& msg) throw (dtUtil::Exception)
   {
      const std::string typeName = msg.GetActorTypeName();
      const std::string catName = msg.GetActorTypeCategory();

      dtCore::RefPtr<dtDAL::ActorProxy> ap;
      dtCore::RefPtr<dtDAL::ActorType> type = GetGameManager()->FindActorType(catName, typeName);
      if (type == NULL)
         EXCEPT(dtGame::ExceptionEnum::INVALID_PARAMETER, "The actor type parameters with value \"" 
           + catName + "." +  typeName + "\" are invalid because no such actor type is registered.");
           
      ap = GetGameManager()->CreateActor(*type);
      //Change the id to match the one this is ghosting.
      ap->SetId(msg.GetAboutActorId());
         
      if (!ap->IsGameActorProxy())
      {
         EXCEPT(dtGame::ExceptionEnum::INVALID_PARAMETER, "The actor type parameters with value \"" 
            + catName + "." +  typeName + "\" are invalid because the specify a non-game actor proxy.");
      }
      
      GameActorProxy* gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
      
      if (gap == NULL)
         EXCEPT(dtGame::ExceptionEnum::INVALID_PARAMETER, "The actor type parameters with value \"" 
            + catName + "." +  typeName 
            + "\" are invalid because the specify a non-game actor proxy despite IsGameActorProxy returning true.");
      
      return gap;
   }

   void DefaultMessageProcessor::ProcessRemoteUpdateActor(const ActorUpdateMessage &msg)
   {
      dtGame::GameActorProxy *ap = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      if (ap == NULL)
      {
         LOG_ERROR("The about actor id is invalid");
         return;
      }
       
      ap->ApplyActorUpdate(msg);
   }

   void DefaultMessageProcessor::ProcessRemoteDeleteActor(const ActorDeletedMessage &msg)
   {
      dtGame::GameActorProxy *ap = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      if (ap == NULL)
      {
         LOG_WARNING("Actor deleted msg does not specify a valid actor.");
         return;
      }
      
      GetGameManager()->DeleteActor(*ap);
   }

   void DefaultMessageProcessor::ProcessCreateActor(const ActorUpdateMessage &msg)
   {
      GameActorProxy *proxy = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      if (proxy == NULL)
      {
         //just to make sure the message is actually remote
         if (msg.GetSource() != GetGameManager()->GetMachineInfo())
         {
      
            try
            {
               dtCore::RefPtr<GameActorProxy> gap = ProcessRemoteCreateActor(msg);
               gap->ApplyActorUpdate(msg);
               GetGameManager()->AddActor(*gap, true, false);
            }
            catch (const dtUtil::Exception& ex)
            {
               LOG_ERROR(ex.TypeEnum().GetName() 
                  + " exception encountered trying to create a remote actor.  The actor will be ignored. Message: " + ex.What());
            }
   
            ProcessRemoteUpdateActor(msg);
         }
      }
      else if (!proxy->IsRemote())
      {
         ProcessLocalCreateActor(msg);
      }
      else 
      {
         ProcessRemoteUpdateActor(msg);
      }
   }

   void DefaultMessageProcessor::ProcessUpdateActor(const ActorUpdateMessage &msg)
   {
      GameActorProxy *proxy = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      if (proxy != NULL)
      {
         if (proxy->IsRemote())
            ProcessRemoteUpdateActor(msg);
         else
            ProcessLocalUpdateActor(msg);
      }
      else
         ProcessCreateActor(msg);   
   }

   void DefaultMessageProcessor::ProcessDeleteActor(const ActorDeletedMessage &msg)
   {
      GameActorProxy *proxy = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      if (proxy != NULL)
      {
         if (proxy->IsRemote())
            ProcessRemoteDeleteActor(msg);
         else
            ProcessLocalDeleteActor(msg);
      }
      else
      {
         LOG_DEBUG("Message received for an unknown proxy");
         return;
      }
   }


   void DefaultMessageProcessor::ProcessPauseCommand(const Message& msg)
   {
      GetGameManager()->SetPaused(true);
   }

   void DefaultMessageProcessor::ProcessResumeCommand(const Message& msg)
   {
      GetGameManager()->SetPaused(false);
   }

   void DefaultMessageProcessor::ProcessTimeChangeCommand(const TimeChangeMessage& msg)
   {
      GetGameManager()->ChangeTimeSettings(msg.GetSimulationTime(), msg.GetTimeScale(), (dtCore::Timer_t)(msg.GetSimulationClockTime() * 1000000));
   }

   void DefaultMessageProcessor::ProcessUnhandledLocalMessage(const Message &msg)
   {
      if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::string msgStr;
         msg.ToString(msgStr);
         LOG_DEBUG(msgStr);
      }
   }
      
   void DefaultMessageProcessor::ProcessUnhandledRemoteMessage(const Message &msg)
   {
      if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::string msgStr;
         msg.ToString(msgStr);
         LOG_DEBUG(msgStr);
      }
   }
}
