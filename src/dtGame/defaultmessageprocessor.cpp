/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005-2009, Alion Science and Technology.
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
 * William E. Johnson II, Curtiss Murphy
 */
#include <prefix/dtgameprefix.h>
#include <dtGame/defaultmessageprocessor.h>

#include <dtCore/actortype.h>
#include <dtCore/timer.h>

#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/gameactor.h>
#include <dtGame/gamemanager.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>

#include <dtUtil/log.h>

#include <dtCore/propertymacros.h>

namespace dtGame
{

   ///////////////////////////////////////////////////////////////////////////////
   DefaultMessageProcessor::DefaultMessageProcessor(dtCore::SystemComponentType& type)
   : GMComponent(type)
   , mAcceptMapLoadRequests(true)
   , mAcceptMapChangeRequests(true)
   , mAcceptTimeRequests(true)
   {
      // So subclasses will use the same name.
      SetName(DEFAULT_NAME);
   }

   ///////////////////////////////////////////////////////////////////////////////
   DefaultMessageProcessor::~DefaultMessageProcessor()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessMessage(const Message& msg)
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

      const dtGame::MessageType& msgType = msg.GetMessageType();

      bool processed = false;

      if (msgType == MessageType::TICK_LOCAL ||
            msgType == MessageType::TICK_REMOTE)
      {
         ProcessTick(static_cast<const TickMessage&>(msg));
         processed = true;
      }
      else if (msgType.GetCategory() == MessageType::CATEGORY_INFO)
      {
         processed = true;
         if (msgType == MessageType::INFO_ACTOR_UPDATED)
         {
            ProcessUpdateActor(static_cast<const ActorUpdateMessage&>(msg));
         }
         else if (msgType == MessageType::INFO_ACTOR_CREATED)
         {
            ProcessCreateActor(static_cast<const ActorUpdateMessage&>(msg));
         }
         else if (msgType == MessageType::INFO_ACTOR_DELETED)
         {
            ProcessDeleteActor(msg);
         }
         else if (msgType == MessageType::INFO_PLAYER_ENTERED_WORLD)
         {
            ProcessPlayerEnteredWorldMessage(msg);
         }
         else
         {
            processed = false;
         }
      }
      else if (msgType.GetCategory() == MessageType::CATEGORY_COMMAND)
      {
         //remote = GetGameManager()->GetMachineInfo() != msg.GetSource();
         processed = true;
         if (msgType == MessageType::COMMAND_PAUSE)
         {
            ProcessPauseCommand(msg);
         }
         else if (msgType == MessageType::COMMAND_RESUME)
         {
            ProcessResumeCommand(msg);
         }
         else if (msgType == MessageType::COMMAND_RESTART)
         {
            ProcessRestartCommand(static_cast<const RestartMessage&>(msg));
         }
         else if (msgType == MessageType::COMMAND_SET_TIME)
         {
            ProcessTimeChangeCommand(static_cast<const TimeChangeMessage&>(msg));
         }
         else
         {
            processed = false;
         }
      }
      else if (msgType.GetCategory() == MessageType::CATEGORY_REQUEST)
      {
         if (mAcceptTimeRequests)
         {
            if (msgType == MessageType::REQUEST_PAUSE)
            {
               ProcessPauseCommand(msg);
               processed = true;
            }
            else if (msgType == MessageType::REQUEST_RESUME)
            {
               ProcessResumeCommand(msg);
               processed = true;
            }
            else if (msgType == MessageType::REQUEST_RESTART)
            {
               ProcessRestartCommand(static_cast<const RestartMessage&>(msg));
               processed = true;
            }
            else if (msgType == MessageType::REQUEST_SET_TIME)
            {
               ProcessTimeChangeCommand(static_cast<const TimeChangeMessage&>(msg));
               processed = true;
            }
         }
         if (!processed && mAcceptMapChangeRequests && msgType == MessageType::REQUEST_CHANGE_MAP)
         {
            ProcessMapChange(static_cast<const MapMessage&>(msg));
            processed = true;
         }
         if (!processed && mAcceptMapLoadRequests)
         {
            if (msgType == MessageType::REQUEST_LOAD_MAP)
            {
               ProcessMapLoad(static_cast<const MapMessage&>(msg));
               processed = true;
            }
            else if (msgType == MessageType::REQUEST_UNLOAD_MAP)
            {
               ProcessMapUnload(static_cast<const MapMessage&>(msg));
               processed = true;
            }
         }
      }

      if (!processed)
      {
         LOG_DEBUG("DefaultMessageProcessor is processing an unhandled local message of type: " + msgType.GetName());
         ProcessUnhandledLocalMessage(msg);
      }
   }

   DT_IMPLEMENT_ACCESSOR(DefaultMessageProcessor, bool, AcceptMapLoadRequests);
   DT_IMPLEMENT_ACCESSOR(DefaultMessageProcessor, bool, AcceptMapChangeRequests);
   DT_IMPLEMENT_ACCESSOR(DefaultMessageProcessor, bool, AcceptTimeRequests);


   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::BuildPropertyMap()
   {
      const dtUtil::RefString GROUP(TYPE->GetName());
      typedef dtCore::PropertyRegHelper<DefaultMessageProcessor> RegHelperType;
      RegHelperType propReg(*this, this, GROUP);

      DT_REGISTER_PROPERTY(AcceptMapLoadRequests, "Accept request messages to load or unload additional maps into or out of the GM.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(AcceptMapChangeRequests, "Accept request messages to Change the map set in the GM.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(AcceptTimeRequests, "Accept request messages to Change the map set in the GM.", RegHelperType, propReg);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessLocalUpdateActor(const ActorUpdateMessage& msg, GameActorProxy* proxy)
   {
      // just to make sure the message is actually remote
      if (msg.GetSource() != GetGameManager()->GetMachineInfo() && proxy != NULL && !proxy->IsRemote())
      {
         proxy->ApplyActorUpdate(msg, true);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<GameActorProxy> DefaultMessageProcessor::ProcessRemoteCreateActor(const ActorUpdateMessage& msg)
   {
      // partial updates don't contain enough props to create the actor, so ignore it.
      if (msg.IsPartialUpdate())
      {
         return NULL;
      }

      dtGame::GameManager* gm = GetGameManager();

      const dtCore::UniqueId* prototypeID = &msg.GetPrototypeID();
      bool prototypeEmpty = prototypeID->ToString().empty();

      dtCore::RefPtr<dtGame::GameActorProxy> gap;
      dtCore::RefPtr<const dtCore::ActorType> type = msg.GetActorType();

      if (prototypeEmpty && !msg.GetPrototypeName().empty())
      {
         dtCore::BaseActorObject* actor = NULL;
         gm->FindPrototypeByName(msg.GetPrototypeName(), actor);
         if (actor != NULL)
         {
            prototypeID = &actor->GetId();
            prototypeEmpty = prototypeID->ToString().empty();
         }
      }

      // If the message has a prototype, then use it to create the actor
      if (!prototypeEmpty)
      {
         dtCore::RefPtr<dtGame::GameActorProxy> prototypeProxy = NULL;
         gm->FindPrototypeByID(*prototypeID, prototypeProxy);
         if (prototypeProxy == NULL)
         {
            throw dtGame::InvalidParameterException( "The prototype with value name \""
               + msg.GetPrototypeName() + "\" and UniqueId " + prototypeID->ToString() + " is invalid. No such prototype exists. Remote actor will not be created.", __FILE__, __LINE__);
         }

         gap = dynamic_cast<dtGame::GameActorProxy*>
            (gm->CreateActorFromPrototype(prototypeProxy->GetId(), true).get());
      }

      // Regular create (ie no prototype)
      else
      {
         if (!type.valid())
         {
            const std::string& typeName = msg.GetActorTypeName();
            const std::string& catName = msg.GetActorTypeCategory();

            std::string fullType = typeName + "." + catName;
            if (fullType.length() == 1)
            {
               fullType = "EMPTY";
            }

            throw dtGame::InvalidParameterException( "The actor type parameters with value \""
              + fullType + "\" are invalid because no such actor type is registered.", __FILE__, __LINE__);
         }
         gap = gm->CreateRemoteActor(*type);
      }

      // Change the id to match the one this is ghosting.
      gap->SetId(msg.GetAboutActorId());

      return gap;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessRemoteUpdateActor(const ActorUpdateMessage& msg, GameActorProxy* ap)
   {
      //dtGame::GameActorProxy* ap = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      if (ap == NULL)
      {
         LOG_ERROR("The about actor is invalid");
         return;
      }

      ap->ApplyActorUpdate(msg);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessRemoteDeleteActor(const Message& msg)
   {
      GetGameManager()->DeleteActor(msg.GetAboutActorId());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessCreateActor(const ActorUpdateMessage& msg)
   {
      GameActorProxy* proxy = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      if (proxy == NULL)
      {
         // just to make sure the message is actually remote
         if (msg.GetSource() != GetGameManager()->GetMachineInfo())
         {
            try
            {
               dtCore::RefPtr<GameActorProxy> gap = ProcessRemoteCreateActor(msg);
               if (gap.valid())
               {
                  ProcessRemoteUpdateActor(msg, gap.get());
                  GetGameManager()->AddActor(*gap, true, false);
               }
            }
            catch (const dtUtil::Exception& ex)
            {
               LOG_ERROR("Exception encountered trying to create a remote actor named \"" + msg.GetName()
                        + "\".  The actor will be ignored. Message: \"" + ex.What()
                        + "\" | Actor ID: " + dtUtil::ToString(msg.GetAboutActorId()));
            }
         }
      }
      else if (!proxy->IsRemote())
      {
         ProcessLocalCreateActor(msg);
      }
      else
      {
         ProcessRemoteUpdateActor(msg, proxy);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessUpdateActor(const ActorUpdateMessage& msg)
   {
      GameActorProxy* proxy = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      if (proxy != NULL)
      {
         if (proxy->IsRemote())
         {
            ProcessRemoteUpdateActor(msg, proxy);
         }
         else
         {
            ProcessLocalUpdateActor(msg, proxy);
         }
      }
      else
      {
         ProcessCreateActor(msg);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessDeleteActor(const Message& msg)
   {
      GameActorProxy* proxy = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
      if (proxy != NULL)
      {
         if (proxy->IsRemote())
         {
            ProcessRemoteDeleteActor(msg);
         }
         else
         {
            ProcessLocalDeleteActor(msg);
         }
      }
      else
      {
         LOGN_INFO("DefaultMessageProcessor", "Delete Message received for an unknown actor with ID " + msg.GetAboutActorId().ToString());
         return;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessPauseCommand(const Message& msg)
   {
      GetGameManager()->SetPaused(true);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessResumeCommand(const Message& msg)
   {
      GetGameManager()->SetPaused(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessTimeChangeCommand(const TimeChangeMessage& msg)
   {
      GetGameManager()->ChangeTimeSettings(msg.GetSimulationTime(), msg.GetTimeScale(), (dtCore::Timer_t)(msg.GetSimulationClockTime() * 1000000));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessUnhandledLocalMessage(const Message& msg)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessUnhandledRemoteMessage(const Message& msg)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessMapChange(const MapMessage& msg)
   {
      dtGame::GameManager::NameVector names;
      msg.GetMapNames(names);
      GetGameManager()->ChangeMapSet(names);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessMapLoad(const MapMessage& msg)
   {
      dtGame::GameManager::NameVector names;
      msg.GetMapNames(names);
      GetGameManager()->OpenAdditionalMapSet(names);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DefaultMessageProcessor::ProcessMapUnload(const MapMessage& msg)
   {
      dtGame::GameManager::NameVector names;
      msg.GetMapNames(names);
      GetGameManager()->CloseAdditionalMapSet(names);
   }


} // namespace dtGame
