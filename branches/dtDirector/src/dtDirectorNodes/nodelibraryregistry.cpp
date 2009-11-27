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
#include <dtDirectorNodes/inputnode.h>
#include <dtDirectorNodes/namedevent.h>

// Actions
#include <dtDirectorNodes/outputnode.h>
#include <dtDirectorNodes/operationaction.h>

// Values
#include <dtDirectorNodes/externalvaluenode.h>
#include <dtDirectorNodes/intvalue.h>


using dtCore::RefPtr;

namespace dtDirector
{
   // Events
   RefPtr<NodeType> NodeLibraryRegistry::INPUT_NODE_TYPE(new dtDirector::NodeType("Input", "Core", "Links", "An Input node for a sub tier."));
   RefPtr<NodeType> NodeLibraryRegistry::NAMED_EVENT_NODE_TYPE(new dtDirector::NodeType("Named Event", "General", "Events", "A simple event with a custom name."));

   // Actions
   RefPtr<NodeType> NodeLibraryRegistry::OUTPUT_NODE_TYPE(new dtDirector::NodeType("Output", "Core", "Links", "An Output node for a sub tier."));
   RefPtr<NodeType> NodeLibraryRegistry::OPERATION_ACTION_NODE_TYPE(new dtDirector::NodeType("Binary Operation", "General", "Math", "Performs a simple binary math operation given two values."));

   // Values
   RefPtr<NodeType> NodeLibraryRegistry::EXTERNAL_VALUE_NODE_TYPE(new dtDirector::NodeType("External Value", "Core", "Links", "An External Value node for a sub tier."));
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
      : dtDirector::NodePluginRegistry("Core set of nodes that can be used anywhere.")
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeLibraryRegistry::RegisterNodeTypes()
   {
      // Events
      mNodeFactory->RegisterType<InputNode>(INPUT_NODE_TYPE.get());
      mNodeFactory->RegisterType<NamedEvent>(NAMED_EVENT_NODE_TYPE.get());

      // Actions
      mNodeFactory->RegisterType<OutputNode>(OUTPUT_NODE_TYPE.get());
      mNodeFactory->RegisterType<OperationAction>(OPERATION_ACTION_NODE_TYPE.get());

      // Values
      mNodeFactory->RegisterType<ExternalValueNode>(EXTERNAL_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<IntValue>(INT_VALUE_NODE_TYPE.get());
   }
}
