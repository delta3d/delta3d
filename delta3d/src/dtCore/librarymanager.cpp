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
 * Matthew W. Campbell and David Guthrie
 */
#include <prefix/dtcoreprefix.h>

#include <dtCore/librarymanager.h>
#include <dtCore/actortype.h>
#include <dtCore/exceptionenum.h>

#include <dtUtil/log.h>

#include <osgDB/FileUtils>

#include <sstream>

namespace dtCore
{
   static const std::string ACTOR_LIBRARY("dtActors");
   static const std::string AUDIO_ACTOR_LIBRARY("dtAudio");
   static const std::string ANIM_ACTOR_LIBRARY("dtAnim");

   //Singleton global variable for the library manager.
   dtCore::RefPtr<LibraryManager> LibraryManager::mInstance(NULL);

   LibraryManager::RegistryEntry::RegistryEntry()
   : registry(NULL)
   , createFn(NULL)
   , destroyFn(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   LibraryManager::LibraryManager()
   {
      mLogger = &dtUtil::Log::GetInstance("librarymanager.cpp");
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, "Initializing actor library manager.");
      LoadActorRegistry(ACTOR_LIBRARY);

      //try to load some optional actor libraries that depend on optional
      //external dependencies.  If the file isn't found, don't try to load it.
      LoadOptionalActorRegistry(AUDIO_ACTOR_LIBRARY);
      LoadOptionalActorRegistry(ANIM_ACTOR_LIBRARY);
   }

   /////////////////////////////////////////////////////////////////////////////
   LibraryManager::~LibraryManager()
   {
      mActors.clear();

      //We have to manually free the registries so we ensure the memory gets
      //freed in the dynamic libraries since that's where the memory was
      //allocated.
      RegistryMapItor itor = mRegistries.begin();
      while (itor != mRegistries.end())
      {
         ActorPluginRegistry* reg = itor->second.registry;
         itor->second.destroyFn(reg);
         ++itor;
      }
      mRegistries.clear();

      //remove any registered ActorType replacements
      mReplacementActors.clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool LibraryManager::IsInRegistry(const std::string& libName) const
   {
      RegistryMap::const_iterator regItor = mRegistries.find(libName);
      if (regItor != mRegistries.end())
      {
         return true;
      }
      else
      {
         return false;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void LibraryManager::LoadActorRegistry(const std::string& libName)
   {
      //Used to format log messages.
      std::ostringstream msg;

      //To add a new registry, we first make sure its name is unique.  If
      //it is, we add it to the manager's list of currently loaded registries.
      //If it is not, we generate a unique name for it and then add it.
      if (IsInRegistry(libName))
      {
         msg.clear();
         msg.str("");
         msg << "Registry for library with name " << libName <<
            " already exists.  Library must already be loaded.";
         LOGN_ERROR("librarymanager.cpp", msg.str());
         return;
      }

      dtUtil::LibrarySharingManager& lsm = dtUtil::LibrarySharingManager::GetInstance();

      RegistryEntry newEntry;

      try
      {
         newEntry.lib = lsm.LoadSharedLibrary(libName);
      }
      catch (const dtUtil::Exception&)
      {
         msg.clear();
         msg.str("");
         msg << "Unable to load actor registry " << libName;
         throw dtCore::ProjectResourceErrorException(msg.str(), __FILE__, __LINE__);
      }

      if (IsInRegistry(libName))
      {
         msg.clear();
         msg.str("");
         msg << "Library auto-registered, no further action is needed. " << libName;

         mRegistries[libName].lib = newEntry.lib;
      }
      else
      {

         dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS createFn;
         dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS destroyFn;
         createFn = newEntry.lib->FindSymbol("CreatePluginRegistry");
         destroyFn = newEntry.lib->FindSymbol("DestroyPluginRegistry");

         //Make sure the plugin actually implemented these functions and they
         //have been exported.
         if (!createFn)
         {
            msg.clear();
            msg.str("");
            msg << "Actor plugin libraries must implement the function " <<
               " CreatePluginRegistry.";
            throw dtCore::ProjectResourceErrorException(msg.str(), __FILE__, __LINE__);
         }

         if (!destroyFn)
         {
            msg.clear();
            msg.str("");
            msg << "Actor plugin libraries must implement the function " <<
               " DestroyPluginRegistry.";
            throw dtCore::ProjectResourceErrorException(msg.str(), __FILE__, __LINE__);
         }

         //Well we made it here so that means the plugin was loaded
         //successfully and the create and destroy functions were found.
         newEntry.createFn = (CreatePluginRegistryFn)createFn;
         newEntry.destroyFn = (DestroyPluginRegistryFun)destroyFn;
         newEntry.registry = newEntry.createFn();

         if (!AddRegistryEntry(libName,newEntry))
         {
            msg.clear();
            msg.str("");
            msg << "Can't add Registry Entry: " << libName << " to Registry. " <<
               "Possibly it might have been added already.";
            throw dtCore::ProjectResourceErrorException( msg.str(), __FILE__, __LINE__);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool LibraryManager::AddRegistryEntry(const std::string& libName, const RegistryEntry& entry)
   {
      //Finally we can actually add the new registry to the library manager.
      //The map key is the system independent library name.
      bool inserted = mRegistries.insert(std::make_pair(libName,entry)).second;
      if (!inserted)
      {
         return false;
      }

      //Used to format log messages.
      std::ostringstream msg;

      //Second we map actor type to the registry that owns it.
      std::vector<dtCore::RefPtr<const ActorType> > actorTypes;
      entry.registry->RegisterActorTypes();
      entry.registry->GetSupportedActorTypes(actorTypes);

      dtCore::ActorPluginRegistry::ActorTypeReplacements replacements;
      entry.registry->GetReplacementActorTypes(replacements);
      mReplacementActors.insert(mReplacementActors.end(), replacements.begin(), replacements.end());

      int numUniqueActors = 0;
      for (unsigned int i = 0; i < actorTypes.size(); ++i)
      {
         ActorTypeMapItor itor = mActors.find(dtCore::RefPtr<const ActorType>(actorTypes[i].get()));
         if (itor != mActors.end())
         {
            msg.clear();
            msg.str("");
            msg << "Duplicate actor type " << *actorTypes[i] << " found. Will not be added.";
            LOG_ERROR(msg.str());
         }
         else
         {
            mActors.insert(std::make_pair(dtCore::RefPtr<const ActorType>(actorTypes[i].get()),entry.registry));
            ++numUniqueActors;
         }
      }

      msg.clear();
      msg.str("");
      msg << "Loaded actor plugin registry. (Name: " << libName <<
         ", Number of Actors: " << numUniqueActors << ")";
      LOG_INFO(msg.str());

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void LibraryManager::GetActorTypes(std::vector<const ActorType*>& actorTypes) const
   {
      actorTypes.clear();
      actorTypes.reserve(mActors.size());
      ConstActorTypeMapItor itor = mActors.begin();
      while (itor != mActors.end())
      {
         actorTypes.push_back(itor->first.get());
         ++itor;
      }
   }

   struct SortFunctor
   {
      bool operator()(const dtCore::ActorType* a, const dtCore::ActorType* b)
      {
         return a->GetFullName() < b->GetFullName();
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> LibraryManager::GetClassTypes() const
   {
      std::vector<const dtCore::ActorType*> types;
      GetActorTypes(types);

      std::sort(types.begin(), types.end(), SortFunctor());

      std::vector<std::string> list;
      list.push_back("<None>");

      for (size_t index = 0; index < types.size(); ++index)
      {
         list.push_back(types[index]->GetFullName());
      }

      return list;
   }

   /////////////////////////////////////////////////////////////////////////////
   const ActorType* LibraryManager::FindActorType(const std::string& category,
                                                  const std::string& name)
   {
      dtCore::RefPtr<const ActorType> typeToFind = new ActorType(name,category);
      ActorTypeMapItor itor = mActors.find(typeToFind);
      if (itor != mActors.end())
      {
         typeToFind = itor->first;
         return typeToFind.get();
      }
      else
      {
         return NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string LibraryManager::FindActorTypeReplacement(const std::string& fullName) const
   {
      ActorPluginRegistry::ActorTypeReplacements::const_iterator itr = mReplacementActors.begin();
      while (itr != mReplacementActors.end())
      {
         if ((*itr).first == fullName)
         {
            return((*itr).second);
         }
         ++itr;
      }

      return std::string();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseActorObject> LibraryManager::CreateActor(const ActorType& actorType)
   {
      ActorPluginRegistry* apr = GetRegistryForType(actorType);

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Creating actor proxy of type \"%s\".",
            actorType.GetFullName().c_str());
      }

      //Now we know which registry to use, so tell the registry to
      //create the proxy object and return it.
      dtCore::RefPtr<BaseActorObject> proxy = apr->CreateActor(actorType).get();
      return proxy;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseActorObject> LibraryManager::CreateActor(const std::string& category, const std::string& name)
   {
      dtCore::RefPtr<const ActorType> type = FindActorType(category, name);
      if (!type.valid())
      {
         throw dtCore::ObjectFactoryUnknownTypeException(
         "No actor exists of the specified name [" + name + "] and category[" +
         category + "].", __FILE__, __LINE__);
      }

      return CreateActor(*type);
   }

   /////////////////////////////////////////////////////////////////////////////
   ActorPluginRegistry* LibraryManager::GetRegistry(const std::string& name)
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         for (RegistryMapItor i = mRegistries.begin(); i != mRegistries.end(); ++i)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                              "Library manager has loaded registry library \"%s\"",
                              i->first.c_str());
         }
      }

      RegistryMapItor itor = mRegistries.find(name);

      if (itor == mRegistries.end())
      {
         return NULL;
      }
      else
      {
         return itor->second.registry;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   ActorPluginRegistry* LibraryManager::GetRegistryForType(const ActorType& actorType)
   {
      std::ostringstream error;

      //To create an new actor proxy, first we search our map of actor types
      //to locate the actor registry that knows how to create a proxy of the
      //requested type.
      dtCore::RefPtr<const ActorType> actorTypePtr(&actorType);

      ActorTypeMapItor found = mActors.find(actorTypePtr);
      if (found == mActors.end())
      {
         error << "Requested actor type: \"" << actorType.GetCategory() << "." <<  actorType.GetName() <<
            "\" but is unknown or has not been registered.";
         throw dtCore::ObjectFactoryUnknownTypeException(error.str(), __FILE__, __LINE__);
      }

      return found->second;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string LibraryManager::GetLibraryNameForRegistry(ActorPluginRegistry* registry)
   {
      for (RegistryMapItor i = mRegistries.begin(); i != mRegistries.end(); ++i)
      {
         if (i->second.registry == registry)
         {
            return i->first;
         }
      }

      return "";
   }

   /////////////////////////////////////////////////////////////////////////////
   void LibraryManager::UnloadActorRegistry(const std::string& libName)
   {
      if (libName == ACTOR_LIBRARY)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Unloading the default actor library \"%s\".", ACTOR_LIBRARY.c_str());
      }

      RegistryMapItor regItor = mRegistries.find(libName);

      if (regItor == mRegistries.end())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Attempted to unload actor registry \"%s\" which was not loaded.",
            libName.c_str());
         return;
      }

      //First remove all the actor types this registry supports.
      RegistryEntry regEntry = regItor->second;
      std::vector<dtCore::RefPtr<const ActorType> > actorTypes;
      std::vector<dtCore::RefPtr<const ActorType> >::iterator actorItor;

      regEntry.registry->GetSupportedActorTypes(actorTypes);
      for (actorItor=actorTypes.begin(); actorItor!=actorTypes.end(); ++actorItor)
      {
         mActors.erase(dtCore::RefPtr<const ActorType>(actorItor->get()));
      }

      ///remove the actor type unloadedReplacements supported by this registry
      ActorPluginRegistry::ActorTypeReplacements unloadedReplacements;
      regEntry.registry->GetReplacementActorTypes(unloadedReplacements);

      ActorPluginRegistry::ActorTypeReplacements::iterator unloadedReplacementsItr = unloadedReplacements.begin();
      while (unloadedReplacementsItr != unloadedReplacements.end())
      {
         ActorPluginRegistry::ActorTypeReplacements::iterator found;
         found = std::find(mReplacementActors.begin(),
                           mReplacementActors.end(),
                           (*unloadedReplacementsItr));

         if (found != mReplacementActors.end())
         {
            //found an entry in our list, erase it
            mReplacementActors.erase(found);
         }
         ++unloadedReplacementsItr;
      }

      mRegistries.erase(regItor);

      // If it has no destroyFn, then it was added with an autoregister class, so just ignore it, it should go away on its own.
      if (regEntry.destroyFn != NULL)
      {
         //Now that all references are gone, take the pointer to the registry so that we can
         //manually free it in the plugin.
         ActorPluginRegistry* reg = regEntry.registry;

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Unloading actor plugin registry: \"%s\"", reg->GetName().c_str());
         }

         regEntry.destroyFn(reg);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   LibraryManager& LibraryManager::GetInstance()
   {
      if (!LibraryManager::mInstance.valid())
      {
         LibraryManager::mInstance = new LibraryManager();
      }
      return *(LibraryManager::mInstance.get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LibraryManager::Destroy()
   {
      LibraryManager::mInstance = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string LibraryManager::GetPlatformSpecificLibraryName(const std::string& libBase)
   {
      return dtUtil::LibrarySharingManager::GetPlatformSpecificLibraryName(libBase);
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string LibraryManager::GetPlatformIndependentLibraryName(const std::string& libName)
   {
      return dtUtil::LibrarySharingManager::GetPlatformIndependentLibraryName(libName);
   }

   /////////////////////////////////////////////////////////////////////////////
   void LibraryManager::LoadOptionalActorRegistry(const std::string& libName)
   {
      const std::string actualLibName = GetPlatformSpecificLibraryName(libName);
      std::string fullLibraryName = osgDB::findLibraryFile(actualLibName);

      //If the file wasn't found using OSG paths, try the LibrarySharingManager's paths
      if (fullLibraryName.empty())
      {
         fullLibraryName = dtUtil::LibrarySharingManager::GetInstance().FindLibraryInSearchPath(actualLibName);
      }

      if (fullLibraryName.empty())
      {
         LOG_INFO("The optional actor library '" + libName + "' wasn't found.");
         return;
      }

      try
      {
         LoadActorRegistry(libName);
      }
      catch (const dtUtil::Exception&)
      {
         //this shouldn't happen, but if it does (corrupt file, etc) then
         //try to handle this quietly since its not critical.
         LOG_WARNING("Failed loading optional library '" + libName + "'.  Some actors may not be available.");
      }
   }
}
