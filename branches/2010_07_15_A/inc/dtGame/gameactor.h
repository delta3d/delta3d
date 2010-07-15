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
 * William E. Johnson II, David Guthrie, Curtiss Murphy
 */

#ifndef DELTA_GAMEACTOR
#define DELTA_GAMEACTOR

#include <dtGame/export.h>
#include <dtGame/gameactorproxy.h> //needed for private member
#include <dtCore/observerptr.h>
#include <dtCore/physical.h>
#include <dtGame/actorcomponentbase.h>

namespace dtUtil
{
   class Log;
}

namespace dtGame
{
   class Message;
   class TickMessage;

   /**
    * @class GameActor
    * This class will be the main base class for actors being used by the
    * Game Manager
    * @see dtGame::GameManager
    */
   class DT_GAME_EXPORT GameActor
      : public dtCore::Physical
      , public dtGame::ActorComponentBase
   {
   public:
      /// Constructor
      GameActor(GameActorProxy& proxy);

      /**
       * Overloaded constructor will use the supplied node instead of
       * creating one internally.
       * @param proxy : the GameActorProxy representing this GameActor
       * @param node : A node this class should use internally
       * @param name : The name of this instance
       */
      GameActor(GameActorProxy& proxy, TransformableNode &node, const std::string &name = "GameActor");

      /**
       * @return the GameActorProxy for this game actor.
       */
      GameActorProxy& GetGameActorProxy();

      /**
       * @return the GameActorProxy for this game actor.
       */
      const GameActorProxy& GetGameActorProxy() const;

      /** 
        * Override this to add your own Actor Components. 
        * Note - actor components do the BuildPropertyMap during the AddComponent method, so set
        * your default values after you that. 
        */
      virtual void BuildActorComponents();

      /**
       * Returns if the actor is remote
       * @return True is the actor is remote, false if not
       */
      inline bool IsRemote() const { return mRemote; }

      /**
       * Returns is the actor is published
       * @return True is the actor is published, false if not
       */
      inline bool IsPublished() const { return mPublished; }

      /**
       * Method for handling local ticks.  This will called by the "Tick Local" invokable.
       * This is designed to be registered to receive TICK_LOCAL messages, but that registration is not done
       * be default
       * @see GameManager#RegisterForMessages
       * @param tickMessage the actual message
       */
      virtual void OnTickLocal(const TickMessage& tickMessage);

      ///Deprecated, override the one that takes a TickMessage;
      virtual void TickLocal(const Message& tickMessage);

      /**
       * Method for handling remote ticks.  This will called by the "Tick Remote" invokable
       * This is designed to be registered to receive TICK_REMOTE messages, but that registration is not done
       * be default
       * @see GameManager#RegisterForMessages
       * @param tickMessage the actual message
       */
      virtual void OnTickRemote(const TickMessage& tickMessage);

      ///Deprecated, override the one that takes a TickMessage;
      virtual void TickRemote(const Message& tickMessage);

      /**
       * Default invokable for handling messages. This is only called if you register
       * a message using the default PROCESS_MSG_INVOKABLE name. To use this, override it
       * and handle any messages that you want. Then, in the OnEnteredWorld() method on your
       * proxy, add a line, something either of these:
       *    RegisterForMessages(dtGame::MessageType::INFO_GAME_EVENT);
       *    RegisterForMessages(dtGame::MessageType::INFO_GAME_EVENT, PROCESS_MSG_INVOKABLE);
       * @param message the actual message
       */
      virtual void ProcessMessage(const Message& message);

      /**
       * Sets the shader group on the game actor.  This implementation uses
       * the default shader in the group to shade the game actor.
       * @param groupName The name of the shader group.
       */
      virtual void SetShaderGroup(const std::string& groupName);

      /**
       * Gets the current shader group assigned to this game actor.
       * @return The name of the group.
       */
      std::string GetShaderGroup() const { return mShaderGroup; }

      virtual void OnShaderGroupChanged();

      /**
       * See GetPrototypeName().
       * @param prototypeName The prototype that was used to create this actor. Set by the GM.
       */
      void SetPrototypeName(const std::string& prototypeName);

      /**
       * This value is used for updating/creating remote actors that need to be recreated from prototype
       * This value is set automatically by the GM when an actor is created from prototype.
       * If the prototype is non-null, then when the actor is created by the message
       * processor, it will attempt to look up the prototype first. Extremely useful for networking.
       * @return The prototype that was used to create this actor. Set by the GM.
       */
      const std::string& GetPrototypeName() const;

   protected:
      /// Destructor
      virtual ~GameActor();

      /**
       * Called when an actor is first placed in the "world"
       */
      virtual void OnEnteredWorld() { }

   private:
      static const std::string NULL_PROXY_ERROR;
      /**
       * Sets is an actor is remote
       * @param remote Should be true is the actor is remote, false if not
       */
      void SetRemote(bool remote);

      /**
       * Sets is an actor is published
       * @param remote Should be true is the actor is published, false if not
       */
      void SetPublished(bool published);

      friend class GameActorProxy;
      dtCore::ObserverPtr<GameActorProxy> mProxy;
      bool mPublished;
      bool mRemote;
      std::string mShaderGroup;
      dtUtil::Log& mLogger;
      std::string mPrototypeName;
   };

} // namespace dtGame

#endif // DELTA_GAMEACTOR
