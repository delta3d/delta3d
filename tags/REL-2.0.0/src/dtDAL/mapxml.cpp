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
#include <prefix/dtdalprefix-src.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>
#include <cmath>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4267) // for warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned int', possible loss of data
#endif

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/internal/XMLGrammarPoolImpl.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include <osgDB/FileNameUtils>

#include <dtCore/globals.h>

#include <dtDAL/mapxml.h>
#include <dtDAL/map.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actortype.h>
#include <dtDAL/datatype.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventmanager.h>
#include <dtDAL/namedparameter.h>
#include <dtDAL/mapxmlconstants.h>
#include <dtDAL/mapcontenthandler.h>

#include <dtUtil/fileutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

XERCES_CPP_NAMESPACE_USE;

namespace dtDAL
{

   static const std::string logName("mapxml.cpp");

   /////////////////////////////////////////////////////////////////

   void MapParser::StaticInit()
   {
      try
      {
         XMLPlatformUtils::Initialize();
      }
      catch (const XMLException& toCatch)
      {
         //if this happens, something is very very wrong.
         char* message = XMLString::transcode( toCatch.getMessage() );
         std::string msg(message);
         LOG_ERROR("Error during parser initialization!: "+ msg)
            XMLString::release( &message );
         return;
      }
   }

   /////////////////////////////////////////////////////////////////

   void MapParser::StaticShutdown()
   {
      //This causes too many problems and it in only called at app shutdown
      //so the memory leak in not a problem.
      //XMLPlatformUtils::Terminate();
   }

   /////////////////////////////////////////////////////////////////

   Map* MapParser::Parse(const std::string& path)
   {
      try
      {
         mParsing = true;
         mXercesParser->setContentHandler(mHandler.get());
         mXercesParser->setErrorHandler(mHandler.get());
         mXercesParser->parse(path.c_str());
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing complete.\n");
         dtCore::RefPtr<Map> mapRef = mHandler->GetMap();
         mHandler->ClearMap();
         mParsing = false;
         return mapRef.release();
      }
      catch (const OutOfMemoryException&)
      {
         mParsing = false;
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
         throw dtUtil::Exception(dtDAL::ExceptionEnum::MapLoadParsingError, "Ran out of memory parsing save file.", __FILE__, __LINE__);
      }
      catch (const XMLException& toCatch)
      {
         mParsing = false;
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %ls :\n",
                             toCatch.getMessage());
         throw dtUtil::Exception(dtDAL::ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.", __FILE__, __LINE__);
      }
      catch (const SAXParseException&)
      {
         mParsing = false;
         //this will already by logged by the
         throw dtUtil::Exception(dtDAL::ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.", __FILE__, __LINE__);
      }
      return NULL;
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
         mXercesParser->setContentHandler(mHandler.get());
         mXercesParser->setErrorHandler(mHandler.get());

         if (mXercesParser->parseFirst(path.c_str(), token))
         {
            parserNeedsReset = true;

            bool cont = mXercesParser->parseNext(token);
            while (cont && !mHandler->HasFoundMapName())
            {
               cont = mXercesParser->parseNext(token);
            }

            parserNeedsReset = false;
            //reSet the parser and close the file handles.
            mXercesParser->parseReset(token);

            if (mHandler->HasFoundMapName())
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing complete.");
               std::string name = mHandler->GetMap()->GetName();
               mHandler->ClearMap();
               return name;
            }
            else
            {
               throw dtUtil::Exception(dtDAL::ExceptionEnum::MapLoadParsingError, "Parser stopped without finding the map name.", __FILE__, __LINE__);
            }
         }
         else
         {
            throw dtUtil::Exception(dtDAL::ExceptionEnum::MapLoadParsingError, "Parsing to find the map name did not begin.", __FILE__, __LINE__);
         }
      }
      catch (const OutOfMemoryException&)
      {
         if (parserNeedsReset)
            mXercesParser->parseReset(token);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
         throw dtUtil::Exception(dtDAL::ExceptionEnum::MapLoadParsingError, "Ran out of memory parsing save file.", __FILE__, __LINE__);
      }
      catch (const XMLException& toCatch)
      {
         if (parserNeedsReset)
            mXercesParser->parseReset(token);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %ls :\n",
                             toCatch.getMessage());
         throw dtUtil::Exception(dtDAL::ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.", __FILE__, __LINE__);
      }
      catch (const SAXParseException&)
      {
         if (parserNeedsReset)
            mXercesParser->parseReset(token);

         //this will already by logged by the content handler
         throw dtUtil::Exception(dtDAL::ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.", __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////
   Map* MapParser::GetMapBeingParsed()
   {
      if (!IsParsing())
      {
         return NULL;
      }
      
      return mHandler->GetMap();
   }

   /////////////////////////////////////////////////////////////////
   const Map* MapParser::GetMapBeingParsed() const
   {
      if (!IsParsing())
      {
         return NULL;
      }
      
      return mHandler->GetMap();
   }

   /////////////////////////////////////////////////////////////////
   MapParser::MapParser() :
      mHandler(new MapContentHandler()),
      mParsing(false)
   { 
      mLogger = &dtUtil::Log::GetInstance(logName);

      mXercesParser = XMLReaderFactory::createXMLReader();

      mXercesParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
      mXercesParser->setFeature(XMLUni::fgXercesDynamic, false);

      mXercesParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
      mXercesParser->setFeature(XMLUni::fgXercesSchema, true);
      mXercesParser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);
      mXercesParser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, true);
      mXercesParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);
      mXercesParser->setFeature(XMLUni::fgXercesCacheGrammarFromParse, true);

      std::string schemaFileName = dtCore::FindFileInPathList("map.xsd");

      if (!dtUtil::FileUtils::GetInstance().FileExists(schemaFileName))
      {
         throw dtUtil::Exception(dtDAL::ExceptionEnum::ProjectException, "Unable to load required file \"map.xsd\", can not load map.", __FILE__, __LINE__);
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
      delete mXercesParser;
   }

   /////////////////////////////////////////////////////////////////

   const std::set<std::string>& MapParser::GetMissingActorTypes() 
   { 
      return mHandler->GetMissingActorTypes(); 
   }
   
   /////////////////////////////////////////////////////////////////

   const std::vector<std::string>& MapParser::GetMissingLibraries() 
   { 
      return mHandler->GetMissingLibraries(); 
   }

   /////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////

   MapWriter::MapFormatTarget::MapFormatTarget(): mOutFile(NULL)
   {
      mLogger = &dtUtil::Log::GetInstance(logName);
   }

   /////////////////////////////////////////////////////////////////

   MapWriter::MapFormatTarget::~MapFormatTarget()
   {
      SetOutputFile(NULL);
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::MapFormatTarget::SetOutputFile(FILE* newFile)
   {
      if (mOutFile != NULL)
         fclose(mOutFile);

      mOutFile = newFile;
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::MapFormatTarget::writeChars(
      const XMLByte* const toWrite,
      const unsigned int count,
      xercesc::XMLFormatter* const formatter)
   {

      if (mOutFile != NULL)
      {
         size_t size = fwrite((char *) toWrite, sizeof(char), (size_t)count, mOutFile);
         if (size < (size_t)count)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                "Error writing to file.  Write count less than expected.");
         }

         //fflush(mOutFile);
      }
      else
      {
         XERCES_STD_QUALIFIER cout.write((char *) toWrite, (int) count);
         XERCES_STD_QUALIFIER cout.flush();
      }
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::MapFormatTarget::flush()
   {
      if (mOutFile != NULL)
      {
         fflush(mOutFile);
      }
      else
      {
         XERCES_STD_QUALIFIER cout.flush();
      }
   }

   //////////////////////////////////////////////////

   MapWriter::MapWriter():
      mFormatter("UTF-8", NULL, &mFormatTarget, XMLFormatter::NoEscapes, XMLFormatter::DefaultUnRep)
   {
      mLogger = &dtUtil::Log::GetInstance(logName);
   }
   /////////////////////////////////////////////////////////////////


   MapWriter::~MapWriter()
   {
   }

   /////////////////////////////////////////////////////////////////

   template <typename VecType>
   void MapWriter::WriteVec(const VecType& vec, char* numberConversionBuffer, const size_t bufferMax)
   {
      switch (VecType::num_components) {
      case 2:
         BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC2_ELEMENT);
         break;
      case 3:
         BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC3_ELEMENT);
         break;
      case 4:
         BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC4_ELEMENT);
         break;
      default:
         //LOG error
         return;
      } 

      BeginElement(MapXMLConstants::ACTOR_VEC_1_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%lf", vec[0]);
      AddCharacters(numberConversionBuffer);
      EndElement();
      
      BeginElement(MapXMLConstants::ACTOR_VEC_2_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%lf", vec[1]);
      AddCharacters(numberConversionBuffer);
      EndElement();
      
      if (VecType::num_components > 2)
      {
         BeginElement(MapXMLConstants::ACTOR_VEC_3_ELEMENT);
         snprintf(numberConversionBuffer, bufferMax, "%lf", vec[2]);
         AddCharacters(numberConversionBuffer);
         EndElement();
      }
      
      if (VecType::num_components > 3)
      {
         BeginElement(MapXMLConstants::ACTOR_VEC_4_ELEMENT);
         snprintf(numberConversionBuffer, bufferMax, "%lf", vec[3]);
         AddCharacters(numberConversionBuffer);
         EndElement();
      }
      
      EndElement();
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::Save(Map& map, const std::string& filePath)
   {
      FILE* outfile = fopen(filePath.c_str(), "w");

      if (outfile == NULL)
      {
         throw dtUtil::Exception(dtDAL::ExceptionEnum::MapSaveError, std::string("Unable to open map file \"") + filePath + "\" for writing.", __FILE__, __LINE__);
      }

      mFormatTarget.SetOutputFile(outfile);

      try {

         mFormatter << MapXMLConstants::BEGIN_XML_DECL << mFormatter.getEncodingName() << MapXMLConstants::END_XML_DECL << chLF;

         time_t currTime;
         time(&currTime);
         const std::string& utcTime = dtUtil::TimeAsUTC(currTime);

         BeginElement(MapXMLConstants::MAP_ELEMENT, MapXMLConstants::MAP_NAMESPACE);
         BeginElement(MapXMLConstants::HEADER_ELEMENT);
         BeginElement(MapXMLConstants::MAP_NAME_ELEMENT);
         AddCharacters(map.GetName());
         EndElement();
         if(!map.GetPathNodeFileName().empty())
         {
            BeginElement(MapXMLConstants::WAYPOINT_FILENAME_ELEMENT);
            AddCharacters(map.GetPathNodeFileName());
            EndElement();

            BeginElement(MapXMLConstants::WAYPOINT_CREATE_NAVMESH);
            AddCharacters(dtUtil::ToString(map.GetCreateNavMesh()));
            EndElement();
         }
         BeginElement(MapXMLConstants::DESCRIPTION_ELEMENT);
         AddCharacters(map.GetDescription());
         EndElement();
         BeginElement(MapXMLConstants::AUTHOR_ELEMENT);
         AddCharacters(map.GetAuthor());
         EndElement();
         BeginElement(MapXMLConstants::COMMENT_ELEMENT);
         AddCharacters(map.GetComment());
         EndElement();
         BeginElement(MapXMLConstants::COPYRIGHT_ELEMENT);
         AddCharacters(map.GetCopyright());
         EndElement();
         BeginElement(MapXMLConstants::CREATE_TIMESTAMP_ELEMENT);
         if (map.GetCreateDateTime().length() == 0)
         {
            map.SetCreateDateTime(utcTime);
         }
         AddCharacters(map.GetCreateDateTime());
         EndElement();
         BeginElement(MapXMLConstants::LAST_UPDATE_TIMESTAMP_ELEMENT);
         AddCharacters(utcTime);
         EndElement();
         BeginElement(MapXMLConstants::EDITOR_VERSION_ELEMENT);
         AddCharacters(std::string(MapXMLConstants::EDITOR_VERSION));
         EndElement();
         BeginElement(MapXMLConstants::SCHEMA_VERSION_ELEMENT);
         AddCharacters(std::string(MapXMLConstants::SCHEMA_VERSION));
         EndElement();
         EndElement();

         BeginElement(MapXMLConstants::LIBRARIES_ELEMENT);
         const std::vector<std::string>& libs = map.GetAllLibraries();
         for (std::vector<std::string>::const_iterator i = libs.begin(); i != libs.end(); ++i)
         {
            BeginElement(MapXMLConstants::LIBRARY_ELEMENT);
            BeginElement(MapXMLConstants::LIBRARY_NAME_ELEMENT);
            AddCharacters(*i);
            EndElement();
            BeginElement(MapXMLConstants::LIBRARY_VERSION_ELEMENT);
            AddCharacters(map.GetLibraryVersion(*i));
            EndElement();
            EndElement();
         }
         EndElement();

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
               EndElement();
               BeginElement(MapXMLConstants::EVENT_NAME_ELEMENT);
               AddCharacters((*i)->GetName());
               EndElement();
               BeginElement(MapXMLConstants::EVENT_DESCRIPTION_ELEMENT);
               AddCharacters((*i)->GetDescription());
               EndElement();
               EndElement();
            }
            EndElement();

         }

         BeginElement(MapXMLConstants::ACTORS_ELEMENT);

         if(map.GetEnvironmentActor() != NULL)
         {
            ActorProxy &proxy = *map.GetEnvironmentActor();
            BeginElement(MapXMLConstants::ACTOR_ENVIRONMENT_ACTOR_ELEMENT);
            AddCharacters(proxy.GetId().ToString());
            EndElement();
         }

         const std::map<dtCore::UniqueId, dtCore::RefPtr<ActorProxy> >& proxies = map.GetAllProxies();
         for (std::map<dtCore::UniqueId, dtCore::RefPtr<ActorProxy> >::const_iterator i = proxies.begin();
              i != proxies.end(); i++)
         {
            const ActorProxy& proxy = *i->second.get();
            //printf("Proxy pointer %x\n", &proxy);
            //printf("Actor pointer %x\n", proxy.getActor());

            //ghost proxies arent saved
            //added 7/10/06 -banderegg
            if(proxy.IsGhostProxy()) 
               continue;

            BeginElement(MapXMLConstants::ACTOR_ELEMENT);
            BeginElement(MapXMLConstants::ACTOR_TYPE_ELEMENT);
            AddCharacters(proxy.GetActorType().ToString());
            EndElement();
            BeginElement(MapXMLConstants::ACTOR_ID_ELEMENT);
            AddCharacters(proxy.GetId().ToString());
            EndElement();
            BeginElement(MapXMLConstants::ACTOR_NAME_ELEMENT);
            AddCharacters(proxy.GetName());
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "Found Proxy Named: %s", proxy.GetName().c_str());
            }
            EndElement();
            std::vector<const ActorProperty*> propList;
            proxy.GetPropertyList(propList);
            //int x = 0;
            for (std::vector<const ActorProperty*>::const_iterator i = propList.begin();
                 i != propList.end(); ++i)
            {
               //printf("Printing actor property number %d", x++);
               const ActorProperty& property = *(*i);

               // If the property is read only, skip it
               if(property.IsReadOnly())
                  continue;
                                 
               WriteProperty(property);

            }
            EndElement();
         }
         EndElement();

         EndElement();
         //closes the file.
         mFormatTarget.SetOutputFile(NULL);
      }
      catch (dtUtil::Exception& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Caught Exception \"%s\" while attempting to save map \"%s\".",
                             ex.What().c_str(), map.GetName().c_str());
         mFormatTarget.SetOutputFile(NULL);
         throw ex;
      }
      catch (...)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Unknown exception while attempting to save map \"%s\".",
                             map.GetName().c_str());
         mFormatTarget.SetOutputFile(NULL);
         throw dtUtil::Exception(dtDAL::ExceptionEnum::MapSaveError, std::string("Unknown exception saving map \"") + map.GetName() + ("\"."), __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::WriteParameter(const NamedParameter& parameter)
   {
      const size_t bufferMax = 512;
      char numberConversionBuffer[bufferMax];
      const DataType& dataType = parameter.GetDataType();

      BeginElement(MapXMLConstants::ACTOR_PROPERTY_PARAMETER_ELEMENT);
      
      BeginElement(MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT);
      AddCharacters(parameter.GetName());
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "Found Parameter of GroupActorProperty Named: %s", parameter.GetName().c_str());
      }
      EndElement();

      switch (dataType.GetTypeId())
      {
         case DataType::FLOAT_ID:
         case DataType::DOUBLE_ID:
         case DataType::INT_ID:
         case DataType::LONGINT_ID:
         case DataType::STRING_ID:
         case DataType::BOOLEAN_ID:
         case DataType::ACTOR_ID:
         case DataType::GAMEEVENT_ID:
         case DataType::ENUMERATION_ID:
            WriteSimple(parameter);
            break;
         case DataType::VEC2_ID:
            WriteVec(static_cast<const NamedVec2Parameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC2F_ID:
            WriteVec(static_cast<const NamedVec2fParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC2D_ID:
            WriteVec(static_cast<const NamedVec2dParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC3_ID:
            WriteVec(static_cast<const NamedVec3Parameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC3F_ID:
            WriteVec(static_cast<const NamedVec3fParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC3D_ID:
            WriteVec(static_cast<const NamedVec3dParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC4_ID:
            WriteVec(static_cast<const NamedVec4Parameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC4F_ID:
            WriteVec(static_cast<const NamedVec4fParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC4D_ID:
            WriteVec(static_cast<const NamedVec4dParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::RGBACOLOR_ID:
            WriteColorRGBA(static_cast<const NamedRGBAColorParameter&>(parameter), numberConversionBuffer, bufferMax);
            break;
         case DataType::GROUP_ID:
         {
            BeginElement(MapXMLConstants::ACTOR_PROPERTY_GROUP_ELEMENT);
            std::vector<const NamedParameter*> parameters;
            static_cast<const NamedGroupParameter&>(parameter).GetParameters(parameters);
            for (size_t i = 0; i < parameters.size(); ++i) 
            {
               WriteParameter(*parameters[i]);
            }
            EndElement();                     
            break;
         }
         default:
         {
            if (dataType.IsResource())
            {
               const NamedResourceParameter& p =
                  static_cast<const NamedResourceParameter&>(parameter);

               const ResourceDescriptor* rd = p.GetValue();

               BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT);
               AddCharacters(parameter.GetDataType().GetName());
               EndElement();

               BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT);
               if (rd != NULL)
                  AddCharacters(rd->GetDisplayName());
               EndElement();
               BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT);
               if (rd != NULL)
                  AddCharacters(rd->GetResourceIdentifier());
               EndElement();
            }
            else
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                    "Unhandled datatype in MapWriter: %s.",
                                    dataType.GetName().c_str());
            }
         }
      }
      //End the parameter element.
      EndElement();
   }

   /////////////////////////////////////////////////////////////////

   template <typename Type>
   void MapWriter::WriteColorRGBA(const Type& holder, char* numberConversionBuffer, size_t bufferMax)
   {
      osg::Vec4f val = holder.GetValue();

      BeginElement(MapXMLConstants::ACTOR_PROPERTY_COLOR_RGBA_ELEMENT);
      
      BeginElement(MapXMLConstants::ACTOR_COLOR_R_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%lf", val[0]);
      AddCharacters(numberConversionBuffer);
      EndElement();
      
      BeginElement(MapXMLConstants::ACTOR_COLOR_G_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%lf", val[1]);
      AddCharacters(numberConversionBuffer);
      EndElement();
      
      BeginElement(MapXMLConstants::ACTOR_COLOR_B_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%lf", val[2]);
      AddCharacters(numberConversionBuffer);
      EndElement();
   
      BeginElement(MapXMLConstants::ACTOR_COLOR_A_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%lf", val[3]);
      AddCharacters(numberConversionBuffer);
      EndElement();

      EndElement();
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::WriteSimple(const AbstractParameter& holder)
   {
      switch (holder.GetDataType().GetTypeId())
      {
         case DataType::FLOAT_ID:
            BeginElement(MapXMLConstants::ACTOR_PROPERTY_FLOAT_ELEMENT);
            break;
         case DataType::DOUBLE_ID:
            BeginElement(MapXMLConstants::ACTOR_PROPERTY_DOUBLE_ELEMENT);
            break;
         case DataType::INT_ID:
            BeginElement(MapXMLConstants::ACTOR_PROPERTY_INTEGER_ELEMENT);
            break;
         case DataType::LONGINT_ID:
            BeginElement(MapXMLConstants::ACTOR_PROPERTY_LONG_ELEMENT);
            break;
         case DataType::STRING_ID:
            BeginElement(MapXMLConstants::ACTOR_PROPERTY_STRING_ELEMENT);
            break;
         case DataType::BOOLEAN_ID:
            BeginElement(MapXMLConstants::ACTOR_PROPERTY_BOOLEAN_ELEMENT);
            break;
         case DataType::ACTOR_ID:
            BeginElement(MapXMLConstants::ACTOR_PROPERTY_ACTOR_ID_ELEMENT);
            break;
         case DataType::GAMEEVENT_ID:
            BeginElement(MapXMLConstants::ACTOR_PROPERTY_GAMEEVENT_ELEMENT);
            break;
         case DataType::ENUMERATION_ID:
            BeginElement(MapXMLConstants::ACTOR_PROPERTY_ENUM_ELEMENT);
            break;
         default:
            //LOG ERROR
            return;
      }
      AddCharacters(holder.ToString());
      EndElement();
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::WriteProperty(const ActorProperty& property)
   {
      const size_t bufferMax = 512;
      char numberConversionBuffer[bufferMax];
      const DataType& propertyType = property.GetDataType();

      BeginElement(MapXMLConstants::ACTOR_PROPERTY_ELEMENT);
      
      BeginElement(MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT);
      AddCharacters(property.GetName());
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "Found Property Named: %s", property.GetName().c_str());
      }
      EndElement();

      switch (propertyType.GetTypeId())
      {
         case DataType::FLOAT_ID:
         case DataType::DOUBLE_ID:
         case DataType::INT_ID:
         case DataType::LONGINT_ID:
         case DataType::STRING_ID:
         case DataType::BOOLEAN_ID:
         case DataType::ACTOR_ID:
         case DataType::GAMEEVENT_ID:
         case DataType::ENUMERATION_ID:
            WriteSimple(property);
            break;
         case DataType::VEC2_ID:
            WriteVec(static_cast<const Vec2ActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC2F_ID:
            WriteVec(static_cast<const Vec2fActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC2D_ID:
            WriteVec(static_cast<const Vec2dActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC3_ID:
            WriteVec(static_cast<const Vec3ActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC3F_ID:
            WriteVec(static_cast<const Vec3fActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC3D_ID:
            WriteVec(static_cast<const Vec3dActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC4_ID:
            WriteVec(static_cast<const Vec4ActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC4F_ID:
            WriteVec(static_cast<const Vec4fActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC4D_ID:
            WriteVec(static_cast<const Vec4dActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::RGBACOLOR_ID:
            WriteColorRGBA(static_cast<const ColorRgbaActorProperty&>(property), numberConversionBuffer, bufferMax);
            break;
         case DataType::GROUP_ID:
         {
            BeginElement(MapXMLConstants::ACTOR_PROPERTY_GROUP_ELEMENT);
            dtCore::RefPtr<NamedGroupParameter> gp = static_cast<const GroupActorProperty&>(property).GetValue();
            if (gp.valid())
            {
               std::vector<const NamedParameter*> parameters;
               gp->GetParameters(parameters);
               for (size_t i = 0; i < parameters.size(); ++i)
               {
                  WriteParameter(*parameters[i]);
               }
            }
            EndElement();
            break;
         }
         default:
         {
            if (propertyType.IsResource())
            {
               const ResourceActorProperty& p =
                  static_cast<const ResourceActorProperty&>(property);

               ResourceDescriptor* rd = p.GetValue();

               BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT);
               AddCharacters(property.GetDataType().GetName());
               EndElement();

               BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT);
               if (rd != NULL)
                  AddCharacters(rd->GetDisplayName());
               EndElement();
               BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT);
               if (rd != NULL)
                  AddCharacters(rd->GetResourceIdentifier());
               EndElement();
            }
            else
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                    "Unhandled datatype in MapWriter: %s.",
                                    propertyType.GetName().c_str());
            }
         }
      }
      
      //end property element
      EndElement();
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::BeginElement(const XMLCh* name, const XMLCh* attributes)
   {
      xmlCharString s(name);
      mElements.push(name);
      AddIndent();

      mFormatter << chOpenAngle << name;
      if (attributes != NULL)
         mFormatter << chSpace << attributes;

      mFormatter << chCloseAngle;
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::EndElement()
   {
      const xmlCharString& name = mElements.top();
      if (mLastCharWasLF)
         AddIndent();

      mFormatter << MapXMLConstants::END_XML_ELEMENT << name.c_str() << chCloseAngle << chLF;
      mLastCharWasLF = true;
      mElements.pop();
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::AddIndent()
   {
      if (!mLastCharWasLF)
         mFormatter << chLF;

      mLastCharWasLF = false;

      size_t indentCount = mElements.size() - 1;

      if (mElements.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Invalid end element when saving a map: ending with no beginning.");
         
         indentCount = 0;
      }
      for (size_t x = 0; x < indentCount; x++)
      {
         for (int y = 0; y < MapWriter::indentSize; y++)
            mFormatter << chSpace;
      }
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::AddCharacters(const xmlCharString& string)
   {
      mLastCharWasLF = false;
      mFormatter << string.c_str();
   }

   /////////////////////////////////////////////////////////////////

   void MapWriter::AddCharacters(const std::string& string)
   {
      mLastCharWasLF = false;
      XMLCh * stringX = XMLString::transcode(string.c_str());
      mFormatter << stringX;
      XMLString::release(&stringX);
   }
}
