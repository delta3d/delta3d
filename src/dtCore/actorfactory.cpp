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

#include <dtCore/actorfactory.h>
#include <dtCore/actortype.h>
#include <dtCore/exceptionenum.h>

#include <dtUtil/log.h>

#include <osgDB/FileUtils>

#include <sstream>

namespace dtCore
{
   const std::string ActorFactory::DEFAULT_ACTOR_LIBRARY("dtActors");

   //Singleton global variable for the library manager.
   dtCore::RefPtr<ActorFactory> ActorFactory::mInstance(nullptr);

   ActorFactory::RegistryEntry::RegistryEntry()
   : registry(nullptr)
   , createFn(nullptr)
   , destroyFn(nullptr)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   ActorFactory::ActorFactory()
   {
      mLogger = &dtUtil::Log::GetInstance("actorfactory.cpp");
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, "Initializing actor library manager.");
   }

   /////////////////////////////////////////////////////////////////////////////
   ActorFactory::~ActorFactory()
   {
      mActorTypeCache.clear();

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
   bool ActorFactory::IsInRegistry(const std::string& libName) const
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
   void ActorFactory::LoadActorRegistry(const std::string& libName)
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
         LOGN_DEBUG("actorfactory.cpp", msg.str());
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
   bool ActorFactory::AddRegistryEntry(const std::string& libName, const RegistryEntry& entry)
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

      //Initialize the registry
      entry.registry->RegisterActorTypes();

      // Get the replacements list
      dtCore::ActorPluginRegistry::ActorTypeReplacements replacements;
      entry.registry->GetReplacementActorTypes(replacements);
      mReplacementActors.insert(mReplacementActors.end(), replacements.begin(), replacements.end());

      if (dtUtil::Log::GetInstance("actorfactory.cpp").IsLevelEnabled(dtUtil::Log::LOG_INFO))
      {
         std::vector<dtCore::RefPtr<const ActorType> > actorTypes;
         entry.registry->GetSupportedActorTypes(actorTypes);
         msg.clear();
         msg.str("");
         msg << "Loaded actor plugin registry. (Name: " << libName <<
               ", Number of Actors: " << actorTypes.size() << ")";
         LOGN_INFO("actorfactory.cpp", msg.str());
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActorFactory::GetActorTypes(ActorTypeList& actorTypes) const
   {
      RegistryMapConstItor i, iend;
      i = mRegistries.begin();
      iend = mRegistries.end();
      for( ; i != iend; ++i)
      {
         ActorPluginRegistry* curReg = i->second.registry;
         curReg->GetSupportedActorTypes(actorTypes);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   struct SortFunctor
   {
      bool operator()(const dtCore::ActorType* a, const dtCore::ActorType* b)
      {
         return a->GetFullName() < b->GetFullName();
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> ActorFactory::GetClassTypes() const
   {
      ActorTypeList types;
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
   const ActorType* ActorFactory::FindActorType(const std::string& category,
         const std::string& name) const
   {
      const ActorType* result = nullptr;
      dtCore::RefPtr<const ActorType> typeToFind = new ActorType(name, category);
      ActorTypeMapItor itor = mActorTypeCache.find(typeToFind);
      // Verify the cache is correct.  This is to support dynamically changing actor registries
      if (itor != mActorTypeCache.end() && itor->second->IsActorTypeSupported(*itor->first))
      {
         typeToFind = itor->first;
         result = typeToFind.get();
      }
      else
      {
         if (itor != mActorTypeCache.end())
         {
            mActorTypeCache.erase(itor);
         }

         RegistryMapConstItor i, iend;
         i = mRegistries.begin();
         iend = mRegistries.end();
         for (; result == nullptr && i != iend; ++i)
         {
            ActorPluginRegistry* curReg = i->second.registry;
            result = curReg->GetActorType(category, name);
            if (result != nullptr)
               mActorTypeCache.insert(std::make_pair(result, curReg));
         }
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string ActorFactory::FindActorTypeReplacementName(const std::string& fullName) const
   {
      std::string resultName;
      ActorPluginRegistry::ActorTypeReplacements::const_iterator itr = mReplacementActors.begin();
      while (itr != mReplacementActors.end())
      {
         if ((*itr).first == fullName)
         {
            resultName = ((*itr).second);
         }
         ++itr;
      }

      return resultName;
   }

   /////////////////////////////////////////////////////////////////////////////
   const ActorType* ActorFactory::FindActorTypeReplacement(const std::string& category, const std::string& name) const
   {
      std::string fullName = category + "." + name;
      std::string resultName = FindActorTypeReplacementName(fullName);
      const ActorType* result = nullptr;
      if (!resultName.empty())
      {
         std::pair<std::string, std::string> replacementName = ActorType::ParseNameAndCategory(resultName);
         result = FindActorType(replacementName.second, replacementName.first);
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseActorObject> ActorFactory::CreateActor(const ActorType& actorType)
   {
      ActorPluginRegistry* apr = GetRegistryForType(actorType);

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Creating actor proxy of type \"%s\".",
               actorType.GetFullName().c_str());
      }

      dtCore::RefPtr<BaseActorObject> result;
      if (apr != nullptr)
      {
         //Now we know which registry to use, so tell the registry to
         //create the proxy object and return it.
         result = apr->CreateActor(actorType);
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseActorObject> ActorFactory::CreateActor(const std::string& category, const std::string& name)
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
   ActorPluginRegistry* ActorFactory::GetRegistry(const std::string& name)
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
         return nullptr;
      }
      else
      {
         return itor->second.registry;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   ActorPluginRegistry* ActorFactory::GetRegistryForType(const ActorType& actorType)
   {
      std::ostringstream error;

      //To create an new actor proxy, first we search our map of actor types
      //to locate the actor registry that knows how to create a proxy of the
      //requested type.
      dtCore::RefPtr<const ActorType> actorTypePtr(&actorType);

      ActorPluginRegistry* result = nullptr;
      ActorTypeMapItor found = mActorTypeCache.find(actorTypePtr);
      if (found == mActorTypeCache.end() || !found->second->IsActorTypeSupported(actorType))
      {
         if (found != mActorTypeCache.end())
            mActorTypeCache.erase(found);

         for (RegistryMapItor i = mRegistries.begin(); i != mRegistries.end(); ++i)
         {
            ActorPluginRegistry* curReg = i->second.registry;
            if (curReg->IsActorTypeSupported(actorType))
            {
               result = curReg;
               mActorTypeCache.insert(std::make_pair(actorTypePtr, curReg));
               break;
            }
         }


         if (result == nullptr)
         {
            error << "Requested actor type: \"" << actorType.GetFullName() <<
                  "\" but is unknown or has not been registered.";
            throw dtCore::ObjectFactoryUnknownTypeException(error.str(), __FILE__, __LINE__);
         }
      }
      else
      {
         result = found->second;
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string ActorFactory::GetLibraryNameForRegistry(const ActorPluginRegistry& registry) const
   {
      std::string result;
      for (RegistryMapConstItor i = mRegistries.begin(); i != mRegistries.end(); ++i)
      {
         if (i->second.registry == &registry)
         {
            result = i->first;
         }
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActorFactory::UnloadActorRegistry(const std::string& libName)
   {
      if (libName == DEFAULT_ACTOR_LIBRARY)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Unloading the default actor library \"%s\".", DEFAULT_ACTOR_LIBRARY.c_str());
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
         mActorTypeCache.erase(dtCore::RefPtr<const ActorType>(actorItor->get()));
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
      if (regEntry.destroyFn != nullptr)
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
   ActorFactory& ActorFactory::GetInstance()
   {
      if (!ActorFactory::mInstance.valid())
      {
         ActorFactory::mInstance = new ActorFactory();
      }
      return *(ActorFactory::mInstance.get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorFactory::Destroy()
   {
      ActorFactory::mInstance = nullptr;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string ActorFactory::GetPlatformSpecificLibraryName(const std::string& libBase)
   {
      return dtUtil::LibrarySharingManager::GetPlatformSpecificLibraryName(libBase);
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string ActorFactory::GetPlatformIndependentLibraryName(const std::string& libName)
   {
      return dtUtil::LibrarySharingManager::GetPlatformIndependentLibraryName(libName);
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActorFactory::LoadOptionalActorRegistry(const std::string& libName)
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
