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

#include <dtDirector/nodetype.h>
#include <dtDirectorNodes/nodelibraryregistry.h>

// Events
#include <dtDirectorNodes/namedevent.h>

// Actions
#include <dtDirectorNodes/operationaction.h>

// Values
#include <dtDirectorNodes/intvalue.h>


using dtCore::RefPtr;

namespace dtDirector
{
   // Events
   RefPtr<NodeType> NodeLibraryRegistry::NAMED_EVENT_NODE_TYPE(new dtDirector::NodeType("Named Event", "General", "Events", "A simple event with a custom name."));

   // Actions
   RefPtr<NodeType> NodeLibraryRegistry::OPERATION_ACTION_NODE_TYPE(new dtDirector::NodeType("Binary Operation", "General", "Math", "Performs a simple binary math operation given two values"));

   // Values
   RefPtr<NodeType> NodeLibraryRegistry::INT_VALUE_NODE_TYPE(new dtDirector::NodeType("Int", "General", "Variables", "An integer data storage."));

   //////////////////////////////////////////////////////////////////////////
   extern "C" NODE_LIBRARY_EXPORT dtDirector::NodePluginRegistry* CreatePluginRegistry()
   {
      return new NodeLibraryRegistry;
   }

   //////////////////////////////////////////////////////////////////////////
   extern "C" NODE_LIBRARY_EXPORT void DestroyPluginRegistry(dtDirector::NodePluginRegistry* registry)
   {
      delete registry;
   }

   //////////////////////////////////////////////////////////////////////////
   NodeLibraryRegistry::NodeLibraryRegistry()
      : dtDirector::NodePluginRegistry("General Nodes that can be used anywhere.")
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeLibraryRegistry::RegisterNodeTypes()
   {
      // Events
      mNodeFactory->RegisterType<NamedEvent>(NAMED_EVENT_NODE_TYPE.get());

      // Actions
      mNodeFactory->RegisterType<OperationAction>(OPERATION_ACTION_NODE_TYPE.get());

      // Values
      mNodeFactory->RegisterType<IntValue>(INT_VALUE_NODE_TYPE.get());
   }
}
