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
 * David Guthrie, Curtiss Murphy, Matthew W. Campbell, William E. Johnson II,
 */

#ifndef DELTA_GAMEMANANGER
#define DELTA_GAMEMANANGER

#include <set>
#include <map>

#include <dtGame/export.h>
#include <dtGame/gameactorproxy.h> //for RefPtr
#include <dtCore/refptr.h>
#include <dtCore/base.h>
#include <dtCore/timer.h>
#include <dtUtil/enumeration.h> //for ComponentPriority
#include <dtGame/exceptionenum.h>

namespace dtUtil
{
   class Log;
   class ConfigProperties;
}

namespace dtCore
{
   class DeltaDrawable;
   class Scene;
}

// Forward declarations
namespace dtCore
{
   class BaseActorObject;
   class ActorType;
   class ActorPluginRegistry;
   class ActorFactory;
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
   class GMStatistics;
   class GMImpl;
   class GMSettings;
   class MachineInfo;
   class Message;
   class MessageFactory;
   class MessageType;
   class IEnvGameActorProxy;

   class DT_GAME_EXPORT GameManager : public dtCore::Base
   {
      DECLARE_MANAGEMENT_LAYER(GameManager)

      friend class GMStatistics;

   public:
      static const std::string CONFIG_STATISTICS_INTERVAL;
      static const std::string CONFIG_STATISTICS_TO_CONSOLE;
      static const std::string CONFIG_STATISTICS_OUTPUT_FILE;

      typedef std::vector<std::string> NameVector;

      class DT_GAME_EXPORT ComponentPriority : public dtUtil::Enumeration
      {
         DECLARE_ENUM(ComponentPriority);
      public:
         ///Highest possible priority.  Components with this priority will get messages first.
         static ComponentPriority HIGHEST;

         ///Higher priority.  Components with this priority will get messages after HIGHEST, but before any others.
         static ComponentPriority HIGHER;

         ///Normal priority.  Components with this priority will get messages after any HIGHER priority, but before LOWER.
         static ComponentPriority NORMAL;

         ///Lower priority.  Components with this priority will get messages after any NORMAL or HIGHER priority, but before LOWEST.
         static ComponentPriority LOWER;

         ///Lowest priority.  Components with this priority will get messages after all others.
         static ComponentPriority LOWEST;

         ///@return the order id.  The higher the priority, the lower the number.
         unsigned int GetOrderId() const;

      protected:
         ComponentPriority(const std::string& name, unsigned int orderId);

         unsigned int mOrderId;
      };

      /// Constructor
      GameManager(dtCore::Scene& scene);

   protected:
      /// Destructor
      virtual ~GameManager();

   public:
      /**
       * Loads an actor registry
       * @param libName the name of the library to load
       */
      void LoadActorRegistry(const std::string& libName);

      /**
       * Unloads an actor registry
       * @param libName the name of the library to unload
       */
      void UnloadActorRegistry(const std::string& libName);

      /**
       * Returns a list of all the actor types the library manager knows how
       * to create.
       * @param actorTypes A vector to fill
       */
      void GetActorTypes(dtCore::ActorTypeVec& actorTypes);

      /**
       * Gets a single actor type that matches the name and category specified.
       * @param category Category of the actor type to find.
       * @param name Name of the actor type.
       * @return A pointer to the actor type if the actor type was found or NULL otherwise.
       */
      const dtCore::ActorType* FindActorType(const std::string& category, const std::string& name);

      /**
       * Fills a vector with the game proxys whose types match the name parameter
       * @param The name to search for
       * @param The vector to fill
       */
      void FindPrototypesByActorType(const dtCore::ActorType& type, dtCore::ActorPtrVector& toFill) const;

      /**
       * Fills a vector with the game proxys whose names match the name parameter
       * @param The name to search for
       * @param The vector to fill
       */
      void FindPrototypesByName(const std::string& name, dtCore::ActorPtrVector& toFill) const;

      /**
       * Convenience method to return a single prototype
       * @param The name to search for
       * @param The proxy to cast
       */
      template <class ProxyType>
      void FindPrototypeByName(const std::string& name, ProxyType*& proxy) const
      {
         dtCore::ActorPtrVector toFill;
         FindPrototypesByName(name, toFill);
         if (!toFill.empty())
         {
            proxy = dynamic_cast<ProxyType*>(toFill[0]);
         }
      }

      /**
       * Fills a vector with all the templates.
       * @param The vector to fill
       */
      void GetAllPrototypes(dtCore::ActorPtrVector& toFill) const;

      /**
       * @param The uniqueID to look for or NULL for error
       * @return the actor proxy with that ID
       */
      dtCore::BaseActorObject* FindPrototypeByID(const dtCore::UniqueId& uniqueID);

      /**
       * @param The uniqueID to look for or NULL for error
       * @return the actor proxy with that ID
       */
      template <typename T>
      void FindPrototypeByID(const dtCore::UniqueId& uniqueID, dtCore::RefPtr<T>& proxy)
      {
         dtCore::BaseActorObject* tempProxy = FindPrototypeByID(uniqueID);
         proxy = dynamic_cast<T*>(tempProxy);
      }

      /// Clears the mPrototypeActors map.
      void DeleteAllPrototypes();

      /// Deletes a single Template.
      void DeletePrototype(const dtCore::UniqueId& uniqueId);

      /**
       * Makes a new GameActorProxy, and returns it to the user
       * @param uniqueID The unique id of the prototype to create an actor from.
       * @param isRemote Whether the new actor should be marked as remote or not.
       */
      dtCore::RefPtr<dtCore::BaseActorObject> CreateActorFromPrototype(const dtCore::UniqueId& uniqueID, bool isRemote = false);

      template <typename T>
      void CreateActorFromPrototype(const dtCore::UniqueId& uniqueID,
               dtCore::RefPtr<T>& proxy)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> baseProxy = CreateActorFromPrototype(uniqueID);
         proxy = dynamic_cast<T*>(baseProxy.get());
      }

      /**
       * Create actors from a prefab.  It will use the map loaded in the GM or try to fake something.
       */
      void CreateActorsFromPrefab(const dtCore::ResourceDescriptor&, dtCore::ActorRefPtrVector& actorsOut, bool isRemote = false);

      /**
       * Wraps up several methods used to lookup and create actors from prototypes.
       * It attempts to create a new actor from a prototype by using the name.  Assumes only 1 match.
       * @param prototypeName The unique name to look for.
       * @param proxy Where the new actor proxy will go (with correct type) - ex dtCore::RefPtr<dtCore::BaseActorObject> newProxy
       */
      template <typename T>
      void CreateActorFromPrototype(const std::string& prototypeName, dtCore::RefPtr<T>& proxy)
      {
         dtCore::BaseActorObject* prototypeProxy = NULL;
         FindPrototypeByName(prototypeName, prototypeProxy);
         if (prototypeProxy != NULL)
         {
            dtCore::RefPtr<dtCore::BaseActorObject> newBaseActorObject =
               CreateActorFromPrototype(prototypeProxy->GetId());
            proxy = dynamic_cast<T*>(newBaseActorObject.get());
         }
      }

      /**
       * Gets a registry currently loaded by the library manager.
       * @param name The name of the library.  Note, this name is the
       * system independent name.
       * @return A handle to the registry or NULL if it is not currently loaded.
       */
      dtCore::ActorPluginRegistry* GetRegistry(const std::string& name);

      /**
       * @param actorType the actor type to get the registry for.
       */
      dtCore::ActorPluginRegistry* GetRegistryForType(dtCore::ActorType& actorType);

      /**
       * Determines which platform we are running on and returns a
       * platform dependent library name.
       * @param libBase Platform independent library name.
       * @return A platform dependent library name.
       * @note
       *  For example.  If the platform independent library name is
       *  ExampleActors then on Windows platforms in release mode, the resulting dependent
       *  library name would be ExampleActors.dll, however, on Unix-based
       *  platforms, the resulting name could be libExampleActors.so.
       */
      std::string GetPlatformSpecificLibraryName(const std::string& libBase);

      /**
       * Strips off the path and platform specific library prefixes and extensions
       * and returns a system independent file name.
       * @param libName The platform specific library name.
       * @return A platform independent library name.
       */
      std::string GetPlatformIndependentLibraryName(const std::string& libName);

      /**
       * Called by the dtCore::Base class
       * @param The data from the message
       * @see dtCore::Base
       */
      virtual void OnMessage(MessageData* data);

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
      void AddComponent(GMComponent& component, ComponentPriority& priority = ComponentPriority::NORMAL);

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
      GMComponent* GetComponentByName(const std::string& name);

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
      void GetComponentByName(const std::string& name, ComponentType*& component)
      {
         component = dynamic_cast<ComponentType*>(GetComponentByName(name));
      }

      /**
       * Returns a const component of the requested name or NULL if none exists
       * @return A pointer to the requested component, or NULL
       */
      const GMComponent* GetComponentByName(const std::string& name) const;

      /**
       * Const version of the other templated GetComponentByName method
       */
      template <typename ComponentType>
      void GetComponentByName(const std::string& name, const ComponentType*& component) const
      {
         component = dynamic_cast<const ComponentType*>(GetComponentByName(name));
      }

      /**
       * Sets an environment actor on the game manager
       * @param envActor The environment actor to set
       */
      void SetEnvironmentActor(IEnvGameActorProxy* envActor);

      /**
       * Gets the environment actor on the game manager
       * @return mEnvProxy or NULL if no environment actor has been set
       */
      IEnvGameActorProxy* GetEnvironmentActor();

      /**
       * Creates a game actor based on the actor type but sets the isRemote status to true (== remote).
       * @param The actor type to create.
       * @throws dtCore::ExceptionEnum::ObjectFactoryUnknownType
       * @throws dtGame::ExceptionEnum::INVALID_PARAMETER if actortype is NOT a game actor
       */
      dtCore::RefPtr<dtGame::GameActorProxy> CreateRemoteActor(const dtCore::ActorType& actorType);
      DEPRECATE_FUNC dtCore::RefPtr<dtGame::GameActorProxy> CreateRemoteGameActor(const dtCore::ActorType& actorType) { return CreateRemoteActor(actorType); }


      /**
       * Creates an actor marked as remote based on the actor type and stores it in a ref pointer.
       * This method is templated so that the caller can create a ref pointer to the actual type of the actor,
       * not BaseActorObject.  This is very handy with GameActorProxies since it is typical that uses will create those most often.
       * @param The actor type to create.
       * @param proxy a RefPtr to fill with the created actor.  If the actor is not type specified, the RefPtr will be NULL.
       * @throws dtCore::ExceptionEnum::ObjectFactoryUnknownType
       * @throws dtGame::ExceptionEnum::INVALID_PARAMETER if actortype is NOT a game actor
       */
      template <typename ActorT>
      void CreateRemoteActor(const dtCore::ActorType& actorType, dtCore::RefPtr<ActorT>& actor)
      {
         CreateActor(actorType, actor);

         if (actor.valid())
         {
            actor->SetRemote(true);
         }
         else
         {
            throw dtGame::InvalidParameterException( "The actor type \""
               + actorType.GetFullName() + "\" is invalid because it doesn't exist or is not a game actor type."
               , __FILE__, __LINE__);
         }
      }

      /**
       * Creates an actor based on the actor type.
       * @param The actor type to create.
       * @throws dtCore::ExceptionEnum::ObjectFactoryUnknownType
       */
      dtCore::ActorPtr CreateActor(const dtCore::ActorType& actorType);

      /**
       * Creates an actor based on the actor type and stores it in a ref pointer.
       * This method is templated so that the caller can create a ref pointer to the actual type of the actor,
       * not BaseActorObject.  This is very handy with GameActorProxies since it is typical that uses will create those most often.
       * @param The actor type to create.
       * @param proxy a RefPtr to fill with the created actor.  If the actor is not type specified, the RefPtr will be NULL.
       * @throws dtCore::ExceptionEnum::ObjectFactoryUnknownType
       */
      template <typename ActorT>
      void CreateActor(const dtCore::ActorType& actorType, dtCore::RefPtr<ActorT>& actorOut)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> tmpActorPtr = CreateActor(actorType);
         actorOut = dynamic_cast<ActorT*>(tmpActorPtr.get());
      }

      /**
       * Creates an actor based on the actor type
       * @param category The category corresponding to the actor type
       * @param name The name corresponding to the actor type
       * @throws dtCore::ExceptionEnum::ObjectFactoryUnknownType
       */
      dtCore::ActorPtr CreateActor(const std::string& category, const std::string& name);

      /**
       * Creates an actor based on the string version of the actor type and store it in a ref pointer.
       * This method is templated so that the caller can create a ref pointer to the actual type of the proxy,
       * not BaseActorObject.  This is very handy with GameActorProxies since it is typical that uses will create those most often.
       * @param The actor type to create.
       * @param proxy a RefPtr to fill with the created actor.  If the actor is not type specified, the RefPtr will be NULL.
       * @throws dtCore::ExceptionEnum::ObjectFactoryUnknownType
       */
      template <typename ProxyType>
      void CreateActor(const std::string& category, const std::string& name, dtCore::RefPtr<ProxyType>& proxy)
      {
         dtCore::ActorPtr tmpProxy = CreateActor(category, name);
         proxy = dynamic_cast<ProxyType*>(tmpProxy.get());
      }

      /**
       * Game Actors added during a batch will not have OnEnteredWorld called immediately.  All will be called at the
       * end.  If an actor tries to access one of the actors in the batch via find, or another create, the latter will be initialized before being returned
       * unless there is a dependency loop such as A reference B which reference A.  In that case, A will not be in mid-init.
       * @see ScopedGMBatchAdd (at the bottom the GM header)
       */
      void BeginBatchAdd();

      /// @see #BeginBatchAdd
      void CompleteBatchAdd();

      /**
       * Adds an actor to the list of actors that the game manager knows about
       * @param actorProxy  The actor proxy to add
       */
      void AddActor(dtCore::BaseActorObject& actorProxy);

      /**
       * Adds a game actor and all its tree children to the list of actors that the game manager knows about
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
       * Changes an actor to local by calling OnRemovedFromWorld, changing the actor setting to local,
       * and then calling OnEnteredWorld.  The actor must be able to cleanup and reset, and any ramifications are up to the application.
       * The actor is reset if it's already local
       */
      void SwitchActorToLocal(GameActorProxy& gameActorProxy, bool publish = false);

      /**
       * Changes an actor to local by calling OnRemovedFromWorld, changing the actor setting to remote,
       * and then calling OnEnteredWorld.  The actor must be able to cleanup and reset, and any ramifications are up to the application.
       * The actor is reset if it's already remote
       */
      void SwitchActorToRemote(GameActorProxy& gameActorProxy);

      /**
       * Removes all game actors and actors from the game manager
       * Currently all actors are removed immediately, but this should not be
       * assumed to be true going forward.
       * INFO_ACTOR_DELETE messages are sent for all actors.
       */
      void DeleteAllActors() { DeleteAllActors(false); }

      /**
       * Removes an actor or game actor from the game manager.
       * Game actors are not actually removed until the end of the current frame so that
       * messages can propagate.  Regular actor proxies are removed immediately.
       * An INFO_ACTOR_DELETE message is only sent if it's a game actor and is local.
       * @param The actor to remove
       */
      void DeleteActor(dtCore::BaseActorObject& actorProxy);
      void DeleteActor(const dtCore::UniqueId& id);

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
      void GetUsedActorTypes(std::set<const dtCore::ActorType*>& vec) const;

      /**
       * Gets the number of game actors currently managed by this game
       * manager.
       * @return The number of game actors in the system.
       */
      size_t GetNumGameActors() const;

      /**
       * Retrieves all the game actors added to the GM
       * @param toFill The vector to fill
       */
      void GetAllGameActors(std::vector<GameActorProxy*>& toFill) const;

      /**
       * Retrieves all the non game actors added to the GM
       * @param toFill The vector to fill
       */
      void GetAllNonGameActors(dtCore::ActorPtrVector& toFill) const;

      /**
       * Retrieves all the actors added to the GM
       * @param toFill The vector to fill
       */
      void GetAllActors(dtCore::ActorPtrVector& toFill) const;

      /**
       * Get the number of all Actors currently managed.
       * @return The number of BaseActorObject and GameActorProxy currently
       * managed by this GameManager.
       */
      size_t GetNumAllActors() const;

      /**
       * Allows performing an operation on each actor, excluding prototypes, in the game manager.
       * @param func a class with an operator() that takes an actor proxy by reference (dtCore::BaseActorObject&)
       * @note you must include dtGame/gamemanager.inl to use the method.
       */
      template <typename UnaryFunctor>
      void ForEachActor(UnaryFunctor func, bool applyOnlyToGameActors = false);

      /**
       * Allows performing an operation on each prototype actor in the game manager.
       * @param func a class with an operator() that takes an actor proxy by reference (dtCore::BaseActorObject&)
       * @note you must include dtGame/gamemanager.inl to use the method.
       */
      template <typename UnaryFunctor>
      void ForEachPrototype(UnaryFunctor func) const;

      /**
       * Allows custom searching on each non-prototype actor in the game manager.
       * @param ifFunc a class with an operator() that takes an actor proxy by reference (dtCore::BaseActorObject&)
       * and returns true if you want to add it the vector.
       * @param toFill the vector to fill with the results. It will be cleared before searching.
       * @note you must include dtGame/gamemanager.inl to use the method.
       */
      template <typename FindFunctor>
      void FindActorsIf(FindFunctor ifFunc, dtCore::ActorPtrVector& toFill);

      /**
       * Allows custom searching on each prototype actor in the game manager.
       * @param ifFunc a class with an operator() that takes an actor proxy by reference (dtCore::BaseActorObject&)
       * and returns true if you want to add it the vector.
       * @param toFill the vector to fill with the results. It will be cleared before searching.
       * @note you must include dtGame/gamemanager.inl to use the method.
       */
      template <typename FindFunctor>
      void FindPrototypesIf(FindFunctor ifFunc, dtCore::ActorPtrVector& toFill) const;

      /**
       * Fills a vector with the game proxys whose names match the name parameter
       * @param The name to search for
       * @param The vector to fill
       */
      void FindActorsByName(const std::string& name, dtCore::ActorPtrVector& toFill);

      /**
       * Convenience method to return an actor
       * @param The name to search for
       * @param The proxy to cast
       */
      template <class ProxyType>
      void FindActorByName(const std::string& name, ProxyType*& proxy)
      {
         proxy = NULL;
         dtCore::ActorPtrVector toFill;
         FindActorsByName(name, toFill);
         if (!toFill.empty())
         {
            // Iterate until we find a proxy of the proper type.
            for (dtCore::ActorPtrVector::iterator i = toFill.begin();
               i != toFill.end();
               ++i)
            {
               proxy = dynamic_cast<ProxyType*>(*i);

               if (proxy != NULL)
               {
                  break;
               }
            }
         }
      }

      /**
       * Fills a vector with the game proxys whose types match the type parameter
       * @param The type to search for
       * @param The vector to fill
       */
      void FindActorsByType(const dtCore::ActorType& type, dtCore::ActorPtrVector& toFill);

      /**
       * Convenience method to return an actor
       * @param The type to search for
       * @param The proxy to cast
       */
      template <class ProxyType>
      void FindActorByType(const dtCore::ActorType& type, ProxyType*& proxy)
      {
         proxy = NULL;
         dtCore::ActorPtrVector toFill;
         FindActorsByType(type, toFill);
         if (!toFill.empty())
         {
            proxy = dynamic_cast<ProxyType*>(toFill[0]);
         }
      }

      /**
       * Fills out a vector of actors with the specified class name
       * @param className the classname
       * @param toFill The vector to fill
       */
      void FindActorsByClassName(const std::string& className, dtCore::ActorPtrVector& toFill);


      /**
       * Returns the game actor proxy whose is matches the parameter
       * @param id The id of the proxy to find
       * @return The proxy, or NULL if not found
       */
      GameActorProxy* FindGameActorById(const dtCore::UniqueId& id);

      /**
       * Getst the game actor proxy whose is matches the parameter
       * @param id The id of the proxy to find
       */
      template<typename ProxyType>
      void FindGameActorById(const dtCore::UniqueId& id, ProxyType*& proxy)
      {
         proxy = dynamic_cast<ProxyType*>(FindGameActorById(id));
      }

      /**
       * Returns the actor proxy whose is matches the parameter. This will search both the game actors and the
       * regular actor proxies.
       * @param id The id of the proxy to find
       * @return The proxy, or NULL if not found
       */
      dtCore::BaseActorObject* FindActorById(const dtCore::UniqueId& id);

      /**
       * Returns the actor proxy whose is matches the parameter. This will search both the game actors and the
       * regular actor proxies.
       * @param id The id of the proxy to find
       * @return The proxy, or NULL if not found
       */
      template<typename ProxyType>
      void FindActorById(const dtCore::UniqueId& id, ProxyType*& proxy)
      {
         proxy = dynamic_cast<ProxyType*>(FindActorById(id));
      }

      /**
       * Loads a single map, closing any currenly opened maps.  For loading multiple maps together, you should
       * call ChangeMapSet.  It also describes the sequence of messages.
       * @see #ChangeMapSet
       * @see #CloseCurrentMap()
       * @param mapName       The name of the map to load.
       * @param addBillboards optional parameter that defaults to false that says whether or not proxy billboards should be
       *                      added to the scene.  This should only be true for debugging purposes.
       * @throws ExceptionEnum::INVALID_PARAMETER if no map name is supplied.
       * @throws ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if map change is already in progress.
       * @note ChangeMap() requires numerous "frames" for the Map change to occur.  This
       * typically means dtCore::System needs to step a few times before all the Actors
       * have been loaded.
       */
      void ChangeMap(const std::string& mapName, bool addBillboards = false);


      /**
       * Changes the maps being used by the Game Manager.  All actors, Game events, and associated data will be deleted.
       * The process of changing maps takes several frames, but code should not depend on the exact sequence of steps. It should
       * instead look for the messages that are sent after each step.
       * First it will send INFO_MAP_CHANGE_BEGIN
       * If a map or maps is currently open, it will send INFO_MAP_UNLOAD_BEGIN.
       * Once that map or map set is closed, it will set INFO_MAP_UNLOADED
       * Right before it begins loading maps, it sends INFO_MAP_LOAD_BEGIN
       * When that finishes, it will send INFO_MAP_LOADED.
       * At the very end it sends INFO_MAP_CHANGED.
       *
       * Listening for INFO_MAP_CHANGED and INFO_MAP_LOADED both are equally valid for doing things
       * once the map is finished loading since unloading a map does not send the change messages.
       * Only one of each message is sent, regardless of the number of maps being loaded.
       * @see #CloseCurrentMap()
       * @see dtGame::MapChangeStateData
       * @param mapNames      The list of names of maps to load.
       * @param addBillboards optional parameter that defaults to false that says whether or not proxy billboards should be
       *                      added to the scene.  This should only be true for debugging purposes.
       * @throws ExceptionEnum::INVALID_PARAMETER if no map name is supplied.
       * @throws ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if map change is already in progress.
       * @note ChangeMapSet() requires numerous "frames" for the Map change to occur.  This
       * typically means dtCore::System needs to step a few times before all the Actors
       * have been loaded.
       */
      void ChangeMapSet(const NameVector& mapNames, bool addBillboards = false);

      /**
       * Closes the open maps, if any, being used by the Game Manager.  All actors will be deleted whether maps are closed or not.
       *
       * It will send an INFO_MAP_UNLOAD_BEGIN at the beginning and an INFO_MAP_UNLOADED message
       * if a map is actually closed.
       */
      void CloseCurrentMap();


      /// Load a set of maps.
      /**
       This method adds a set of map to what is currently loaded in the scene.
       For every added actor, and INFO_ACTOR_CREATE message is fired,
       and the end of the sequence is marked by the INFO_MAPS_OPENED message.
       The whole process is done in a single frame.
       \param: mapNames the set of map names to be loaded.
       \param: addBillboards  optional parameter that defaults to false that says whether or not proxy billboards should be
                              added to the scene.  This should only be true for debugging purposes.
       \see: ChangeMapSet
       */
      void OpenAdditionalMapSet(const NameVector& mapNames /*, bool addBillboards = false*/);

      /// Unload a set of maps.
      /**
       This method unload a set of maps from the GameManager.
       Actors belonging to the maps to be unloaded are deleted from the scene and a
       INFO_ACTOR_DELETE message is fired for each actor.
       At the end of the sequence, and INFO_MAPS_CLOSED message is fired.
       \note: The function ONLY removes actors belonging to maps currently loaded in the GameManager,
              and leaves other actors in the scene. For a complete map change, check ChangeMapSet().
       \param: mapNames the set of map names to be loaded.
       \see: ChangeMapSet
       */
      void CloseAdditionalMapSet(const NameVector& mapNames);



      /**
       * Sets a timer on the GameManager.  It will send out a TimerElapsedMessage 
       * of type MessageType::INFO_TIMER_ELAPSED when it expires.
       * @param name The name of the timer
       * @param aboutActor the actor to put in the "about" field of the message.  If this
       *    is NULL, the timer is a "global" timer in that it is not bound to any
       *    particular actor.  This is generally only useful if components need to use the
       *    timer functionality of the game manager.
       * @param time The time of the timer in seconds.
       * @param repeat True to repeat the timer every time seconds; false to 
       *               fire the timer only once.
       * @param realTime True if this time should use real time, or false if it should use simulation time.
       * @see ClearTimer()
       */
      void SetTimer(const std::string& name, const GameActorProxy* aboutActor, float time,
               bool repeat = false, bool realTime = false);

      /**
       * Removes the timer with the given name.  If no timer by that name exists, this is a no-op.
       * @param name the name of the timer to remove.
       * @param proxy The proxy this timer is associated with or NULL if the timer
       *    was registered as a "global" timer.
       * @see SetTimer()
       */
      void ClearTimer(const std::string& name, const GameActorProxy* proxy);

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
      void SetScene(dtCore::Scene& newScene);

      ///@return the configuration properties.
      dtUtil::ConfigProperties& GetConfiguration();

      ///@return the configuration properties.
      const dtUtil::ConfigProperties& GetConfiguration() const;

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
      int GetStatisticsInterval() const;

      /**
       * @return true if the Debug Statistics are set to log to the console.
       */
      bool GetStatisticsToConsole() const;

      /**
       * @return the log file for the Debug Statistics or empty if it is set
       *         to log to the console or statistics are turned off.
       */
      const std::string& GetStatisticsLogFilePath() const;

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
      bool GetRemoveGameEventsOnMapChange() const;

      /**no
       * Sets the flag for whether we will remove the Game Events when we change a map or not.
       * Normally, when a map is closed, the Game Manager removes the events that came from the
       * map. This flag allows the case where sometimes an event comes from a map but you don't want
       * it to go away.  The default is true.
       * @param removeGameEventsOnMapChange Flag for whether we remove a map's GameEvents from
       * the GameEventManager when closing a map. (default is true)
       */
      void SetRemoveGameEventsOnMapChange(const bool removeGameEventsOnMapChange);

      /**
       * Retrieves the message factor that is controlled by the GameManager
       * @return mFactory he message factory
       * @see class dtGame::MessageFactory
       */
      MessageFactory& GetMessageFactory();

      /**
       * Retrieves the message factor that is controlled by the GameManager
       * @return mFactory he message factory
       * @see class dtGame::MessageFactory
       */
      const MessageFactory& GetMessageFactory() const;

      /**
       * Gets the const version of the machine info
       * @return mMachineInfo
       */
      const MachineInfo& GetMachineInfo() const;

      /**
       * Non const version to get the machine info
       * @return mMachineInfo
       */
      MachineInfo& GetMachineInfo();

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
       * @return the current simulation time. This is in SECONDs.
       * @see dtCore::System#GetSimulationTime
       */
      double GetSimulationTime() const;

      /**
       * @return the current simulation time. This is in SECONDS.
       * @see dtCore::System#GetSimTimeSinceStartup
       */
      double GetSimTimeSinceStartup() const;

      /**
       * @return the current simulation wall-clock time. This is in MICRO SECONDS (seconds * 1000000LL).
       * @see dtCore::System#GetSimulationClockTime
       */
      dtCore::Timer_t GetSimulationClockTime() const;

      /**
       * @return The current real clock time. This is in MICRO SECONDS (seconds * 1000000LL).
       * @see dtCore::System#GetRealClockTime
       */
      dtCore::Timer_t GetRealClockTime() const;

      /**
       * Change the time settings.
       * @param newTime The new simulation time. In SECONDS.
       * @param newTimeScale the new simulation time progression as a factor of real time.
       * @param newClockTime  The new simulation wall-clock time. In MICRO SECONDs (seconds * 1000000LL).
       */
      void ChangeTimeSettings(double newTime, float newTimeScale, const dtCore::Timer_t& newClockTime);

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
               std::vector< std::pair<GameActorProxy*, std::string> >& toFill) const;

      /**
       * @param type
       * @param proxy
       * @param invokableName
       */
      void RegisterForMessages(const MessageType& type, GameActorProxy& actor, const std::string& invokableName);

      /**
       * @param type
       * @param proxy
       * @param invokableName
       */
      void UnregisterForMessages(const MessageType& type, GameActorProxy& actor, const std::string& invokableName);

      /**
       * @param type
       * @param targetActorId
       * @param proxy
       * @param invokableName
       */
      void RegisterForMessagesAboutActor(
               const MessageType& type,
               const dtCore::UniqueId& targetActorId,
               GameActorProxy& actor,
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
               GameActorProxy& actor,
               const std::string& invokableName);

      /**
       * @param proxy
       */
      void UnregisterAllMessageListenersForActor(GameActorProxy& actor);

      /**
       * @return true if the GameManager is paused
       */
      bool IsPaused() const;

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
      void RejectMessage(const Message& reasonMessage, const std::string& rejectDescription);

      /**
       * Wrapper method to encapsulate SetContext on dtCore::Project so an outside
       * user does not have to know about dtCore::Project at all when they are writing
       * an application using GameStart
       * @param context A path to the context to use
       * @param readOnly True to open the context in read only mode
       */
      void SetProjectContext(const std::string& context, bool readOnly = false);

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

      /**
       * @return true if the GM in the process of shutting down.
       */
      bool IsShuttingDown() const;

      ///@return the GMSettings class. You can change this directly.
      GMSettings& GetGMSettings();

      ///overwrites the gmsettings instance on the GM. Allows you to make your own settings subclasses.
      void SetGMSettings(GMSettings& newSettings);

   protected:

      /**
       * Implements the functionality that will happen on the PostEventTraversal event
       * @param deltaSimTime the change in simulation time since the last frame.
       * @param deltaRealTime the change in real time since the last frame.
       */
      virtual void PostEventTraversal(double deltaSimTime, double deltaRealTime);

      /**
       * Implements the functionality that will happen on the PostEventTraversal event
       * @param deltaSimTime the change in simulation time since the last frame.
       * @param deltaRealTime the change in real time since the last frame.
       */
      virtual void FrameSynch(double deltaSimTime, double deltaRealTime);

      /**
       * Implements the functionality that will happen on the PreFrame event
       * @param deltaSimTime the change in simulation time since the last frame.
       * @param deltaRealTime the change in real time since the last frame.
       */
      virtual void PreFrame(double deltaSimTime, double deltaRealTime);

      /// Implements the functionality that will happen on the PostFrame event
      virtual void PostFrame(double deltaSimTime, double deltaRealTime);

      void PopulateTickMessage(TickMessage& tickMessage,
               double deltaSimTime, double deltaRealTime, double simulationTime);

      /// Sends network messages to components until the queue is empty.
      void DoSendNetworkMessages();
      /// Sends messages until the queue is empty.
      void DoSendMessages();
      /// Sends a single message to components and actors.
      void DoSendMessage(const Message& message);
      /// Sends a single message just to components.
      void DoSendMessageToComponents(const Message& message, bool toNetwork);
      void InvokeGlobalInvokables(const Message& message);
      void InvokeForActorInvokables(const Message& message, GameActorProxy& aboutActor);
      void InvokeOtherActorInvokables(const Message& message);

      /** Removes all actors from the list of deleted actors and returns true if no actors were deleted by other actors.
       * That is, if an actor deletes another actor, messages will be left sitting in the queue, and these messages 
       * must be processed before the actors can be removed from the deleted list, otherwise some things in the system
       * won't work correctly.
       */ 
      bool RemoveDeletedActors();

      /**
       * SwitchActorToLocal and SwitchActorToRemote call this so the code can be shared.
       */
      void SwitchActorToLocalOrRemote(GameActorProxy& gameActorProxy, bool local, bool publish);

   private:
      GMImpl* mGMImpl; // Pimple pattern for private data

      // -----------------------------------------------------------------------
      //  Unimplemented constructors and operators
      // -----------------------------------------------------------------------
      GameManager(const GameManager&);
      GameManager& operator=(const GameManager&);
   };

   /// Use this class to do a batch add in a function so it will always close the transaction.
   class ScopedGMBatchAdd
   {
   public:
      ScopedGMBatchAdd(GameManager& gm)
      : mGM(gm)
      {
         mGM.BeginBatchAdd();
      }
      ~ScopedGMBatchAdd()
      {
         mGM.CompleteBatchAdd();
      }
   private:
      GameManager& mGM;
   };

} // namespace dtGame

#endif // DELTA_GAMEMANANGER
