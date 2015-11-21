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

#include <dtUtil/deprecationmgr.h>
DT_DISABLE_WARNING_ALL_START

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#if XERCES_VERSION_MAJOR < 3
#   include <xercesc/internal/XMLGrammarPoolImpl.hpp>
#endif
#include <xercesc/sax/SAXParseException.hpp>

#include <osgDB/FileNameUtils>
DT_DISABLE_WARNING_END


#include <dtCore/transformable.h>
#include <dtCore/transform.h>

#include <dtCore/actorhierarchynode.h>
#include <dtCore/actorcomponentcontainer.h>
#include <dtCore/actorpropertyserializer.h>
#include <dtCore/mapxml.h>
#include <dtCore/map.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/actorfactory.h>
#include <dtCore/mapxmlconstants.h>
#include <dtCore/mapcontenthandler.h>
#include <dtCore/project.h>
#include <dtCore/transformableactorproxy.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datetime.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>
#include <dtUtil/mswinmacros.h>

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

         bool ParseMap(std::istream& str, bool prefab)
         {
            dtCore::RefPtr<MapParser> parser = Project::GetInstance().GetCurrentMapParser();

            if(parser == NULL)
            {
               parser = new MapParser();
            }

            parser->Parse(str, &mMapPtr, prefab);
            mMap = mMapPtr;


            return mMap.valid();
         }


         bool ParseMapHeader(std::istream& str, bool prefab)
         {
            std::string mapName;

            dtCore::RefPtr<MapParser> parser = Project::GetInstance().GetCurrentMapParser();

            if(parser == NULL)
            {
               parser = new MapParser();
            }

            mMap = parser->ParseMapHeaderData(str, prefab);
            mMapPtr = mMap.get();

            return mMap.valid();
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
         supportsExtension(Map::MAP_FILE_EXTENSION,"delta3d map file format");
         supportsExtension(Map::PREFAB_FILE_EXTENSION,"delta3d prefab file format");
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

               bool headerOnly = options != nullptr && !options->getPluginStringData("DELTA3D_HEADER").empty();
               bool prefab = options != nullptr && !options->getPluginStringData("DELTA3D_PREFAB").empty();

               //using this option will keep the parser from parsing the whole map
               if (headerOnly)
               {
                  result = ms->ParseMapHeader(fileStream, prefab);
               }
               else
               {
                  result = ms->ParseMap(fileStream, prefab);
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

         bool headerOnly = !options->getPluginStringData("DELTA3D_HEADER").empty();
         bool prefab = !options->getPluginStringData("DELTA3D_PREFAB").empty();

         //using this option will keep the parser from parsing the whole map
         if (headerOnly)
         {
            result = ms->ParseMapHeader(fin, prefab);
         }
         else
         {
            result = ms->ParseMap(fin, prefab);
         }

         if (result)
         {
            return osgDB::ReaderWriter::ReadResult(ms);
         }

         return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
      }

   };


   REGISTER_OSGPLUGIN(dtmap, MapReaderWriter)


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
   bool MapParser::Parse(const std::string& path, Map** map, bool prefab)
   {
      bool result = false;
      dtCore::RefPtr<MapReaderWriter::MapStream> mapStreamObject;

      //if the map is an .xml file we must load it manually
      //temporarily here to support non .dtmap extensions
      bool isBackupExt = false;
      std::string fileExt = osgDB::getLowerCaseFileExtension(path);
      if (fileExt == "backup")
      {
         fileExt = osgDB::getLowerCaseFileExtension(osgDB::getNameLessExtension(path));
         if(fileExt == "xml" || fileExt == Map::MAP_FILE_EXTENSION)
         {
            isBackupExt = true;
         }
      }

      if(!prefab && (isBackupExt || fileExt == "xml"))
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
         osgDB::Registry* reg = osgDB::Registry::instance();
         //setting this option will keep the map reader writer from having to parse the whole map
         osg::ref_ptr<osgDB::ReaderWriter::Options> options = reg->getOptions() ?
            static_cast<osgDB::ReaderWriter::Options*>(reg->getOptions()->clone(osg::CopyOp::SHALLOW_COPY)) :
         new osgDB::ReaderWriter::Options;

         if (prefab)
            options->setPluginStringData("DELTA3D_PREFAB", "1");

         dtCore::RefPtr<osg::Object> obj = dtUtil::FileUtils::GetInstance().ReadObject(path, options);
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
   bool MapParser::Parse(std::istream& stream, Map** map, bool prefab)
   {
      if (!prefab)
         mMapHandler->SetMapMode();
      else
         mMapHandler->SetPrefabMode();

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
   MapPtr MapParser::ParseMapHeaderData(std::istream& stream, bool prefab) const
   {
      bool parserNeedsReset = false;
      XMLPScanToken token;

      if (!prefab)
         mMapHandler->SetMapMode();
      else
         mMapHandler->SetPrefabMode();

      mXercesParser->setContentHandler(mMapHandler.get());
      mXercesParser->setErrorHandler(mMapHandler.get());

      MapPtr result;

      try
      {
         InputSourcefStream xerStream(stream);

         if (mXercesParser->parseFirst(xerStream, token))
         {
            parserNeedsReset = true;

            bool cont = mXercesParser->parseNext(token);
            while (cont && !mMapHandler->HasParsedHeader())
            {
               cont = mXercesParser->parseNext(token);
            }

            parserNeedsReset = false;
            //reSet the parser and close the file handles.
            mXercesParser->parseReset(token);

            if (mMapHandler->HasParsedHeader())
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing complete.");
               result = mMapHandler->GetMap();
               mMapHandler->ClearMap();
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
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   MapPtr MapParser::ParseMapHeaderData(const std::string& path, bool prefab) const
   {
      osgDB::Registry* reg = osgDB::Registry::instance();
      dtCore::RefPtr<MapReaderWriter::MapStream> mapStreamObject;

      //setting this option will keep the map reader writer from having to parse the whole map
      osg::ref_ptr<osgDB::ReaderWriter::Options> options = reg->getOptions() ?
         static_cast<osgDB::ReaderWriter::Options*>(reg->getOptions()->clone(osg::CopyOp::SHALLOW_COPY)) :
      new osgDB::ReaderWriter::Options;

      options->setPluginStringData("DELTA3D_HEADER", "1");
      if (prefab)
         options->setPluginStringData("DELTA3D_PREFAB", "1");

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

      MapPtr result;
      if(mapStreamObject.valid() && mapStreamObject->mMap.valid())
      {
         result = mapStreamObject->mMap;
      }
      else
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! Unable to parse map name.");
         throw dtCore::MapParsingException( "Error while parsing map file. See log for more information.", __FILE__, __LINE__);
      }
      return result;

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
   void MapWriter::Save(Map& map, const std::string& filePath, bool prefab)
   {
      std::ofstream stream(filePath.c_str(), std::ios_base::trunc|std::ios_base::binary);
      if (!stream.is_open())
      {
         throw dtCore::MapSaveException( std::string("Unable to open map file \"") + filePath + "\" for writing.", __FILE__, __LINE__);
      }
      Save(map, stream, prefab);
   }

   /////////////////////////////////////////////////////////////////////////////
   void MapWriter::Save(Map& map, std::ostream& stream, bool prefab)
   {
      map.CorrectLibraryList(false);
      mFormatTarget.SetOutputStream(&stream);
      mPropSerializer->Reset();
      mPropSerializer->SetMap(&map);

      try
      {
         mFormatter << MapXMLConstants::BEGIN_XML_DECL << mFormatter.getEncodingName() << MapXMLConstants::END_XML_DECL << chLF;

         const std::string& utcTime = dtUtil::DateTime::ToString(dtUtil::DateTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME),
            dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT);

         if (prefab)
            BeginElement(MapXMLConstants::PREFAB_ELEMENT, MapXMLConstants::PREFAB_NAMESPACE);
         else
            BeginElement(MapXMLConstants::MAP_ELEMENT, MapXMLConstants::MAP_NAMESPACE);

         BeginElement(MapXMLConstants::HEADER_ELEMENT);
         BeginElement(MapXMLConstants::NAME_ELEMENT);
         AddCharacters(map.GetName());
         EndElement(); // End Map Name Element.
         BeginElement(MapXMLConstants::DESCRIPTION_ELEMENT);
         AddCharacters(map.GetDescription());
         EndElement(); // End Description Element.
         if (!prefab)
         {
            BeginElement(MapXMLConstants::AUTHOR_ELEMENT);
            AddCharacters(map.GetAuthor());
            EndElement(); // End Author Element.
            BeginElement(MapXMLConstants::COMMENT_ELEMENT);
            AddCharacters(map.GetComment());
            EndElement(); // End Comment Element.
            BeginElement(MapXMLConstants::COPYRIGHT_ELEMENT);
            AddCharacters(map.GetCopyright());
            EndElement(); // End Copyright Element.
         }
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
         if (prefab)
         {
            BeginElement(MapXMLConstants::ICON_ELEMENT);
            AddCharacters(map.GetIconFile());
            EndElement(); //End Icon Element
         }
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

         if (!prefab)
         {
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
         }

         BeginElement(MapXMLConstants::ACTORS_ELEMENT);

         if (!prefab && map.GetEnvironmentActor() != NULL)
         {
            BaseActorObject& proxy = *map.GetEnvironmentActor();
            BeginElement(MapXMLConstants::ACTOR_ENVIRONMENT_ACTOR_ELEMENT);
            AddCharacters(proxy.GetId().ToString());
            EndElement(); // End Actor Environment Actor Element.
         }

         typedef std::map<dtCore::UniqueId, dtCore::RefPtr<BaseActorObject> > ActorMap;
         const ActorMap& actorMap = map.GetAllProxies();

         ActorMap::const_iterator curIter = actorMap.begin();
         for (; curIter != actorMap.end(); ++curIter)
         {
            BaseActorObject* actor = curIter->second.get();
            ActorComponentContainer* compContainer = NULL;
            bool isActorComp = actor->IsActorComponent();
            
            // Attempt a cast if the object is not an ActorComponent.
            if ( ! isActorComp)
            {
               compContainer = dynamic_cast<ActorComponentContainer*>(actor);
            }

            // Skip actors that have parents since they will be
            // written nested within a top level actor definition.
            //ghost proxies arent saved
            //added 7/10/06 -banderegg
            if (actor->IsGhost() || (compContainer != NULL && compContainer->GetParentBaseActor() != NULL))
            {
               continue;
            }

            if(isActorComp)
            {
               LOG_ERROR("Cannot write an ActorComponent \"" + actor->GetName()
                  + "\" (type " + actor->GetActorType().GetName()
                  + ") directly to the map root. The actor component must be contained within an actor.");
            }
            else if (compContainer == NULL)
            {
               LOG_WARNING("Actor \"" + actor->GetName()
                  + "\" (type " + actor->GetActorType().GetName()
                  + ") could not be cast to a component container.");
               
               WriteActor(*actor, true);
            }
            else
            {
               dtCore::RefPtr<ActorComponentContainer::ActorIterator> iter = compContainer->GetIterator();
               
               // Iterate over the current actor and its children in order.
               BaseActorObject* curActor = NULL;
               while ( ! iter->IsAtEnd())
               {
                  curActor = *(*iter);

                  WriteActor(*curActor, true);

                  ++(*iter);
               }
            }
         }
         EndElement(); // End Actors Element

         if (!prefab)
         {
            BeginElement(MapXMLConstants::ACTOR_GROUPS_ELEMENT);
            {
               int groupCount = map.GetGroupCount();
               for (int groupIndex = 0; groupIndex < groupCount; groupIndex++)
               {
                  BeginElement(MapXMLConstants::ACTOR_GROUP_ELEMENT);

                  int actorCount = map.GetGroupActorCount(groupIndex);
                  for (int actorIndex = 0; actorIndex < actorCount; actorIndex++)
                  {
                     dtCore::BaseActorObject* actor = map.GetActorFromGroup(groupIndex, actorIndex);
                     if (actor != NULL)
                     {
                        BeginElement(MapXMLConstants::ACTOR_GROUP_ACTOR_ELEMENT);
                        AddCharacters(actor->GetId().ToString());
                        EndElement(); // End Groups Actor Size Element.
                     }
                  }

                  EndElement(); // End Group Element.
               }
            }
            EndElement(); // End Groups Element.
         }

         /*dtCore::ActorComponentContainer* hier
            = dynamic_cast<dtCore::ActorComponentContainer*>(map.GetDrawableActorHierarchy());
         if (hier != NULL)
         {
            BeginElement(MapXMLConstants::HIERARCHY_ELEMENT);
               dtCore::ActorComponentContainer::ActorIterator iter = hier->GetIterator();
            
               while ( ! iter.IsAtEnd())
               {
                  dtCore::ActorComponentContainer* actor
                     = dynamic_cast<dtCore::ActorComponentContainer*>(*iter);
                  if (actor != NULL)
                  {
                     WriteHierarchyBranch(*actor);
                  }
               }
            EndElement(); //End Drawable Hierarchy Element
         }*/

         if (!prefab)
         {
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
         }

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
   /*void MapWriter::WriteHierarchyBranch(dtCore::ActorComponentContainer& actor)
   {
      std::string idAtt = "actorID='";
      idAtt += actor.GetId().ToString();
      idAtt += "'";
      XMLCh* unicodeForm = XMLString::transcode(idAtt.c_str());

      BeginElement(MapXMLConstants::HIERARCHY_ELEMENT_NODE, unicodeForm);

      dtCore::ActorComponentContainer::Iterator iter = actor.GetIterator();
      dtCore::ActorComponentContainer* curActor = NULL;
      while ( ! iter.IsAtEnd())
      {
         curActor = dynamic_cast<dtCore::ActorComponentContainer*>(*iter);

         if (curActor != NULL)
         {
            WriteHierarchyBranch(*curActor);
         }

         ++iter;
      }

      EndElement(); //end HIERARCHY_ELEMENT_NODE
   }*/


   /////////////////////////////////////////////////////////////////////////////
   int MapWriter::WriteActor(dtCore::BaseActorObject& actor, bool allowReadOnlyProps)
   {
      // Ghost actors should not be saved.
      if (actor.IsGhost())
      {
         return 0;
      }

      int results = 0;

      mPropSerializer->PushPropertyContainer(actor);

      BeginElement(MapXMLConstants::ACTOR_ELEMENT);

         // TYPE
         BeginElement(MapXMLConstants::ACTOR_TYPE_ELEMENT);
         AddCharacters(actor.GetActorType().GetFullName());
         EndElement(); // End Actor Type Element.

         // ID
         const std::string& id = actor.GetId().ToString();
         BeginElement(MapXMLConstants::ACTOR_ID_ELEMENT);
         AddCharacters(id);
         EndElement(); // End Actor ID Element.

         // NAME
         const std::string& name = actor.GetName();
         BeginElement(MapXMLConstants::ACTOR_NAME_ELEMENT);
         AddCharacters(name);
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Writing Actor Named: %s [%s]", name.c_str(), id.c_str());
         }
         EndElement(); // End Actor Name Element.

         // PARENT ID
         dtCore::ActorComponentContainer* compContainer = dynamic_cast<dtCore::ActorComponentContainer*>(&actor);
         if (compContainer != NULL)
         {
            BaseActorObject* parent = compContainer->GetParentBaseActor();
            if (parent != NULL)
            {
               BeginElement(MapXMLConstants::ACTOR_PARENT_ID_ELEMENT);
               AddCharacters(parent->GetId().ToString());
               EndElement(); // End Actor Component Element.
            }
         }


         // ACTOR COMPONENTS
         if (compContainer != NULL)
         {
            dtCore::ActorPtrVector comps;
            compContainer->GetAllComponents(comps);

            if ( ! comps.empty())
            {
               BeginElement(MapXMLConstants::ACTOR_COMPONENTS_ELEMENT);
               
               dtCore::BaseActorObject* curComp = NULL;
               dtCore::ActorPtrVector::iterator curIter = comps.begin();
               dtCore::ActorPtrVector::iterator endIter = comps.end();
               for (; curIter != endIter; ++curIter)
               {
                  curComp = *curIter;

                  WriteActor(*curComp, allowReadOnlyProps);
               }

               EndElement(); // End Actor Component Element.
            }
         }


         // CHILDREN (Nested)
         // Child components come before direct properties so that all
         // components exsist before deprecated properties are handled.
         /*if (compContainer != NULL)
         {
            ActorComponentContainer::ActorIterator iter = compContainer->GetIterator();
            
            // Skip the current parent actor.
            ++iter;

            if ( ! iter.IsAtEnd())
            {
               BeginElement(MapXMLConstants::ACTOR_CHILDREN_ELEMENT);

               // Iterate over the child actors.
               while ( ! iter.IsAtEnd())
               {
                  BaseActorObject* child = *iter;

                  WriteActor(*child, allowReadOnlyProps);

                  ++iter;
               }

               EndElement(); // End Actor Children Element.
         }*/


         // PROPERTIES
         typedef std::vector<const ActorProperty*> PropertyList;
         PropertyList propList;
         actor.GetPropertyList(propList);

         PropertyList::const_iterator curIter = propList.begin();
         for (; curIter != propList.end(); ++curIter)
         {
            const ActorProperty* prop = *curIter;

            // If the property is read only and read only is not allowed,
            // skip the property and go to the next.
            if (prop->IsReadOnly() && ! allowReadOnlyProps)
            {
               continue;
            }

            mPropSerializer->WriteProperty(*prop);

         }

      EndElement(); // End Actor Element.

      mPropSerializer->PopPropertyContainer();

      return results;
   }

}


/////////////////////////////////////////////////////////////////////////////

