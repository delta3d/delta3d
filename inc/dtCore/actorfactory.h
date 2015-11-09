/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 * Copyright (C) 2015, Caper Holdings, LLC
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
 * Matthew W. Campbell and David Guthrie
 */

#ifndef DELTA_LIBRARY_MANAGER
#define DELTA_LIBRARY_MANAGER

#include <map>
#include <dtCore/refptr.h>
#include <dtUtil/librarysharingmanager.h>
#include <osg/Referenced>
#include <dtCore/actorpluginregistry.h>
#include <dtCore/export.h>

namespace dtCore
{

   /**
    * This class is the main class in the dynamic actor library portion of the
    * DAL (Dynamic Actor Layer).  It has a list of the dynamic libraries that
    * are registered with the Manager as well as a list of the ActorTypes
    * that each library can create.  It is also the main vehicle for
    * creating a new BaseActorObject.
    * @note The ActorFactory follows the Singleton design pattern.
    * @see ActorType
    * @see BaseActorObject
    * @see ActorPluginRegistry
    */
   class DT_CORE_EXPORT ActorFactory : public osg::Referenced
   {
      private:
         /**
          * Function pointer to the create function implemented in the ActorPlugin.
          */
         typedef ActorPluginRegistry* (*CreatePluginRegistryFn)();

         /**
          * Function pointer to the destroy function implemented in the ActorPlugin.
          * This is called just before closing the dynamic library owning the
          * ActorPluginRegistry.
          */
         typedef void (*DestroyPluginRegistryFun)(ActorPluginRegistry*);

      public:

         static const std::string DEFAULT_ACTOR_LIBRARY;

         /**
          * Simple structure for grouping the data corresponding to a
          * registry entry.
          */
         struct DT_CORE_EXPORT RegistryEntry
         {
            RegistryEntry();
            ActorPluginRegistry* registry;
            dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> lib;
            CreatePluginRegistryFn createFn;
            DestroyPluginRegistryFun destroyFn;
         };


         typedef std::map<dtCore::RefPtr<const ActorType>, ActorPluginRegistry*, ActorType::RefPtrComp> ActorTypeMap;
         typedef ActorTypeMap::iterator ActorTypeMapItor;
         typedef ActorTypeMap::const_iterator ConstActorTypeMapItor;

         typedef std::map<std::string, RegistryEntry> RegistryMap;
         typedef RegistryMap::iterator RegistryMapItor;
         typedef RegistryMap::const_iterator RegistryMapConstItor;
         typedef dtCore::ActorTypeVec ActorTypeList;

         /**
          * Gets the singleton instance of the ActorFactory.
          */
         static ActorFactory& GetInstance();

         /**
          * Destroy the singleton instance of ActorFactory, if one was created.
          * Should only be called when the application is finished with it.
          */
         static void Destroy();

         /**
          * Loads an actor registry by loading a dynamic library
          * containing the actor registry implementation.
          * @note The library file must contain two exported "C" methods:
          * "CreatePluginRegistry" and "DestroyPluginRegistry".  See ActorPluginRegistry
          * for more information.
          *
          * @param libName The system independent name of the library to load.
          * @throws ProjectResourceError Throws an exception if the
          *         library cannot be found or the create and destroy
          *         functions are not found in the library.
          */
         void LoadActorRegistry(const std::string& libName);

         ///Is the supplied library name already in the registry?
         bool IsInRegistry(const std::string& libName) const;

         ///If the supplied library exists, then try to load it.
         void LoadOptionalActorRegistry(const std::string& libName);

         /**
           * Inserts the pair of parameters into the container.
           * Also, registers the supported Actor types with the manager.
           *
           * @param libName The system independent name of the library that was loaded
           * @param entry The RegistryEntry mapped to the library name.
           * @return true if successully added, false if not
           */
         bool AddRegistryEntry(const std::string& libName, const RegistryEntry& entry);

         /**
          * Unloads an actor registry.  This unloads the dynamic library
          * containing the actor registry implementation and also removes
          * all the actor types that the registry supported.
          *
          * @param libName The system independent name of the library to load.
          */
         void UnloadActorRegistry(const std::string& libName);

         /**
          * Returns a list of all the actor types the library manager knows how
          * to create.
          */
         void GetActorTypes(ActorTypeList& actorTypes) const;

         /**
          * Returns a list of all the class types the library manager knows how
          * to create by value so that it can be used with property functors.
          */
         std::vector<std::string> GetClassTypes() const;

         /**
          * Gets a single actor type that matches the name and category specified.
          *
          * @param category Category of the actor type to find.
          * @param name Name of the actor type.
          * @return A pointer if the actor type was found or NULL if not.
          */
         const ActorType* FindActorType(const std::string& category,
                                        const std::string& name) const;

         /**
          * Try to lookup the actual actor type a replacement references.  This may not work if the library of the replace
          * is different.
          */
         const ActorType* FindActorTypeReplacement(const std::string& category, const std::string& name) const;

         /**
          *  Given the supplied ActorType full name, see if an ActorType
          *  replacement has been registered.
          * @param fullName : The fullName of an older, deprecated ActorType
          * @return The full ActorType name to use instead (or empty, if nothing is registered)
          */
         std::string FindActorTypeReplacementName(const std::string& fullName) const;

         /**
          * Creates a new actor object.  The actor type is used by the library
          * manager to determine which type of actor proxy to create.
          * @return Returns a pointer to the base actor proxy which can be
          * safely typecast'd to the appropriate derived type.
          * @throws Throws a ObjectFactoryUnknownType exception if the type
          * is unknown.
          */
         dtCore::RefPtr<BaseActorObject> CreateActor(const ActorType& actorType);

         /**
          * Creates a new actor object.  The actor type is used by the library
          * manager to determine which type of actor proxy to create.
          * @return Returns a pointer to the base actor proxy which can be
          * safely typecast'd to the appropriate derived type.
          * @throws Throws a ObjectFactoryUnknownType exception if the type
          * @param category The category corresponding to the actor type
          * @param name The name corresponding to the actor type
          * is unknown.
          */
         dtCore::RefPtr<BaseActorObject> CreateActor(const std::string& category, const std::string& name);

         /**
          * Gets a registry currently loaded by the library manager.
          * @param name The name of the library.  Note, this name is the
          * system independent name.
          * @return A handle to the registry or NULL if it is not currently loaded.
          */
         ActorPluginRegistry* GetRegistry(const std::string &name);

         /**
          * @param actorType the actor type to get the registry for.
          */
         ActorPluginRegistry* GetRegistryForType(const ActorType& actorType);

         /**
         * Gets the name of a library given the registry.
         * @param registry The registry.
         * @return The name of the actor library.
         */
         std::string GetLibraryNameForRegistry(const ActorPluginRegistry& registry) const;

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
         std::string GetPlatformSpecificLibraryName(const std::string& libBase);

         /**
          * Strips off the path and platform specific library prefixs and extensions
          * and returns a system independent file name.
          * @param libName The platform specific library name.
          * @return A platform independent library name.
          */
         std::string GetPlatformIndependentLibraryName(const std::string& libName);

      private:

         /**
          * Constructs the library manager.  The constructor will also load
          * the engine objects since this is provided by the Delta3D engine.
          */
         ActorFactory();

         /**
          * Free resources used by the library manager.
          */
         virtual ~ActorFactory();


         ///Singleton instance of this class.
         static dtCore::RefPtr<ActorFactory> mInstance;

         ///Maps an actor type to the registry that created it last.
         mutable ActorTypeMap mActorTypeCache;

         ActorPluginRegistry::ActorTypeReplacements mReplacementActors;

         ///List of the currently loaded actor registries.
         RegistryMap mRegistries;

         dtUtil::Log* mLogger;
   };

   template<typename RegistryClass>
   class AutoLibraryRegister
   {
   public:
      AutoLibraryRegister(const std::string& regName)
      : mRegistry(NULL)
      , mRegName(regName)
      {
         ActorFactory::RegistryEntry entry;
         mRegistry = new RegistryClass;
         entry.registry = mRegistry;
         ActorFactory::GetInstance().AddRegistryEntry(mRegName, entry);
      }

      ~AutoLibraryRegister()
      {
         ActorFactory& lm = ActorFactory::GetInstance();
         if (lm.GetRegistry(mRegName) == mRegistry)
         {
            lm.UnloadActorRegistry(mRegName);
         }
         delete mRegistry;
      }
   private:
      RegistryClass* mRegistry;
      const std::string mRegName;
   };
}

#endif
