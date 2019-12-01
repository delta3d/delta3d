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
#ifndef DELTA_DEFAULT_NETWORK_PUBLISHING_COMPONENT
#define DELTA_DEFAULT_NETWORK_PUBLISHING_COMPONENT

#include <dtGame/gmcomponent.h>

namespace dtUtil
{
   class Log;
}

namespace dtGame
{
   class GameManager;
   class TickMessage;
   class ActorUpdateMessage;

   class DT_GAME_EXPORT DefaultNetworkPublishingComponent : public GMComponent
   {
   public:

      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;
      static const std::string DEFAULT_NAME;

      /// Constructor
      DefaultNetworkPublishingComponent(dtCore::SystemComponentType& type = *TYPE);

      /**
       * Processes messages sent from the Game Manager
       * @param The message to process
       * @see dtGame::GameManager
       */
      virtual void ProcessMessage(const Message& msg);

      /**
       * Sends a message
       * @param The message to send
       */
      virtual void DispatchNetworkMessage(const Message& msg);

      /// @return true if the given type has been added a type to publish, aside from the default behavior.
      bool PublishesAdditionalMessageType(const MessageType& type) const;
      /// Adds a message type to publish over the network.
      void AddMessageTypeToPublish(const MessageType& type);
      /// Removes a message type to publish over the network.
      void RemoveMessageTypeToPublish(const MessageType& type);

   protected:

      /// Destructor
      virtual ~DefaultNetworkPublishingComponent();

      /**
       * Processes an incoming actor published message
       * @param The message
       */
      virtual void ProcessPublishActor(const Message& msg, GameActorProxy& gap);

      /**
       * Processes an incoming actor deleted message
       * @param The message
       */
      virtual void ProcessDeleteActor(const Message& msg);

      /**
       * Processes an incoming actor update message
       * @param The message
       */
      virtual void ProcessUpdateActor(const ActorUpdateMessage& msg);

      /**
       * Processes an unhandled local message
       * @param The message
       */
      virtual void ProcessUnhandledLocalMessage(const Message& msg);

      /**
       * Processes an unhandled remote message
       * @param The message
       */
      virtual void ProcessUnhandledRemoteMessage(const Message& msg);

      dtUtil::Log* mLogger;
   private:
      std::set<const MessageType*> mMessageTypesToPublish;
   };
}

#endif
