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

#ifndef DIRECTOR_NODE_MANAGER
#define DIRECTOR_NODE_MANAGER

#include <map>

#include <dtDirector/export.h>
#include <dtDirector/nodepluginregistry.h>
#include <dtDirector/nodetype.h>

#include <dtCore/refptr.h>

#include <dtUtil/librarysharingmanager.h>

namespace dtDirector
{
   class DirectorGraph;

   /**
    * This class is the main class in the dynamic node library portion of
    * dtDirector.  It has a list of the dynamic libraries that
    * are registered with the Manager as well as a list of the NodeTypes
    * that each library can create.  It is also the main vehicle for
    * creating a new Node.
    *
    * @note The NodeManager follows the Singleton design pattern.
    * @see NodeType
    * @see Node
    * @see NodePluginRegistry
    */
   class DT_DIRECTOR_EXPORT NodeManager : public osg::Referenced
   {
   private:
      /**
       * Function pointer to the create function implemented in the NodePlugin.
       */
      typedef NodePluginRegistry* (*CreatePluginRegistryFn)();

      /**
       * Function pointer to the destroy function implemented in the NodePlugin.
       * This is called just before closing the dynamic library owning the
       * NodePluginRegistry.
       */
      typedef void (*DestroyPluginRegistryFun)(NodePluginRegistry*);

   public:

      /**
       * Simple structure for grouping the data corresponding to a
       * registry entry.
       */
      struct RegistryEntry
      {
         NodePluginRegistry* registry;
         dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> lib;
         CreatePluginRegistryFn createFn;
         DestroyPluginRegistryFun destroyFn;
      };


      typedef std::map<dtCore::RefPtr<const NodeType>, NodePluginRegistry*, NodeType::RefPtrComp> NodeTypeMap;
      typedef NodeTypeMap::iterator NodeTypeMapItor;

      typedef std::map<std::string, RegistryEntry> RegistryMap;
      typedef RegistryMap::iterator RegistryMapItor;
      typedef RegistryMap::const_iterator RegistryMapConstItor;

      /**
       * Gets the singleton instance of the NodeManager.
       */
      static NodeManager& GetInstance();

      /**
       * Loads a node registry by loading a dynamic library
       * containing the node registry implementation.
       * @note The library file must contain two exported "C" methods:
       * "CreatePluginRegistry" and "DestroyPluginRegistry".  See NodePluginRegistry
       * for more information.
       *
       * @param[in]  libName     The name of the library to load.
       * @param[in]  scriptType  The type of script this library will be used for.
       *
       * @return     false if the library failed to load.
       *
       * @throws     ProjectResourceError Throws an exception if the
       *              library cannot be found or the create and destroy
       *              functions are not found in the library.
       */
      bool LoadNodeRegistry(const std::string& libName);

      /**
       * Inserts the pair of parameters into the container.
       * Also, registers the supported Nodes with the manager.
       *
       * @param[in]  libName  The system independent name of the library that was loaded
       * @param[in]  entry    The RegistryEntry mapped to the library name.
       *
       * @return              true if successfully added, false if not
       */
      bool AddRegistryEntry(const std::string& libName, const RegistryEntry& entry);

      /**
       * Unloads a node registry.  This unloads the dynamic library
       * containing the node registry implementation and also removes
       * all the nodes that the registry supported.
       *
       * @param[in]  libName  The system independent name of the library to load.
       */
      void UnloadNodeRegistry(const std::string& libName);

      /**
       * Retrieves whether the library is already in the registry.
       *
       * @param[in]  libName  The name of the library.
       *
       * @return  True if the library is in the registry.
       */
      bool IsInRegistry(const std::string& libName) const;

      /**
       * Retrieves the libraries node type (retrieved from the
       * GetNodeLibraryType method called from the library).
       *
       * @param[in]  libName     The name of the library.
       *
       * @return     The type of nodes in the library.
       */
      std::string GetNodeLibraryType(const std::string& libName) const;

      /**
       * Returns a list of all the node types the library manager knows how
       * to create.
       */
      void GetNodeTypes(std::vector<const NodeType*> &nodeTypes);

      /**
       * Gets a single node type that matches the name and category specified.
       *
       * @param[in]  name      The name of the node.
       * @param[in]  category  The category of the node.
       *
       * @return               A pointer if the node type was found or NULL if not.
       */
      const NodeType* FindNodeType(const std::string& name, const std::string& category);

      /**
       * Gets a single node type that matches the data type specified.
       *
       * @param[in]  dataType  The data type.
       *
       * @return               A pointer if the node type was found or NULL if not.
       */
      const NodeType* FindNodeType(const dtCore::DataType& dataType);

      /**
       *  Given the supplied NodeType full name, see if an NodeType
       *  replacement has been registered.
       *
       * @param[in]  fullName  The fullName of an older, deprecated NodeType
       *
       * @return               The full NodeType name to use instead (or empty, if nothing is registered)
       */
      NodePluginRegistry::NodeReplacementData FindNodeTypeReplacement(const std::string& fullName) const;

      /**
       * Creates a new node.  The node type is used by the node
       * manager to determine which type of node to create.
       *
       * @param[in]  nodeType  The type of the node to create.
       * @param[in]  graph     The graph that owns the node.
       *
       * @return  Returns a pointer to the base node which can be
       *          safely typecast to the appropriate derived type.
       *
       * @throws  Throws a ObjectFactoryUnknownType exception if the type
       * is unknown.
       */
      dtCore::RefPtr<Node> CreateNode(const NodeType& nodeType, DirectorGraph* graph);

      /**
       * Creates a new node.  The node type is used by the node
       * manager to determine which type of node to create.
       *
       * @param[in]  name      The name corresponding to the node type
       * @param[in]  category  The category corresponding to the node type
       * @param[in]  graph     The graph that owns the node.
       *
       * @return  Returns a pointer to the base node which can be
       *          safely typecast to the appropriate derived type.
       *
       * @throws  Throws a ObjectFactoryUnknownType exception if the type
       *          is unknown.
       */
      dtCore::RefPtr<Node> CreateNode(const std::string& name, const std::string& category, DirectorGraph* graph);

      /**
       * Gets a registry currently loaded by the node manager.
       *
       * @param[in]  name  The name of the library.  Note, this name is the
       *                   system independent name.
       *
       * @return  A handle to the registry or NULL if it is not currently loaded.
       */
      NodePluginRegistry *GetRegistry(const std::string& name);

      /**
       * Retrieves the registry for a specified node type.
       *
       * @param[in]  nodeType the node type to get the registry for.
       */
      NodePluginRegistry *GetRegistryForType(const NodeType& nodeType);

      /**
      * Retrieves the name of a library given the registry.
      *
      * @param[in]  registry  The registry.
      *
      * @return  The name of the actor library.
      */
      std::string GetLibraryNameForRegistry(NodePluginRegistry* registry);

      /**
       * Determines which platform we are running on and returns a
       * platform dependent library name.
       *
       * @param[in]  libBase  Platform independent library name.
       *
       * @return  A platform dependent library name.
       *
       * @note
       *  For example.  If the platform independent library name is
       *  ExampleActors then on Windows platforms the resulting dependent
       *  library name would be ExampleActors.dll, however, on Unix based
       *  platforms, the resulting name would be libExampleActors.so.
       */
      std::string GetPlatformSpecificLibraryName(const std::string& libBase);

      /**
       * Strips off the path and platform specific library prefix and extensions
       * and returns a system independent file name.
       *
       * @param[in]  libName  The platform specific library name.
       *
       * @return  A platform independent library name.
       */
      std::string GetPlatformIndependentLibraryName(const std::string& libName);

    private:

      /**
       * Constructs the node manager.  The constructor will also load
       * the engine objects since this is provided by the Delta3D engine.
       */
      NodeManager();

      /**
       * Free resources used by the node manager.
       */
      virtual ~NodeManager();

      /**
       * Loads a library if it exists.
       *
       * @param[in]  libName     The library to load.
       * @param[in]  scriptType  The type of script this library will be used for.
       *
       * @return     false if the library failed to load.
       */
      bool LoadOptionalNodeRegistry(const std::string& libName);

      /// Singleton instance of the class.
      static dtCore::RefPtr<NodeManager> mInstance;

      /// Maps an actor type to the registry that created it.
      NodeTypeMap mNodes;

      NodePluginRegistry::NodeTypeReplacements mReplacementNodes;

      /// List of the currently loaded actor registries.
      RegistryMap mRegistries;
   };
}

#endif
