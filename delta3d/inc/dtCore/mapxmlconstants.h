/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * David Guthrie
 */

#ifndef DELTA_MAP_XML_CONSTANTS
#define DELTA_MAP_XML_CONSTANTS

#include <dtCore/export.h>
#include <xercesc/util/XercesDefs.hpp>

namespace dtCore
{
   /**
    * @class MapXMLConstants
    * @brief Holds constants for all of the element names in the map XML.
    * <p>This class defines constants and handles the memory allocation and deallocation
    * for the values.</p>
    * <p>The constants are the names of the elements and any necessary
    * strings used when parsing or generating the map XML.  Since they are used with
    * the xerces parser, they strings are in stored in the multibyte format used with Xerces.
    * Since, however, the strings have to be created at startup, they are not const.  Do not
    * changes the value of these.  They are assumed to be allocated by the memory allocator
    * used internally to xerces and changing them will likely cause memory errors.</p>
    */
   class DT_CORE_EXPORT MapXMLConstants
   {
      public:

         static const char* const EDITOR_VERSION;
         static const char* const SCHEMA_VERSION;
         static const char* const DIRECTOR_SCHEMA_VERSION;

         static XMLCh* BEGIN_XML_DECL;
         static XMLCh* END_XML_DECL;

         static XMLCh* MAP_ELEMENT;
         static XMLCh* MAP_NAMESPACE;

         static XMLCh* PREFAB_ELEMENT;
         static XMLCh* PREFAB_NAMESPACE;

         static XMLCh* DIRECTOR_ELEMENT;
         static XMLCh* DIRECTOR_NAMESPACE;
         static XMLCh* DIRECTOR_SCRIPT_TYPE;
         static XMLCh* DIRECTOR_IMPORTED_SCRIPT;
         static XMLCh* DIRECTOR_READ_ONLY;

         static XMLCh* HEADER_ELEMENT;
         static XMLCh* NAME_ELEMENT;
         static XMLCh* PREFAB_ACTOR_TYPE_ELEMENT;
         static XMLCh* DESCRIPTION_ELEMENT;
         static XMLCh* AUTHOR_ELEMENT;
         static XMLCh* COMMENT_ELEMENT;
         static XMLCh* COPYRIGHT_ELEMENT;
         static XMLCh* CREATE_TIMESTAMP_ELEMENT;
         static XMLCh* LAST_UPDATE_TIMESTAMP_ELEMENT;
         static XMLCh* EDITOR_VERSION_ELEMENT;
         static XMLCh* SCHEMA_VERSION_ELEMENT;
         static XMLCh* ICON_ELEMENT;

         static XMLCh* LIBRARIES_ELEMENT;
         static XMLCh* LIBRARY_ELEMENT;
         static XMLCh* LIBRARY_NAME_ELEMENT;
         static XMLCh* LIBRARY_VERSION_ELEMENT;

         static XMLCh* EVENTS_ELEMENT;
         static XMLCh* EVENT_ELEMENT;
         static XMLCh* EVENT_ID_ELEMENT;
         static XMLCh* EVENT_NAME_ELEMENT;
         static XMLCh* EVENT_DESCRIPTION_ELEMENT;

         static XMLCh* DIRECTOR_GRAPH_ELEMENT;
         static XMLCh* DIRECTOR_EVENT_NODES_ELEMENT;
         static XMLCh* DIRECTOR_ACTION_NODES_ELEMENT;
         static XMLCh* DIRECTOR_VALUE_NODES_ELEMENT;
         static XMLCh* DIRECTOR_NODE_ELEMENT;
         static XMLCh* CATEGORY_ELEMENT;
         static XMLCh* ID_ELEMENT;
         static XMLCh* ID_INDEX_ELEMENT;

         static XMLCh* DIRECTOR_LINKS_INPUT_ELEMENT;
         static XMLCh* DIRECTOR_LINKS_OUTPUT_ELEMENT;
         static XMLCh* DIRECTOR_LINKS_VALUE_ELEMENT;
         static XMLCh* DIRECTOR_LINK_ELEMENT;

         static XMLCh* DIRECTOR_LINK_CHAIN_CONNECTION;
         static XMLCh* DIRECTOR_LINK_VALUE_CONNECTION;
         static XMLCh* DIRECTOR_REMOVED_LINK_CHAIN_CONNECTION;
         static XMLCh* DIRECTOR_REMOVED_LINK_VALUE_CONNECTION;

         static XMLCh* DIRECTOR_LINK_OUTPUT_OWNER_INDEX_ELEMENT;
         static XMLCh* DIRECTOR_LINK_OUTPUT_OWNER_ELEMENT;
         static XMLCh* DIRECTOR_LINK_INPUT_OWNER_INDEX_ELEMENT;
         static XMLCh* DIRECTOR_LINK_INPUT_OWNER_ELEMENT;
         static XMLCh* DIRECTOR_LINK_OUTPUT_NAME_ELEMENT;
         static XMLCh* DIRECTOR_LINK_INPUT_NAME_ELEMENT;

         static XMLCh* DIRECTOR_LINK_VISIBLE_ELEMENT;
         static XMLCh* DIRECTOR_LINK_EXPOSED_ELEMENT;
         static XMLCh* DIRECTOR_LINK_VALUE_IS_OUT_ELEMENT;
         static XMLCh* DIRECTOR_LINK_VALUE_ALLOW_MULTIPLE_ELEMENT;
         static XMLCh* DIRECTOR_LINK_VALUE_TYPE_CHECK_ELEMENT;

         static XMLCh* HIERARCHY_ELEMENT;
         static XMLCh* HIERARCHY_ELEMENT_NODE;

         static XMLCh* ACTORS_ELEMENT;
         static XMLCh* ACTOR_ELEMENT;
         static XMLCh* ACTOR_TYPE_ELEMENT;
         static XMLCh* ACTOR_ID_ELEMENT;
         static XMLCh* ACTOR_NAME_ELEMENT;
         static XMLCh* ACTOR_PARENT_ID_ELEMENT;
         static XMLCh* ACTOR_ENVIRONMENT_ACTOR_ELEMENT;
         static XMLCh* ACTOR_COMPONENTS_ELEMENT;

         static XMLCh* ACTOR_PROPERTY_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_NAME_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_STRING_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_ENUM_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_FLOAT_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_DOUBLE_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_INTEGER_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_LONG_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_VEC2_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_VEC3_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_VEC4_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_BOOLEAN_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_COLOR_RGB_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_COLOR_RGBA_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_BIT_MASK_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_ACTOR_ID_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_GAMEEVENT_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_GROUP_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_ARRAY_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_CONTAINER_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT;
         static XMLCh* ACTOR_PROPERTY_PROPERTY_CONTAINER_ELEMENT;

         static XMLCh* ACTOR_PROPERTY_PARAMETER_ELEMENT;

         static XMLCh* ACTOR_VEC_1_ELEMENT;
         static XMLCh* ACTOR_VEC_2_ELEMENT;
         static XMLCh* ACTOR_VEC_3_ELEMENT;
         static XMLCh* ACTOR_VEC_4_ELEMENT;

         static XMLCh* ACTOR_COLOR_R_ELEMENT;
         static XMLCh* ACTOR_COLOR_G_ELEMENT;
         static XMLCh* ACTOR_COLOR_B_ELEMENT;
         static XMLCh* ACTOR_COLOR_A_ELEMENT;

         static XMLCh* ACTOR_ARRAY_SIZE_ELEMENT;
         static XMLCh* ACTOR_ARRAY_INDEX_ELEMENT;
         static XMLCh* ACTOR_ARRAY_ELEMENT;

         static XMLCh* ACTOR_GROUPS_ELEMENT;
         static XMLCh* ACTOR_GROUP_ELEMENT;
         static XMLCh* ACTOR_GROUP_ACTOR_ELEMENT;

         static XMLCh* PRESET_CAMERAS_ELEMENT;
         static XMLCh* PRESET_CAMERA_ELEMENT;
         static XMLCh* PRESET_CAMERA_INDEX_ELEMENT;
         static XMLCh* PRESET_CAMERA_PERSPECTIVE_VIEW_ELEMENT;
         static XMLCh* PRESET_CAMERA_TOP_VIEW_ELEMENT;
         static XMLCh* PRESET_CAMERA_SIDE_VIEW_ELEMENT;
         static XMLCh* PRESET_CAMERA_FRONT_VIEW_ELEMENT;
         static XMLCh* PRESET_CAMERA_POSITION_X_ELEMENT;
         static XMLCh* PRESET_CAMERA_POSITION_Y_ELEMENT;
         static XMLCh* PRESET_CAMERA_POSITION_Z_ELEMENT;
         static XMLCh* PRESET_CAMERA_ROTATION_X_ELEMENT;
         static XMLCh* PRESET_CAMERA_ROTATION_Y_ELEMENT;
         static XMLCh* PRESET_CAMERA_ROTATION_Z_ELEMENT;
         static XMLCh* PRESET_CAMERA_ROTATION_W_ELEMENT;
         static XMLCh* PRESET_CAMERA_ZOOM_ELEMENT;

         /**
          * this is called automatically on startup.
          */
         static void StaticInit();
         /**
          * this is called automatically on shutdown.
          */
         static void StaticShutdown();
      private:
         //these are private because no one should create an instance of this class.
         MapXMLConstants();
         ~MapXMLConstants();
         MapXMLConstants(const MapXMLConstants& handler);
         MapXMLConstants& operator=(const MapXMLConstants& handler);
   };
}

#endif
