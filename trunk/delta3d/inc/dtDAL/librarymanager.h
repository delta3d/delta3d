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
 * @author Matthew W. Campbell and David Guthrie
 */
#ifndef DELTA_LIBRARY_MANAGER
#define DELTA_LIBRARY_MANAGER

#include <map>
#include <dtCore/refptr.h>
#include <dtUtil/librarysharingmanager.h>
#include <osg/Referenced>
#include "dtDAL/actorpluginregistry.h"
#include "dtDAL/export.h"

namespace dtDAL 
{
	
   /**
    * This class is the main class in the dynamic actor library portion of the
    * DAL (Dynamic Actor Layer).  It has a list of the dynamic libraries that
    * are registered with the Manager as well as a list of the ActorTypes
    * that each library can create.  It is also the main vehicle for
    * creating a new ActorProxy.
    * @note The LibraryManager follows the Singleton design pattern.
    * @see ActorType
    * @see ActorProxy
    */
   class DT_DAL_EXPORT LibraryManager : public osg::Referenced 
   {
      private:
      /**
       * Function pointer to the create function implemented in the ActorPlugin.
       */
      typedef ActorPluginRegistry *(*CreatePluginRegistryFn)();

      /**
       * Function pointer to the destroy function implemented in the ActorPlugin.
       * This is called just before closing the dynamic library owning the
       * ActorPluginRegistry.
       */
      typedef void (*DestroyPluginRegistryFun)(ActorPluginRegistry*);

      /**
       * Simple structure for grouping the data corresponding to a
       * registry entry.
       */
      struct RegistryEntry 
      {
         dtCore::RefPtr<ActorPluginRegistry> registry;
         dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> lib;
         CreatePluginRegistryFn createFn;
         DestroyPluginRegistryFun destroyFn;
      };		

      public:
         typedef std::map<dtCore::RefPtr<ActorType>,
            dtCore::RefPtr<ActorPluginRegistry>, ActorType::RefPtrComp> ActorTypeMap;
         typedef ActorTypeMap::iterator ActorTypeMapItor;

         typedef std::map<std::string, RegistryEntry> RegistryMap;			
         typedef RegistryMap::iterator RegistryMapItor;

         /**
          * Gets the singleton instance of the LibraryManager.
          */
         static LibraryManager &GetInstance();

         /**
          * Loads an actor registry by loading a dynamic library 
          * containing the actor registry implementation.
          * @param libName The system independent name of the library to load.
          * @throws ProjectResourceError Throws an exception if the
          *		library cannot be found or the create and destroy
          *		functions are not found in the library.
          */
         void LoadActorRegistry(const std::string &libName);

         /**
          * Unloads an actor registry.  This unloads the dynamic library
          * containing the actor registry implementation and also removes
          * all the actor types that the registry supported.
          * @param libName The system independent name of the library to load.
          */
         void UnloadActorRegistry(const std::string &libName);            

         /**
          * Returns a list of all the actor types the library manager knows how 
          * to create.
          */
         void GetActorTypes(std::vector<dtCore::RefPtr<ActorType> > &actorTypes);
   		
         /**
          * Gets a single actor type that matches the name and category specified.
          * @param category Category of the actor type to find.
          * @param name Name of the actor type.
          * @return A valid smart pointer if the actor type was found.
          */
         dtCore::RefPtr<ActorType> FindActorType(const std::string &category,
                                                 const std::string &name);

         /**
          * Creates a new actor proxy.  The actor type is used by the library
          * manager to determine which type of actor proxy to create.
          * @return Returns a pointer to the base actor proxy which can be
          * safely typecast'd to the appropriate derived type.
          * @throws Throws a ObjectFactoryUnknownType exception if the type
          * is unknown.
          */
         dtCore::RefPtr<ActorProxy> CreateActorProxy(ActorType& actorType);

         /**
          * Gets a registry currently loaded by the library manager.  
          * @param name The name of the library.  Note, this name is the
          * system independent name.
          * @return A handle to the registry or NULL if it is not currently loaded.
          */
         ActorPluginRegistry *GetRegistry(const std::string &name);

         /**
          * @param actorType the actor type to get the registry for.
          */
         ActorPluginRegistry *GetRegistryForType(ActorType& actorType);

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

      private:

         /**
          * Constructs the library manager.  The constructor will also load
          * the engine objects since this is provided by the Delta3D engine.
          */
         LibraryManager();

         /**
          * Free resources used by the library manager.
          */
         ~LibraryManager();

         ///Singleton instance of this class.
         static dtCore::RefPtr<LibraryManager> mInstance;

         ///Maps an actor type to the registry that created it.
         ActorTypeMap mActors;

         ///List of the currently loaded actor registries.
         RegistryMap mRegistries;
   };
}

#endif
