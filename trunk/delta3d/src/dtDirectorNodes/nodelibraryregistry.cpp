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
#include <dtDirectorNodes/remoteevent.h>

// Actions
#include <dtDirectorNodes/outputnode.h>
#include <dtDirectorNodes/callremoteeventaction.h>
#include <dtDirectorNodes/operationaction.h>
#include <dtDirectorNodes/comparevalueaction.h>
#include <dtDirectorNodes/delayaction.h>

// Values
#include <dtDirectorNodes/externalvaluenode.h>
#include <dtDirectorNodes/referencevalue.h>
#include <dtDirectorNodes/booleanvalue.h>
#include <dtDirectorNodes/intvalue.h>
#include <dtDirectorNodes/floatvalue.h>
#include <dtDirectorNodes/doublevalue.h>
#include <dtDirectorNodes/stringvalue.h>
#include <dtDirectorNodes/actorvalue.h>
#include <dtDirectorNodes/staticmeshvalue.h>
#include <dtDirectorNodes/vecvalue.h>


using dtCore::RefPtr;

namespace dtDirector
{
   // Category naming convention:
   //  Core        - All Core nodes are nodes that are specifically referenced
   //                in Director and are special cases.
   //
   //  General     - General nodes provide general functionality that can be used
   //                in most, if not all, script types.
   //
   //  Math        - Math nodes are any nodes that perform an operation between
   //                values and then outputs another.
   //
   //  Conditional - Conditional nodes have multiple outputs that get triggered
   //                when a condition is met.

   // Events
   RefPtr<NodeType> NodeLibraryRegistry::INPUT_NODE_TYPE(new dtDirector::NodeType("Input Link", "Core", "Links", "This node creates an input link connector in its parent graph."));
   RefPtr<NodeType> NodeLibraryRegistry::REMOTE_EVENT_NODE_TYPE(new dtDirector::NodeType("Remote Event", "Core", "Events", "An remote event."));

   // Actions
   RefPtr<NodeType> NodeLibraryRegistry::OUTPUT_NODE_TYPE(new dtDirector::NodeType("Output Link", "Core", "Links", "This node creates an output link connector in its parent graph."));
   RefPtr<NodeType> NodeLibraryRegistry::CALL_REMOTE_EVENT_ACTION_NODE_TYPE(new dtDirector::NodeType("Call Remote Event", "Core", "Actions", "Calls all remote event nodes with a given name in parallel, and waits until their entire chains are finished before continuing."));
   RefPtr<NodeType> NodeLibraryRegistry::OPERATION_ACTION_NODE_TYPE(new dtDirector::NodeType("Binary Operation", "General", "Math", "Performs a simple operation between two values A and B and outputs to Result."));
   RefPtr<NodeType> NodeLibraryRegistry::COMPARE_VALUE_ACTION_NODE_TYPE(new dtDirector::NodeType("Compare Value", "General", "Condition", "Compares two values A and B."));
   RefPtr<NodeType> NodeLibraryRegistry::DELAY_ACTION_NODE_TYPE(new dtDirector::NodeType("Delay", "General", "Actions", "Performs a time delay."));

   // Values
   RefPtr<NodeType> NodeLibraryRegistry::EXTERNAL_VALUE_NODE_TYPE(new dtDirector::NodeType("Value Link", "Core", "Links", "This node creates a value link connector in its parent graph."));
   RefPtr<NodeType> NodeLibraryRegistry::REFERENCE_VALUE_NODE_TYPE(new dtDirector::NodeType("Reference", "Core", "Variables", "This will reference another value in the script."));
   RefPtr<NodeType> NodeLibraryRegistry::BOOLEAN_VALUE_NODE_TYPE(new dtDirector::NodeType("Boolean", "General", "Variables", "A boolean value."));
   RefPtr<NodeType> NodeLibraryRegistry::INT_VALUE_NODE_TYPE(new dtDirector::NodeType("Int", "General", "Variables", "An integer value."));
   RefPtr<NodeType> NodeLibraryRegistry::FLOAT_VALUE_NODE_TYPE(new dtDirector::NodeType("Float", "General", "Variables", "A float value."));
   RefPtr<NodeType> NodeLibraryRegistry::DOUBLE_VALUE_NODE_TYPE(new dtDirector::NodeType("Double", "General", "Variables", "A double value."));
   RefPtr<NodeType> NodeLibraryRegistry::STRING_VALUE_NODE_TYPE(new dtDirector::NodeType("String", "General", "Variables", "A string value."));
   RefPtr<NodeType> NodeLibraryRegistry::ACTOR_VALUE_NODE_TYPE(new dtDirector::NodeType("Actor", "General", "Variables", "An actor value."));
   RefPtr<NodeType> NodeLibraryRegistry::STATIC_MESH_VALUE_NODE_TYPE(new dtDirector::NodeType("Static Mesh", "General", "Variables", "A Static Mesh resource value."));
   RefPtr<NodeType> NodeLibraryRegistry::VEC_VALUE_NODE_TYPE(new dtDirector::NodeType("Vector", "General", "Variables", "A vector value."));

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
      mNodeFactory->RegisterType<RemoteEvent>(REMOTE_EVENT_NODE_TYPE.get());

      // Actions
      mNodeFactory->RegisterType<OutputNode>(OUTPUT_NODE_TYPE.get());
      mNodeFactory->RegisterType<CallRemoteEventAction>(CALL_REMOTE_EVENT_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<OperationAction>(OPERATION_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<CompareValueAction>(COMPARE_VALUE_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<DelayAction>(DELAY_ACTION_NODE_TYPE.get());

      // Values
      mNodeFactory->RegisterType<ExternalValueNode>(EXTERNAL_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<ReferenceValue>(REFERENCE_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<BooleanValue>(BOOLEAN_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<IntValue>(INT_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<FloatValue>(FLOAT_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<DoubleValue>(DOUBLE_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<StringValue>(STRING_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<ActorValue>(ACTOR_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<StaticMeshValue>(STATIC_MESH_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<VecValue>(VEC_VALUE_NODE_TYPE.get());
   }
}
