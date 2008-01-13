/* -*-c++-*-
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
 * Matthew W. Campbell, William E. Johnson II, and David Guthrie
 */
#ifndef DELTA_GAMEMANANGER
#define DELTA_GAMEMANANGER

#include <set>
#include <map>
#include <queue>

#include <dtGame/export.h>
#include <dtGame/gameactor.h>
#include <dtGame/messagefactory.h>
#include <dtGame/message.h>
#include <dtGame/machineinfo.h>
#include <dtGame/environmentactor.h>

#include <dtCore/refptr.h>
#include <dtCore/base.h>
#include <dtCore/timer.h>

namespace dtUtil
{
   class Log;
}

namespace dtCore
{
   class Scene;
}

// Forward declarations
namespace dtDAL
{
   class ActorType;
   class ActorPluginRegistry;
   class LibraryManager;
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
   class MapChangeStateData;
   class TickMessage;

   class DT_GAME_EXPORT GameManager : public dtCore::Base 
   {
      DECLARE_MANAGEMENT_LAYER(GameManager)

      public:
         typedef std::vector<std::string> NameVector;
         
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
      
         /// Used for statistics information, should never have to call yourself.
         void UpdateDebugStats(const dtCore::UniqueId& uniqueIDToFind, const std::string& nameOfObject, float realTimeElapsed,
            bool isComponent, bool ticklocal);

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
         void GetActorTypes(std::vector<const dtDAL::ActorType*> &actorTypes);

         /**
          * Gets a single actor type that matches the name and category specified.
          * @param category Category of the actor type to find.
          * @param name Name of the actor type.
          * @return A pointer to the actor type if the actor type was found or NULL otherwise.
          */
         const dtDAL::ActorType* FindActorType(const std::string &category, const std::string &name);
         
         /**
         * Fills a vector with the game proxys whose types match the name parameter
         * @param The name to search for
         * @param The vector to fill
         */
         void FindPrototypesByActorType(const dtDAL::ActorType &type, std::vector<dtDAL::ActorProxy*> &toFill) const;

         /**
         * Fills a vector with the game proxys whose names match the name parameter
         * @param The name to search for
         * @param The vector to fill
         */
         void FindPrototypesByName(const std::string &name, std::vector<dtDAL::ActorProxy*> &toFill) const;
         
         /**
         * Fills a vector with all the templates.
         * @param The vector to fill
         */
         void GetAllPrototypes(std::vector<dtDAL::ActorProxy*> &toFill) const;

         /**
          * @param The uniqueID to look for or NULL for error
          * @return the actor proxy with that ID
          */
         dtDAL::ActorProxy* FindPrototypeByID(const dtCore::UniqueId& uniqueID);

         /**
          * @param The uniqueID to look for or NULL for error
          * @return the actor proxy with that ID
          */
         template <typename T>
         void FindPrototypeByID(const dtCore::UniqueId& uniqueID, dtCore::RefPtr<T> &proxy)
         {
            dtDAL::ActorProxy *tempProxy = FindPrototypeByID(uniqueID);
            proxy = dynamic_cast<T*>(tempProxy);
         }

         /// Clears the mPrototypeActors map.
         void DeleteAllPrototypes();

         /// Deletes a single Template.
         void DeletePrototype(const dtCore::UniqueId& uniqueId);

         /// Makes a new GameActorProxy, and returns it to the user
         dtCore::RefPtr<dtDAL::ActorProxy> CreateActorFromPrototype(const dtCore::UniqueId& uniqueID);

         template <typename T>
         void CreateActorFromPrototype(const dtCore::UniqueId& uniqueID, 
                                       dtCore::RefPtr<T> &proxy)
         {
            dtCore::RefPtr<dtDAL::ActorProxy> baseProxy = CreateActorFromPrototype(uniqueID);
            proxy = dynamic_cast<T*>(baseProxy.get());
         }

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
          * Templated version of GetComponentByName that calls the normal one and dynamic casts
          * it to the type of the pointer passed in.  The pointer will be null if the name is not
          * found or the component is not the right type.
          * 
          * Unlike the actor find, this uses a dynamic cast because it is expected to be called
          * much less often, so safety was preferred.
          * 
          * @param name the name of the component to find
          * @param component the pointer to assign with the component found.
          */
         template <typename ComponentType>
         void GetComponentByName(const std::string &name, ComponentType*& component)
         {
            component = dynamic_cast<ComponentType*>(GetComponentByName(name));
         }

         /**
          * Returns a const component of the requested name or NULL if none exists
          * @return A pointer to the requested component, or NULL
          */
         const GMComponent* GetComponentByName(const std::string &name) const;

         /**
          * Const version of the other templated GetComponentByName method
          */
         template <typename ComponentType>
         void GetComponentByName(const std::string &name, const ComponentType* component) const
         {
            component = dynamic_cast<const ComponentType*>(GetComponentByName(name));
         }

         /**
          * Sets an environment actor on the game manager
          * @param envActor The environment actor to set
          */
         void SetEnvironmentActor(IEnvGameActorProxy *envActor);

         /**
          * Gets the environment actor on the game manager
          * @return mEnvProxy or NULL if no environment actor has been set
          */
         IEnvGameActorProxy* GetEnvironmentActor() { return mEnvironment.get(); }

         /**
          * Creates a game actor based on the actor type but sets the isRemote status to true (== remote).
          * @param The actor type to create.
          * @throws dtDAL::ExceptionEnum::ObjectFactoryUnknownType
          * @throws dtGame::ExceptionEnum::INVALID_PARAMETER if actortype is NOT a game actor
          */
         dtCore::RefPtr<dtGame::GameActorProxy> CreateRemoteGameActor(const dtDAL::ActorType& actorType);

         /**
          * Creates an actor based on the actor type.
          * @param The actor type to create.
          * @throws dtDAL::ExceptionEnum::ObjectFactoryUnknownType
          */
         dtCore::RefPtr<dtDAL::ActorProxy> CreateActor(const dtDAL::ActorType& actorType);

         /**
          * Creates an actor based on the actor type and store it in a ref pointer.
          * This method is templated so that the caller can create a ref pointer to the actual type of the proxy,
          * not ActorProxy.  This is very handy with GameActorProxies since it is typical that uses will create those most often.
          * @param The actor type to create.
          * @param proxy a RefPtr to fill with the created actor.  If the actor is not type specified, the RefPtr will be NULL.
          * @throws dtDAL::ExceptionEnum::ObjectFactoryUnknownType
          */
         template <typename ProxyType>
         void CreateActor(const dtDAL::ActorType& actorType, dtCore::RefPtr<ProxyType>& proxy)
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
         dtCore::RefPtr<dtDAL::ActorProxy> CreateActor(const std::string &category, const std::string &name);

         /**
          * Creates an actor based on the string version of the actor type and store it in a ref pointer.
          * This method is templated so that the caller can create a ref pointer to the actual type of the proxy,
          * not ActorProxy.  This is very handy with GameActorProxies since it is typical that uses will create those most often.
          * @param The actor type to create.
          * @param proxy a RefPtr to fill with the created actor.  If the actor is not type specified, the RefPtr will be NULL.
          * @throws dtDAL::ExceptionEnum::ObjectFactoryUnknownType
          */
         template <typename ProxyType>
         void CreateActor(const std::string& category, const std::string& name, dtCore::RefPtr<ProxyType>& proxy)
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
          * @param isRemote true if the actor is remotely controlled, false if not.  If a GameActor
          *                 is remote, then it is assumed it will be controlled via
          *                 dtGame::Message sent through the GameManager.  If it's local,
          *                 the GameActor will be controlling itself and creating and 
          *                 sending dtGame::Message with its updated data.
          * @param publish true if the actor should be immediately published.
          * @throws ExceptionEnum::ACTOR_IS_REMOTE if the actor is remote and publish is true.
          * @throws ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if the actor is flagged as a game actor, but is not a GameActorProxy.
          */
         void AddActor(GameActorProxy& gameActorProxy, bool isRemote, bool publish);

         /**
          * Adds an actor as a prototype actor.
          * @param actorProxy The actor proxy to add
          */
         void AddActorAsAPrototype(GameActorProxy& gameActorProxy);

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
          * Returns a list of actor types that are being used by existing actors in the
          * game manager.  This call is slow.
          * @param The set to fill
          */
         void GetUsedActorTypes(std::set<const dtDAL::ActorType*>& vec) const;

         /**
          * Gets the number of game actors currently managed by this game
          * manager.
          * @return The number of game actors in the system.
          */
         unsigned int GetNumGameActors() const { return mGameActorProxyMap.size(); }
        
         /**
          * Retrieves all the game actors added to the GM
          * @param toFill The vector to fill
          */
         void GetAllGameActors(std::vector<GameActorProxy*> &toFill) const;

         /**
          * Retrieves all the non game actors added to the GM
          * @param toFill The vector to fill
          */
         void GetAllNonGameActors(std::vector<dtDAL::ActorProxy*> &toFill) const;

         /**
          * Retrieves all the actors added to the GM
          * @param toFill The vector to fill
          */
         void GetAllActors(std::vector<dtDAL::ActorProxy*> &toFill) const;

         /**
          * Fills a vector with actors that are currently in the scene
          * @param vec The vector to fill
          */
         void GetActorsInScene(std::vector<dtCore::DeltaDrawable*> &vec) const;

         /**
          * Fills a vector with the game proxys whose names match the name parameter
          * @param The name to search for
          * @param The vector to fill
          */
         void FindActorsByName(const std::string &name, std::vector<dtDAL::ActorProxy*> &toFill) const;

         /**
          * Fills a vector with the game proxys whose types match the type parameter
          * @param The type to search for
          * @param The vector to fill
          */
         void FindActorsByType(const dtDAL::ActorType &type, std::vector<dtDAL::ActorProxy*> &toFill) const;

         /**
          * Fills out a vector of actors with the specified class name
          * @param className the classname
          * @param toFill The vector to fill
          */
         void FindActorsByClassName(const std::string &className, std::vector<dtDAL::ActorProxy*> &toFill) const;
         
         /**
          * Fills a vector with the game proxys whose position is within the radius parameter
          * @param radius The radius to search in 
          * @param toFill The vector to fill
          */
         void FindActorsWithinRadius(const float radius, std::vector<dtDAL::ActorProxy*> &toFill) const;

         /**
          * Returns the game actor proxy whose is matches the parameter
          * @param id The id of the proxy to find
          * @return The proxy, or NULL if not found
          */
         GameActorProxy* FindGameActorById(const dtCore::UniqueId &id) const;

         /**
          * Getst the game actor proxy whose is matches the parameter
          * @param id The id of the proxy to find
          */
         template<typename ProxyType>
         void FindGameActorById(const dtCore::UniqueId &id, ProxyType*& proxy) const
         {
            proxy = dynamic_cast<ProxyType*>(FindGameActorById(id));
         }

         /**
          * Returns the actor proxy whose is matches the parameter. This will search both the game actors and the
          * regular actor proxies.
          * @param id The id of the proxy to find
          * @return The proxy, or NULL if not found
          */
         dtDAL::ActorProxy* FindActorById(const dtCore::UniqueId &id) const;

         /**
          * Returns the actor proxy whose is matches the parameter. This will search both the game actors and the
          * regular actor proxies.
          * @param id The id of the proxy to find
          * @return The proxy, or NULL if not found
          */
         template<typename ProxyType>
         void FindActorById(const dtCore::UniqueId &id, ProxyType*& proxy) const
         {
            proxy = dynamic_cast<ProxyType*>(FindActorById(id));
         }

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
          * Changes the map being used by the Game Manager.  All actors, Game events, and associated data will be deleted.
          * It will send INFO_MAP_LOADED.  If another map is currently open, that map will be closed via calling CloseCurrentMap()
          * @see #CloseCurrentMap()
          * @param mapName       The name of the map to load.
          * @param addBillboards optional parameter that defaults to false that says whether or not proxy billboards should be 
          *                      added to the scene.  This should only be true for debugging purposes.
          * @param enableDatabasePaging optional parameter to enable database paging for paged LODs usually used in
          *                             large terrain databases.  Passing false will not disable paging if it is already enabled.
          * @throws ExceptionEnum::INVALID_PARAMETER if no map name is supplied.
          * @throws ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if map change is already in progress.
          */
         void ChangeMap(const std::string &mapName, bool addBillboards = false, bool enableDatabasePaging = true);

         /**
          * Changes the maps being used by the Game Manager.  All actors, Game events, and associated data will be deleted.
          * It will send INFO_MAP_LOADED.  If another map group of maps is currently open, they will be closed via calling CloseCurrentMap()
          * @see #CloseCurrentMap()
          * @param mapNames      The list of names of maps to load.
          * @param addBillboards optional parameter that defaults to false that says whether or not proxy billboards should be 
          *                      added to the scene.  This should only be true for debugging purposes.
          * @param enableDatabasePaging optional parameter to enable database paging for paged LODs usually used in
          *                             large terrain databases.  Passing false will not disable paging if it is already enabled.
          * @throws ExceptionEnum::INVALID_PARAMETER if no map name is supplied.
          * @throws ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if map change is already in progress.
          */
         void ChangeMapSet(const NameVector& mapNames, bool addBillboards = false, bool enableDatabasePaging = true);

         /**
          * Closes the open maps, if any, being used by the Game Manager.  All actors will be deleted whether maps are closed or not.
          * It will send an INFO_MAP_UNLOADED message if a map is actually closed.
          */
         void CloseCurrentMap();

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
         dtCore::Scene& GetScene();
         /**
          * Accessor to the scene member of the class
          * @return The scene
          */
         const dtCore::Scene& GetScene() const;
         /**
          * Sets the scene member of the class
          * This should not be changed after startup.
          * @param The new scene to assign
          */
         void SetScene(dtCore::Scene &newScene);
         
         ///@return the application that owns this game mananger.
         dtABC::Application& GetApplication(); 

         ///@return the application that owns this game mananger.
         const dtABC::Application& GetApplication() const;
         
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
          * Records statistics about different components and actors.
          * Sets the interval (in seconds) used for writing out GM Statistics. This 
          * is usually a debug setting that can be used to see how much work the GM is doing
          * as compared to how much work your scene is doing.  If this is > 0, and the 
          * appropriate log level is on, the GM will output statistics periodically
          * @param logComponents log timing for components
          * @param logActors log timing for actors
          * @param statisticsInterval The new interval (in seconds). Make sure  > 0
          * @param toConsole true to print to console, false to print to file
          * @param path if toConsole == false, print to this file.
          */
         void DebugStatisticsTurnOn(bool logComponents, bool logActors, 
                                    const int statisticsInterval, bool toConsole = true, 
                                    const std::string& path = "gamemanagerDebugInfo.txt");

         /// Turn off statistics information - params to log before stopping, and if user wants to clear history
         void DebugStatisticsTurnOff(bool logLastTime = false, bool clearList = false);

         /// print out the information from member vars
         void DebugStatisticsPrintOut(const float gmPercentTime);
        
         /**
          * Gets the flag for whether we will remove the Game Events when we change a map or not.
          * Normally, when a map is closed, the Game Manager removes the events that came from the
          * map. This flag allows the case where sometimes an event comes from a map but you don't want
          * it to go away.  The default is true.
          * @return Flag for whether we remove a map's GameEvents from 
          * the GameEventManager when closing a map. (default is true)
          */
         bool GetRemoveGameEventsOnMapChange() const { return mRemoveGameEventsOnMapChange; }

         /**no
          * Sets the flag for whether we will remove the Game Events when we change a map or not.
          * Normally, when a map is closed, the Game Manager removes the events that came from the
          * map. This flag allows the case where sometimes an event comes from a map but you don't want
          * it to go away.  The default is true.
          * @param removeGameEventsOnMapChange Flag for whether we remove a map's GameEvents from 
          * the GameEventManager when closing a map. (default is true)
          */
         void SetRemoveGameEventsOnMapChange(const bool removeGameEventsOnMapChange) { mRemoveGameEventsOnMapChange = removeGameEventsOnMapChange; }

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
          * Gets the name of the first currently loaded map.  This is support apps that
          * only use one map at a time
          * @return the current map or empty string if no map is loaded.
          */
         const std::string& GetCurrentMap() const;

         /**
          * Gets the set of names of the currently loaded map.
          * @return mLoadedMap
          */
         const NameVector& GetCurrentMapSet() const;
         
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
         void ChangeTimeSettings(double newTime, float newTimeScale, const dtCore::Timer_t &newClockTime);
         
         /**
          * Get all of the GameActorProxies registered to receive all messages of a certain type.
          * @param type the message type to query for.
          * @param toFill a vector to fill with pairs GameActorProxies and the name of the invokable.
          */
         void GetRegistrantsForMessages(const MessageType& type, std::vector<std::pair<GameActorProxy*, std::string > >& toFill) const;
         
         /**
          * Get all of the GameActorProxies registered to receive messages of a given type for a given GameActor.
          * @param type the message type to query for.
          * @param targetActorId the id of the GameActor to query for.
          * @param toFill a vector to fill with the GameActorProxies.
          */
         void GetRegistrantsForMessagesAboutActor(
            const MessageType& type, 
            const dtCore::UniqueId& targetActorId, 
            std::vector<std::pair<GameActorProxy*, std::string > >& toFill) const;

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
         void RejectMessage(const Message &reasonMessage, const std::string &rejectDescription);

         /**
          * Wrapper method to encapsulate SetContext on dtDAL::Project so an outside
          * user does not have to know about dtDAL::Project at all when they are writing
          * an application using GameStart
          * @param context A path to the context to use
          * @param readOnly True to open the context in read only mode
          */
         void SetProjectContext(const std::string &context, bool readOnly = false);

         /**
          * Wrapper method that returns a string to the project context currently 
          * being used
          * @return A path to the new context
          */
         const std::string& GetProjectContext() const;

         /**
          * Shuts down the Game Manager. 
          * This method will clear out all internals of the GameManager including
          * removing dtGame::GMComponent and Actors, as well as sending any queued up
          * messages.  This gets called automatically when a dtGame::GameEntryPoint gets
          * destroyed, but can be called manually at the appropriate time as well.
          */
         void Shutdown();

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

         void PopulateTickMessage(TickMessage& tickMessage,
               double deltaSimTime, double deltaRealTime, double simulationTime);
         void DoSendNetworkMessages();
         void DoSendMessages();
         void DoSendMessageToComponents(const Message& message);
         void InvokeGlobalInvokables(const Message& message);
         void InvokeForActorInvokables(const Message& message, GameActorProxy& aboutActor);
         void InvokeOtherActorInvokables(const Message& message);
         void RemoveDeletedActors();

      private:
         class LogDebugInformation : public osg::Referenced
         {
            public:

               LogDebugInformation(const std::string &name, 
                                   const dtCore::UniqueId &uniqueID, 
                                   bool isComponent) : 
                  mTotalTime(0.0f),
                  mTickLocalTime(0.0f),
                  mTimesThrough(1), 
                  mNameOfLogInfo(name), 
                  mUniqueID(uniqueID), 
                  mIsComponent(isComponent)
               {
       
               }

               float          mTotalTime;
               float          mTickLocalTime;
               unsigned int   mTimesThrough;
               std::string    mNameOfLogInfo;
               dtCore::UniqueId  mUniqueID;
               bool           mIsComponent;

            protected:

               virtual ~LogDebugInformation() { }
         };

         std::set<TimerInfo>& GetSimulationTimerList() { return mSimulationTimers; }

         /// Does the work of ClearTimer for each of the timer info sets.
         void ClearTimerSingleSet(std::set<TimerInfo>& timerSet, 
               const std::string& name, const GameActorProxy *proxy);

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
         float ComputeStatsPercent(const float total, const float partial) const;

         /**
          * Private helper method to send an environment changed message
          * @param envActor The about actor of the message
          */
         void SendEnvironmentChangedMessage(IEnvGameActorProxy *envActor);

         template <typename MapType, typename KeyType>
         void CheckForDuplicateRegistration(const KeyType& type, const std::string& typeName,
                  GameActorProxy& proxy, const std::string& invokableName, MapType& mapToCheck);
      
         dtCore::RefPtr<MachineInfo>            mMachineInfo;
         dtCore::RefPtr<IEnvGameActorProxy>  mEnvironment;

         std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> > mGameActorProxyMap;
         std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> > mActorProxyMap;
         std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> > mPrototypeActors;

         std::vector<dtCore::RefPtr<GameActorProxy> > mDeleteList;
         //These are used during changing the map so that 
         //the map code can modify game manager with some control.
         bool mSendCreatesAndDeletes;
         bool mAddActorsToScene;

         std::set<TimerInfo> mSimulationTimers, mRealTimeTimers;

         typedef std::pair<dtCore::RefPtr<GameActorProxy>, std::string> ProxyInvokablePair;
         typedef std::multimap<const MessageType*, ProxyInvokablePair > GlobalMessageListenerMap;
         GlobalMessageListenerMap mGlobalMessageListeners;

         typedef std::multimap<dtCore::UniqueId, ProxyInvokablePair > ProxyInvokableMap;
         typedef std::map<const MessageType*,  ProxyInvokableMap> ActorMessageListenerMap;
         ActorMessageListenerMap mActorMessageListeners;

         std::vector<dtCore::RefPtr<GMComponent> > mComponentList; 
         std::queue<dtCore::RefPtr<const Message> > mSendNetworkMessageQueue;
         std::queue<dtCore::RefPtr<const Message> > mSendMessageQueue;
         dtCore::RefPtr<dtCore::Scene> mScene;
         dtCore::RefPtr<dtDAL::LibraryManager> mLibMgr;
         MessageFactory mFactory;
         
         bool mPaused;
         NameVector mLoadedMaps;
         dtCore::RefPtr<MapChangeStateData> mMapChangeStateData;

         ////////////////////////////////////////////////
         // statistics data
         dtCore::Timer        mStatsTickClock;
         dtCore::Timer_t      mStatsLastFragmentDump;
         long                 mStatsNumProcMessages;
         long                 mStatsNumSendNetworkMessages;
         long                 mStatsNumFrames;
         dtCore::Timer_t      mStatsCumGMProcessTime;
         int                  mStatisticsInterval;                                  ///< how often we print the information out.
         std::string          mFilePathToPrintDebugInformation;                     ///< where the file is located at that we print out to
         bool                 mPrintFileToConsole;                                  ///< if the information goes to console or file
         bool                 mDoStatsOnTheComponents;                              ///< do we fill in the information for the components.
         bool                 mDoStatsOnTheActors;                                  ///< Do we fill in information for the actors
         //std::vector<dtCore::RefPtr<LogDebugInformation> > mDebugLoggerInformation; ///< hold onto all the information.
         std::map<dtCore::UniqueId, dtCore::RefPtr<LogDebugInformation> > mDebugLoggerInformation; ///< hold onto all the information.
         ////////////////////////////////////////////////

         /// application the gm has. the one and only.
         dtABC::Application* mApplication;
         
         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         GameManager(const GameManager&);
         GameManager& operator=(const GameManager&);
         
         bool mRemoveGameEventsOnMapChange;
   };
}

#endif
