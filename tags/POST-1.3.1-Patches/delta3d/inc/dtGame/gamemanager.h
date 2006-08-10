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
* @author Matthew W. Campbell, William E. Johnson II, and David Guthrie
*/
#ifndef DELTA_GAMEMANANGER
#define DELTA_GAMEMANANGER

#include <set>
#include <map>
#include <queue>
#include "dtGame/export.h"
#include "dtGame/gameactor.h"
#include "dtGame/messagefactory.h"
#include "dtGame/message.h"
#include "dtGame/machineinfo.h"
#include "dtGame/environmentactor.h"
#include <dtCore/refptr.h>
#include <dtCore/base.h>
#include <dtCore/scene.h>
#include <dtCore/timer.h>
#include <dtDAL/librarymanager.h>
#include <dtUtil/log.h>

// Forward declarations
namespace dtDAL
{
   class ActorType;
   class ActorPluginRegistry;
}

// Forward declarations
namespace dtABC
{
   class Application;
}


/** 
 * A high-level library that manages interaction between game actors.
 */
namespace dtGame 
{
   //class Message;
   class GMComponent;

   class DT_GAME_EXPORT GameManager : public dtCore::Base 
   {
      DECLARE_MANAGEMENT_LAYER(GameManager);

      public:
      
         class DT_GAME_EXPORT ComponentPriority : public dtUtil::Enumeration
         {
            DECLARE_ENUM(ComponentPriority);
            public:
               ///Highest possible priority.  Components with this priority will get messages first.
               static const ComponentPriority HIGHEST;
      
               ///Higher priority.  Components with this priority will get messages after HIGHEST, but before any others.
               static const ComponentPriority HIGHER;
               
               ///Normal priority.  Components with this priority will get messages after any HIGHER priority, but before LOWER.
               static const ComponentPriority NORMAL;
               
               ///Lower priority.  Components with this priority will get messages after any NORMAL or HIGHER priority, but before LOWEST.
               static const ComponentPriority LOWER;
      
               ///Lowest priority.  Components with this priority will get messages after all others.
               static const ComponentPriority LOWEST;
      
               ///@return the order id.  The higher the priority, the lower the number.
               unsigned int GetOrderId() const { return mOrderId; }
      
            protected:
               ComponentPriority(const std::string &name, unsigned int orderId) : Enumeration(name), mOrderId(orderId)
               {
                  AddInstance(this);
               }
               unsigned int mOrderId;
         };

         /// Constructor
         GameManager(dtCore::Scene &scene);

      protected:

         /// Destructor
         virtual ~GameManager();
      
      public:
      
         /**
          * Loads an actor registry
          * @param libName the name of the library to load
          */
         void LoadActorRegistry(const std::string &libName);

         /**
          * Unloads an actor registry
          * @param libName the name of the library to unload
          */
         void UnloadActorRegistry(const std::string &libName);

         /**
          * Returns a list of all the actor types the library manager knows how 
          * to create.
          * @param actorTypes A vector to fill
          */
         void GetActorTypes(std::vector<dtCore::RefPtr<dtDAL::ActorType> > &actorTypes);

         /**
          * Gets a single actor type that matches the name and category specified.
          * @param category Category of the actor type to find.
          * @param name Name of the actor type.
          * @return A valid smart pointer if the actor type was found.
          */
         dtCore::RefPtr<dtDAL::ActorType> FindActorType(const std::string &category, const std::string &name);

         /**
          * Gets a registry currently loaded by the library manager.  
          * @param name The name of the library.  Note, this name is the
          * system independent name.
          * @return A handle to the registry or NULL if it is not currently loaded.
          */
         dtDAL::ActorPluginRegistry* GetRegistry(const std::string &name);

         /**
          * @param actorType the actor type to get the registry for.
          */
         dtDAL::ActorPluginRegistry* GetRegistryForType(dtDAL::ActorType& actorType);

         /**
          * Determines which platform we are running on and returns a
          * platform dependent library name.
          * @param libBase Platform independent library name.
          * @return A platform dependent library name.
          * @note
          *  For example.  If the platform independent library name is
          *  ExampleActors then on Windows platforms the resulting dependent
          *  library name would be ExampleActors.dll, however, on Unix based
          *  platforms, the resulting name would be libExampleActors.so.
          */
         std::string GetPlatformSpecificLibraryName(const std::string &libBase);

         /**
          * Strips off the path and platform specific library prefixs and extensions
          * and returns a system independent file name.
          * @param libName The platform specific library name.
          * @return A platform independent library name.
          */
         std::string GetPlatformIndependentLibraryName(const std::string &libName);


         /**
          * Called by the dtCore::Base class
          * @param The data from the message
          * @see dtCore::Base
          */
         virtual void OnMessage(MessageData *data);

         /**
          * Calls DispatchNetworkMessage on all of the components
          * @param The message to send
          */
         void SendNetworkMessage(const Message& message);

         /**
          * Calls ProcessMessage on all of the components
          * @param The message to process
          */
         void SendMessage(const Message& message);

         /**
          * Adds a component to the list of components the game mananger
          * will communicate with
          * @param The component to add
          * @param priority the priority of the component.  This translates into the order of message delivery.
          * @throw dtGame::ExceptionEnum::INVALID_PARAMETER if the component lacks a unique name
          */
         void AddComponent(GMComponent& component, const ComponentPriority& priority);

         /**
          * Removes a component to the list of components the game mananger
          * will communicate with
          * @param The component to remove
          */
         void RemoveComponent(GMComponent& component);

         /**
          * Fills a vector with all of the components in the game manager.
          * @param toFill the vector to fill.
          */
         void GetAllComponents(std::vector<GMComponent*>& toFill);

         /**
          * Fills a vector with all of the components in the game manager.
          * @param toFill the vector to fill.
          */
         void GetAllComponents(std::vector<const GMComponent*>& toFill) const;

         /**
          * Returns a const component of the requested name or NULL if none exists
          * @return A pointer to the requested component, or NULL
          */
         GMComponent* GetComponentByName(const std::string &name);

         /**
          * Returns a const component of the requested name or NULL if none exists
          * @return A pointer to the requested component, or NULL
          */
         const GMComponent* GetComponentByName(const std::string &name) const;

         /**
          * Sets an environment actor on the game manager
          * @param envActor The environment actor to set
          */
         void SetEnvironmentActor(EnvironmentActorProxy *envActor);

         /**
          * Gets the environment actor on the game manager
          * @return mEnvProxy or NULL if no environment actor has been set
          */
         EnvironmentActorProxy* GetEnvironmentActor() { return mEnvironment.get(); }

         /**
          * Creates an actor based on the actor type.
          * @param The actor type to create.
          * @throws dtDAL::ExceptionEnum::ObjectFactoryUnknownType
          */
         dtCore::RefPtr<dtDAL::ActorProxy> CreateActor(dtDAL::ActorType& actorType) throw(dtUtil::Exception);

         /**
          * Creates an actor based on the actor type and store it in a ref pointer.
          * This method is templated so that the caller can create a ref pointer to the actual type of the proxy,
          * not ActorProxy.  This is very handy with GameActorProxies since it is typical that uses will create those most often.
          * @param The actor type to create.
          * @param proxy a RefPtr to fill with the created actor.  If the actor is not type specified, the RefPtr will be NULL.
          * @throws dtDAL::ExceptionEnum::ObjectFactoryUnknownType
          */
         template <typename ProxyType>
         void CreateActor(dtDAL::ActorType& actorType, dtCore::RefPtr<ProxyType>& proxy) throw(dtUtil::Exception)
         {
            dtCore::RefPtr<dtDAL::ActorProxy> tmpProxy = CreateActor(actorType);
            proxy = dynamic_cast<ProxyType*>(tmpProxy.get());
         }

         /**
          * Creates an actor based on the actor type
          * @param category The category corresponding to the actor type
          * @param name The name corresponding to the actor type
          * @throws dtDAL::ExceptionEnum::ObjectFactoryUnknownType
          */
         dtCore::RefPtr<dtDAL::ActorProxy> CreateActor(const std::string &category, const std::string &name) throw(dtUtil::Exception);

         /**
          * Creates an actor based on the string version of the actor type and store it in a ref pointer.
          * This method is templated so that the caller can create a ref pointer to the actual type of the proxy,
          * not ActorProxy.  This is very handy with GameActorProxies since it is typical that uses will create those most often.
          * @param The actor type to create.
          * @param proxy a RefPtr to fill with the created actor.  If the actor is not type specified, the RefPtr will be NULL.
          * @throws dtDAL::ExceptionEnum::ObjectFactoryUnknownType
          */
         template <typename ProxyType>
         void CreateActor(const std::string& category, const std::string& name, dtCore::RefPtr<ProxyType>& proxy) throw(dtUtil::Exception)
         {
            dtCore::RefPtr<dtDAL::ActorProxy> tmpProxy = CreateActor(category, name);
            proxy = dynamic_cast<ProxyType*>(tmpProxy.get());
         }

         /**
          * Adds an actor to the list of actors that the game manager knows about
          * @param actorProxy  The actor proxy to add
          */
         void AddActor(dtDAL::ActorProxy& actorProxy);

         /**
          * Adds a game actor to the list of actors that the game manager knows about
          * @param actorProxy The actor proxy to add
          * @param isRemote true if the actor is remote, false if not
          * @param publish true if the actor should be immediately published.
          * @throws ExceptionEnum::ACTOR_IS_REMOTE if the actor is remote and publish is true.
          * @throws ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if the actor is flagged as a game actor, but is not a GameActorProxy.
          */
         void AddActor(GameActorProxy& gameActorProxy, bool isRemote, bool publish) throw(dtUtil::Exception);

         /**
          * Publishes an actor to the world.  Remote actors may not be published
          * @param The actor to publish
          * @throws ExceptionEnum::ACTOR_IS_REMOTE if the actor is remote.
          */
         void PublishActor(GameActorProxy& gameActorProxy);

          /**
          * Removes all game actors and actors from the game manager
          * Currently all actors are removed immediately, but this should not be 
          * assumed to be true going forward.
          * INFO_ACTOR_DELETE messages are only sent for local actors.
          * @note This method causes delete messages to be sent for all actors 
          *    that need to be deleted.  If an immediate delete or clear of the 
          *    game manager is required call this method with a true flag.
          */
         void DeleteAllActors() { DeleteAllActors(false); }

         /**
          * Removes an actor or game actor from the game manager.
          * Game actors are not actually removed until the end of the current frame so that
          * messages can propogate.  Regular actor proxies are removed immediately.  
          * An INFO_ACTOR_DELETE message is only sent if it's a game actor and is local.
          * @param The actor to remove
           */
         void DeleteActor(dtDAL::ActorProxy& actorProxy);

         /**
          * Removes all game actors and actors from the game manager
          * Currently all actors are removed immediately, but this should not be 
          * assumed to be true going forward.
          * @param immediate False if the message about deleting should be sent, if
          *    true, this method will clear all actor management related lists thus
          *    "immediately" removing all actors from the game manager.  Calling this
          *    method with "true" is useful for complete game state changes such as 
          *    map changes.
          */
         void DeleteAllActors(bool immediate);
         
         /**
          * Returns a list of actor types that have been registered with the
          * game manager
          * @param The vector to fill
          */
         void GetActorTypes(std::vector<const dtDAL::ActorType*> &vec) const;

         /**
          * Gets the number of game actors currently managed by this game
          * manager.
          * @return The number of game actors in the system.
          */
         unsigned int GetNumGameActors() const
         {
            return mGameActorProxyMap.size();
         }
         
         /**
          * Retrieves all the game actors added to the GM
          * @param toFill The vector to fill
          */
         void GetAllGameActors(std::vector<dtCore::RefPtr<GameActorProxy> > &toFill);

         /**
          * Retrieves all the non game actors added to the GM
          * @param toFill The vector to fill
          */
         void GetAllNonGameActors(std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill);

         /**
          * Retrieves all the actors added to the GM
          * @param toFill The vector to fill
          */
         void GetAllActors(std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill);

         /**
          * Fills a vector with actors that are currently in the scene
          * @param vec The vector to fill
          */
         void GetActorsInScene(std::vector<dtCore::RefPtr<dtCore::DeltaDrawable> > &vec);

         /**
          * Fills a vector with the game proxys whose names match the name parameter
          * @param The name to search for
          * @param The vector to fill
          */
         void FindActorsByName(const std::string &name, std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill);

         /**
          * Fills a vector with the game proxys whose types match the type parameter
          * @param The type to search for
          * @param The vector to fill
          */
         void FindActorsByType(const dtDAL::ActorType &type, std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill);

         /**
          * Fills out a vector of actors with the specified class name
          * @param className the classname
          * @param toFill The vector to fill
          */
         void FindActorsByClassName(const std::string &className, std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill);
         
         /**
          * Fills a vector with the game proxys whose position is within the radius parameter
          * @param The radius to search in 
          * @param The vector to fill
          */
         void FindActorsWithinRadius(const float radius, std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill);

         /**
          * Returns the game actor proxy whose is matches the parameter
          * @param The id of the proxy to find
          * @return The proxy, or NULL if not found
          */
         GameActorProxy* FindGameActorById(const dtCore::UniqueId &id);

         /**
          * Returns the actor proxy whose is matches the parameter. This will search both the game actors and the
          * regular actor proxies.
          * @param The id of the proxy to find
          * @return The proxy, or NULL if not found
          */
         dtDAL::ActorProxy* FindActorById(const dtCore::UniqueId &id);

         /**
          * Saves the game state
          * @return True if saved successfully, false if error
          */
         bool SaveGameState();

         /**
          * Loads a game state
          * @return True if loaded successfully, false if error
          */
         bool LoadGameState();

         /**
          * Changes the map being used by the Game Manager
          * @param mapName       The name of the map to load.
          * @param addBillboards optional parameter that defaults to false that says whether or not proxy billboards should be 
          *                      added to the scene.  This should only be true for debugging purposes.
          * @param enableDatabasePaging optional parameter to enable database paging for paged LODs usually used in
          *                             large terrain databases.  Passing false will not disable paging if it is already enabled.
          * @throws ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if an actor is flagged as a game actor, but is not a GameActorProxy.
          */
         void ChangeMap(const std::string &mapName, bool addBillboards = false, bool enableDatabasePaging = true) throw(dtUtil::Exception);

         /**
          * Sets a timer on the game mananger.  It will send out a timer message when it expires.
          * @param name The name of the timer
          * @param aboutActor the actor to put in the about field of the message.  If this
          *    is NULL, the timer is a "global" timer in that it is not bound to any
          *    particular actor.  This is generally only useful if components need to use the
          *    timer functionality of the game manager.
          * @param time The time of the timer in seconds.
          * @param repeat True to repeat the timer, false if once only
          * @param realTime True if this time should use real time, or false if it should use simulation time.
          */
         void SetTimer(const std::string& name, const GameActorProxy* aboutActor, float time, 
            bool repeat = false, bool realTime = false);

         /**
          * Removes the timer with the given name.  If no timer by that name exists, this is a no-op.
          * @param name the name of the timer to remove.
          * @param proxy The proxy this timer is associated with or NULL if the timer
          *    was registered as a "global" timer.
          */
         void ClearTimer(const std::string& name, const GameActorProxy *proxy);

         /**
          * Accessor to the scene member of the class
          * @return The scene
          */
         dtCore::Scene& GetScene() { return *mScene; }

         /**
          * Accessor to the scene member of the class
          * @return The scene
          */
         const dtCore::Scene& GetScene() const { return *mScene; }

         /**
          * Sets the scene member of the class
          * This should not be changed after startup.
          * @param The new scene to assign
          */
         void SetScene(dtCore::Scene &newScene) { mScene = &newScene; }

         ///@return the application that owns this game mananger.
         dtABC::Application& GetApplication() throw(dtUtil::Exception); 

         ///@return the application that owns this game mananger.
         const dtABC::Application& GetApplication() const throw(dtUtil::Exception);
         
         ///sets the application that owns this game manager.  This should NOT be changes after startup.
         void SetApplication(dtABC::Application& application);

         /**
          * Gets the interval (in seconds) used for writing out GM Statistics. This 
          * is usually a debug setting that can be used to see how much work the GM is doing
          * as compared to how much work your scene is doing.  If this is > 0, and the 
          * appropriate log level is on, the GM will output statistics periodically
          * Default is 0.
          */
         int GetStatisticsInterval() { return mStatisticsInterval; }

         /**
          * Sets the interval (in seconds) used for writing out GM Statistics. This 
          * is usually a debug setting that can be used to see how much work the GM is doing
          * as compared to how much work your scene is doing.  If this is > 0, and the 
          * appropriate log level is on, the GM will output statistics periodically
          * @param The new interval (in seconds).  <=0 turns off statistics logging
          */
         void SetStatisticsInterval(const int statisticsInterval) { mStatisticsInterval = statisticsInterval; }

         /**
          * Retrieves the message factor that is controlled by the GameManager
          * @return mFactory he message factory
          * @see class dtGame::MessageFactory
          */
         MessageFactory& GetMessageFactory()  { return mFactory; } 

         /**
          * Retrieves the message factor that is controlled by the GameManager
          * @return mFactory he message factory
          * @see class dtGame::MessageFactory
          */
         const MessageFactory& GetMessageFactory() const { return mFactory; } 
         
         /**
          * Gets the const version of the machine info
          * @return mMachineInfo
          */
         const MachineInfo& GetMachineInfo() const { return *mMachineInfo; }
         
         /**
          * Non const version to get the machine info
          * @return mMachineInfo
          */
         MachineInfo& GetMachineInfo() { return *mMachineInfo; }
         
         /**
          * Gets the name of the currently loaded map
          * @return mLoadedMap
          */
         const std::string& GetCurrentMap() const { return mLoadedMap; }
         
         /**
          * @return The scale of realtime the GameManager is running at.
          */
         float GetTimeScale() const;
                  
         /**
          * @return the current simulation time
          * @see dtCore::System#GetSimulationTime
          */
         double GetSimulationTime() const;

         /**
          * @return the current simulation wall-clock time
          * @see dtCore::System#GetSimulationClockTime
          */
         dtCore::Timer_t GetSimulationClockTime() const;
         
         /**
          * @return The current real clock time
          * @see dtCore::System#GetRealClockTime
          */
         dtCore::Timer_t GetRealClockTime() const;

         /**
          * Change the time settings.
          * @param newTime The new simulation time.
          * @param newTimeScale the new simulation time progression as a factor of real time.
          * @param newClockTime  The new simulation wall-clock time.
          */
         void ChangeTimeSettings(double newTime, float newTimeScale, dtCore::Timer_t newClockTime);
         
         /**
          * Get all of the GameActorProxies registered to receive all messages of a certain type.
          * @param type the message type to query for.
          * @param toFill a vector to fill with pairs GameActorProxies and the name of the invokable.
          */
         void GetRegistrantsForMessages(const MessageType& type, std::vector<std::pair<GameActorProxy*, std::string > >& toFill);
         
         /**
          * Get all of the GameActorProxies registered to receive messages of a given type for a given GameActor.
          * @param type the message type to query for.
          * @param targetActorId the id of the GameActor to query for.
          * @param toFill a vector to fill with the GameActorProxies.
          */
         void GetRegistrantsForMessagesAboutActor(
            const MessageType& type, 
            const dtCore::UniqueId& targetActorId, 
            std::vector<std::pair<GameActorProxy*, std::string > >& toFill);

         /**
          * @param type
          * @param proxy
          * @param invokableName
          */ 
         void RegisterForMessages(const MessageType& type, GameActorProxy& proxy, const std::string& invokableName);
         
         /**
          * @param type
          * @param proxy
          * @param invokableName
          */ 
         void UnregisterForMessages(const MessageType& type, GameActorProxy& proxy, const std::string& invokableName);
      
         /**
          * @param type
          * @param targetActorId
          * @param proxy
          * @param invokableName
          */ 
         void RegisterForMessagesAboutActor(
            const MessageType& type, 
            const dtCore::UniqueId& targetActorId, 
            GameActorProxy& proxy, 
            const std::string& invokableName);
         
         /**
          * @param type
          * @param targetActorId
          * @param proxy
          * @param invokableName
          */ 
         void UnregisterForMessagesAboutActor(
            const MessageType& type, 
            const dtCore::UniqueId& targetActorId, 
            GameActorProxy& proxy, 
            const std::string& invokableName);
            
         /**
          * @param proxy
          */
         void UnregisterAllMessageListenersForActor(GameActorProxy& proxy);
         
         /**
          * @return true if the GameManager is paused
          */
         bool IsPaused() const { return mPaused; }
         
         /**
          * Pauses or unpauses this GameManager.
          * @param pause true of false if this GM should be paused. If this value is 
          *              the same as the current state, this call is a noop.
          */
         void SetPaused(bool pause);

         /**
          * Handles a reject message.  This is typically called by a component (usually server side)
          * when it has determined that a request message is invalid and it needs to reject it.
          * The method creates a ServerMessageRejected - SERVER_REQUEST_REJECTED message.  If the 
          * reasonMessage has a MachineInfo that indicates it came from this server, then this method
          * does a SendMessage on the new rejected message.  Otherwise, it does a SendNetworkMessage.
          * The resulting reject message will eventually make its way back to the source client-component.
          * @param toReject the Message that you are trying to reject.
          * @param rejectDescription A text message describing why the message was rejected.
          */
         void RejectMessage(const dtGame::Message &reasonMessage, const std::string &rejectDescription);

      protected:

         struct TimerInfo 
         {
            std::string name;
            dtCore::UniqueId aboutActor;
            dtCore::Timer_t time;
            bool repeat;
            dtCore::Timer_t interval;

            bool operator < (const TimerInfo &rhs) const
            {
               if (time == rhs.time)
                  return this < &rhs;
               return time < rhs.time; 
            }
         };

         dtUtil::Log* mLogger;

         /**
          * Implements the functionality that will happen on the PreFrame event
          * @param deltaSimTime the change in simulation time since the last frame.
          * @param deltaRealTime the change in real time since the last frame.
          */
         virtual void PreFrame(double deltaSimTime, double deltaRealTime);

         /// Implements the functionality that will happen on the PostFrame event
         virtual void PostFrame();        
      
      private:

         std::set<TimerInfo>& GetSimulationTimerList() { return mSimulationTimers; }

         /**
          * Private helper method to process the timers. This is called from PreFrame
          * @param listToProcess The timer list to process 
          * @param clockTime The time to use
          * @note The clock time should correspond to the list to be processed
          */
         void ProcessTimers(std::set<TimerInfo> &listToProcess, dtCore::Timer_t clockTime);
         
         /**
          * Removes the proxy from the scene
          * @param proxy the proxy to remove from the scene.
          */
         void RemoveActorFromScene(dtDAL::ActorProxy& proxy);

         /**
          * Internal timer statistics calculation.  Computes what percent the partial time 
          * is of the total time. Basically (1.0 - ((total - partial) / total)) * 100.
          * Result is truncated to something like: 98.5, 42.3, ... 
          * @param total The total value used to determine the percentage
          * @param partial The partial amount that we are using for the percentage
          */
         float ComputeStatsPercent(dtCore::Timer_t total, dtCore::Timer_t partial);

         /**
          * Private helper method to send an environment changed message
          * @param envActor The about actor of the message
          */
         void SendEnvironmentChangedMessage(EnvironmentActorProxy *envActor);
      
         dtCore::RefPtr<MachineInfo> mMachineInfo;
         dtCore::RefPtr<EnvironmentActorProxy> mEnvironment;

         std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> > mGameActorProxyMap;
         std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> > mActorProxyMap;
         std::vector<dtCore::RefPtr<GameActorProxy> > mDeleteList;

         std::set<TimerInfo> mSimulationTimers, mRealTimeTimers;

         std::multimap<const MessageType*, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> > mGlobalMessageListeners;
         std::map<const MessageType*, std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> > > mActorMessageListeners;

         std::vector<dtCore::RefPtr<GMComponent> > mComponentList; 
         std::queue<dtCore::RefPtr<const Message> > mSendNetworkMessageQueue;
         std::queue<dtCore::RefPtr<const Message> > mSendMessageQueue;
         dtCore::RefPtr<dtCore::Scene> mScene;
         dtCore::RefPtr<dtDAL::LibraryManager> mLibMgr;
         MessageFactory mFactory;
         
         bool mPaused;
         std::string mLoadedMap;
         int mStatisticsInterval;

         // statistics data
         dtCore::Timer_t mStatsLastFragmentDump;
         long mStatsNumProcMessages;
         long mStatsNumSendNetworkMessages;
         long mStatsNumFrames;
         dtCore::Timer_t mStatsCumGMProcessTime;
         
         dtABC::Application* mApplication;
         
         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         GameManager(const GameManager&);
         GameManager& operator=(const GameManager&);
         
   };
}

#endif
