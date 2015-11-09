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
#include <dtCore/transformable.h>
#include <dtGame/actorcomponentcontainer.h>

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
    * This drawable subclass is deprecated.  All functionality exists on the GameActorProxy, which can have
    * any drawable type now and it also holds the components.
    * Game Manager
    * @see dtGame::GameManager
    */
   class DT_GAME_EXPORT GameActor
      : public dtCore::Transformable
      , public dtGame::ActorComponentContainer
   {
   public:
      typedef dtCore::Transformable BaseClass;
      /// Constructor
      GameActor(GameActorProxy& parent, const std::string& name = "GameActor");

      /**
       * Overloaded constructor will use the supplied node instead of
       * creating one internally.
       * @param proxy : the GameActorProxy representing this GameActor
       * @param node : A node this class should use internally
       * @param name : The name of this instance
       */
      GameActor(GameActorProxy& parent, TransformableNode& node, const std::string& name = "GameActor");


      void SetName(const std::string& name) override;

      /**
       * @return the GameActorProxy for this game actor.
       */
      GameActorProxy& GetGameActorProxy();

      /**
       * @return the GameActorProxy for this game actor.
       */
      const GameActorProxy& GetGameActorProxy() const;

      /// this exists only to get around an issue that happens during delete of the game actor.  It should always be true otherwise.
      bool IsGameActorProxyValid() const;

      /**
       * Override the one on GameActorProxy
       */
      DEPRECATE_FUNC virtual void BuildActorComponents();

      /**
       * Returns if the actor is remote
       * @return True is the actor is remote, false if not
       */
      bool IsRemote() const;

      /**
       * Returns is the actor is published
       * @return True is the actor is published, false if not
       */
      bool IsPublished() const;

      /**
       * Method for handling local ticks.  This will called by the "Tick Local" invokable.
       * This is designed to be registered to receive TICK_LOCAL messages, but that registration is not done
       * be default
       * @see GameManager#RegisterForMessages
       * @param tickMessage the actual message
       */
      virtual void OnTickLocal(const TickMessage& tickMessage);

      /**
       * Method for handling remote ticks.  This will called by the "Tick Remote" invokable
       * This is designed to be registered to receive TICK_REMOTE messages, but that registration is not done
       * be default
       * @see GameManager#RegisterForMessages
       * @param tickMessage the actual message
       */
      virtual void OnTickRemote(const TickMessage& tickMessage);

      /**
       * This is going away.  It is still the default for the invokable, but
       * it will be removed.  Make your invokables call another function.
       */
      virtual void ProcessMessage(const Message& message);

      /**
       * This value is set automatically by the GM when an actor is created from prototype.
       * @return The prototype that was used to create this actor. Set by the GM.
       */
      DEPRECATE_FUNC std::string GetPrototypeName() const;

      /**
       * This value is used for updating/creating remote actors that need to be recreated from prototype
       * This value is set automatically by the GM when an actor is created from prototype.
       * If the prototype is non-null, then when the actor is created by the message
       * processor, it will attempt to look up the prototype first. Extremely useful for networking.
       * @return The id of the prototype that was used to create this actor. Set by the GM.
       */
      DEPRECATE_FUNC dtCore::UniqueId GetPrototypeID() const;

      //////////////////////////////
      // Actor Components have been moved to BaseActorObject or
      // To a dtGame subclass of it.  Some of the accessor methods for them are
      // here to make that transition easier, that is, to make code written to use them
      // from GameActorProxy still build and allow people to transition away from
      // accessing them from the GameActor.  Calling these methods should be fine since
      // they won't go away, but they may be moved around.

      /**
       * Get all components matching this type string
       * @param type The type-string of the ActorComponent to get
       * @return the selected ActorComponents (will be empty if not found)
       */
      DEPRECATE_FUNC virtual std::vector<ActorComponent*> GetComponents(ActorComponent::ACType type) const override;
      DEPRECATE_FUNC virtual void GetComponents(ActorComponent::ACType type, ActorComponentVector& toFill) const override;

      /**
       * Fill the vector with all the actor components.
       */
      DEPRECATE_FUNC virtual void GetAllComponents(ActorComponentVector& toFill) override;
      DEPRECATE_FUNC virtual void GetAllComponents(ActorComponentVectorConst& toFill) const override;

      /**
       * Does base contain a component of given type?
       * @param type The type-string of the ActorComponent to query
       * @return true if ActorComponent is found, false otherwise
       */
      DEPRECATE_FUNC virtual bool HasComponent(ActorComponent::ACType type) const override;

      /**
       * Add an ActorComponent. Only one ActorComponent of a given type can be added.
       * @param component The ActorComponent to try to add
       */
      DEPRECATE_FUNC virtual void AddComponent(ActorComponent& component) override;

      /**
       * Remove component by reference
       * @param component : Pointer to the ActorComponent to remove
       */
      DEPRECATE_FUNC virtual void RemoveComponent(ActorComponent& component) override;

      /**
       * Removes all components with a particular type
       * @param type The type-string of the ActorComponent to remove
       */
      DEPRECATE_FUNC void RemoveAllComponentsOfType(ActorComponent::ACType type) override;

      /**
       * Remove all contained ActorComponent
       */
      DEPRECATE_FUNC virtual void RemoveAllComponents() override;

      /**
       * Loop through all ActorComponents call their OnEnteredWorld()
       */
      DEPRECATE_FUNC virtual void CallOnEnteredWorldForActorComponents() override;

      /**
       * Loop through all ActorComponents call their OnRemovedWorld()
       */
      DEPRECATE_FUNC virtual void CallOnRemovedFromWorldForActorComponents() override;

      /**
       * Call the BuildPropertyMap() method of all registered ActorComponent
       */
      DEPRECATE_FUNC virtual void BuildComponentPropertyMaps() override;
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
      dtCore::ObserverPtr<GameActorProxy> mOwner;
      dtUtil::Log& mLogger;
   };

} // namespace dtGame

#endif // DELTA_GAMEACTOR
