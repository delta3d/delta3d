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

#ifndef NODE_LIBRARY_REGISTRY
#define NODE_LIBRARY_REGISTRY

#include <dtDirector/nodepluginregistry.h>
#include <dtDirectorNodes/nodelibraryexport.h>

////////////////////////////////////////////////////////////////////////////////

namespace dtDirector
{
   /**
    * Class that exports the applicable nodes to a library
    */
   class NODE_LIBRARY_EXPORT NodeLibraryRegistry : public dtDirector::NodePluginRegistry
   {
   public:

      /// Events
      static dtCore::RefPtr<dtDirector::NodeType> INPUT_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> START_EVENT_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> REMOTE_EVENT_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> TRIGGER_VOLUME_EVENT_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> GAME_MESSAGE_EVENT_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> GAME_EVENT_MESSAGE_EVENT_NODE_TYPE;

      /// Actions
      static dtCore::RefPtr<dtDirector::NodeType> OUTPUT_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> CALL_REMOTE_EVENT_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> REFERENCE_SCRIPT_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> DELAY_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> SOUND_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> SET_VALUE_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> SET_ARRAY_VALUE_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> GET_ARRAY_VALUE_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> GET_ARRAY_SIZE_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> REMOVE_ARRAY_VALUE_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> GET_ACTOR_PROPERTY_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> SET_ACTOR_PROPERTY_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> OPERATION_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> TOGGLE_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> COMPARE_VALUE_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> SCHEDULER_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> LERP_ACTOR_ROTATION_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> LERP_ACTOR_SCALE_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> LERP_ACTOR_TRANSLATION_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> ANIMATE_ACTOR_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> SEND_MESSAGE_ACTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> SEND_EVENT_MESSAGE_ACTION_NODE_TYPE;

      /// Values
      static dtCore::RefPtr<dtDirector::NodeType> EXTERNAL_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> REFERENCE_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> PLAYER_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> BOOLEAN_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> INT_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> FLOAT_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> RANDOM_FLOAT_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> DOUBLE_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> STRING_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> STRING_ARRAY_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> ACTOR_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> ACTOR_ARRAY_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> STATIC_MESH_VALUE_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> VEC_VALUE_NODE_TYPE;

      /// Misc
      static dtCore::RefPtr<dtDirector::NodeType> GROUP_BOX_NODE_TYPE;

      /**
       *	Constructor.
       */
      NodeLibraryRegistry();

      /**
       *	Registers all of the nodes to be exported.
       */
      void RegisterNodeTypes();
   };
}

#endif // NODE_LIBRARY_REGISTRY
