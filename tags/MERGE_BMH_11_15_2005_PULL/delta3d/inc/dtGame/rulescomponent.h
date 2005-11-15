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
#ifndef DELTA_RULESCOMPONENT
#define DELTA_RULESCOMPONENT

#include "dtGame/gmcomponent.h"
#include <dtCore/scene.h>
#include <dtUtil/log.h>

namespace dtGame
{
   class GameManager;
   class TickMessage;
   class ActorPublishedMessage;
   class ActorDeletedMessage;
   class ActorUpdateMessage;

   class DT_GAME_EXPORT RulesComponent : public GMComponent
   {
      public:

         /// Constructor
         RulesComponent();

         /**
          * Processes messages sent from the Game Manager
          * @param The message to process
          * @see dtGame::GameManager
          */
         virtual void ProcessMessage(const Message &msg);

         /**
          * Sends a message
          * @param The message to send
          */
         virtual void SendMessage(const Message &msg);

      protected:

         /// Destructor
         virtual ~RulesComponent();

         /**
          * Processes an incoming tick message
          * @param The message
          */
         virtual void ProcessTick(const TickMessage &msg);

         /**
          * Processes an incoming actor published message
          * @param The message
          */
         virtual void ProcessPublishActor(const ActorPublishedMessage &msg);

         /**
          * Processes an incoming actor deleted message
          * @param The message
          */
         virtual void ProcessDeleteActor(const ActorDeletedMessage &msg);

         /**
          * Processes an incoming actor update message
          * @param The message
          */
         virtual void ProcessUpdateActor(const ActorUpdateMessage &msg);

         /**
          * Processes an unhandled local message
          * @param The message
          */
         virtual void ProcessUnhandledLocalMessage(const Message &msg);

         /**
          * Processes an unhandled remote message
          * @param The message
          */
         virtual void ProcessUnhandleRemoteMessage(const Message &msg);

         dtUtil::Log* logger;
      private:
   };
}

#endif
