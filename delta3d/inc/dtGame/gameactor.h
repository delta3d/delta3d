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
   		GameActor();
             
         /// Destructor
   		virtual ~GameActor();
                	
         /** 
          * Returns if the actor is remote
          * @return True is the actor is remote, false if not
          */
         inline bool IsRemote() const { return isRemote; }
   	
         /**
          * Returns is the actor is published
          * @return True is the actor is published, false if not
          */
         inline bool IsPublished() const { return isPublished; }
      	
         /**
          * Method for handling local ticks.  This will called by the "Tick Local" invokable.
          * This is designed to be registered to receive TICK_LOCAL messages, but that registration is not done
          * be default
          * @see GameActorProxy#RegisterMessageHandler
          * @param tickMessage the actual message
          */
         virtual void TickLocal(const Message& tickMessage);

         /**
          * Method for handling remote ticks.  This will called by the "Tick Remote" invokable
          * This is designed to be registered to receive TICK_REMOTE messages, but that registration is not done
          * be default
          * @see GameActorProxy#RegisterMessageHandler
          * @param tickMessage the actual message
          */
         virtual void TickRemote(const Message& tickMessage);

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
         inline void SetRemote(bool remote) { isRemote    = remote;    }
      		
         /** 
          * Sets is an actor is published
          * @param remote Should be true is the actor is published, false if not
          */
         inline void SetPublished(bool published) { isPublished = published; }
   
         friend class GameActorProxy;
         bool isPublished;
         bool isRemote;
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
         void GetInvokableList(std::vector<Invokable*>& toFill);

         /**
          * Gets a const list of the invokables currently registered for this
          * Game actor proxy.
          */
         void GetInvokableList(std::vector<const Invokable*>& toFill) const;
         
         /**
          * Populates an update message from the actor proxy.
          * @param update The message to populate.
          * @param propNames  the properties to include in the message.
          */
         void PopulateActorUpdate(ActorUpdateMessage& update, const std::vector<std::string> &propNames);
          
         /**
          * Populates an update message from the actor proxy.  It will add all property values to the message.
          * @param update The message to populate.
          */
         void PopulateActorUpdate(ActorUpdateMessage& update);
         
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
         		          
      protected:
         
         /**
          * Called when an actor is first placed in the "world"
          */
         virtual void OnEnteredWorld() { }	
         
         void RegisterMessageHandler(const MessageType& type, const std::string& invokableName);	
         void UnregisterMessageHandler(const MessageType& type, const std::string& invokableName);   
		
      private:

         /**
          * Instantiates the actor that the proxy abstracts
          */
         virtual void CreateActor() { mActor = new GameActor; }
             
         /**
          * Populates an update message from the actor proxy.
          * @param update The message to populate.
          * @param propNames the list of properties to include in the message.
          * @param limitProperties true if the propNames list should be respected or false if all properties should added.
          */
         void PopulateActorUpdate(ActorUpdateMessage& update, const std::vector<std::string> &propNames, bool limitProperties);
	
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
