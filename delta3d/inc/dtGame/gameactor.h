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

#ifndef DELTA_GAMEACTOR
#define DELTA_GAMEACTOR

#include <dtCore/physical.h>
#include <dtDAL/physicalactorproxy.h>
#include <dtDAL/exceptionenum.h>
#include "dtGame/invokable.h"
#include "dtGame/export.h"

namespace dtGame
{
   class Message;
   class ActorUpdateMessage;
   class GameManager;
   
   class GameActorProxy;
   /**
    * @class GameActor
    * This class will be the main base class for actors being used by the
    * Game Manager
    * @see dtGame::GameManager
    */
   class DT_GAME_EXPORT GameActor : public dtCore::Physical
   {
      public:
         /// Constructor
   		GameActor(GameActorProxy& proxy);
             
         /// Destructor
   		virtual ~GameActor();
         
         /**
          * @return the GameActorProxy for this game actor.
          */
         GameActorProxy& GetGameActorProxy() { return *mProxy; }

         /**
          * @return the GameActorProxy for this game actor.
          */
         const GameActorProxy& GetGameActorProxy() const { return *mProxy; }
                	                  
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
         virtual void TickLocal(const Message& tickMessage);

         /**
          * Method for handling remote ticks.  This will called by the "Tick Remote" invokable
          * This is designed to be registered to receive TICK_REMOTE messages, but that registration is not done
          * be default
          * @see GameManager#RegisterForMessages
          * @param tickMessage the actual message
          */
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
          * Sets the shader group on the terrain actor.  This implementation uses
          * the default shader in the group to shade the terrain.
          * @param groupName The name of the shader group.
          */
         virtual void SetShaderGroup(const std::string &groupName);

         /**
          * Gets the current shader group assigned to this terrain.
          * @return The name of the group.
          */
         std::string GetShaderGroup() const { return mShaderGroup; }
         
         virtual void OnShaderGroupChanged();

      protected:
   
          /**
           * Called when an actor is first placed in the "world"
           */
         virtual void OnEnteredWorld() { }
      
      private:
   		
         /** 
          * Sets is an actor is remote
          * @param remote Should be true is the actor is remote, false if not
          */
         void SetRemote(bool remote) { mRemote = remote; }
      		
         /** 
          * Sets is an actor is published
          * @param remote Should be true is the actor is published, false if not
          */
         void SetPublished(bool published) { mPublished = published; }
            
         friend class GameActorProxy;
         GameActorProxy* mProxy;
         
         bool mPublished;
         bool mRemote;
         std::string mShaderGroup;
   };
		
   /**
    * class GameActorProxy
    * This will be the base class for all of the actor proxies utilized by the 
    * Game Manager
    * @see dtGame::GameManager
    */
	class DT_GAME_EXPORT GameActorProxy : public dtDAL::PhysicalActorProxy
	{
		public:
         // Use this when you register a message type and want to receive it in ProcessMessage()
         static const std::string PROCESS_MSG_INVOKABLE;
         // invokables for tick local and remote - will call TickLocal() and TickRemote();
         static const std::string TICK_LOCAL_INVOKABLE;
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
               Ownership(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         /// Constructor
			GameActorProxy();

         /// Destructor
			virtual ~GameActorProxy();
		   /**
          * This is a shortcut to avoid having to dynamic cast to a GameActorProxy.  
          * @return true always
          */
         virtual bool IsGameActorProxy() const { return true; }
                           
         GameActor& GetGameActor() 
         {
            GameActor* ga = dynamic_cast<GameActor*> (mActor.get());
            if(ga == NULL)
            {
               EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type GameActor");
            }
            return *ga;
         }

         /**
          * @return a const pointer to the parent game manager that owns this actor.
          */
         const GameManager* GetGameManager() const { return mParent; }
         
         /**
          * @return a pointer to the parent game manager that owns this actor.
          */
         GameManager* GetGameManager() { return mParent; }

         /**
          * Retrieves the game actor that this proxy represents. 
          * @return The Game Actor
          */
         const GameActor& GetGameActor() const
         {
            const GameActor* ga = dynamic_cast<const GameActor*> (mActor.get());
            if(ga == NULL)
            {
               EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type GameActor");
            }
            return *ga;
         }

         /**
          * Creates the properties associated with this proxy
          */
         virtual void BuildPropertyMap();

         /**
          * Creates the invokables associated with this proxy.
          */
         virtual void BuildInvokables();
         
         /**
          * Adds a new invokable to the this proxy's list of invokables.
          * @param the new invokable to add.
          * @note
          *      Invokables must have unique names, therefore, if one
          *      is added that who's name collides with another, the invokable
          *      is not added and an error message is logged.
          */
         void AddInvokable(Invokable& newInvokable);
         
         /**
          * @return the invokable with the given name or NULL if it doesn't have one with that name.
          */
         Invokable* GetInvokable(const std::string& name);

         /**
          * Gets a list of the invokables currently registered for this
          * Game actor proxy.
          */
         void GetInvokables(std::vector<Invokable*>& toFill);

         /**
          * Gets a const list of the invokables currently registered for this
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
          * This is like NotifyFullActorUpdate() except that on subclasses, it might
          * only update some fields.  The Full would always send all properties, where
          * this one might only send what it thinks has updated since the last update. 
          * The default implementation just calls NotifyFullActorUpdate().
          * Note - This will do nothing if the actor is Remote.
          * @see NotifyFullActorUpdate
          */
         virtual void NotifyActorUpdate();

         /**
          * Populates an update message from the actor proxy.
          * @param update The message to populate.
          * @param propNames  the properties to include in the message.
          */
         virtual void PopulateActorUpdate(ActorUpdateMessage& update, const std::vector<std::string> &propNames) throw();
          
         /**
          * Populates an update message from the actor proxy.  It will add all property values to the message.
          * @param update The message to populate.
          */
         virtual void PopulateActorUpdate(ActorUpdateMessage& update) throw();

         /**
          * Takes and actor update message and applys the parameter values to change the 
          * the property values of the this actor proxy.  This is virtual so it can be extended
          * or replaced to accomodate special behavior or special subclasses of actor update.
          * 
          * @param msg the message to apply.
          */
         virtual void ApplyActorUpdate(const ActorUpdateMessage& msg) throw(); 
         
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
         bool IsRemote() const
         { 
            return GetGameActor().IsRemote();
         }

         /** 
          * Returns if this proxy is published, calls the private actor definition
          * @see dtGame::GameActor
          * @return True if the actor is published, false if not
          */
			bool IsPublished() const 
			{ 
				return GetGameActor().IsPublished();
			}

         /** 
          * Returns the ownership of the actor proxy
          * @return The ownership, corresponding with the ownership class
          * @see dtGame::GameActorProxy::Ownership
          */
         inline Ownership& GetInitialOwnership() { return *ownership; }
          
         /** 
          * Sets the ownership of the actor proxy
          * @return The ownership, corresponding with the ownership class
          * @see dtGame::GameActorProxy::Ownership
          */
         inline void SetInitialOwnership(Ownership &newOwnership) { ownership = &newOwnership; }
         
         /**
          * Registers to receive a specific tyep of message from the GM.  You will receive 
          * all instances of this message, regardless of whether you are the about actor or not.
          * By default, it will use the ProcessMessage() invokable on GameActor (PROCESS_MSG_INVOKABLE)
          * @see dtGame::GameActorProxy::RegisterForMessagesAboutOtherActor
          * @see dtGame::GameActorProxy::RegisterForMessagesAboutSelf
          * @see dtGame::GameActor::ProcessMessage
          * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
          */
         void RegisterForMessages(const MessageType& type, 
            const std::string& invokableName = PROCESS_MSG_INVOKABLE);

         /**
          * Registers to receive a specific type of message from the GM.  You will ONLY receive 
          * messages about this other actor.  Use this when you want to track interactions with 
          * other actors - such as a player listening for when a vehicle he is in gets damaged.
          * By default, it will use the ProcessMessage() invokable on GameActor (PROCESS_MSG_INVOKABLE)
          * @see dtGame::GameActorProxy::RegisterForMessages
          * @see dtGame::GameActorProxy::RegisterForMessagesAboutSelf
          * @see dtGame::GameActor::ProcessMessage
          * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
          */
         void RegisterForMessagesAboutOtherActor(const MessageType& type, 
            const dtCore::UniqueId& targetActorId, 
            const std::string& invokableName = PROCESS_MSG_INVOKABLE);

         /**
          * Registers to receive a specific type of message from the GM.  You will ONLY receive 
          * messages about yourself. This is the normal use case for registering for messages.
          * Typically, you only want to know when YOU have fired a weapon, or when YOU have been 
          * shot. Not when another player is shot.
          * By default, it will use the ProcessMessage() invokable on GameActor (PROCESS_MSG_INVOKABLE)
          * @see dtGame::GameActorProxy::RegisterForMessages
          * @see dtGame::GameActorProxy::RegisterForMessagesAboutOtherActor
          * @see dtGame::GameActor::ProcessMessage
          * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
          */
         void RegisterForMessagesAboutSelf(const MessageType& type, 
            const std::string& invokableName = PROCESS_MSG_INVOKABLE);

         /**
          * Unregisters the invokable for a specific type of message from the GM.  This is the 
          * reverse of RegisterForMessages for global messages. By default, it will unregister the 
          * ProcessMessage() invokable on GameActor (PROCESS_MSG_INVOKABLE)
          * @see dtGame::GameActorProxy::RegisterForMessages
          * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
          */
         void UnregisterForMessages(const MessageType& type, 
            const std::string& invokableName = PROCESS_MSG_INVOKABLE);

         /**
          * Unregisters the invokable for a specific type of message for a specific Actor.  This 
          * is the reverse of RegisterForMessagesAboutOtherActor. By default, it will unregister the 
          * ProcessMessage() invokable on GameActor (PROCESS_MSG_INVOKABLE)
          * @see dtGame::GameActorProxy::RegisterForMessagesAboutOtherActor
          * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
          */
         void UnregisterForMessagesAboutOtherActor(const MessageType& type, 
            const dtCore::UniqueId& targetActorId, 
            const std::string& invokableName = PROCESS_MSG_INVOKABLE);

         /**
          * Unregisters the invokable for a specific type of message from the GM.  This is the 
          * reverse of RegisterForMessagesAboutSelf. By default, it will unregister the 
          * ProcessMessage() invokable on GameActor (PROCESS_MSG_INVOKABLE)
          * @see dtGame::GameActorProxy::RegisterForMessagesAboutSelf
          * @see dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE
          */
         void UnregisterForMessagesAboutSelf(const MessageType& type, 
            const std::string& invokableName = PROCESS_MSG_INVOKABLE);

      protected:
         
         /**
          * Instantiates the actor that the proxy abstracts.
          * This must ALSO call SetGameActorProxy
          */
         virtual void CreateActor() = 0;

         /**
          * Called when an actor is first placed in the "world"
          */
         virtual void OnEnteredWorld() { }	
         
         //void RegisterMessageHandler(const MessageType& type, const std::string& invokableName);	
         //void UnregisterMessageHandler(const MessageType& type, const std::string& invokableName);   
		
      private:
             
         /**
          * Populates an update message from the actor proxy.
          * @param update The message to populate.
          * @param propNames the list of properties to include in the message.
          * @param limitProperties true if the propNames list should be respected or false if all properties should added.
          */
         void PopulateActorUpdate(ActorUpdateMessage& update, const std::vector<std::string> &propNames, bool limitProperties) throw();
	
         /** 
          * Sets if the actor is remote by invoking the actor implementation
          * @param True if the actor should be remote, false if not
          */
         void SetRemote(bool remote);
            
          /** 
           * Sets if the actor is published by invoking the actor implementation
           * @param True if the actor should be remote, false if not
           */
      	void SetPublished(bool published);
   
          /**
           * Invokes the OnEnteredWorld function of the actor, and then the proxy
           */
      	void InvokeEnteredWorld();
         
         /**
          * Sets whether or not this is a game actor
          * @note, this property is read only, the the setter definition is merely a 
          * dummy definition since it is needed to make the functor for this property
          * internally, if a property is read only its setter method is ignored, so this 
          * isn't an issue
          */
         void SetIsGameActorProxy(bool b) {}  
               
         /**
          * This is called by the GameManager since it is a friend class.
          */      
         void SetGameManager(GameManager* gm) { mParent = gm; }      
               
         friend class GameManager;
         GameManager* mParent;
         Ownership *ownership;
         std::map<std::string, dtCore::RefPtr<Invokable> > mInvokables;
         std::multimap<const MessageType*, dtCore::RefPtr<Invokable> > mMessageHandlers;
	};
}

#endif 
