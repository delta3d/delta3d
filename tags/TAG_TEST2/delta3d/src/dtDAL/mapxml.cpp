/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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

#include <prefix/dtdalprefix.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable : 4267) // for warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned int', possible loss of data
#endif

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#if XERCES_VERSION_MAJOR < 3
#   include <xercesc/internal/XMLGrammarPoolImpl.hpp>
#endif
#include <xercesc/sax/SAXParseException.hpp>

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

#include <osgDB/FileNameUtils>

#include <dtCore/transformable.h>
#include <dtCore/transform.h>

#include <dtDAL/actorhierarchynode.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/map.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventmanager.h>
#include <dtDAL/mapxmlconstants.h>
#include <dtDAL/mapcontenthandler.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/actorpropertyserializer.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datetime.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

#include <iostream>
#include <fstream>

XERCES_CPP_NAMESPACE_USE

namespace dtDAL
{

   static const std::string logName("mapxml.cpp");

   /////////////////////////////////////////////////////////////////
   MapParser::MapParser()
   : BaseXMLParser()
   , mMapHandler(new MapContentHandler())
   {
      SetHandler(mMapHandler.get());

      mXercesParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
      mXercesParser->setFeature(XMLUni::fgXercesDynamic, false);

      mXercesParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
      mXercesParser->setFeature(XMLUni::fgXercesSchema, true);
      mXercesParser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);
      mXercesParser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, true);
      mXercesParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);
      mXercesParser->setFeature(XMLUni::fgXercesCacheGrammarFromParse, true);

      std::string schemaFileName = dtUtil::FindFileInPathList("map.xsd");

      if (!dtUtil::FileUtils::GetInstance().FileExists(schemaFileName))
      {
         throw dtDAL::ProjectException( "Unable to load required file \"map.xsd\", can not load map.", __FILE__, __LINE__);
      }

      XMLCh* value = XMLString::transcode(schemaFileName.c_str());
      LocalFileInputSource inputSource(value);
      //cache the schema
      mXercesParser->loadGrammar(inputSource, Grammar::SchemaGrammarType, true);
      XMLString::release(&value);
   }

   /////////////////////////////////////////////////////////////////
   MapParser::~MapParser()
   {
   }

   /////////////////////////////////////////////////////////////////
   bool MapParser::Parse(const std::string& path, Map** map)
   {
      mMapHandler->SetMapMode();

      std::ifstream  mapfstream(path.c_str(), std::ios_base::binary);
      if (BaseXMLParser::Parse(mapfstream))
      {
         dtCore::RefPtr<Map> mapRef = mMapHandler->GetMap();
         mMapHandler->ClearMap();

         if (map)
         {
            *map = mapRef.release();
         }

         return true;
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////
   bool MapParser::ParsePrefab(const std::string& path, std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> >& proxyList, dtDAL::Map* map)
   {
      mMapHandler->SetPrefabMode(proxyList, dtDAL::MapContentHandler::PREFAB_READ_ALL, map);
      std::ifstream mapfstream(path.c_str());
      if (BaseXMLParser::Parse(mapfstream))
      {
         dtCore::RefPtr<Map> mapRef = mMapHandler->GetMap();
         mMapHandler->ClearMap();
         return true;
      }

      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string MapParser::GetPrefabIconFileName(const std::string& path)
   {
      std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > proxyList; //just an empty list
      std::string iconFileName = "";

      SetParsing(true);
      mMapHandler->SetPrefabMode(proxyList, MapContentHandler::PREFAB_ICON_ONLY);
      mXercesParser->setContentHandler(mMapHandler.get());
      mXercesParser->setErrorHandler(mMapHandler.get());

      std::ifstream fileStream(path.c_str());
      try
      {
         InputSourcefStream xerStream(fileStream);
         mXercesParser->parse(xerStream);
      }
      catch(const dtUtil::Exception& iconFoundWeAreDone)
      {
         //Probably the icon has been found, the exception to stop parsing has
         //been thrown, so there's nothing to do here.  
      }
      
      iconFileName = mMapHandler->GetPrefabIconFileName();
   
      mMapHandler->ClearMap();
      SetParsing(false);

      return iconFileName;
   }
   
   /////////////////////////////////////////////////////////////////
   const std::string MapParser::ParseMapName(const std::string& path)
   {
      //this is a flag that will make sure
      //the parser gets reset if an exception is thrown.
      bool parserNeedsReset = false;
      XMLPScanToken token;
      try
      {
         mXercesParser->setContentHandler(mMapHandler.get());
         mXercesParser->setErrorHandler(mMapHandler.get());

         std::ifstream fileStream(path.c_str());
         InputSourcefStream xerStream(fileStream);

         if (mXercesParser->parseFirst(xerStream, token))
         {
            parserNeedsReset = true;

            bool cont = mXercesParser->parseNext(token);
            while (cont && !mMapHandler->HasFoundMapName())
            {
               cont = mXercesParser->parseNext(token);
            }

            parserNeedsReset = false;
            //reSet the parser and close the file handles.
            mXercesParser->parseReset(token);

            if (mMapHandler->HasFoundMapName())
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing complete.");
               std::string name = mMapHandler->GetMap()->GetName();
               mMapHandler->ClearMap();
               return name;
            }
            else
            {
               throw dtDAL::MapParsingException( "Parser stopped without finding the map name.", __FILE__, __LINE__);
            }
         }
         else
         {
            throw dtDAL::MapParsingException( "Parsing to find the map name did not begin.", __FILE__, __LINE__);
         }
      }
      catch (const OutOfMemoryException&)
      {
         if (parserNeedsReset)
            mXercesParser->parseReset(token);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
         throw dtDAL::MapParsingException( "Ran out of memory parsing save file.", __FILE__, __LINE__);
      }
      catch (const XMLException& toCatch)
      {
         if (parserNeedsReset)
            mXercesParser->parseReset(token);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %ls :\n",
                             toCatch.getMessage());
         throw dtDAL::MapParsingException( "Error while parsing map file. See log for more information.", __FILE__, __LINE__);
      }
      catch (const SAXParseException&)
      {
         if (parserNeedsReset)
            mXercesParser->parseReset(token);

         //this will already by logged by the content handler
         throw dtDAL::MapParsingException( "Error while parsing map file. See log for more information.", __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////
   Map* MapParser::GetMapBeingParsed()
   {
      if (!IsParsing())
      {
         return NULL;
      }

      return mMapHandler->GetMap();
   }

   /////////////////////////////////////////////////////////////////
   const Map* MapParser::GetMapBeingParsed() const
   {
      if (!IsParsing())
      {
         return NULL;
      }

      return mMapHandler->GetMap();
   }

   /////////////////////////////////////////////////////////////////

   const std::set<std::string>& MapParser::GetMissingActorTypes()
   {
      return mMapHandler->GetMissingActorTypes();
   }

   /////////////////////////////////////////////////////////////////

   const std::vector<std::string>& MapParser::GetMissingLibraries()
   {
      return mMapHandler->GetMissingLibraries();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MapParser::HasDeprecatedProperty() const
   {
      return mMapHandler->HasDeprecatedProperty();
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   MapWriter::MapWriter()
      : BaseXMLWriter()
   {
      mPropSerializer = new ActorPropertySerializer(this);
   }

   //////////////////////////////////////////////////////////////////////////
   MapWriter::~MapWriter()
   {
   }

   /////////////////////////////////////////////////////////////////
   void MapWriter::Save(Map& map, const std::string& filePath)
   {
      std::ofstream stream(filePath.c_str(), std::ios_base::trunc|std::ios_base::binary);
      if (!stream.is_open())
      {
         throw dtDAL::MapSaveException( std::string("Unable to open map file \"") + filePath + "\" for writing.", __FILE__, __LINE__);
      }
      Save(map, stream);
   }

   /////////////////////////////////////////////////////////////////
   void MapWriter::Save(Map& map, std::ostream& stream)
   {
      mFormatTarget.SetOutputStream(&stream);

      try {

         mFormatter << MapXMLConstants::BEGIN_XML_DECL << mFormatter.getEncodingName() << MapXMLConstants::END_XML_DECL << chLF;

         const std::string& utcTime = dtUtil::DateTime::ToString(dtUtil::DateTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME),
            dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT);

         BeginElement(MapXMLConstants::MAP_ELEMENT, MapXMLConstants::MAP_NAMESPACE);
         BeginElement(MapXMLConstants::HEADER_ELEMENT);
         BeginElement(MapXMLConstants::NAME_ELEMENT);
         AddCharacters(map.GetName());
         EndElement(); // End Map Name Element.
         BeginElement(MapXMLConstants::DESCRIPTION_ELEMENT);
         AddCharacters(map.GetDescription());
         EndElement(); // End Description Element.
         BeginElement(MapXMLConstants::AUTHOR_ELEMENT);
         AddCharacters(map.GetAuthor());
         EndElement(); // End Author Element.
         BeginElement(MapXMLConstants::COMMENT_ELEMENT);
         AddCharacters(map.GetComment());
         EndElement(); // End Comment Element.
         BeginElement(MapXMLConstants::COPYRIGHT_ELEMENT);
         AddCharacters(map.GetCopyright());
         EndElement(); // End Copyright Element.
         BeginElement(MapXMLConstants::CREATE_TIMESTAMP_ELEMENT);
         if (map.GetCreateDateTime().length() == 0)
         {
            map.SetCreateDateTime(utcTime);
         }
         AddCharacters(map.GetCreateDateTime());
         EndElement(); // End Create Timestamp Element.
         BeginElement(MapXMLConstants::LAST_UPDATE_TIMESTAMP_ELEMENT);
         AddCharacters(utcTime);
         EndElement(); // End Last Update Timestamp Element
         BeginElement(MapXMLConstants::EDITOR_VERSION_ELEMENT);
         AddCharacters(std::string(MapXMLConstants::EDITOR_VERSION));
         EndElement(); // End Editor Version Element.
         BeginElement(MapXMLConstants::SCHEMA_VERSION_ELEMENT);
         AddCharacters(std::string(MapXMLConstants::SCHEMA_VERSION));
         EndElement(); // End Scema Version Element.         
         EndElement(); // End Header Element.

         BeginElement(MapXMLConstants::LIBRARIES_ELEMENT);
         const std::vector<std::string>& libs = map.GetAllLibraries();
         for (std::vector<std::string>::const_iterator i = libs.begin(); i != libs.end(); ++i)
         {
            BeginElement(MapXMLConstants::LIBRARY_ELEMENT);
            BeginElement(MapXMLConstants::LIBRARY_NAME_ELEMENT);
            AddCharacters(*i);
            EndElement(); // End Library Name Element.
            BeginElement(MapXMLConstants::LIBRARY_VERSION_ELEMENT);
            AddCharacters(map.GetLibraryVersion(*i));
            EndElement(); // End Library Version Element.
            EndElement(); // End Library Element.
         }
         EndElement(); // End Libraries Element.

         std::vector<GameEvent* > events;
         map.GetEventManager().GetAllEvents(events);
         if (!events.empty())
         {
            BeginElement(MapXMLConstants::EVENTS_ELEMENT);
            for (std::vector<GameEvent* >::const_iterator i = events.begin(); i != events.end(); ++i)
            {
               BeginElement(MapXMLConstants::EVENT_ELEMENT);
               BeginElement(MapXMLConstants::EVENT_ID_ELEMENT);
               AddCharacters((*i)->GetUniqueId().ToString());
               EndElement(); // End ID Element.
               BeginElement(MapXMLConstants::EVENT_NAME_ELEMENT);
               AddCharacters((*i)->GetName());
               EndElement(); // End Event Name Element.
               BeginElement(MapXMLConstants::EVENT_DESCRIPTION_ELEMENT);
               AddCharacters((*i)->GetDescription());
               EndElement(); // End Event Description Element.
               EndElement(); // End Event Element.
            }
            EndElement(); // End Events Element.
         }

         BeginElement(MapXMLConstants::ACTORS_ELEMENT);

         if (map.GetEnvironmentActor() != NULL)
         {
            BaseActorObject& proxy = *map.GetEnvironmentActor();
            BeginElement(MapXMLConstants::ACTOR_ENVIRONMENT_ACTOR_ELEMENT);
            AddCharacters(proxy.GetId().ToString());
            EndElement(); // End Actor Environment Actor Element.
         }

         const std::map<dtCore::UniqueId, dtCore::RefPtr<BaseActorObject> >& proxies = map.GetAllProxies();
         for (std::map<dtCore::UniqueId, dtCore::RefPtr<BaseActorObject> >::const_iterator i = proxies.begin();
              i != proxies.end(); i++)
         {
            const BaseActorObject& proxy = *i->second.get();
            //printf("Proxy pointer %x\n", &proxy);
            //printf("Actor pointer %x\n", proxy.getActor());

            //ghost proxies arent saved
            //added 7/10/06 -banderegg
            if (proxy.IsGhostProxy())
               continue;

            BeginElement(MapXMLConstants::ACTOR_ELEMENT);
            BeginElement(MapXMLConstants::ACTOR_TYPE_ELEMENT);
            AddCharacters(proxy.GetActorType().GetFullName());
            EndElement(); // End Actor Type Element.
            BeginElement(MapXMLConstants::ACTOR_ID_ELEMENT);
            AddCharacters(proxy.GetId().ToString());
            EndElement(); // End Actor ID Element.
            BeginElement(MapXMLConstants::ACTOR_NAME_ELEMENT);
            AddCharacters(proxy.GetName());
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "Found Proxy Named: %s", proxy.GetName().c_str());
            }
            EndElement(); // End Actor Name Element.
            std::vector<const ActorProperty*> propList;
            proxy.GetPropertyList(propList);
            //int x = 0;
            for (std::vector<const ActorProperty*>::const_iterator i = propList.begin();
                 i != propList.end(); ++i)
            {
               //printf("Printing actor property number %d", x++);
               const ActorProperty& property = *(*i);

               mPropSerializer->WriteProperty(property);

            }
            EndElement(); // End Actor Element.
         }
         EndElement(); // End Actors Element

         BeginElement(MapXMLConstants::ACTOR_GROUPS_ELEMENT);
         {
            int groupCount = map.GetGroupCount();
            for (int groupIndex = 0; groupIndex < groupCount; groupIndex++)
            {
               BeginElement(MapXMLConstants::ACTOR_GROUP_ELEMENT);

               int actorCount = map.GetGroupActorCount(groupIndex);
               for (int actorIndex = 0; actorIndex < actorCount; actorIndex++)
               {
                  dtDAL::BaseActorObject* proxy = map.GetActorFromGroup(groupIndex, actorIndex);
                  if (proxy)
                  {
                     BeginElement(MapXMLConstants::ACTOR_GROUP_ACTOR_ELEMENT);
                     AddCharacters(proxy->GetId().ToString());
                     EndElement(); // End Groups Actor Size Element.
                  }
               }

               EndElement(); // End Group Element.
            }
         }
         EndElement(); // End Groups Element.

         BeginElement(MapXMLConstants::HIERARCHY_ELEMENT);
            dtDAL::ActorHierarchyNode* hier = map.GetDrawableActorHierarchy();
            for(unsigned int i = 0; i < hier->GetNumChildren(); ++i)
            {
               WriteHierarchyBranch(hier->GetChild(i));
            }
         EndElement(); //End Drawable Hierarchy Element

         BeginElement(MapXMLConstants::PRESET_CAMERAS_ELEMENT);
         {
            char numberConversionBuffer[80];

            for (int presetIndex = 0; presetIndex < 10; presetIndex++)
            {
               // Skip elements that are invalid.
               Map::PresetCameraData data = map.GetPresetCameraData(presetIndex);
               if (!data.isValid)
               {
                  continue;
               }

               BeginElement(MapXMLConstants::PRESET_CAMERA_ELEMENT);
               {
                  BeginElement(MapXMLConstants::PRESET_CAMERA_INDEX_ELEMENT);
                  snprintf(numberConversionBuffer, 80, "%d", presetIndex);
                  AddCharacters(numberConversionBuffer);
                  EndElement(); // End Preset Camera Index Element.

                  BeginElement(MapXMLConstants::PRESET_CAMERA_PERSPECTIVE_VIEW_ELEMENT);
                  {
                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_X_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.persPosition.x());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position X Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_Y_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.persPosition.y());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position Y Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_Z_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.persPosition.z());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position Z Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_ROTATION_X_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.persRotation.x());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Rotation X Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_ROTATION_Y_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.persRotation.y());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Rotation Y Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_ROTATION_Z_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.persRotation.z());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Rotation Z Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_ROTATION_W_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.persRotation.w());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Rotation W Element.
                  }
                  EndElement(); // End Preset Camera Perspective View Element.

                  BeginElement(MapXMLConstants::PRESET_CAMERA_TOP_VIEW_ELEMENT);
                  {
                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_X_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.topPosition.x());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position X Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_Y_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.topPosition.y());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position Y Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_Z_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.topPosition.z());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position Z Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_ZOOM_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.topZoom);
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Zoom Element.
                  }
                  EndElement(); // End Preset Camera Top View Element;

                  BeginElement(MapXMLConstants::PRESET_CAMERA_SIDE_VIEW_ELEMENT);
                  {
                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_X_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.sidePosition.x());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position X Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_Y_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.sidePosition.y());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position Y Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_Z_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.sidePosition.z());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position Z Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_ZOOM_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.sideZoom);
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Zoom Element.
                  }
                  EndElement(); // End Preset Camera Side View Element;

                  BeginElement(MapXMLConstants::PRESET_CAMERA_FRONT_VIEW_ELEMENT);
                  {
                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_X_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.frontPosition.x());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position X Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_Y_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.frontPosition.y());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position Y Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_POSITION_Z_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.frontPosition.z());
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Position Z Element.

                     BeginElement(MapXMLConstants::PRESET_CAMERA_ZOOM_ELEMENT);
                     snprintf(numberConversionBuffer, 80, "%f", data.frontZoom);
                     AddCharacters(numberConversionBuffer);
                     EndElement(); // End Preset Camera Zoom Element.
                  }
                  EndElement(); // End Preset Camera Front View Element;
               }
               EndElement(); // End Preset Camera Element.
            }
         }
         EndElement(); // End Preset Camera Element.

         EndElement(); // End Map Element.

         //closes the file.
         mFormatTarget.SetOutputStream(NULL);
      }
      catch (dtUtil::Exception& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Caught Exception \"%s\" while attempting to save map \"%s\".",
                             ex.What().c_str(), map.GetName().c_str());
         mFormatTarget.SetOutputStream(NULL);
         throw ex;
      }
      catch (...)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Unknown exception while attempting to save map \"%s\".",
                             map.GetName().c_str());
         mFormatTarget.SetOutputStream(NULL);
         throw dtDAL::MapSaveException( std::string("Unknown exception saving map \"") + map.GetName() + ("\"."), __FILE__, __LINE__);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MapWriter::WriteHierarchyBranch(dtDAL::ActorHierarchyNode* hierNode)
   {
      std::string idAtt = "actorID='";
      idAtt += hierNode->GetBaseActorObject()->GetId().ToString();
      idAtt += "'";
      XMLCh* unicodeForm = XMLString::transcode(idAtt.c_str());

      BeginElement(MapXMLConstants::HIERARCHY_ELEMENT_NODE, unicodeForm);

      for (unsigned int i = 0; i < hierNode->GetNumChildren(); ++i)
      {
         WriteHierarchyBranch(hierNode->GetChild(i));
      }

      EndElement(); //end HIERARCHY_ELEMENT_NODE
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MapWriter::SavePrefab(const std::vector<dtCore::RefPtr<BaseActorObject> > proxyList,
                              const std::string& filePath, const std::string& description,
                              const std::string& iconFile /* = "" */)
   {
      std::ofstream stream(filePath.c_str(), std::ios_base::trunc|std::ios_base::binary);
      if (!stream.is_open())
      {
         throw dtDAL::MapSaveException( std::string("Unable to open map file \"") + filePath + "\" for writing.", __FILE__, __LINE__);
      }

      mFormatTarget.SetOutputStream(&stream);

      try
      {
         WriteHeader();

         //const std::string& utcTime = dtUtil::DateTime::ToString(dtUtil::DateTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME),
            //dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT);

         BeginElement(MapXMLConstants::PREFAB_ELEMENT, MapXMLConstants::PREFAB_NAMESPACE);

         BeginElement(MapXMLConstants::HEADER_ELEMENT);

         BeginElement(MapXMLConstants::DESCRIPTION_ELEMENT);
         AddCharacters(description);
         EndElement(); // End Description Element.
         BeginElement(MapXMLConstants::EDITOR_VERSION_ELEMENT);
         AddCharacters(std::string(MapXMLConstants::EDITOR_VERSION));
         EndElement(); // End Editor Version Element.
         BeginElement(MapXMLConstants::SCHEMA_VERSION_ELEMENT);
         AddCharacters(std::string(MapXMLConstants::SCHEMA_VERSION));
         EndElement(); // End Schema Version Element.
         BeginElement(MapXMLConstants::ICON_ELEMENT);
         AddCharacters(iconFile);
         EndElement(); //End Icon Element

         EndElement(); // End Header Element.

         BeginElement(MapXMLConstants::LIBRARIES_ELEMENT);
         for (int proxyIndex = 0; proxyIndex < (int)proxyList.size(); proxyIndex++)
         {
            BaseActorObject* proxy = proxyList[proxyIndex].get();

            // We can't do anything without a proxy.
            if (!proxy)
            {
               continue;
            }

            const dtDAL::ActorType& type = proxy->GetActorType();
            dtDAL::ActorPluginRegistry* registry = dtDAL::LibraryManager::GetInstance().GetRegistryForType(type);
            if (registry)
            {
               BeginElement(MapXMLConstants::LIBRARY_ELEMENT);
               BeginElement(MapXMLConstants::LIBRARY_NAME_ELEMENT);
               AddCharacters(dtDAL::LibraryManager::GetInstance().GetLibraryNameForRegistry(registry));
               EndElement(); // End Library Name Element.
               BeginElement(MapXMLConstants::LIBRARY_VERSION_ELEMENT);
               AddCharacters("");
               EndElement(); // End Library Version Element.
               EndElement(); // End Library Element.
            }
         }
         EndElement(); // End Libraries Element.

         osg::Vec3 origin;
         bool originSet = false;
         BeginElement(MapXMLConstants::ACTORS_ELEMENT);
         for (int proxyIndex = 0; proxyIndex < (int)proxyList.size(); proxyIndex++)
         {
            BaseActorObject* proxy = proxyList[proxyIndex].get();

            // We can't do anything without a proxy.
            if (!proxy)
            {
               continue;
            }

            //ghost proxies arent saved
            if (proxy->IsGhostProxy())
               continue;

            // If this is the first proxy, store the translation as the origin.
            dtDAL::TransformableActorProxy* tProxy = dynamic_cast<dtDAL::TransformableActorProxy*>(proxy);
            if (tProxy)
            {
               if (!originSet)
               {
                  origin = tProxy->GetTranslation();
                  originSet = true;
               }

               tProxy->SetTranslation(tProxy->GetTranslation() - origin);
            }

            BeginElement(MapXMLConstants::ACTOR_ELEMENT);
            BeginElement(MapXMLConstants::ACTOR_TYPE_ELEMENT);
            AddCharacters(proxy->GetActorType().GetFullName());
            EndElement(); // End Actor Type Element.
            BeginElement(MapXMLConstants::ACTOR_ID_ELEMENT);
            AddCharacters(proxy->GetId().ToString());
            EndElement(); // End Actor ID Element.
            BeginElement(MapXMLConstants::ACTOR_NAME_ELEMENT);
            AddCharacters(proxy->GetName());
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Found Proxy Named: %s", proxy->GetName().c_str());
            }
            EndElement(); // End Actor Name Element.
            std::vector<const ActorProperty*> propList;
            proxy->GetPropertyList(propList);
            //int x = 0;
            for (std::vector<const ActorProperty*>::const_iterator i = propList.begin();
               i != propList.end(); ++i)
            {
               //printf("Printing actor property number %d", x++);
               const ActorProperty& property = *(*i);

               // If the property is read only, skip it
               if (property.IsReadOnly())
                  continue;

               mPropSerializer->WriteProperty(property);
            }
            EndElement(); // End Actor Element.

            // Now undo the translation.
            if (tProxy && originSet)
            {
               tProxy->SetTranslation(tProxy->GetTranslation() + origin);
            }
         }
         EndElement(); // End Actors Element

         EndElement(); // End Prefab Element.

         //closes the file.
         mFormatTarget.SetOutputStream(NULL);
      }
      catch (dtUtil::Exception& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Caught Exception \"%s\" while attempting to save prefab \"%s\".",
            ex.What().c_str(), filePath.c_str());
         mFormatTarget.SetOutputStream(NULL);
         throw ex;
      }
      catch (...)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Unknown exception while attempting to save prefab \"%s\".",
            filePath.c_str());
         mFormatTarget.SetOutputStream(NULL);
         throw dtDAL::MapSaveException( std::string("Unknown exception saving map \"") + filePath + ("\"."), __FILE__, __LINE__);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
