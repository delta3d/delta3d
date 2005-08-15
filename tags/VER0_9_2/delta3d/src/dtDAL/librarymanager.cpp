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
* @author Matthew W. Campbell
*/
#include "dtDAL/librarymanager.h"
#include "dtDAL/actortype.h"
#include <dtUtil/log.h>
#include "dtDAL/exception.h"
#include "dtDAL/actorproxyicon.h"
#include <sstream>
#include <osgDB/Registry>
#include <osgDB/FileNameUtils>
#include <dtCore/scene.h>

namespace dtDAL
{
    //Singleton global variable for the library manager.
    osg::ref_ptr<LibraryManager> LibraryManager::mInstance(NULL);

    ///////////////////////////////////////////////////////////////////////////////
    LibraryManager::LibraryManager()
    {
        ActorProxyIcon::staticInitialize();
        dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_WARNING);

        LOG_INFO("Initializing actor library manager.");
        #ifdef _DEBUG
        LoadActorRegistry("dtActorsd");
        #else
        LoadActorRegistry("dtActors");
        #endif
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
            //At this point, we know that the smart pointer holding the registry has a
            //reference count of 1 since the LibraryManager's registry map is the owner
            //of the registry and is the last to be destructed.  Therefore,
            //take control from the smart pointer so it doesn't get deleted and manually
            //delete it in the plugin.
            ActorPluginRegistry *reg = itor->second.registry.take();
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

        osgDB::DynamicLibrary *dynLib;
        RegistryEntry newEntry;
        std::string actualLibName;

        //Get the system dependent name of the library.
        actualLibName = GetPlatformSpecificLibraryName(libName);

        //First, try and load the dynamic library.
        msg << "Loading actor registry library " << actualLibName;
        LOG_INFO(msg.str());
        dynLib = osgDB::DynamicLibrary::loadLibrary(actualLibName);
        if (!dynLib)
        {
            msg.clear();
            msg.str("");
            msg << "Unable to load actor registry " << actualLibName;
            EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError,msg.str());
        }
        else
        {
            newEntry.dynLib = dynLib;
        }

        //Now lookup the symbols for creating and destroying the library.
        osgDB::DynamicLibrary::PROC_ADDRESS createFn;
        osgDB::DynamicLibrary::PROC_ADDRESS destroyFn;
        createFn = dynLib->getProcAddress("CreatePluginRegistry");
        destroyFn = dynLib->getProcAddress("DestroyPluginRegistry");

        //Make sure the plugin actually implemented these functions and they
        //have been exported.
        if (!createFn)
        {
            msg.clear();
            msg.str("");
            msg << "Actor plugin libraries must implement the function " <<
                " CreatePluginRegistry.";
            EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError,msg.str());
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
            ActorTypeMapItor itor = mActors.find(actorTypes[i]);
            if (itor != mActors.end())
            {
                msg.clear();
                msg.str("");
                msg << "Duplicate actor type " << *actorTypes[i] << " found. Will not be added.";
                LOG_ERROR(msg.str());
            }
            else
            {
                mActors.insert(std::make_pair(actorTypes[i],newEntry.registry));
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
    void LibraryManager::GetActorTypes(std::vector<osg::ref_ptr<ActorType> > &actorTypes)
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
    osg::ref_ptr<ActorType> LibraryManager::FindActorType(const std::string &category,
            const std::string &name)
    {
        osg::ref_ptr<ActorType> typeToFind = new ActorType(name,category);
        ActorTypeMapItor itor = mActors.find(typeToFind);
        if (itor != mActors.end())
            return itor->first;
        else
            return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::ref_ptr<ActorProxy> LibraryManager::CreateActorProxy(ActorType& actorType)
    {
        ActorPluginRegistry* apr = GetRegistryForType(actorType);

        //Now we know which registry to use, so tell the registry to
        //create the proxy object and return it.
        osg::ref_ptr<ActorProxy> proxy = apr->CreateActorProxy(actorType);
        return proxy;
    }

    ///////////////////////////////////////////////////////////////////////////////
    ActorPluginRegistry *LibraryManager::GetRegistry(const std::string &name)
    {
        RegistryMapItor itor = mRegistries.find(name);

        if (itor == mRegistries.end())
            return NULL;
        else
            return itor->second.registry.get();
    }

    ///////////////////////////////////////////////////////////////////////////////
    ActorPluginRegistry *LibraryManager::GetRegistryForType(ActorType& actorType) 
    {
        std::ostringstream error;

        //To create an new actor proxy, first we search our map of actor types
        //to locate the actor registry that knows how to create a proxy of the
        //requested type.
        osg::ref_ptr<ActorType> actorTypePtr(&actorType);
        
        ActorTypeMapItor found = mActors.find(actorTypePtr);
        if (found == mActors.end())
        {
            error << "Requested actor type: " << actorType.GetName() <<
                " is unknown or has not been registered.";
            EXCEPT(ExceptionEnum::ObjectFactoryUnknownType,error.str());
        }
        
        return found->second.get();        
    }

    ///////////////////////////////////////////////////////////////////////////////
    void LibraryManager::UnloadActorRegistry(const std::string &libName)
    {
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
            mActors.erase(*actorItor);

        mRegistries.erase(regItor);

        //Now that all references are gone, take the pointer to the registry so that we can
        //manually free it in the plugin.
        ActorPluginRegistry *reg = regEntry.registry.take();

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
        #if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
        return libBase + ".dll";
        #elif defined(__APPLE__)
        return "lib" + libBase + ".so";
        #else
        return "lib" + libBase + ".so";
        #endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    std::string LibraryManager::GetPlatformIndependentLibraryName(const std::string &libName)
    {
        std::string iName = osgDB::getStrippedName(libName);

        #if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
        return iName;
        #else
        return std::string(iName.begin()+3,iName.end());
        #endif
    }
}
