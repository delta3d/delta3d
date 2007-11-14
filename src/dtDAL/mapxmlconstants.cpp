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

#include <prefix/dtdalprefix-src.h>
#include <dtDAL/mapxmlconstants.h>

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4267) // for warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned int', possible loss of data
#endif

#ifdef _MSC_VER
#	pragma warning(pop)
#endif

namespace dtDAL
{
   /////////////////////////////////////////////////////
   //// Constant Initialization ////////////////////////
   /////////////////////////////////////////////////////


   const char* const MapXMLConstants::EDITOR_VERSION = "1.3.0";
   const char* const MapXMLConstants::SCHEMA_VERSION = "1.0";

   XMLCh* MapXMLConstants::END_XML_ELEMENT = NULL;
   XMLCh* MapXMLConstants::BEGIN_XML_DECL = NULL;
   XMLCh* MapXMLConstants::END_XML_DECL = NULL;

   XMLCh* MapXMLConstants::MAP_ELEMENT = NULL;
   XMLCh* MapXMLConstants::MAP_NAMESPACE = NULL;

   XMLCh* MapXMLConstants::HEADER_ELEMENT = NULL;
   XMLCh* MapXMLConstants::MAP_NAME_ELEMENT = NULL;
   XMLCh* MapXMLConstants::WAYPOINT_CREATE_NAVMESH = NULL;
   XMLCh* MapXMLConstants::WAYPOINT_FILENAME_ELEMENT = NULL;
   XMLCh* MapXMLConstants::DESCRIPTION_ELEMENT = NULL;
   XMLCh* MapXMLConstants::AUTHOR_ELEMENT = NULL;
   XMLCh* MapXMLConstants::COMMENT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::COPYRIGHT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::CREATE_TIMESTAMP_ELEMENT = NULL;
   XMLCh* MapXMLConstants::LAST_UPDATE_TIMESTAMP_ELEMENT = NULL;
   XMLCh* MapXMLConstants::EDITOR_VERSION_ELEMENT = NULL;
   XMLCh* MapXMLConstants::SCHEMA_VERSION_ELEMENT = NULL;

   XMLCh* MapXMLConstants::LIBRARIES_ELEMENT = NULL;
   XMLCh* MapXMLConstants::LIBRARY_ELEMENT = NULL;
   XMLCh* MapXMLConstants::LIBRARY_NAME_ELEMENT = NULL;
   XMLCh* MapXMLConstants::LIBRARY_VERSION_ELEMENT = NULL;

   XMLCh* MapXMLConstants::EVENTS_ELEMENT = NULL;
   XMLCh* MapXMLConstants::EVENT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::EVENT_ID_ELEMENT = NULL;
   XMLCh* MapXMLConstants::EVENT_NAME_ELEMENT = NULL;
   XMLCh* MapXMLConstants::EVENT_DESCRIPTION_ELEMENT = NULL;

   XMLCh* MapXMLConstants::ACTORS_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_TYPE_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_ID_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_NAME_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_ENVIRONMENT_ACTOR_ELEMENT = NULL;

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
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_ACTOR_ID_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_GAMEEVENT_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_GROUP_ELEMENT = NULL;

   XMLCh* MapXMLConstants::ACTOR_PROPERTY_PARAMETER_ELEMENT = NULL;

   XMLCh* MapXMLConstants::ACTOR_VEC_1_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_VEC_2_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_VEC_3_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_VEC_4_ELEMENT = NULL;

   XMLCh* MapXMLConstants::ACTOR_COLOR_R_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_COLOR_G_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_COLOR_B_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_COLOR_A_ELEMENT = NULL;


   void MapXMLConstants::StaticInit()
   {
      END_XML_ELEMENT = xercesc::XMLString::transcode("</");
      BEGIN_XML_DECL = xercesc::XMLString::transcode("<?xml version=\"1.0\" encoding=\"");
      END_XML_DECL = xercesc::XMLString::transcode("\"?>");

      MAP_ELEMENT = xercesc::XMLString::transcode("map");
      MAP_NAMESPACE = xercesc::XMLString::transcode("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"delta3dMap\" xsi:schemaLocation=\"delta3dMap map.xsd\"");

      HEADER_ELEMENT = xercesc::XMLString::transcode("header");
      MAP_NAME_ELEMENT = xercesc::XMLString::transcode("name");
      WAYPOINT_FILENAME_ELEMENT = xercesc::XMLString::transcode("waypointFileName");
      WAYPOINT_CREATE_NAVMESH = xercesc::XMLString::transcode("createNavMesh");
      DESCRIPTION_ELEMENT = xercesc::XMLString::transcode("description");
      AUTHOR_ELEMENT = xercesc::XMLString::transcode("author");
      COMMENT_ELEMENT = xercesc::XMLString::transcode("comment");
      COPYRIGHT_ELEMENT = xercesc::XMLString::transcode("copyright");
      CREATE_TIMESTAMP_ELEMENT = xercesc::XMLString::transcode("createTimestamp");
      LAST_UPDATE_TIMESTAMP_ELEMENT = xercesc::XMLString::transcode("lastUpdateTimestamp");
      EDITOR_VERSION_ELEMENT = xercesc::XMLString::transcode("editorVersion");
      SCHEMA_VERSION_ELEMENT = xercesc::XMLString::transcode("schemaVersion");

      LIBRARIES_ELEMENT = xercesc::XMLString::transcode("libraries");
      LIBRARY_ELEMENT = xercesc::XMLString::transcode("library");
      LIBRARY_NAME_ELEMENT = xercesc::XMLString::transcode("name");
      LIBRARY_VERSION_ELEMENT = xercesc::XMLString::transcode("version");

      EVENTS_ELEMENT = xercesc::XMLString::transcode("events");
      EVENT_ELEMENT = xercesc::XMLString::transcode("event");
      EVENT_ID_ELEMENT = xercesc::XMLString::transcode("id");
      EVENT_NAME_ELEMENT = xercesc::XMLString::transcode("name");
      EVENT_DESCRIPTION_ELEMENT = xercesc::XMLString::transcode("description");

      ACTORS_ELEMENT = xercesc::XMLString::transcode("actors");
      ACTOR_ELEMENT = xercesc::XMLString::transcode("actor");
      ACTOR_TYPE_ELEMENT = xercesc::XMLString::transcode("type");
      ACTOR_ID_ELEMENT = xercesc::XMLString::transcode("id");
      ACTOR_NAME_ELEMENT = xercesc::XMLString::transcode("name");
      ACTOR_ENVIRONMENT_ACTOR_ELEMENT = xercesc::XMLString::transcode("environmentActor");

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
      ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT = xercesc::XMLString::transcode("resourceDisplayName");
      ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT = xercesc::XMLString::transcode("resource");
      ACTOR_PROPERTY_ACTOR_ID_ELEMENT = xercesc::XMLString::transcode("actorId");
      ACTOR_PROPERTY_GROUP_ELEMENT = xercesc::XMLString::transcode("group");

      ACTOR_PROPERTY_PARAMETER_ELEMENT = xercesc::XMLString::transcode("parameter");

      ACTOR_VEC_1_ELEMENT = xercesc::XMLString::transcode("value1");
      ACTOR_VEC_2_ELEMENT = xercesc::XMLString::transcode("value2");
      ACTOR_VEC_3_ELEMENT = xercesc::XMLString::transcode("value3");
      ACTOR_VEC_4_ELEMENT = xercesc::XMLString::transcode("value4");

      ACTOR_COLOR_R_ELEMENT = xercesc::XMLString::transcode("valueR");
      ACTOR_COLOR_G_ELEMENT = xercesc::XMLString::transcode("valueG");
      ACTOR_COLOR_B_ELEMENT = xercesc::XMLString::transcode("valueB");
      ACTOR_COLOR_A_ELEMENT = xercesc::XMLString::transcode("valueA");
   }

   void MapXMLConstants::StaticShutdown()
   {
      xercesc::XMLString::release(&END_XML_ELEMENT);
      xercesc::XMLString::release(&BEGIN_XML_DECL);
      xercesc::XMLString::release(&END_XML_DECL);

      xercesc::XMLString::release(&MAP_ELEMENT);
      xercesc::XMLString::release(&MAP_NAMESPACE);

      xercesc::XMLString::release(&HEADER_ELEMENT);
      xercesc::XMLString::release(&MAP_NAME_ELEMENT);
      xercesc::XMLString::release(&WAYPOINT_FILENAME_ELEMENT);
      xercesc::XMLString::release(&WAYPOINT_CREATE_NAVMESH);
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
      
      xercesc::XMLString::release(&ACTORS_ELEMENT);
      xercesc::XMLString::release(&ACTOR_ELEMENT);
      xercesc::XMLString::release(&ACTOR_TYPE_ELEMENT);
      xercesc::XMLString::release(&ACTOR_ID_ELEMENT);
      xercesc::XMLString::release(&ACTOR_NAME_ELEMENT);
      xercesc::XMLString::release(&ACTOR_ENVIRONMENT_ACTOR_ELEMENT);

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
      xercesc::XMLString::release(&ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_ACTOR_ID_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_GAMEEVENT_ELEMENT);
      xercesc::XMLString::release(&ACTOR_PROPERTY_GROUP_ELEMENT);

      xercesc::XMLString::release(&ACTOR_PROPERTY_PARAMETER_ELEMENT);

      xercesc::XMLString::release(&ACTOR_VEC_1_ELEMENT);
      xercesc::XMLString::release(&ACTOR_VEC_2_ELEMENT);
      xercesc::XMLString::release(&ACTOR_VEC_3_ELEMENT);
      xercesc::XMLString::release(&ACTOR_VEC_4_ELEMENT);

      xercesc::XMLString::release(&ACTOR_COLOR_R_ELEMENT);
      xercesc::XMLString::release(&ACTOR_COLOR_G_ELEMENT);
      xercesc::XMLString::release(&ACTOR_COLOR_B_ELEMENT);
      xercesc::XMLString::release(&ACTOR_COLOR_A_ELEMENT);
   }

   MapXMLConstants::MapXMLConstants() {}
   MapXMLConstants::~MapXMLConstants() {}
   MapXMLConstants::MapXMLConstants(const MapXMLConstants&) {}
   MapXMLConstants& MapXMLConstants::operator=(const MapXMLConstants&) {return *this;}
}
