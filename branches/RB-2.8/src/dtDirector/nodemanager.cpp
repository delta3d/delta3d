/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#include <sstream>

#include <osgDB/FileUtils>

#include <dtDirector/director.h>
#include <dtDirector/nodemanager.h>
#include <dtDirector/nodetype.h>

#include <dtCore/exceptionenum.h>

#include <dtUtil/log.h>

namespace dtDirector
{
   static const std::string NODE_LIBRARY("dtDirectorNodes");

   // Singleton global variable for the library manager.
   dtCore::RefPtr<NodeManager> NodeManager::mInstance(NULL);

   /////////////////////////////////////////////////////////////////////////////
   NodeManager::NodeManager()
   {
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, "Initializing node library manager.");

//      LoadNodeRegistry(NODE_LIBRARY);
   }

   /////////////////////////////////////////////////////////////////////////////
   NodeManager::~NodeManager()
   {
      mNodes.clear();

      // We have to manually free the registries so we ensure the memory gets
      // freed in the dynamic libraries since that's where the memory was
      // allocated.
      RegistryMapItor itor = mRegistries.begin();
      while (itor != mRegistries.end())
      {
         NodePluginRegistry* reg = itor->second.registry;
         itor->second.destroyFn(reg);
         ++itor;
      }
      mRegistries.clear();

      // Remove any registered NodeType replacements
      mReplacementNodes.clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool NodeManager::IsInRegistry(const std::string& libName) const
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
   std::string NodeManager::GetNodeLibraryType(const std::string& libName) const
   {
      std::string type = "";
      RegistryMap::const_iterator regItor = mRegistries.find(libName);
      if (regItor != mRegistries.end())
      {
         type = regItor->second.registry->GetNodeLibraryType();
      }

      return type;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool NodeManager::LoadNodeRegistry(const std::string& libName)
   {
      // Used to format log messages.
      std::ostringstream msg;

      // To add a new registry, we first make sure its name is unique.  If
      // it is, we add it to the manager's list of currently loaded registries.
      // If it is not, we generate a unique name for it and then add it.
      if (IsInRegistry(libName))
      {
         msg.clear();
         msg.str("");
         msg << "Registry for library with name " << libName <<
            " already exists.  Library must already be loaded.";
         LOG_ERROR(msg.str());
         return false;
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
         msg << "Unable to load node registry " << libName;
         throw dtCore::ProjectResourceErrorException(msg.str(), __FILE__, __LINE__);
      }

      dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS createFn;
      dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS destroyFn;
      createFn = newEntry.lib->FindSymbol("CreateNodePluginRegistry");
      destroyFn = newEntry.lib->FindSymbol("DestroyNodePluginRegistry");


      //////////////////////////// Start deprecation handling ////////////////////////////
      if (createFn == NULL)
      {
         createFn = newEntry.lib->FindSymbol("CreatePluginRegistry");
         if (createFn != NULL)
         {
            LOG_WARNING("Loading Node library using deprecated symbol CreatePluginRegistry, please change this to CreateNodePluginRegistry.");
         }
      }

      if (destroyFn == NULL)
      {
         destroyFn = newEntry.lib->FindSymbol("DestroyPluginRegistry");
         if (destroyFn != NULL)
         {
             LOG_WARNING("Loading Node library using deprecated symbol DestroyPluginRegistry, please change this to DestroyNodePluginRegistry.");
         }
       }
      //////////////////////////// End deprecation handling ////////////////////////////

      // Make sure the plugin actually implemented these functions and they
      // have been exported.
      if (createFn == NULL)
      {
         msg.clear();
         msg.str("");
         msg << "Node plugin libraries must implement the function " <<
            " CreateNodePluginRegistry.";
         throw dtCore::ProjectResourceErrorException( msg.str(), __FILE__, __LINE__);
      }

      if (destroyFn == NULL)
      {
         msg.clear();
         msg.str("");
         msg << "Node plugin libraries must implement the function " <<
            " DestroyNodePluginRegistry.";
         throw dtCore::ProjectResourceErrorException(msg.str(), __FILE__, __LINE__);
      }

      // Well we made it here so that means the plugin was loaded
      // successfully and the create and destroy functions were found.
      newEntry.createFn = (CreatePluginRegistryFn)createFn;
      newEntry.destroyFn = (DestroyPluginRegistryFun)destroyFn;
      newEntry.registry = newEntry.createFn();

      if (!AddRegistryEntry(libName, newEntry))
      {
         msg.clear();
         msg.str("");
         msg << "Can't add Registry Entry: " << libName << " to Registry. " <<
            "Possibly it might have been added already.";
         throw dtCore::ProjectResourceErrorException( msg.str(), __FILE__, __LINE__);
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool NodeManager::AddRegistryEntry(const std::string& libName, const RegistryEntry& entry)
   {
      // Finally we can actually add the new registry to the library manager.
      // The map key is the system independent library name.
      bool inserted = mRegistries.insert(std::make_pair(libName, entry)).second;
      if(!inserted)
      {
         return false;
      }

      // Used to format log messages.
      std::ostringstream msg;

      // Second we map actor type to the registry that owns it.
      std::vector<dtCore::RefPtr<const NodeType> > nodeTypes;
      entry.registry->RegisterNodeTypes();
      entry.registry->GetSupportedNodeTypes(nodeTypes);

      NodePluginRegistry::NodeTypeReplacements replacements;
      entry.registry->GetReplacementNodeTypes(replacements);
      mReplacementNodes.insert(mReplacementNodes.end(), replacements.begin(), replacements.end());

      int numUniqueActors = 0;
      for (unsigned int i = 0; i < nodeTypes.size(); i++)
      {
         NodeTypeMapItor itor = mNodes.find(dtCore::RefPtr<const NodeType>(nodeTypes[i].get()));
         if (itor != mNodes.end())
         {
            msg.clear();
            msg.str("");
            msg << "Duplicate node type " << *nodeTypes[i] << " found. Will not be added.";
            LOG_ERROR(msg.str());
         }
         else
         {
            mNodes.insert(std::make_pair(dtCore::RefPtr<const NodeType>(nodeTypes[i].get()), entry.registry));
            ++numUniqueActors;
         }
      }

      msg.clear();
      msg.str("");
      msg << "Loaded node plugin registry. (Name: " << libName <<
         ", Number of Actors: " << numUniqueActors << ")";
      LOG_INFO(msg.str());

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void NodeManager::GetNodeTypes(std::vector<const NodeType*>& nodeTypes)
   {
      nodeTypes.clear();
      nodeTypes.reserve(mNodes.size());
      NodeTypeMapItor itor = mNodes.begin();
      while (itor != mNodes.end())
      {
         nodeTypes.push_back(itor->first.get());
         ++itor;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const NodeType* NodeManager::FindNodeType(const std::string& name, const std::string& category)
   {
      dtCore::RefPtr<const NodeType> typeToFind = new NodeType(NodeType::UNKNOWN_NODE, name, category);
      NodeTypeMapItor itor = mNodes.find(typeToFind);
      if (itor != mNodes.end())
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
   const NodeType* NodeManager::FindNodeType(const dtCore::DataType& dataType)
   {
      NodeTypeMapItor itor = mNodes.begin();
      while (itor != mNodes.end())
      {
         dtCore::RefPtr<const NodeType> nodeType = itor->first;
         if (nodeType->GetDataType() == dataType)
         {
            return nodeType.get();
         }

         ++itor;
      }

      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   NodePluginRegistry::NodeReplacementData NodeManager::FindNodeTypeReplacement(const std::string& fullName) const
   {
      NodePluginRegistry::NodeTypeReplacements::const_iterator itr = mReplacementNodes.begin();
      while (itr != mReplacementNodes.end())
      {
         if ((*itr).first == fullName)
         {
            return((*itr).second);
         }
         ++itr;
      }

      return NodePluginRegistry::NodeReplacementData();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Node> NodeManager::CreateNode(const NodeType& nodeType, DirectorGraph* graph)
   {
      NodePluginRegistry* apr = GetRegistryForType(nodeType);

      if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Creating node of type \"%s\".",
            nodeType.GetFullName().c_str());
      }

      // Now we know which registry to use, so tell the registry to
      // create the node and return it.
      return apr->CreateNode(nodeType, graph);
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Node> NodeManager::CreateNode(const std::string& name, const std::string& category, DirectorGraph* graph)
   {
      dtCore::RefPtr<const NodeType> type = FindNodeType(name, category);
      if(!type.valid())
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "No node exists of the specified name \"%s\" (%s) and category \"%s\".",
            name.c_str(), graph->GetDirector()->GetScriptName().c_str(), category.c_str());
         return NULL;
      }

      return CreateNode(*type, graph);
   }

   /////////////////////////////////////////////////////////////////////////////
   NodePluginRegistry* NodeManager::GetRegistry(const std::string& name)
   {
      if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         for (RegistryMapItor i = mRegistries.begin(); i != mRegistries.end(); ++i)
         {
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                              "Node manager has loaded registry library \"%s\"",
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
   NodePluginRegistry* NodeManager::GetRegistryForType(const NodeType& nodeType)
   {
      std::ostringstream error;

      // To create an new node, first we search our map of node types
      // to locate the node registry that knows how to create that node.
      dtCore::RefPtr<const NodeType> nodeTypePtr(&nodeType);

      NodeTypeMapItor found = mNodes.find(nodeTypePtr);
      if (found == mNodes.end())
      {
         error << "Requested actor type: \"" << nodeType.GetCategory() << "." <<  nodeType.GetName() <<
            "\" but is unknown or has not been registered.";
         throw dtCore::ObjectFactoryUnknownTypeException(error.str(), __FILE__, __LINE__);
      }

      return found->second;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string NodeManager::GetLibraryNameForRegistry(NodePluginRegistry* registry)
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
   void NodeManager::UnloadNodeRegistry(const std::string& libName)
   {
      if (libName == NODE_LIBRARY)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Unloading the default director node library \"%s\".", NODE_LIBRARY.c_str());
      }

      RegistryMapItor regItor = mRegistries.find(libName);

      if (regItor == mRegistries.end())
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Attempted to unload director node registry \"%s\" which was not loaded.",
            libName.c_str());
         return;
      }

      // First remove all the node types this registry supports.
      RegistryEntry regEntry = regItor->second;
      std::vector<dtCore::RefPtr<const NodeType> > nodeTypes;
      std::vector<dtCore::RefPtr<const NodeType> >::iterator nodeItor;

      regEntry.registry->GetSupportedNodeTypes(nodeTypes);
      for (nodeItor = nodeTypes.begin(); nodeItor != nodeTypes.end(); ++nodeItor)
      {
         mNodes.erase(dtCore::RefPtr<const NodeType>(nodeItor->get()));
      }

      // Remove the node type unloadedReplacements supported by this registry
      NodePluginRegistry::NodeTypeReplacements unloadedReplacements;
      regEntry.registry->GetReplacementNodeTypes(unloadedReplacements);

      NodePluginRegistry::NodeTypeReplacements::iterator unloadedReplacementsItr = unloadedReplacements.begin();
      while (unloadedReplacementsItr != unloadedReplacements.end())
      {
         NodePluginRegistry::NodeTypeReplacements::iterator found;
         found = std::find(mReplacementNodes.begin(),
                           mReplacementNodes.end(),
                           (*unloadedReplacementsItr));

         if (found != mReplacementNodes.end())
         {
            // Found an entry in our list, erase it
            mReplacementNodes.erase(found);
         }
         ++unloadedReplacementsItr;
      }

      mRegistries.erase(regItor);

      // Now that all references are gone, take the pointer to the registry so that we can
      // manually free it in the plugin.
      NodePluginRegistry* reg = regEntry.registry;

      if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Unloading director node plugin registry: \"%s\"", reg->GetName().c_str());

      regEntry.destroyFn(reg);
   }

   /////////////////////////////////////////////////////////////////////////////
   NodeManager& NodeManager::GetInstance()
   {
      if (!NodeManager::mInstance.valid())
         NodeManager::mInstance = new NodeManager();
      return *(NodeManager::mInstance.get());
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string NodeManager::GetPlatformSpecificLibraryName(const std::string& libBase)
   {
      return dtUtil::LibrarySharingManager::GetPlatformSpecificLibraryName(libBase);
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string NodeManager::GetPlatformIndependentLibraryName(const std::string& libName)
   {
      return dtUtil::LibrarySharingManager::GetPlatformIndependentLibraryName(libName);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool NodeManager::LoadOptionalNodeRegistry(const std::string& libName)
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
         LOG_INFO("The optional director node library '" + libName + "' was not found.");
         return false;
      }

      try
      {
         return LoadNodeRegistry(libName);
      }
      catch (const dtUtil::Exception&)
      {
         //this shouldn't happen, but if it does (corrupt file, etc) then
         //try to handle this quietly since its not critical.
         LOG_WARNING("Failed loading optional library '" + libName + "'.  Some nodes may not be available.");
      }

      return false;
   }
}
