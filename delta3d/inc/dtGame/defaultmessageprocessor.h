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
#ifndef DELTA_DEFAULTMESSAGEPROCESSOR
#define DELTA_DEFAULTMESSAGEPROCESSOR

#include "dtGame/gmcomponent.h"

namespace dtGame
{
   class GameManager;
   class Message;
   class TickMessage;
   class TimeChangeMessage;
   class ActorUpdateMessage;
   class ActorDeletedMessage;
   class RestartMessage;
   class GameActorProxy;

   class DT_GAME_EXPORT DefaultMessageProcessor : public GMComponent
   {
      public:

         /// Constructor
         DefaultMessageProcessor();

         /**
          * Called publicily from other classes to process a message and 
          * pass it off to its correct handler function based on its type
          * @param msg The message
          */
         virtual void ProcessMessage(const Message &msg);

      protected:

         /// Destructor
         virtual ~DefaultMessageProcessor();

         /**
          * Processes a tick message
          * @param msg The message
          */
         virtual void ProcessTick(const TickMessage &msg) {}

         /**
          * Processes a local create message
          * @param msg The message
          */
         virtual void ProcessLocalCreateActor(const ActorUpdateMessage &msg) {}

         /**
          * Processes an local actor update message
          * @param msg The message
          */
         virtual void ProcessLocalUpdateActor(const ActorUpdateMessage &msg) {}

         /**
          * Processes a local actor delete message
          * @param msg The message
          */
         virtual void ProcessLocalDeleteActor(const ActorDeletedMessage &msg) {}

         /**
          * Processes a remote create actor message
          * @param msg The message
          * @return A pointer to the newly created GameActorProxy.
          * @throws dtUtil::Exception if it was unable to create the proper actor.
          */
         virtual dtCore::RefPtr<GameActorProxy> ProcessRemoteCreateActor(const ActorUpdateMessage &msg) throw (dtUtil::Exception);

         /**
          * Processes a remote update actor message
          * @param msg The message
          */
         virtual void ProcessRemoteUpdateActor(const ActorUpdateMessage &msg);

         /**
          * Processes a remote delete actor message
          * @param msg The message
          */
         virtual void ProcessRemoteDeleteActor(const ActorDeletedMessage &msg);

         /**
          * Processes a create actor message
          * @param msg The message
          */
         void ProcessCreateActor(const ActorUpdateMessage &msg);

         /**
          * Processes an update actor message
          * @param msg The message
          */
         void ProcessUpdateActor(const ActorUpdateMessage &msg);

         /**
          * Processes a delete actor message
          * @param msg The message
          */
         void ProcessDeleteActor(const ActorDeletedMessage &msg);

         /**
          * Called when a pause command message is received.
          * @param msg the pause message.
          */
         virtual void ProcessPauseCommand(const Message& msg);
          
         /**
          * Called when a resume command message is received.
          * @param msg the resume message.
          */
         virtual void ProcessResumeCommand(const Message& msg);

         /**
          * Called when a restart command message is received.
          * This should be overridden to handle restarting the game, 
          * whatever that means to the current game or simulation.
          * @param msg the restart message.
          */
         virtual void ProcessRestartCommand(const RestartMessage& msg) {}

         /**
          * Called when a time change command message is received.
          * @param msg the message to change the time settings.
          */
         virtual void ProcessTimeChangeCommand(const TimeChangeMessage& msg);

         /**
          * Processes an unhandled local message
          * @param msg The message
          */
         virtual void ProcessUnhandledLocalMessage(const Message &msg);

         /**
          * Processes an unhandled remote message
          * @param msg The message
          */
         virtual void ProcessUnhandledRemoteMessage(const Message &msg);

      private:
   };
}
#endif
