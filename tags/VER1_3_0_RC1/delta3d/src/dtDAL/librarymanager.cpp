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
 * @author Matthew W. Campbell and David Guthrie
 */
#include "dtDAL/librarymanager.h"
#include "dtDAL/actortype.h"
#include "dtDAL/exceptionenum.h"
#include "dtDAL/actorproxyicon.h"
#include <dtUtil/log.h>
#include <osgDB/FileNameUtils>
#include <dtCore/scene.h>
#include <sstream>

namespace dtDAL
{
   static const std::string ACTOR_LIBRARY("dtActors");
   
   //Singleton global variable for the library manager.
   dtCore::RefPtr<LibraryManager> LibraryManager::mInstance(NULL);

   ///////////////////////////////////////////////////////////////////////////////
   LibraryManager::LibraryManager()
   {
      ActorProxyIcon::staticInitialize();
      //dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_WARNING);

      LOG_INFO("Initializing actor library manager.");
      LoadActorRegistry(ACTOR_LIBRARY);
   }

   ///////////////////////////////////////////////////////////////////////////////
   LibraryManager::~LibraryManager()
   {
      mActors.clear();

      //We have to manually free the registries so we ensure the memory gets
      //freed in the dynamic libraries since that's where the memory was
      //allocated.
      RegistryMapItor itor = mRegistries.begin();
      while (itor != mRegistries.end())
      {
         ActorPluginRegistry *reg = itor->second.registry;
         itor->second.destroyFn(reg);
         ++itor;
      }
      mRegistries.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LibraryManager::LoadActorRegistry(const std::string &libName)
   {
      //Used to format log messages.
      std::ostringstream msg;

      //To add a new registry, we first make sure its name is unique.  If
      //it is, we add it to the manager's list of currently loaded registries.
      //If it is not, we generate a unique name for it and then add it.
      RegistryMapItor regItor = mRegistries.find(libName);
      if (regItor != mRegistries.end())
      {
         msg.clear();
         msg.str("");
         msg << "Registry for library with name " + libName <<
            " already exists.  Library must already be loaded.";
         LOG_ERROR(msg.str());
         return;
      }

      dtUtil::LibrarySharingManager& lsm = dtUtil::LibrarySharingManager::GetInstance();
        
      RegistryEntry newEntry;        
        
      try
      {
         newEntry.lib = lsm.LoadSharedLibrary(libName);
      }
      catch (dtUtil::Exception)
      {
         msg.clear();
         msg.str("");
         msg << "Unable to load actor registry " << libName;
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError,msg.str());
      }
        
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
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError, msg.str());
      }

      if (!destroyFn)
      {
         msg.clear();
         msg.str("");
         msg << "Actor plugin libraries must implement the function " <<
            " DestroyPluginRegistry.";
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError,msg.str());
      }

      //Well we made it here so that means the plugin was loaded
      //successfully and the create and destroy functions were found.
      newEntry.createFn = (CreatePluginRegistryFn)createFn;
      newEntry.destroyFn = (DestroyPluginRegistryFun)destroyFn;
      newEntry.registry = newEntry.createFn();

      //Finally we can actually add the new registry to the library manager.
      //The map key is the system independent library name.
      mRegistries.insert(std::make_pair(libName,newEntry));

      //Second we map actor type to the registry that owns it.
      std::vector<osg::ref_ptr<ActorType> > actorTypes;
      newEntry.registry->RegisterActorTypes();
      newEntry.registry->GetSupportedActorTypes(actorTypes);
      int numUniqueActors = 0;
      for (unsigned int i=0; i<actorTypes.size(); i++)
      {
         ActorTypeMapItor itor = mActors.find(dtCore::RefPtr<ActorType>(actorTypes[i].get()));
         if (itor != mActors.end())
         {
            msg.clear();
            msg.str("");
            msg << "Duplicate actor type " << *actorTypes[i] << " found. Will not be added.";
            LOG_ERROR(msg.str());
         }
         else
         {
            mActors.insert(std::make_pair(dtCore::RefPtr<ActorType>(actorTypes[i].get()),newEntry.registry));
            ++numUniqueActors;
         }
      }

      msg.clear();
      msg.str("");
      msg << "Loaded actor plugin registry. (Name: " << libName <<
         ", Number of Actors: " << numUniqueActors << ")";
      LOG_INFO(msg.str());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LibraryManager::GetActorTypes(std::vector<dtCore::RefPtr<ActorType> > &actorTypes)
   {
      actorTypes.clear();
      ActorTypeMapItor itor = mActors.begin();
      while (itor != mActors.end())
      {
         actorTypes.push_back(itor->first);
         ++itor;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ActorType> LibraryManager::FindActorType(const std::string &category,
                                                           const std::string &name)
   {
      dtCore::RefPtr<ActorType> typeToFind = new ActorType(name,category);
      ActorTypeMapItor itor = mActors.find(typeToFind);
      if (itor != mActors.end())
         return itor->first;
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ActorProxy> LibraryManager::CreateActorProxy(ActorType& actorType)
   {
      ActorPluginRegistry* apr = GetRegistryForType(actorType); 

      //Now we know which registry to use, so tell the registry to
      //create the proxy object and return it.
      dtCore::RefPtr<ActorProxy> proxy = apr->CreateActorProxy(actorType).get();
      return proxy;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ActorProxy> LibraryManager::CreateActorProxy(const std::string &category, const std::string &name)
   {
      dtCore::RefPtr<ActorType> type = FindActorType(category, name);
      if(!type.valid())
         EXCEPT(dtDAL::ExceptionEnum::ObjectFactoryUnknownType, "No actor exists of the specified name and category");

      return CreateActorProxy(*type);
   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorPluginRegistry *LibraryManager::GetRegistry(const std::string &name)
   {
      dtUtil::Log& logger = dtUtil::Log::GetInstance("librarymanager.cpp");
      if (logger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         for (RegistryMapItor i = mRegistries.begin(); i != mRegistries.end(); ++i)
         {
            logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                              "Library manager contains has loaded registry library %s", 
                              i->first.c_str());
         }          
      }
       
      RegistryMapItor itor = mRegistries.find(name);

      if (itor == mRegistries.end())
         return NULL;
      else
         return itor->second.registry;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorPluginRegistry *LibraryManager::GetRegistryForType(ActorType& actorType) 
   {
      std::ostringstream error;

      //To create an new actor proxy, first we search our map of actor types
      //to locate the actor registry that knows how to create a proxy of the
      //requested type.
      dtCore::RefPtr<ActorType> actorTypePtr(&actorType);
        
      ActorTypeMapItor found = mActors.find(actorTypePtr);
      if (found == mActors.end())
      {
         error << "Requested actor type: " << actorType.GetName() <<
            " is unknown or has not been registered.";
         EXCEPT(dtDAL::ExceptionEnum::ObjectFactoryUnknownType,error.str());
      }
        
      return found->second;        
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LibraryManager::UnloadActorRegistry(const std::string &libName)
   {
      if (libName == ACTOR_LIBRARY)
      {
         LOG_DEBUG("Unloading the default actor library.");
      }
                      
      RegistryMapItor regItor = mRegistries.find(libName);

      if (regItor == mRegistries.end())
      {
         LOG_WARNING("Attempted to remove an actor registry that was not loaded.");
         return;
      }

      //First remove all the actor types this registry supports.
      RegistryEntry regEntry = regItor->second;
      std::vector<osg::ref_ptr<ActorType> > actorTypes;
      std::vector<osg::ref_ptr<ActorType> >::iterator actorItor;

      regEntry.registry->GetSupportedActorTypes(actorTypes);
      for (actorItor=actorTypes.begin(); actorItor!=actorTypes.end(); ++actorItor)
         mActors.erase(dtCore::RefPtr<ActorType>(actorItor->get()));

      mRegistries.erase(regItor);

      //Now that all references are gone, take the pointer to the registry so that we can
      //manually free it in the plugin.
      ActorPluginRegistry *reg = regEntry.registry;

      LOG_INFO("Unloading actor plugin registry: " + reg->GetName());
      regEntry.destroyFn(reg);
   }

   ///////////////////////////////////////////////////////////////////////////////
   LibraryManager &LibraryManager::GetInstance()
   {
      if (LibraryManager::mInstance.get() == NULL)
         LibraryManager::mInstance = new LibraryManager();
      return *(LibraryManager::mInstance.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::string LibraryManager::GetPlatformSpecificLibraryName(const std::string &libBase)
   {
      return dtUtil::LibrarySharingManager::GetPlatformSpecificLibraryName(libBase);
   }
   ///////////////////////////////////////////////////////////////////////////////
   std::string LibraryManager::GetPlatformIndependentLibraryName(const std::string &libName)
   {
      return dtUtil::LibrarySharingManager::GetPlatformIndependentLibraryName(libName);
   }
}
