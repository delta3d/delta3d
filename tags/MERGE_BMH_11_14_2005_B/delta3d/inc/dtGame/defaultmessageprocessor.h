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
   class ActorUpdateMessage;
   class ActorDeletedMessage;
   class GameActorProxy;

   class DT_GAMEMANAGER_EXPORT DefaultMessageProcessor : public GMComponent
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
         virtual void ProcessTick(const TickMessage &msg);

         /**
          * Processes a local create message
          * @param msg The message
          */
         virtual void ProcessLocalCreateActor(const ActorUpdateMessage &msg);

         /**
          * Processes an local actor update message
          * @param msg The message
          */
         virtual void ProcessLocalUpdateActor(const ActorUpdateMessage &msg);

         /**
          * Processes a local actor delete message
          * @param msg The message
          */
         virtual void ProcessLocalDeleteActor(const ActorDeletedMessage &msg);

         /**
          * Processes a remote create actor message
          * @param msg The message
          * @return A pointer to the newly created GameActorProxy, or NULL if error
          */
         virtual GameActorProxy* ProcessRemoteCreateActor(const ActorUpdateMessage &msg);

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
