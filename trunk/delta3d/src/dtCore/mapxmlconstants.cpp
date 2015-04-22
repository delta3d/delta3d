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

#include <prefix/dtcoreprefix.h>
#include <dtCore/mapxmlconstants.h>

#include <dtUtil/version.h>


#include <dtUtil/deprecationmgr.h>
DT_DISABLE_WARNING_ALL_START
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
DT_DISABLE_WARNING_END


namespace dtCore
{
   /////////////////////////////////////////////////////
   //// Constant Initialization ////////////////////////
   /////////////////////////////////////////////////////


   const char* const MapXMLConstants::EDITOR_VERSION = Delta3DGetVersion();
   const char* const MapXMLConstants::SCHEMA_VERSION = "1.0";
   const char* const MapXMLConstants::DIRECTOR_SCHEMA_VERSION = "2.0";

   XMLCh* MapXMLConstants::BEGIN_XML_DECL = NULL;
   XMLCh* MapXMLConstants::END_XML_DECL = NULL;

   XMLCh* MapXMLConstants::MAP_ELEMENT = NULL;
   XMLCh* MapXMLConstants::MAP_NAMESPACE = NULL;

   XMLCh* MapXMLConstants::PREFAB_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PREFAB_NAMESPACE = NULL;

   XMLCh* MapXMLConstants::DIRECTOR_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_NAMESPACE = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_SCRIPT_TYPE = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_IMPORTED_SCRIPT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_READ_ONLY = NULL;

   XMLCh* MapXMLConstants::HEADER_ELEMENT = NULL;
   XMLCh* MapXMLConstants::NAME_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PREFAB_ACTOR_TYPE_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DESCRIPTION_ELEMENT = NULL;
   XMLCh* MapXMLConstants::AUTHOR_ELEMENT = NULL;
   XMLCh* MapXMLConstants::COMMENT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::COPYRIGHT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::CREATE_TIMESTAMP_ELEMENT = NULL;
   XMLCh* MapXMLConstants::LAST_UPDATE_TIMESTAMP_ELEMENT = NULL;
   XMLCh* MapXMLConstants::EDITOR_VERSION_ELEMENT = NULL;
   XMLCh* MapXMLConstants::SCHEMA_VERSION_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ICON_ELEMENT = NULL;

   XMLCh* MapXMLConstants::LIBRARIES_ELEMENT = NULL;
   XMLCh* MapXMLConstants::LIBRARY_ELEMENT = NULL;
   XMLCh* MapXMLConstants::LIBRARY_NAME_ELEMENT = NULL;
   XMLCh* MapXMLConstants::LIBRARY_VERSION_ELEMENT = NULL;

   XMLCh* MapXMLConstants::EVENTS_ELEMENT = NULL;
   XMLCh* MapXMLConstants::EVENT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::EVENT_ID_ELEMENT = NULL;
   XMLCh* MapXMLConstants::EVENT_NAME_ELEMENT = NULL;
   XMLCh* MapXMLConstants::EVENT_DESCRIPTION_ELEMENT = NULL;

   XMLCh* MapXMLConstants::DIRECTOR_GRAPH_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_EVENT_NODES_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_ACTION_NODES_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_VALUE_NODES_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_NODE_ELEMENT = NULL;
   XMLCh* MapXMLConstants::CATEGORY_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ID_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ID_INDEX_ELEMENT = NULL;

   XMLCh* MapXMLConstants::DIRECTOR_LINKS_INPUT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINKS_OUTPUT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINKS_VALUE_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINK_ELEMENT = NULL;

   XMLCh* MapXMLConstants::DIRECTOR_LINK_CHAIN_CONNECTION = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINK_VALUE_CONNECTION = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_REMOVED_LINK_CHAIN_CONNECTION = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_REMOVED_LINK_VALUE_CONNECTION = NULL;

   XMLCh* MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_INDEX_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_INDEX_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINK_OUTPUT_NAME_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINK_INPUT_NAME_ELEMENT = NULL;

   XMLCh* MapXMLConstants::DIRECTOR_LINK_VISIBLE_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINK_EXPOSED_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINK_VALUE_IS_OUT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINK_VALUE_ALLOW_MULTIPLE_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DIRECTOR_LINK_VALUE_TYPE_CHECK_ELEMENT = NULL;

   XMLCh* MapXMLConstants::HIERARCHY_ELEMENT = NULL;
   XMLCh* MapXMLConstants::HIERARCHY_ELEMENT_NODE = NULL;

   XMLCh* MapXMLConstants::ACTORS_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_TYPE_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_ID_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_NAME_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PARENT_ID_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_ENVIRONMENT_ACTOR_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_COMPONENTS_ELEMENT = NULL;

   XMLCh* MapXMLConstants::ACTOR_PROPERTY_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_STRING_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_ENUM_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_FLOAT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_DOUBLE_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_INTEGER_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_LONG_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_VEC2_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_VEC3_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_VEC4_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_BOOLEAN_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_COLOR_RGB_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_COLOR_RGBA_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_BIT_MASK_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_ACTOR_ID_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_GAMEEVENT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_GROUP_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_ARRAY_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_PROPERTY_CONTAINER_ELEMENT = NULL;

   XMLCh* MapXMLConstants::ACTOR_PROPERTY_PARAMETER_ELEMENT = NULL;

   XMLCh* MapXMLConstants::ACTOR_VEC_1_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_VEC_2_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_VEC_3_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_VEC_4_ELEMENT = NULL;

   XMLCh* MapXMLConstants::ACTOR_COLOR_R_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_COLOR_G_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_COLOR_B_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_COLOR_A_ELEMENT = NULL;

   XMLCh* MapXMLConstants::ACTOR_ARRAY_SIZE_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_ARRAY_INDEX_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_ARRAY_ELEMENT = NULL;

   XMLCh* MapXMLConstants::ACTOR_GROUPS_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_GROUP_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_GROUP_ACTOR_ELEMENT = NULL;

   XMLCh* MapXMLConstants::PRESET_CAMERAS_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_INDEX_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_PERSPECTIVE_VIEW_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_TOP_VIEW_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_SIDE_VIEW_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_FRONT_VIEW_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_POSITION_X_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_POSITION_Y_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_POSITION_Z_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_ROTATION_X_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_ROTATION_Y_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_ROTATION_Z_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_ROTATION_W_ELEMENT = NULL;
   XMLCh* MapXMLConstants::PRESET_CAMERA_ZOOM_ELEMENT = NULL;

   class MaxXMLConstantsInit
   {
   public:
      MaxXMLConstantsInit() { MapXMLConstants::StaticInit(); }
      ~MaxXMLConstantsInit() { MapXMLConstants::StaticShutdown(); }
   };

   static MaxXMLConstantsInit gInitMe;

   void MapXMLConstants::StaticInit()
   {
      BEGIN_XML_DECL = xercesc::XMLString::transcode("<?xml version=\"1.0\" encoding=\"");
      END_XML_DECL = xercesc::XMLString::transcode("\"?>");

      MAP_ELEMENT = xercesc::XMLString::transcode("map");
      MAP_NAMESPACE = xercesc::XMLString::transcode("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"delta3dMap\" xsi:schemaLocation=\"delta3dMap map.xsd\"");

      PREFAB_ELEMENT = xercesc::XMLString::transcode("prefab");
      PREFAB_NAMESPACE = xercesc::XMLString::transcode("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"delta3dMap\" xsi:schemaLocation=\"delta3dMap map.xsd\"");

      DIRECTOR_ELEMENT = xercesc::XMLString::transcode("director");
      DIRECTOR_NAMESPACE = xercesc::XMLString::transcode("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"delta3dDirector\" xsi:schemaLocation=\"delta3dDirector director.xsd\"");
      DIRECTOR_SCRIPT_TYPE = xercesc::XMLString::transcode("scriptType");
      DIRECTOR_IMPORTED_SCRIPT = xercesc::XMLString::transcode("importedScript");
      DIRECTOR_READ_ONLY = xercesc::XMLString::transcode("readOnly");

      HEADER_ELEMENT = xercesc::XMLString::transcode("header");
      NAME_ELEMENT = xercesc::XMLString::transcode("name");
      PREFAB_ACTOR_TYPE_ELEMENT = xercesc::XMLString::transcode("name");
      DESCRIPTION_ELEMENT = xercesc::XMLString::transcode("description");
      AUTHOR_ELEMENT = xercesc::XMLString::transcode("author");
      COMMENT_ELEMENT = xercesc::XMLString::transcode("comment");
      COPYRIGHT_ELEMENT = xercesc::XMLString::transcode("copyright");
      CREATE_TIMESTAMP_ELEMENT = xercesc::XMLString::transcode("createTimestamp");
      LAST_UPDATE_TIMESTAMP_ELEMENT = xercesc::XMLString::transcode("lastUpdateTimestamp");
      EDITOR_VERSION_ELEMENT = xercesc::XMLString::transcode("editorVersion");
      SCHEMA_VERSION_ELEMENT = xercesc::XMLString::transcode("schemaVersion");
      ICON_ELEMENT = xercesc::XMLString::transcode("icon");

      LIBRARIES_ELEMENT = xercesc::XMLString::transcode("libraries");
      LIBRARY_ELEMENT = xercesc::XMLString::transcode("library");
      LIBRARY_NAME_ELEMENT = xercesc::XMLString::transcode("name");
      LIBRARY_VERSION_ELEMENT = xercesc::XMLString::transcode("version");

      EVENTS_ELEMENT = xercesc::XMLString::transcode("events");
      EVENT_ELEMENT = xercesc::XMLString::transcode("event");
      EVENT_ID_ELEMENT = xercesc::XMLString::transcode("id");
      EVENT_NAME_ELEMENT = xercesc::XMLString::transcode("name");
      EVENT_DESCRIPTION_ELEMENT = xercesc::XMLString::transcode("description");

      DIRECTOR_GRAPH_ELEMENT = xercesc::XMLString::transcode("graph");
      DIRECTOR_EVENT_NODES_ELEMENT = xercesc::XMLString::transcode("eventnodes");
      DIRECTOR_ACTION_NODES_ELEMENT = xercesc::XMLString::transcode("actionnodes");
      DIRECTOR_VALUE_NODES_ELEMENT = xercesc::XMLString::transcode("valuenodes");
      DIRECTOR_NODE_ELEMENT = xercesc::XMLString::transcode("node");
      CATEGORY_ELEMENT = xercesc::XMLString::transcode("category");
      ID_ELEMENT = xercesc::XMLString::transcode("id");
      ID_INDEX_ELEMENT = xercesc::XMLString::transcode("idIndex");

      DIRECTOR_LINKS_INPUT_ELEMENT = xercesc::XMLString::transcode("inputlink");
      DIRECTOR_LINKS_OUTPUT_ELEMENT = xercesc::XMLString::transcode("outputlink");
      DIRECTOR_LINKS_VALUE_ELEMENT = xercesc::XMLString::transcode("valuelink");
      DIRECTOR_LINK_ELEMENT = xercesc::XMLString::transcode("link");

      DIRECTOR_LINK_CHAIN_CONNECTION = xercesc::XMLString::transcode("chainLinkConnection");
      DIRECTOR_LINK_VALUE_CONNECTION = xercesc::XMLString::transcode("valueLinkConnection");
      DIRECTOR_REMOVED_LINK_CHAIN_CONNECTION = xercesc::XMLString::transcode("removedChainLinkConnection");
      DIRECTOR_REMOVED_LINK_VALUE_CONNECTION = xercesc::XMLString::transcode("removedValueLinkConnection");

      DIRECTOR_LINK_OUTPUT_OWNER_INDEX_ELEMENT = xercesc::XMLString::transcode("outputOwnerIndex");
      DIRECTOR_LINK_OUTPUT_OWNER_ELEMENT = xercesc::XMLString::transcode("outputOwner");
      DIRECTOR_LINK_INPUT_OWNER_INDEX_ELEMENT = xercesc::XMLString::transcode("inputOwnerIndex");
      DIRECTOR_LINK_INPUT_OWNER_ELEMENT = xercesc::XMLString::transcode("inputOwner");
      DIRECTOR_LINK_OUTPUT_NAME_ELEMENT = xercesc::XMLString::transcode("outputName");
      DIRECTOR_LINK_INPUT_NAME_ELEMENT = xercesc::XMLString::transcode("inputName");

      DIRECTOR_LINK_VISIBLE_ELEMENT = xercesc::XMLString::transcode("visible");
      DIRECTOR_LINK_EXPOSED_ELEMENT = xercesc::XMLString::transcode("exposed");
      DIRECTOR_LINK_VALUE_IS_OUT_ELEMENT = xercesc::XMLString::transcode("isout");
      DIRECTOR_LINK_VALUE_ALLOW_MULTIPLE_ELEMENT = xercesc::XMLString::transcode("allowmultiple");
      DIRECTOR_LINK_VALUE_TYPE_CHECK_ELEMENT = xercesc::XMLString::transcode("typecheck");

      HIERARCHY_ELEMENT = xercesc::XMLString::transcode("drawablehierarchy");
      HIERARCHY_ELEMENT_NODE = xercesc::XMLString::transcode("drawablehierarchynode");

      ACTORS_ELEMENT = xercesc::XMLString::transcode("actors");
      ACTOR_ELEMENT = xercesc::XMLString::transcode("actor");
      ACTOR_TYPE_ELEMENT = xercesc::XMLString::transcode("type");
      ACTOR_ID_ELEMENT = xercesc::XMLString::transcode("id");
      ACTOR_NAME_ELEMENT = xercesc::XMLString::transcode("name");
      ACTOR_PARENT_ID_ELEMENT = xercesc::XMLString::transcode("parentId");
      ACTOR_ENVIRONMENT_ACTOR_ELEMENT = xercesc::XMLString::transcode("environmentActor");
      ACTOR_COMPONENTS_ELEMENT = xercesc::XMLString::transcode("actorComponents");

      ACTOR_PROPERTY_ELEMENT = xercesc::XMLString::transcode("property");
      ACTOR_PROPERTY_NAME_ELEMENT = xercesc::XMLString::transcode("name");
      ACTOR_PROPERTY_STRING_ELEMENT = xercesc::XMLString::transcode("string");
      ACTOR_PROPERTY_ENUM_ELEMENT = xercesc::XMLString::transcode("enumerated");
      ACTOR_PROPERTY_GAMEEVENT_ELEMENT = xercesc::XMLString::transcode("gameevent");
      ACTOR_PROPERTY_FLOAT_ELEMENT = xercesc::XMLString::transcode("float");
      ACTOR_PROPERTY_DOUBLE_ELEMENT = xercesc::XMLString::transcode("double");
      ACTOR_PROPERTY_INTEGER_ELEMENT = xercesc::XMLString::transcode("integer");
      ACTOR_PROPERTY_LONG_ELEMENT = xercesc::XMLString::transcode("long");
      ACTOR_PROPERTY_VEC2_ELEMENT = xercesc::XMLString::transcode("vec2");
      ACTOR_PROPERTY_VEC3_ELEMENT = xercesc::XMLString::transcode("vec3");
      ACTOR_PROPERTY_VEC4_ELEMENT = xercesc::XMLString::transcode("vec4");
      ACTOR_PROPERTY_BOOLEAN_ELEMENT = xercesc::XMLString::transcode("boolean");
      ACTOR_PROPERTY_COLOR_RGB_ELEMENT = xercesc::XMLString::transcode("colorRGB");
      ACTOR_PROPERTY_COLOR_RGBA_ELEMENT = xercesc::XMLString::transcode("colorRGBA");
      ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT = xercesc::XMLString::transcode("resourceType");
      ACTOR_PROPERTY_BIT_MASK_ELEMENT = xercesc::XMLString::transcode("bitMask");
      ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT = xercesc::XMLString::transcode("resourceDisplayName");
      ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT = xercesc::XMLString::transcode("resource");
      ACTOR_PROPERTY_ACTOR_ID_ELEMENT = xercesc::XMLString::transcode("actorId");
      ACTOR_PROPERTY_GROUP_ELEMENT = xercesc::XMLString::transcode("group");
      ACTOR_PROPERTY_ARRAY_ELEMENT = xercesc::XMLString::transcode("array");
      ACTOR_PROPERTY_CONTAINER_ELEMENT = xercesc::XMLString::transcode("container");
      ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT = xercesc::XMLString::transcode("containerSelector");
      ACTOR_PROPERTY_PROPERTY_CONTAINER_ELEMENT = xercesc::XMLString::transcode("propertyContainer");

      ACTOR_PROPERTY_PARAMETER_ELEMENT = xercesc::XMLString::transcode("parameter");

      ACTOR_VEC_1_ELEMENT = xercesc::XMLString::transcode("value1");
      ACTOR_VEC_2_ELEMENT = xercesc::XMLString::transcode("value2");
      ACTOR_VEC_3_ELEMENT = xercesc::XMLString::transcode("value3");
      ACTOR_VEC_4_ELEMENT = xercesc::XMLString::transcode("value4");

      ACTOR_COLOR_R_ELEMENT = xercesc::XMLString::transcode("valueR");
      ACTOR_COLOR_G_ELEMENT = xercesc::XMLString::transcode("valueG");
      ACTOR_COLOR_B_ELEMENT = xercesc::XMLString::transcode("valueB");
      ACTOR_COLOR_A_ELEMENT = xercesc::XMLString::transcode("valueA");

      ACTOR_ARRAY_SIZE_ELEMENT = xercesc::XMLString::transcode("size");
      ACTOR_ARRAY_INDEX_ELEMENT = xercesc::XMLString::transcode("index");
      ACTOR_ARRAY_ELEMENT = xercesc::XMLString::transcode("element");

      ACTOR_GROUPS_ELEMENT = xercesc::XMLString::transcode("groups");
      ACTOR_GROUP_ELEMENT = xercesc::XMLString::transcode("group");
      ACTOR_GROUP_ACTOR_ELEMENT = xercesc::XMLString::transcode("actor");

      PRESET_CAMERAS_ELEMENT = xercesc::XMLString::transcode("presetCameras");
      PRESET_CAMERA_ELEMENT = xercesc::XMLString::transcode("presetCamera");
      PRESET_CAMERA_INDEX_ELEMENT = xercesc::XMLString::transcode("index");
      PRESET_CAMERA_PERSPECTIVE_VIEW_ELEMENT = xercesc::XMLString::transcode("perspectiveView");
      PRESET_CAMERA_TOP_VIEW_ELEMENT = xercesc::XMLString::transcode("topView");
      PRESET_CAMERA_SIDE_VIEW_ELEMENT = xercesc::XMLString::transcode("sideView");
      PRESET_CAMERA_FRONT_VIEW_ELEMENT = xercesc::XMLString::transcode("frontView");
      PRESET_CAMERA_POSITION_X_ELEMENT = xercesc::XMLString::transcode("positionX");
      PRESET_CAMERA_POSITION_Y_ELEMENT = xercesc::XMLString::transcode("positionY");
      PRESET_CAMERA_POSITION_Z_ELEMENT = xercesc::XMLString::transcode("positionZ");
      PRESET_CAMERA_ROTATION_X_ELEMENT = xercesc::XMLString::transcode("rotationX");
      PRESET_CAMERA_ROTATION_Y_ELEMENT = xercesc::XMLString::transcode("rotationY");
      PRESET_CAMERA_ROTATION_Z_ELEMENT = xercesc::XMLString::transcode("rotationZ");
      PRESET_CAMERA_ROTATION_W_ELEMENT = xercesc::XMLString::transcode("rotationW");
      PRESET_CAMERA_ZOOM_ELEMENT = xercesc::XMLString::transcode("zoom");
   }

   void MapXMLConstants::StaticShutdown()
   {
      xercesc::XMLString::release(&BEGIN_XML_DECL);
      xercesc::XMLString::release(&END_XML_DECL);

      xercesc::XMLString::release(&MAP_ELEMENT);
      xercesc::XMLString::release(&MAP_NAMESPACE);

      xercesc::XMLString::release(&PREFAB_ELEMENT);
      xercesc::XMLString::release(&PREFAB_NAMESPACE);

      xercesc::XMLString::release(&DIRECTOR_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_NAMESPACE);

      xercesc::XMLString::release(&HEADER_ELEMENT);
      xercesc::XMLString::release(&NAME_ELEMENT);
      xercesc::XMLString::release(&DESCRIPTION_ELEMENT);
      xercesc::XMLString::release(&AUTHOR_ELEMENT);
      xercesc::XMLString::release(&COMMENT_ELEMENT);
      xercesc::XMLString::release(&COPYRIGHT_ELEMENT);
      xercesc::XMLString::release(&CREATE_TIMESTAMP_ELEMENT);
      xercesc::XMLString::release(&LAST_UPDATE_TIMESTAMP_ELEMENT);
      xercesc::XMLString::release(&EDITOR_VERSION_ELEMENT);
      xercesc::XMLString::release(&SCHEMA_VERSION_ELEMENT);

      xercesc::XMLString::release(&LIBRARIES_ELEMENT);
      xercesc::XMLString::release(&LIBRARY_ELEMENT);
      xercesc::XMLString::release(&LIBRARY_NAME_ELEMENT);
      xercesc::XMLString::release(&LIBRARY_VERSION_ELEMENT);

      xercesc::XMLString::release(&EVENTS_ELEMENT);
      xercesc::XMLString::release(&EVENT_ELEMENT);
      xercesc::XMLString::release(&EVENT_ID_ELEMENT);
      xercesc::XMLString::release(&EVENT_NAME_ELEMENT);
      xercesc::XMLString::release(&EVENT_DESCRIPTION_ELEMENT);

      xercesc::XMLString::release(&DIRECTOR_GRAPH_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_EVENT_NODES_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_ACTION_NODES_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_VALUE_NODES_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_NODE_ELEMENT);
      xercesc::XMLString::release(&CATEGORY_ELEMENT);
      xercesc::XMLString::release(&ID_ELEMENT);
      xercesc::XMLString::release(&ID_INDEX_ELEMENT);

      xercesc::XMLString::release(&DIRECTOR_LINKS_INPUT_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINKS_OUTPUT_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINKS_VALUE_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINK_ELEMENT);

      xercesc::XMLString::release(&DIRECTOR_LINK_CHAIN_CONNECTION);
      xercesc::XMLString::release(&DIRECTOR_LINK_VALUE_CONNECTION);
      xercesc::XMLString::release(&DIRECTOR_REMOVED_LINK_CHAIN_CONNECTION);
      xercesc::XMLString::release(&DIRECTOR_REMOVED_LINK_VALUE_CONNECTION);

      xercesc::XMLString::release(&DIRECTOR_LINK_OUTPUT_OWNER_INDEX_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINK_OUTPUT_OWNER_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINK_INPUT_OWNER_INDEX_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINK_INPUT_OWNER_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINK_OUTPUT_NAME_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINK_INPUT_NAME_ELEMENT);

      xercesc::XMLString::release(&DIRECTOR_LINK_VISIBLE_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINK_EXPOSED_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINK_VALUE_IS_OUT_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINK_VALUE_ALLOW_MULTIPLE_ELEMENT);
      xercesc::XMLString::release(&DIRECTOR_LINK_VALUE_TYPE_CHECK_ELEMENT);

      xercesc::XMLString::release(&ACTORS_ELEMENT);
      xercesc::XMLString::release(&ACTOR_ELEMENT);
      xercesc::XMLString::release(&ACTOR_TYPE_ELEMENT);
      xercesc::XMLString::release(&ACTOR_ID_ELEMENT);
      xercesc::XMLString::release(&ACTOR_NAME_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PARENT_ID_ELEMENT);
      xercesc::XMLString::release(&ACTOR_ENVIRONMENT_ACTOR_ELEMENT);
      xercesc::XMLString::release(&ACTOR_COMPONENTS_ELEMENT);

      xercesc::XMLString::release(&ACTOR_PROPERTY_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_NAME_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_STRING_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_ENUM_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_FLOAT_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_DOUBLE_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_INTEGER_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_LONG_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_VEC2_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_VEC3_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_VEC4_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_BOOLEAN_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_COLOR_RGB_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_COLOR_RGBA_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_BIT_MASK_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_ACTOR_ID_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_GAMEEVENT_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_GROUP_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_ARRAY_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_CONTAINER_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_PROPERTY_CONTAINER_ELEMENT);

      xercesc::XMLString::release(&ACTOR_PROPERTY_PARAMETER_ELEMENT);

      xercesc::XMLString::release(&ACTOR_VEC_1_ELEMENT);
      xercesc::XMLString::release(&ACTOR_VEC_2_ELEMENT);
      xercesc::XMLString::release(&ACTOR_VEC_3_ELEMENT);
      xercesc::XMLString::release(&ACTOR_VEC_4_ELEMENT);

      xercesc::XMLString::release(&ACTOR_COLOR_R_ELEMENT);
      xercesc::XMLString::release(&ACTOR_COLOR_G_ELEMENT);
      xercesc::XMLString::release(&ACTOR_COLOR_B_ELEMENT);
      xercesc::XMLString::release(&ACTOR_COLOR_A_ELEMENT);

      xercesc::XMLString::release(&ACTOR_GROUPS_ELEMENT);
      xercesc::XMLString::release(&ACTOR_GROUP_ELEMENT);
      xercesc::XMLString::release(&ACTOR_GROUP_ACTOR_ELEMENT);

      xercesc::XMLString::release(&PRESET_CAMERAS_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_INDEX_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_PERSPECTIVE_VIEW_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_TOP_VIEW_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_SIDE_VIEW_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_FRONT_VIEW_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_POSITION_X_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_POSITION_Y_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_POSITION_Z_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_ROTATION_X_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_ROTATION_Y_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_ROTATION_Z_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_ROTATION_W_ELEMENT);
      xercesc::XMLString::release(&PRESET_CAMERA_ZOOM_ELEMENT);
   }

   MapXMLConstants::MapXMLConstants() {}
   MapXMLConstants::~MapXMLConstants() {}
   MapXMLConstants::MapXMLConstants(const MapXMLConstants&) {}
   MapXMLConstants& MapXMLConstants::operator=(const MapXMLConstants&) {return *this;}
}
