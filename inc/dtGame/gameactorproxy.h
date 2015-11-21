/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009
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
 *
 */

#ifndef gameactorproxy_h__
#define gameactorproxy_h__

#include <dtGame/export.h>
#include <dtCore/actorproperty.h>
#include <dtCore/transformableactorproxy.h>
#include <dtGame/actorcomponentbase.h>
#include <dtGame/invokable.h>
#include <dtGame/messagetype.h>
#include <dtUtil/getsetmacros.h>
#include <dtUtil/tree.h>

namespace dtUtil
{
   class Log;
}

namespace dtGame
{
   class GameManager;
   class GameActor;
   class ActorUpdateMessage;
   class ActorComponent;

   class GameActorProxy;

   /////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   /////////////////////////////////////////////////////////////////////////////
   typedef dtUtil::Tree<GameActorProxy*, dtCore::TransformableActorProxy> ActorTree;
   typedef dtCore::RefPtr<dtGame::GameActorProxy> GameActorPtr;
   typedef dtCore::ObserverPtr<dtGame::GameActorProxy> GameActorWeakPtr;



   /**
    * class GameActorProxy
    * This is the base class for all of the actor proxies utilized by the
    * Game Manager. A GameActorProxy/GameActor can receive and send GameManager
    * messages and is also a container for ActorComponents.
    * @see dtGame::GameManager
    */
   class DT_GAME_EXPORT GameActorProxy : public ActorTree, public dtGame::ActorComponentBase
   {
   public:
      typedef ActorTree BaseClass;

      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      /**
       * DEPRECATED  Put all your message handling logic on you subclass of GameActorProxy (name change pending) and call that
       * your actor.  The class that is a descendant of DeltaDrawable or GameActor (deprecated) is your drawable
       * and should do only rendering related things.
       */
      static const std::string PROCESS_MSG_INVOKABLE;
      /**
       * This is not deprecated, but it works differently.  If you don't override it, it will attempt to call the one on GameActor
       * if you have one of those.
       */
      static const std::string TICK_LOCAL_INVOKABLE;
      /**
       * This is not deprecated, but it works differently.  If you don't override it, it will attempt to call the one on GameActor
       * if you have one of those.
       */
      static const std::string TICK_REMOTE_INVOKABLE;

      /// Internal class to represent the ownership of an actor proxy
      class DT_GAME_EXPORT Ownership : public dtUtil::Enumeration
      {
         DECLARE_ENUM(Ownership);
      public:
         static Ownership SERVER_PUBLISHED;
         static Ownership SERVER_LOCAL;
         static Ownership CLIENT_LOCAL;
         static Ownership CLIENT_AND_SERVER_LOCAL;
         static Ownership NOT_MANAGED;
         static Ownership PROTOTYPE;
      protected:
         Ownership(const std::string& name);
      };

      /// Internal class to represent the ownership of an actor proxy
      class DT_GAME_EXPORT LocalActorUpdatePolicy : public dtUtil::Enumeration
      {
         DECLARE_ENUM(LocalActorUpdatePolicy);
      public:
         static LocalActorUpdatePolicy IGNORE_ALL;
         static LocalActorUpdatePolicy ACCEPT_ALL;
         static LocalActorUpdatePolicy ACCEPT_WITH_PROPERTY_FILTER;
      protected:
         LocalActorUpdatePolicy(const std::string& name);
      };

      /// Internal class to iterate over the actor's tree structure.
      class GameActorIterator : public dtCore::ActorComponentContainer::ActorIterator
      {
      public:
         typedef dtCore::ActorComponentContainer::ActorIterator BaseClass;

         GameActorIterator(GameActorProxy& actor);
         GameActorIterator(GameActorIterator& iter);

         virtual ActorIterator& operator++ ();
         virtual BaseActorObject* operator* () const;

         virtual bool IsAtEnd() const;

      protected:
         virtual ~GameActorIterator();

         GameActorProxy::iterator mIter;
         dtCore::ObserverPtr<GameActorProxy> mActor;
      };

      /// Constructor
      GameActorProxy();

      /**
       * Override of the BaseActorObject Clone, which might be temporary.
       *
       * This method performs extra setup required of a GameActor
       * for a successful clone operation. This is typically used
       * by the old prototype creation process.
       */
      dtCore::RefPtr<dtCore::BaseActorObject> Clone() override;

      /**
       * This version of close returns this class to avoid some casting in the implementation.
       * The Clone() version from the base class calls this function.
       */
      virtual dtCore::RefPtr<dtGame::GameActorProxy> CloneGameActor();

      /// Overridden to copy properties from actor components.
      void CopyPropertiesFrom(const PropertyContainer& copyFrom, bool copyMetaData = true) override;

      /// Overridden to call BuildInvokables
      void Init(const dtCore::ActorType& actorType) override;


      virtual void SetParentActor(dtGame::GameActorProxy* parent);

      /**
       * Returns the actor that is the parent to this actor.
       */
      virtual dtGame::GameActorProxy* GetParentActor() const;


      dtCore::RefPtr<dtCore::ActorComponentContainer::ActorIterator> GetIterator() override;

      /**
       * The actor component was probably removed. So, we need to remove each of the properties
       * from the actor component that were added to our actor.
       *
       * Note - this is sort of temporary code because the actor component props are
       * stored on both the game actor proxy AND on the actor component itself. In the future,
       * tools like STAGE should know how to resolve this and this whole method can go away.
       */
      void RemoveActorComponentProperties(ActorComponent& component);

      /**
       * We are probably adding this actor component. So, we need to add each of the properties
       * on the actor component to our proxy.
       *
       * Note - this is sort of temporary code because the actor component props are
       * stored on both the game actor proxy AND on the actor component itself. In the future,
       * tools like STAGE should know how to resolve this and this whole method can go away.
       */
      void AddActorComponentProperties(ActorComponent& component);

      /**
       * This is a shortcut to avoid having to dynamic cast to a GameActorProxy.
       * @return true always
       */
      bool IsGameActor() const override { return true; }

      /**
       * @return a const pointer to the parent game manager that owns this actor.
       */
      GameManager* GetGameManager() const { return mParent; }

      /**
       * Creates the properties associated with this proxy
       */
      void BuildPropertyMap() override;

      /**
       * Overwrite to construct the Invokables for this GameActorProxy. Be sure
       * to call the inherited parent's method to include inherited Invokables.
       * @note The GameActor associated with this GameActorProxy has not been added
       * to the GameManager at this point.
       * @note Don't forget to tie the Invokable to the actual MessageType by calling
       * RegisterForMessages() (typically in OnEnteredWorld())
       * @see AddInvokable()
       */
      virtual void BuildInvokables();

      /**
        * Override this to add your own Actor Components.
        * Note - actor components do the BuildPropertyMap during the AddComponent method, so set
        * your default values after you that.
        */
      virtual void BuildActorComponents();

      /**
       * Adds a new Invokable to the this proxy's list of Invokables.
       * @param newInvokable The new invokable to add.
       * @note
       *      Invokables must have unique names, therefore, if one
       *      is added that who's name collides with another, the invokable
       *      is not added and an error message is logged.
       * @see RemoveInvokable()
       */
      void AddInvokable(Invokable& newInvokable);

      /**
       * Remove this Invokable from the proxy's list
       */
      void RemoveInvokable(const std::string& name);

      /**
       * Remove this Invokable from the proxy's list
       */
      void RemoveInvokable(Invokable*);

      /**
       * @return the Invokable with the given name or NULL if it doesn't have one with that name.
       */
      Invokable* GetInvokable(const std::string& name);

      /**
       * Gets a list of the Invokables currently registered for this
       * Game actor proxy.
       */
      void GetInvokables(std::vector<Invokable*>& toFill);

      /**
       * Gets a const list of the Invokable currently registered for this
       * Game actor proxy.
       */
      void GetInvokables(std::vector<const Invokable*>& toFill) const;

      /**
       * Creates an ActorUpdateMessage, populates it with ALL properties on the actor
       * and calls SendMessage() on the Game Manager.
       * Note - This will do nothing if the actor is Remote.
       */
      virtual void NotifyFullActorUpdate();

      /**
       * Creates an ActorUpdateMessage, populates it with specific properties on the actor
       * and calls SendMessage() on the Game Manager.
       * Note - This will do nothing if the actor is Remote.
       * @param propNames  the properties to include in the update message.
       * @param flagAsPartial Marks the ActorUpdateMessage as partial or not (default is true)
       */
      virtual void NotifyPartialActorUpdate(const std::vector<dtUtil::RefString>& propNames,
         bool flagAsPartial = true);

      /**
       * This is like NotifyFullActorUpdate() except that your subclass might only want to
       * send some properties. If you use this, you MUST override GetPartialUpdateProperties()
       * to set which properties will be sent.
       * Note - This will do nothing if the actor is Remote.
       * @param flagAsPartial Marks the ActorUpdateMessage as partial or not (default is true)
       */
      virtual void NotifyPartialActorUpdate(bool flagAsPartial = true);

      /**
       * Override this and add whatever properties you want to go out when you call
       * NotifyPartialActorUpdate(). Note - you should not use NotifyPartialActorUpdate()
       * without overriding this - the default implementation logs a warning.
       * Note - This will do nothing if the actor is Remote.
       */
      virtual void GetPartialUpdateProperties(std::vector<dtUtil::RefString>& propNamesToFill);


      /**
       * Populates an update message from the actor proxy.  When overwriting this method, be sure to call or
       * duplicate the functionality provided in GameActor::PopulateActorUpdate().
       * @param update The message to populate.
       * @param propNames  the properties to include in the message.
       */
      virtual void PopulateActorUpdate(ActorUpdateMessage& update, const std::vector<dtUtil::RefString>& propNames);

      /**
       * Populates an update message from the actor proxy.  It will add all property values to the message.
       * When overwriting this method, be sure to call or
       * duplicate the functionality provided in GameActor::PopulateActorUpdate().
       * @param update The message to populate.
       */
      virtual void PopulateActorUpdate(ActorUpdateMessage& update);

      /**
       * Takes and actor update message and applies the parameter values to change the
       * the property values of the this actor proxy.  This is virtual so it can be extended
       * or replaced to accommodate special behavior or special subclasses of actor update.
       *
       * @param msg the message to apply.
       * @param checkLocalUpdatePolicy set to true if the policy filtering should be consulted, if false the update will
       *                               just be set with no checking.
       * @see #SetLocalActorUpdatePolicy
       */
      virtual void ApplyActorUpdate(const ActorUpdateMessage& msg, bool checkLocalUpdatePolicy = false);

      /**
       * Get all of the invokables registered for a given message type.
       * @param type the message type to query for.
       * @param toFill a vector to fill with the invokables.
       */
      void GetMessageHandlers(const MessageType& type, std::vector<Invokable*>& toFill);

      /**
       * Returns if this proxy is remote, calls the private actor definition
       * @see dtGame::GameActor
       * @return True if the actor is remote, false if not
       */
      bool IsRemote() const;

      /**
       * Returns if this proxy is published, calls the private actor definition
       * @see dtGame::GameActor
       * @return True if the actor is published, false if not
       */
      bool IsPublished() const;

      /**
       * Returns the ownership of the actor proxy
       * @return The ownership, corresponding with the ownership class
       * @see dtGame::GameActorProxy::Ownership
       */
      Ownership& GetInitialOwnership();

      /**
       * Sets the ownership of the actor proxy
       * @return The ownership, corresponding with the ownership class
       * @see dtGame::GameActorProxy::Ownership
       */
      void SetInitialOwnership(Ownership& newOwnership);

      /**
       * Returns the local actor update policy.  This defines what the actor should do if an INFO_ACTOR_UPDATED
       * message comes in remotely about this actor when it is locally created and simulated.  Essentially that would
       * mean that another system would be trying to change property values on this actor.
       * @return The current policy
       * @see dtGame::GameActorProxy::LocalActorUpdatePolicy
       */
      LocalActorUpdatePolicy& GetLocalActorUpdatePolicy();

      /**
       * Sets a new local actor update policy.  This defines what the actor should do if an INFO_ACTOR_UPDATED
       * message comes in remotely about this actor when it is locally created and simulated.  Essentially that would
       * mean that another system would be trying to change property values on this actor.
       * @see dtGame::GameActorProxy::LocalActorUpdatePolicy
       */
      void SetLocalActorUpdatePolicy(LocalActorUpdatePolicy& newPolicy);

      /**
       * Registers to receive a specific type of message.  You will receive
       * all instances of this message.  It will create an invokable for you.
       * @see dtGame::GameActorProxy::RegisterForMessagesAboutOtherActor
       * @see dtGame::GameActorProxy::RegisterForMessagesAboutSelf
       *
       * A call to this should look like.
       *
       * RegisterForMessages(dtGame::MessageType::INFO_TICK_LOCAL, dtUtil::MakeFunctor(this, &SomeActor::OnTickLocal));
       *
       * It returns the invokable, so you can use that in subsequent calls to Unregister and Register if you want to
       * do that.
       *
       * Your function may take the actual message class, not just dtGame::Message.
       *
       * @return The invokable created so you unregister it.
       */
      template<typename Message_T>
      dtCore::RefPtr<Invokable> RegisterForMessages(const MessageType& type, dtUtil::Functor<void, TYPELIST_1(const Message_T&)> func)
      {
         std::string invokableName = type.GetName() + dtCore::UniqueId().ToString();
         dtCore::RefPtr<Invokable> inv = new Invokable(invokableName, func);
         AddInvokable(*inv);
         RegisterForMessages(type, invokableName);
         return inv;
      }

      /**
       * This is like RegisterForMessages, but you will only receive messages with an aboutActorId that matches the given ID.
       * @see dtGame::GameActorProxy::RegisterForMessages
       * @see dtGame::GameActorProxy::RegisterForMessagesAboutSelf
       * @return The invokable created so you unregister it if need be.
       */
      template<typename Message_T>
      dtCore::RefPtr<Invokable> RegisterForMessagesAboutOtherActor(const MessageType& type,
            const dtCore::UniqueId& targetActorId,
            dtUtil::Functor<void, TYPELIST_1(const Message_T&)> func)
      {
         std::string invokableName = type.GetName() + "-Other-" + dtCore::UniqueId().ToString();
         dtCore::RefPtr<Invokable> inv = new Invokable(invokableName, func);
         AddInvokable(*inv);
         RegisterForMessagesAboutOtherActor(type, targetActorId, invokableName);
         return inv;

      }

      /**
       * This is like RegisterForMessages, but you will only receive messages with an aboutActorId that matches this actor.
       * @see dtGame::GameActorProxy::RegisterForMessages
       * @see dtGame::GameActorProxy::RegisterForMessagesAboutSelf
       * @return The invokable created so you unregister it if need be.
       */
      template<typename Message_T>
      dtCore::RefPtr<Invokable> RegisterForMessagesAboutSelf(const MessageType& type,
            dtUtil::Functor<void, TYPELIST_1(const Message_T&)> func)
      {
         std::string invokableName = type.GetName() + "-Self-" + dtCore::UniqueId().ToString();
         dtCore::RefPtr<Invokable> inv = new Invokable(invokableName, func);
         AddInvokable(*inv);
         RegisterForMessagesAboutSelf(type, invokableName);
         return inv;
      }

      /**
       * Registers to receive a specific type of message from the GM.  You will receive
       * all instances of this message, regardless of whether you are the about actor or not.
       * @see dtGame::GameActorProxy::RegisterForMessagesAboutOtherActor
       * @see dtGame::GameActorProxy::RegisterForMessagesAboutSelf
       * @see dtGame::GameActor::ProcessMessage
       */
      void RegisterForMessages(const MessageType& type,
               const std::string& invokableName);

      /**
       * Registers to receive a specific type of message from the GM.  IT will ONLY receive
       * messages about this other actor.  Use this when you want to track interactions with
       * other actors - such as a player actor listening for when the vehicle being driven receive damage.
       * @see dtGame::GameActorProxy::RegisterForMessages
       * @see dtGame::GameActorProxy::RegisterForMessagesAboutSelf
       * @see dtGame::GameActor::ProcessMessage
       */
      void RegisterForMessagesAboutOtherActor(const MessageType& type,
               const dtCore::UniqueId& targetActorId,
               const std::string& invokableName);

      /**
       * Registers to receive a specific type of message from the GM.  It will ONLY receive
       * messages with an AboutActorId that equals the current actor instance.
       * This is really only applicable if another actor sends a message about this actor or a message
       * comes in from the network about this actor.
       * @see dtGame::GameActorProxy::RegisterForMessages
       * @see dtGame::GameActorProxy::RegisterForMessagesAboutOtherActor
       * @see dtGame::GameActor::ProcessMessage
       */
      void RegisterForMessagesAboutSelf(const MessageType& type,
               const std::string& invokableName);

      /**
       * Unregisters the invokable for a specific type of message from the GM.  This is the
       * reverse of RegisterForMessages for global messages.
       * @see dtGame::GameActorProxy::RegisterForMessages
       */
      void UnregisterForMessages(const MessageType& type,
               const std::string& invokableName);

      /**
       * Unregisters the invokable for a specific type of message for a specific Actor.  This
       * is the reverse of RegisterForMessagesAboutOtherActor.
       * @see dtGame::GameActorProxy::RegisterForMessagesAboutOtherActor
       */
      void UnregisterForMessagesAboutOtherActor(const MessageType& type,
               const dtCore::UniqueId& targetActorId,
               const std::string& invokableName);

      /**
       * Unregisters the invokable for a specific type of message from the GM.  This is the
       * reverse of RegisterForMessagesAboutSelf.
       * @see dtGame::GameActorProxy::RegisterForMessagesAboutSelf
       */
      void UnregisterForMessagesAboutSelf(const MessageType& type,
               const std::string& invokableName);

      /**
       * @return True if this GameActorProxy has been added to the GameManager yet,
       * false otherwise.
       */
      bool IsInGM() const;

      /**
       * Moved to public, since map change state data needs to call this now as well.
       * for actors within the map.
       */
      void SetGameManager(GameManager* gm);

      /**
       * This essetially checks to see if a property name is in the accept filter
       * @see dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_WITH_PROPERTY_FILTER
       */
      bool ShouldAcceptPropertyInLocalUpdate(const dtUtil::RefString& propName) const;

      /**
       * This function walks the child actor components to see if one of them
       * can handle the deprecated property.        *
       * @param[in]  name  The name of the property queried for.
       * @return           A property, or NULL if none found.
       */
      dtCore::RefPtr<dtCore::ActorProperty> GetDeprecatedProperty(const std::string& name) override;

      /**
       * Add an ActorComponent.
       * @param component The ActorComponent to try to add
       */
      void AddComponent(ActorComponent& component) override;

      /**
       * Remove component by reference
       * @param component : Reference to the ActorComponent to remove
       */
      void RemoveComponent(ActorComponent& component) override;

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
       * Sets if the actor is remote by invoking the actor implementation
       * User code should not call this.
       * @param True if the actor should be remote, false if not
       */
      void SetRemote(bool remote);

      /**
       * Sets if the actor is published by invoking the actor implementation
       * User code should not call this.
       * @param True if the actor should be remote, false if not
       */
      void SetPublished(bool published);

      /**
       * Invokes the OnEnteredWorld function of the actor, and then the proxy
       * User code should not call this.
       */
      void InvokeEnteredWorld();

      /**
       * Invokes the OnRemovedFromWorld function of the proxy
       * User code should not call this.
       */
      void InvokeRemovedFromWorld();

      /// This was added so the GameManager can be set on creation.
      void SetIsInGM(bool value);


      /**
       * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
       * This is deprecated because the whole GameActor class being deprecated.
       * In addition, registering for individual messages, then catching them in big if block
       * in one function is just a bad pattern.  Call the one that takes a functor and make a function
       * for each message type you want to receive.
       */
      DEPRECATE_FUNC void RegisterForMessages(const MessageType& type)
      {
         RegisterForMessages(type, PROCESS_MSG_INVOKABLE);
      }

      /**
       * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
       * This is deprecated because the whole GameActor class being deprecated.
       * In addition, registering for individual messages, then catching them in big if block
       * in one function is just a bad and inefficient pattern.  Call the one that takes a functor and make a function
       * for each message type you want to receive.
       */
      DEPRECATE_FUNC void RegisterForMessagesAboutOtherActor(const MessageType& type,
               const dtCore::UniqueId& targetActorId)
      {
         RegisterForMessagesAboutOtherActor(type, targetActorId, PROCESS_MSG_INVOKABLE);
      }

      /**
       * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
       * This is deprecated because the whole GameActor class being deprecated.
       * In addition, registering for individual messages, then catching them in big if block
       * in one function is just a bad and inefficent pattern.  Call the one that takes a functor and make a function
       * for each message type you want to receive.
       */
      DEPRECATE_FUNC void RegisterForMessagesAboutSelf(const MessageType& type)
      {
         RegisterForMessagesAboutSelf(type, PROCESS_MSG_INVOKABLE);
      }

      /**
       * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
       * This is deprecated because the whole GameActor class being deprecated.
       * In addition, registering for individual messages, then catching them in big if block
       * in one function is just a bad and inefficient pattern.  Call the one that takes a functor and make a function
       * for each message type you want to receive.
       */
      DEPRECATE_FUNC void UnregisterForMessages(const MessageType& type)
      {
         UnregisterForMessages(type, PROCESS_MSG_INVOKABLE);
      }

      /**
       * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
       * This is deprecated because the whole GameActor class being deprecated.
       * In addition, registering for individual messages, then catching them in big if block
       * in one function is just a bad pattern.  Call the one that takes a functor and make a function
       * for each message type you want to receive.
       */
      DEPRECATE_FUNC void UnregisterForMessagesAboutOtherActor(const MessageType& type,
               const dtCore::UniqueId& targetActorId)
      {
         UnregisterForMessagesAboutOtherActor(type, targetActorId, PROCESS_MSG_INVOKABLE);
      }

      /**
       * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
       * This is deprecated because the whole GameActor class being deprecated.
       * In addition, registering for individual messages, then catching them in big if block
       * in one function is just a bad pattern.  Call the one that takes a functor and make a function
       * for each message type you want to receive.
       */
      DEPRECATE_FUNC void UnregisterForMessagesAboutSelf(const MessageType& type)
      {
         UnregisterForMessagesAboutSelf(type, PROCESS_MSG_INVOKABLE);
      }

      /**
       * DEPRECATED  Call GetDrawable<DrawableType>()
       * Retrieves the game actor that this represents, which may not be a GameActor.
       * @return The Game Actor
       */
      DEPRECATE_FUNC GameActor& GetGameActor();

      /**
       * DEPRECATED  Call GetDrawable<DrawableType>()
       * Retrieves the game actor that this represents, which may not be a GameActor.
       * @return The Game Actor
       */
      DEPRECATE_FUNC const GameActor& GetGameActor() const;

      /**
       * If this actor is queued to be deleted.
       */
      bool IsDeleted() const;
      void SetDeleted(bool deleted);

      // TEMP:
      void AddActorComponentProperties();

      // TEMP:
      void RemoveActorComponentProperties();

      /**
       * Removes child actors that are attached directly to this actor.
       * @bool deleteFromGM calls DeleteActor() on the GM for each child removed.
       */
      unsigned DetachChildActors(bool deleteFromGM = false);

      /**
       * This is a temporary override from dtCore so it can set the parent/child relationships
       */
      bool SetParentBaseActor(dtCore::BaseActorObject* parent) override;

      /**
       * Returns the actor that is the parent to this actor as a base actor object.
       * Call GetParentActor instead.  This exists just for the map loading code.
       */
      dtCore::BaseActorObject* GetParentBaseActor() const override;
   protected:
      /// Destructor
      virtual ~GameActorProxy();

      /**
       * Called when an actor is first placed in the "world". This is a good place
       * to register Invokables with MessageTypes.
       * @see RegisterForMessages()
       */
      virtual void OnEnteredWorld();

      /**
       * Called when the GM deletes the actor in a NORMAL way, such as DeleteActor().
       * This may not get called from every possible path that an actor can be removed
       * or deleted. For instance, calling DeleteAllActors(true) on the GM would bypass
       * the 'normal' actor delete process. Note, this is sent at the end of the tick and
       * at the end of a 'normal' map change. If you want to find out immediately that you
       * are being deleted, register for the INFO_ACTOR_DELETED with RegisterForMessagesAboutSelf().
       */
      virtual void OnRemovedFromWorld();

      /**
       * Adds a property to the accept list for local updates.
       * If you want to accept a the actor name from a message, you must add PROPERTY_NAME to this list.
       * @see dtGame::GameActorProxy::PROPERTY_NAME
       * @see dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_WITH_PROPERTY_FILTER
       */
      void AddPropertyToLocalUpdateAcceptFilter(const dtUtil::RefString& propName);

      /**
       * Removes a property to the accept list for local updates.
       * @see dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_WITH_PROPERTY_FILTER
       */
      void RemovePropertyFromLocalUpdateAcceptFilter(const dtUtil::RefString& propName);

      /**
       * Temporary convenience method for attaching this actor's drawable to the parent actor's drawable.
       * @param parent Actor with a drawable to attach this actor's drawable to.
       * @param index Index at which to insert the drawable as a child relative to the order of other child drawables.
       * @return TRUE if the drawable was successfully attached.
       */
      bool AttachDrawableToParent(dtGame::GameActorProxy& parent, int index = -1);

      /**
       * Temporary convenience method for detaching this actor's drawable from the parent actor's drawable.
       * @param parent Actor with a drawable to detach this actor's drawable from.
       * @return TRUE if the drawable was successfully attached; FALSE if not found or opration failed.
       */
      bool DetachDrawableFromParent(dtGame::GameActorProxy& parent);

   private:

      /**
       * Override of the Tree base class.
       */
      GameActorProxy::ref_pointer clone() const override;

      /**
       * Populates an update message from the actor proxy.
       * @param update The ActorUpdateMessage to populate with Property values
       * @param propNames Optional list of names of Properties to include in the ActorUpdateMessage.  Default
       *                  will use all existing Properties.
       */
      void PopulateActorUpdateImpl(ActorUpdateMessage& update,
                                   const std::vector<dtUtil::RefString>& propNames = std::vector<dtUtil::RefString>());


      std::string mPrototypeName;
      dtCore::UniqueId mPrototypeID;
      GameManager* mParent;
      Ownership* mOwnership;
      LocalActorUpdatePolicy* mLocalActorUpdatePolicy;
      dtUtil::Log& mLogger;
      std::map<std::string, dtCore::RefPtr<Invokable> > mInvokables;
      std::multimap<const MessageType*, dtCore::RefPtr<Invokable> > mMessageHandlers;
      std::set<dtUtil::RefString> mLocalUpdatePropertyAcceptList;
      bool mIsInGM;
      bool mPublished;
      bool mRemote;
      bool mDrawableIsAGameActor;
      bool mDeleted;

   };
}

#endif // gameactorproxy_h__
