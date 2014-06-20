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

#include <prefix/dtcoreprefix.h>
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

#include <dtCore/actorhierarchynode.h>
#include <dtCore/actorpropertyserializer.h>
#include <dtCore/mapxml.h>
#include <dtCore/map.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/librarymanager.h>
#include <dtCore/mapxmlconstants.h>
#include <dtCore/mapcontenthandler.h>
#include <dtCore/mapheaderhandler.h>
#include <dtCore/prefabiconhandler.h>
#include <dtCore/project.h>
#include <dtCore/transformableactorproxy.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datetime.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>


#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgDB/ReadFile>
#include <osgDB/Registry>

#include <iostream>
#include <fstream>

XERCES_CPP_NAMESPACE_USE

namespace dtCore
{
   /////////////////////////////////////////////////////////////////
   //this class is used as a wrapper to read the map files through osgdb
   //which will support loading through archives such as .zip files
   class MapReaderWriter : public osgDB::ReaderWriter
   {
   private:
      //this is a temporary workaround to make maps with xml extension load
      bool mShouldIgnoreExtension;

   public:

      class MapStream : public osg::Object
      {
      public:
         MapStream(): mMapPtr(NULL){};

         bool ParseMap(std::istream& str)
         {
            dtCore::RefPtr<MapParser> parser = Project::GetInstance().GetCurrentMapParser();

            if(parser == NULL)
            {
               parser = new MapParser();
            }

            parser->Parse(str, &mMapPtr);
            mMap = mMapPtr;


            return mMap.valid();
         }

         std::string ParseMapName(std::istream& str)
         {
            std::string mapName;

            dtCore::RefPtr<MapParser> parser = Project::GetInstance().GetCurrentMapParser();

            if(parser == NULL)
            {
               parser = new MapParser();
            }

            mapName = parser->ParseMapName(str);
            if(!mapName.empty())
            {
               //only the name will be valid
               mMap = new Map(mapName, mapName);

               mMapPtr = mMap.get();
            }

            return mapName;
         }

         virtual const char* libraryName() const {return "dtCore";};
         virtual const char* className() const {return "MapReaderWriter";}

         virtual osg::Object* cloneType() const{ return new MapStream();}
         virtual osg::Object* clone(const osg::CopyOp&) const {return new MapStream();}

         Map* mMapPtr;
         dtCore::RefPtr<Map> mMap;
      };

      MapReaderWriter()
         : mShouldIgnoreExtension(false)
      {
         supportsExtension("dtmap","Delta3D map file format");
      }

      void SetShouldIgnoreExtension(bool b)
      {
         mShouldIgnoreExtension = b;
      }

      bool GetShouldIgnoreExtension() const
      {
         return mShouldIgnoreExtension;
      }

      const char* className() const
      {
         return "Delta3D Map Reader/Writer";
      }

      osgDB::ReaderWriter::ReadResult readObject(const std::string& fileName, const osgDB::ReaderWriter::Options* options = NULL) const
      {
         std::string ext = osgDB::getLowerCaseFileExtension(fileName);

         //this is temporarily to support maps that do not have a .dtmap extension
         if (mShouldIgnoreExtension || acceptsExtension(ext))
         {
            std::ifstream fileStream;
            fileStream.open(fileName.c_str());
            if(!fileStream.fail())
            {
               dtCore::RefPtr<MapStream> ms = new MapStream();
               bool result = false;

               //using this option will keep the parser from parsing the whole map
               if(options != NULL && !(options->getPluginStringData("DELTA3D_PARSEMAPNAME")).empty())
               {
                  std::string name = ms->ParseMapName(fileStream);
                  result = !name.empty();
               }
               else
               {
                  result = ms->ParseMap(fileStream);
               }

               if(result)
               {
                  return osgDB::ReaderWriter::ReadResult(ms);
               }
            }

            return ReadResult::ERROR_IN_READING_FILE;
         }
         else
         {
            return ReadResult::FILE_NOT_HANDLED;
         }
      }


      osgDB::ReaderWriter::ReadResult readObject(std::istream& fin, const osgDB::ReaderWriter::Options* options = NULL) const
      {
         dtCore::RefPtr<MapStream> ms = new MapStream();

         bool result = false;
         //using this option will keep the parser from parsing the whole map
         if (options != NULL && !(options->getPluginStringData("DELTA3D_PARSEMAPNAME").empty()))
         {
            std::string name = ms->ParseMapName(fin);
            result = !name.empty();
         }
         else
         {
            result = ms->ParseMap(fin);
         }

         if (result)
         {
            return osgDB::ReaderWriter::ReadResult(ms);
         }

         return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
      }

   };


   REGISTER_OSGPLUGIN(dtmap, MapReaderWriter)


   static const std::string logName("mapxml.cpp");

   /////////////////////////////////////////////////////////////////////////////
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
         throw dtCore::ProjectException( "Unable to load required file \"map.xsd\", can not load map.", __FILE__, __LINE__);
      }

      XMLCh* value = XMLString::transcode(schemaFileName.c_str());
      LocalFileInputSource inputSource(value);
      //cache the schema
      mXercesParser->loadGrammar(inputSource, Grammar::SchemaGrammarType, true);
      XMLString::release(&value);
   }

   /////////////////////////////////////////////////////////////////////////////
   MapParser::~MapParser()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   bool MapParser::Parse(const std::string& path, Map** map)
   {
      bool result = false;
      dtCore::RefPtr<MapReaderWriter::MapStream> mapStreamObject;

      //if the map is an .xml file we must load it manually
      //temporarily here to support non .dtmap extensions
      bool isBackupExt = false;
      std::string fileExt = osgDB::getLowerCaseFileExtension(path);
      if(fileExt == "backup")
      {
         fileExt = osgDB::getLowerCaseFileExtension(osgDB::getNameLessExtension(path));
         if(fileExt == "xml" || fileExt == "dtmap")
         {
            isBackupExt = true;
         }
      }

      if(isBackupExt || fileExt == "xml")
      {
         std::string filename = dtUtil::FindFileInPathList(path);
         if(!filename.empty())
         {
            dtCore::RefPtr<MapReaderWriter> mrw = new MapReaderWriter();

            mrw->SetShouldIgnoreExtension(true);
            osgDB::ReaderWriter::ReadResult readMapResult = mrw->readObject(filename);

            if(readMapResult.success())
            {
               mapStreamObject = dynamic_cast<MapReaderWriter::MapStream*>(readMapResult.getObject());
            }
         }

      }
      else
      {
         dtCore::RefPtr<osg::Object> obj = dtUtil::FileUtils::GetInstance().ReadObject(path);
         if(obj.valid())
         {
            mapStreamObject = dynamic_cast<MapReaderWriter::MapStream*>(obj.get());
         }
      }

      if(mapStreamObject.valid() && mapStreamObject->mMap.valid())
      {
         dtCore::RefPtr<Map> mapRef = mapStreamObject->mMap;
         mapStreamObject->mMap = NULL;
         *map = mapRef.release();

         result = true;
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool MapParser::Parse(std::istream& stream, Map** map)
   {
      mMapHandler->SetMapMode();

      if (BaseXMLParser::Parse(stream))
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

   /////////////////////////////////////////////////////////////////////////////
   bool MapParser::ParsePrefab(const std::string& path, std::vector<dtCore::RefPtr<dtCore::BaseActorObject> >& proxyList, dtCore::Map* map)
   {
      mMapHandler->SetPrefabMode(proxyList, map);
      std::ifstream mapfstream(path.c_str());
      if (BaseXMLParser::Parse(mapfstream))
      {
         dtCore::RefPtr<Map> mapRef = mMapHandler->GetMap();
         mMapHandler->ClearMap();
         return true;
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string MapParser::GetPrefabIconFileName(const std::string& path)
   {
      dtCore::RefPtr<PrefabIconHandler> handler = new PrefabIconHandler();
      if (!ParseFileByToken(path, handler))
      {
         //error
         throw dtCore::MapParsingException( "Could not parse the Prefab's icon name.", __FILE__, __LINE__);
      }

      return handler->GetIconName();
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string MapParser::ParseMapName(std::istream& stream)
   {
      bool parserNeedsReset = false;
      XMLPScanToken token;

      mXercesParser->setContentHandler(mMapHandler.get());
      mXercesParser->setErrorHandler(mMapHandler.get());


      try
      {
         InputSourcefStream xerStream(stream);

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
               throw dtCore::MapParsingException( "Parser stopped without finding the map name.", __FILE__, __LINE__);
            }
         }
         else
         {
            throw dtCore::MapParsingException( "Parsing to find the map name did not begin.", __FILE__, __LINE__);
         }
      }
      catch (const OutOfMemoryException&)
      {
         if (parserNeedsReset)
         {
            mXercesParser->parseReset(token);
         }

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
         throw dtCore::MapParsingException( "Ran out of memory parsing save file.", __FILE__, __LINE__);
      }
      catch (const XMLException& toCatch)
      {
         if (parserNeedsReset)
            mXercesParser->parseReset(token);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %ls :\n",
            toCatch.getMessage());
         throw dtCore::MapParsingException( "Error while parsing map file. See log for more information.", __FILE__, __LINE__);
      }
      catch (const SAXParseException&)
      {
         if (parserNeedsReset)
         {
            mXercesParser->parseReset(token);
         }

         //this will already by logged by the content handler
         throw dtCore::MapParsingException( "Error while parsing map file. See log for more information.", __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string MapParser::ParseMapName(const std::string& path)
   {
      osgDB::Registry* reg = osgDB::Registry::instance();
      dtCore::RefPtr<MapReaderWriter::MapStream> mapStreamObject;

      //setting this option will keep the map reader writer from having to parse the whole map
      osg::ref_ptr<osgDB::ReaderWriter::Options> options = reg->getOptions() ?
         static_cast<osgDB::ReaderWriter::Options*>(reg->getOptions()->clone(osg::CopyOp::SHALLOW_COPY)) :
      new osgDB::ReaderWriter::Options;

      options->setPluginStringData("DELTA3D_PARSEMAPNAME", "1");

      //if the map is an .xml file we must load it manually
      //this is a temporary workaround and should be deprecated
      if(osgDB::getLowerCaseFileExtension(path) == "xml")
      {
         std::string filename = dtUtil::FindFileInPathList(path);
         if(!filename.empty())
         {
            dtCore::RefPtr<MapReaderWriter> mrw = new MapReaderWriter();
            mrw->SetShouldIgnoreExtension(true);

            osgDB::ReaderWriter::ReadResult readMapResult = mrw->readObject(filename, options);

            if(readMapResult.success())
            {
               mapStreamObject = dynamic_cast<MapReaderWriter::MapStream*>(readMapResult.getObject());
            }
         }
      }
      else
      {
         dtCore::RefPtr<osg::Object> obj = dtUtil::FileUtils::GetInstance().ReadObject(path, options);
         if(obj.valid())
         {
            mapStreamObject = dynamic_cast<MapReaderWriter::MapStream*>(obj.get());
         }
      }

      if(mapStreamObject.valid() && mapStreamObject->mMap.valid())
      {
         std::string mapName = mapStreamObject->mMap->GetName();
         return mapName;
      }
      else
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! Unable to parse map name.");
         throw dtCore::MapParsingException( "Error while parsing map file. See log for more information.", __FILE__, __LINE__);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::MapHeaderData MapParser::ParseMapHeaderData(const std::string& mapFilename) const
   {
      dtCore::RefPtr<MapHeaderHandler> handler = new MapHeaderHandler();
      try
      {
         if (!ParseFileByToken(mapFilename, handler))
         {
            throw dtCore::MapParsingException( "Could not parse the Map's header data.", __FILE__, __LINE__);
         }
      }
      catch (dtCore::XMLLoadParsingException& ex)
      {
         throw dtCore::MapParsingException( "Could not parse the Map's header data with dtCore::XMLLoadParsingException: " + ex.ToString(), __FILE__, __LINE__);
      }

      return handler->GetHeaderData();
   }

   /////////////////////////////////////////////////////////////////////////////
   Map* MapParser::GetMapBeingParsed()
   {
      if (!IsParsing())
      {
         return NULL;
      }

      return mMapHandler->GetMap();
   }

   /////////////////////////////////////////////////////////////////////////////
   const Map* MapParser::GetMapBeingParsed() const
   {
      if (!IsParsing())
      {
         return NULL;
      }

      return mMapHandler->GetMap();
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::set<std::string>& MapParser::GetMissingActorTypes()
   {
      return mMapHandler->GetMissingActorTypes();
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::vector<std::string>& MapParser::GetMissingLibraries()
   {
      return mMapHandler->GetMissingLibraries();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool MapParser::HasDeprecatedProperty() const
   {
      return mMapHandler->HasDeprecatedProperty();
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////


   /////////////////////////////////////////////////////////////////////////////
   MapWriter::MapWriter()
      : BaseXMLWriter()
      , mPropSerializer(NULL)
   {
      mPropSerializer = new ActorPropertySerializer(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   MapWriter::~MapWriter()
   {
      delete mPropSerializer; mPropSerializer = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void MapWriter::Save(Map& map, const std::string& filePath)
   {
      std::ofstream stream(filePath.c_str(), std::ios_base::trunc|std::ios_base::binary);
      if (!stream.is_open())
      {
         throw dtCore::MapSaveException( std::string("Unable to open map file \"") + filePath + "\" for writing.", __FILE__, __LINE__);
      }
      Save(map, stream);
   }

   /////////////////////////////////////////////////////////////////////////////
   void MapWriter::Save(Map& map, std::ostream& stream)
   {
      mFormatTarget.SetOutputStream(&stream);
      mPropSerializer->Reset();
      mPropSerializer->SetMap(&map);

      try
      {
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

            mPropSerializer->SetCurrentPropertyContainer(i->second.get());

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
                  dtCore::BaseActorObject* proxy = map.GetActorFromGroup(groupIndex, actorIndex);
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
            dtCore::ActorHierarchyNode* hier = map.GetDrawableActorHierarchy();
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
         mPropSerializer->SetMap(NULL);
      }
      catch (dtUtil::Exception& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Caught Exception \"%s\" while attempting to save map \"%s\".",
                             ex.What().c_str(), map.GetName().c_str());
         mFormatTarget.SetOutputStream(NULL);
         mPropSerializer->SetMap(NULL);
         throw;
      }
      catch (...)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Unknown exception while attempting to save map \"%s\".",
                             map.GetName().c_str());
         mFormatTarget.SetOutputStream(NULL);
         mPropSerializer->SetMap(NULL);
         throw dtCore::MapSaveException( std::string("Unknown exception saving map \"") + map.GetName() + ("\"."), __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void MapWriter::WriteHierarchyBranch(dtCore::ActorHierarchyNode* hierNode)
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

   /////////////////////////////////////////////////////////////////////////////
   void MapWriter::SavePrefab(const std::vector<dtCore::RefPtr<BaseActorObject> > proxyList,
                              const std::string& filePath, const std::string& description,
                              const std::string& iconFile /* = "" */)
   {
      std::ofstream stream(filePath.c_str(), std::ios_base::trunc|std::ios_base::binary);
      if (!stream.is_open())
      {
         throw dtCore::MapSaveException( std::string("Unable to open map file \"") + filePath + "\" for writing.", __FILE__, __LINE__);
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

            const dtCore::ActorType& type = proxy->GetActorType();
            dtCore::ActorPluginRegistry* registry = dtCore::LibraryManager::GetInstance().GetRegistryForType(type);
            if (registry)
            {
               BeginElement(MapXMLConstants::LIBRARY_ELEMENT);
               BeginElement(MapXMLConstants::LIBRARY_NAME_ELEMENT);
               AddCharacters(dtCore::LibraryManager::GetInstance().GetLibraryNameForRegistry(registry));
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
            dtCore::TransformableActorProxy* tProxy = dynamic_cast<dtCore::TransformableActorProxy*>(proxy);
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

            // Initialize the serializer to write out prefab properties
            mPropSerializer->Reset();
            mPropSerializer->SetCurrentPropertyContainer(proxy);

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
         throw;
      }
      catch (...)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Unknown exception while attempting to save prefab \"%s\".",
            filePath.c_str());
         mFormatTarget.SetOutputStream(NULL);
         throw dtCore::MapSaveException( std::string("Unknown exception saving map \"") + filePath + ("\"."), __FILE__, __LINE__);
      }
   }
}


/////////////////////////////////////////////////////////////////////////////

