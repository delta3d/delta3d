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

#ifndef DIRECTOR_NODE_PLUGIN_REGISTRY
#define DIRECTOR_NODE_PLUGIN_REGISTRY

#include <vector>

#include <dtDirector/nodetype.h>
#include <dtDirector/node.h>
#include <dtDirector/export.h>

#include <dtUtil/objectfactory.h>

namespace dtDirector
{
   class DirectorGraph;

   /**
    * The NodePluginRegistry is the base class that developers extend to
    * build their own Node registries.  Its main purpose is to serve as an
    * object factory which knows how to build Nodes using
    * NodeTypes as the tool by which to do so.
    *
    * @note
    *      Registry objects should only be used with the dtCore::RefPtr<>
    *      construct since they are reference counted objects.
    * @see NodeType
    * @see Node
    * @note The dynamic library file containing the Node Registry must contain
    *  two exported "C" methods, to create and destroy the NodePluginRegistry.
    *
    * Example:
    * @code
    * extern "C" DT_EXAMPLE_EXPORT dtDirector::NodePluginRegistry* CreatePluginRegistry()
    * {
    *    return new TestGameNodeLibrary;
    * }
    *
    * extern "C" DT_EXAMPLE_EXPORT void DestroyPluginRegistry(dtDirector::NodePluginRegistry* registry)
    * {
    *   if (registry != NULL)
    *   delete registry;
    * }
    * @endcode
    */
   class DT_DIRECTOR_EXPORT NodePluginRegistry
   {
   public:

      /**
       * Constructs the registry.  Sets the name and description for
       * this registry.
       */
      NodePluginRegistry(const std::string& name, const std::string& desc = "")
         : mName(name)
         , mDescription(desc)
      {
         mNodeFactory = new dtUtil::ObjectFactory<dtCore::RefPtr<const NodeType>, Node, NodeType::RefPtrComp>;
      }

      /**
       * Empty destructor. This class is not reference counted since we need
       * to manually free pointers to the registry objects from their
       * corresponding dynamic library, therefore, we need access to the
       * object's destructor.
       */
      virtual ~NodePluginRegistry() { }

      /**
       * Registers the actor types that this registry knows how to create.
       * This method is the first method to get called by the NodeManager
       * after it loads a dynamic library and gets a pointer to the
       * registry object it contains.
       */
      virtual void RegisterNodeTypes() = 0;

      /**
       * Sets the name of this registry.
       *
       * @param[in]  name  Name to assign to the registry.
       */
      void SetName(const std::string& name) { mName = name; }

      /**
       * Retrieves the name currently assigned to this registry.
       */
      const std::string& GetName() const { return mName; }

      /**
       * Sets the description for this registry.
       *
       * @param[in]  desc  Description for this node registry.
       */
      void SetDescription(const std::string& desc) { mDescription = desc; }

      /**
       * Gets the description of this registry.
       */
      const std::string& GetDescription() const { return mDescription; }

      /**
       * Gets a list of node types that this registry supports.
       */
      void GetSupportedNodeTypes(std::vector<dtCore::RefPtr<const NodeType> >& nodes);

      /**
        * Container of <old, new> NodeType names.  First entry is the full name of the
        * old NodeType.  Second entry is the full name of the new NodeType to
        * use instead.
        */
      struct NodeReplacementData
      {
         NodeReplacementData()
         {
         }

         ~NodeReplacementData()
         {
         }

         bool operator==(const NodeReplacementData& rhs) const
         {
            return (rhs.library == library && rhs.newName == newName);
         }

         std::string library;
         std::string newName;
         std::string newCategory;
      };
      typedef std::vector<std::pair<std::string, NodeReplacementData> > NodeTypeReplacements;

      /**
       * Get the NodeTypeReplacements for this NodePluginRegistry.  This list
       * is used to provide some backwards compatibility with applications or maps
       * referring to older, deprecated NodeTypes.  Override in derived classes
       * if previous NodeTypes have been modified and backwards compatibility is
       * desired.
       *
       * @param[in]  replacements  The container to fill out with NodeType replacements
       */
      virtual void GetReplacementNodeTypes(NodeTypeReplacements &replacements) const;

      /**
       * Checks to see if this registry supports the given node type.
       *
       * @param[in]  type  The type to check support for.
       *
       * @return  True if supported, false otherwise.
       */
      bool IsNodeTypeSupported(dtCore::RefPtr<const NodeType> type);

      /**
       * Retrieves the type of node library this is.
       * @note: This should be overloaded if the nodes that belong
       * to the library are of a specific type.
       */
      virtual std::string GetNodeLibraryType() const
      {
         return "Core";
      }

      /**
       * Creates a new node based on the NodeType given.
       *
       * @param[in]  type   Type of node to create.
       * @param[in]  graph  The graph that owns this node.
       *
       * @return  A smart pointer to the newly created Node.
       *
       * @throws ExceptionEnum::ObjectFactoryUnknownType
       */
      dtCore::RefPtr<Node> CreateNode(const NodeType& type, DirectorGraph* graph);


   private:

      std::string mName;
      std::string mDescription;

   protected:
      /**
       * Factory object which stores the node types and knows how to
       * create them.
       *
       * @see ObjectFactory
       */
      dtCore::RefPtr<dtUtil::ObjectFactory<dtCore::RefPtr<const NodeType>,
         Node, NodeType::RefPtrComp> > mNodeFactory;
   };
} // namespace dtDirector

#endif // DIRECTOR_NODE_PLUGIN_REGISTRY
