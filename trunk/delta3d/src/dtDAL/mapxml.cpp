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
 * @author David Guthrie
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <math.h>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4267) // for warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned int', possible loss of data
#endif

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/internal/XMLGrammarPoolImpl.hpp>

#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include <dtCore/scene.h>

#include <osg/Vec2f>
#include <osg/Vec2d>
#include <osg/Vec3f>
#include <osg/Vec3d>

#include <osgDB/FileNameUtils>

#include "dtDAL/mapxml.h"
#include "dtDAL/librarymanager.h"
#include "dtDAL/enginepropertytypes.h"

#include <dtUtil/fileutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/xercesutils.h>

XERCES_CPP_NAMESPACE_USE;

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #ifndef snprintf
      #define snprintf _snprintf
   #endif
#endif

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

   void MapParser::StaticShutdown()
   {
      //This causes too many problems and it in only called at app shutdown
      //so the memory leak in not a problem.
      //XMLPlatformUtils::Terminate();
   }

   Map* MapParser::Parse(const std::string& path)
   {
      try
      {
         mXercesParser->setContentHandler(mHandler.get());
         mXercesParser->setErrorHandler(mHandler.get());
         mXercesParser->parse(path.c_str());
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing complete.\n");
         osg::ref_ptr<Map> mapRef = mHandler->GetMap();
         mHandler->ClearMap();
         return mapRef.release();
      }
      catch (const OutOfMemoryException&)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
         EXCEPT(dtDAL::ExceptionEnum::MapLoadParsingError, "Ran out of memory parsing save file.");
      }
      catch (const XMLException& toCatch)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %ls :\n",
                             toCatch.getMessage());
         EXCEPT(dtDAL::ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.");
      }
      catch (const SAXParseException&)
      {
         //this will already by logged by the
         EXCEPT(dtDAL::ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.");
      }
      return NULL;
   }

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
               EXCEPT(dtDAL::ExceptionEnum::MapLoadParsingError, "Parser stopped without finding the map name.");
            }
         }
         else
         {
            EXCEPT(dtDAL::ExceptionEnum::MapLoadParsingError, "Parsing to find the map name did not begin.");
         }
      }
      catch (const OutOfMemoryException&)
      {
         if (parserNeedsReset)
            mXercesParser->parseReset(token);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
         EXCEPT(dtDAL::ExceptionEnum::MapLoadParsingError, "Ran out of memory parsing save file.");
      }
      catch (const XMLException& toCatch)
      {
         if (parserNeedsReset)
            mXercesParser->parseReset(token);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %ls :\n",
                             toCatch.getMessage());
         EXCEPT(dtDAL::ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.");
      }
      catch (const SAXParseException&)
      {
         if (parserNeedsReset)
            mXercesParser->parseReset(token);

         //this will already by logged by the content handler
         EXCEPT(dtDAL::ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.");
      }
   }

   MapParser::MapParser() :
      mHandler( new MapContentHandler() )
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

      std::string schemaFileName = osgDB::findDataFile("map.xsd");

      if (!dtUtil::FileUtils::GetInstance().FileExists(schemaFileName))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                             "Error, unable to load required file \"map.xsd\".  Aborting.");
         EXCEPT(dtDAL::ExceptionEnum::ProjectException, "Error, unable to load required file \"map.xsd\".  Aborting.");
      }

      XMLCh* value = XMLString::transcode(schemaFileName.c_str());
      LocalFileInputSource inputSource(value);
      //cache the schema
      mXercesParser->loadGrammar(inputSource, Grammar::SchemaGrammarType, true);
      XMLString::release(&value);

   }

   MapParser::~MapParser()
   {
      delete mXercesParser;
   }

   /////////////////////////////////////////////////////////////////

   bool MapContentHandler::IsPropertyCorrectType()
   {
      if (mActorProperty == NULL || mActorPropertyType == NULL || mActorProxy == NULL)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Call made to isPropertyCorrectType with content handler in incorrect state.");
         return false;
      }

      bool result = mActorProperty->GetPropertyType() == *mActorPropertyType;
      if (!result)
      {
         if(mActorProperty->GetPropertyType() == DataType::VEC2D || mActorProperty->GetPropertyType() == DataType::VEC2F ||
            mActorProperty->GetPropertyType() == DataType::VEC3D || mActorProperty->GetPropertyType() == DataType::VEC3F ||
            mActorProperty->GetPropertyType() == DataType::VEC4D || mActorProperty->GetPropertyType() == DataType::VEC4F)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                "Problem differentiating between a osg::vecf/osg::vecd and a osg::vec. Ignoring.");
            return true;
         }

         mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                             "Property %s of actor %s was saved as type %s, but is now of type %s. Data will be ignored",
                             mActorProperty->GetName().c_str(), mActorProxy->GetName().c_str(),
                             mActorPropertyType->GetName().c_str(), mActorProperty->GetPropertyType().GetName().c_str());
      }
      return result;
   }

   void MapContentHandler::characters(const XMLCh* const chars, const unsigned int length)
   {
      xmlCharString& topEl = mElements.top();
      if (mInMap)
      {
         if (mInHeader)
         {
            if (topEl == MapXMLConstants::MAP_NAME_ELEMENT)
            {
               mMap->SetName(dtUtil::XMLStringConverter(chars).ToString());
               //this flag is only used when the parser is just looking for the map name.
               mFoundMapName = true;
            }
            else if (topEl == MapXMLConstants::WAYPOINT_FILENAME_ELEMENT)
            {
               mMap->SetPathNodeFileName(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::DESCRIPTION_ELEMENT)
            {
               mMap->SetDescription(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::AUTHOR_ELEMENT)
            {
               mMap->SetAuthor(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::COMMENT_ELEMENT)
            {
               mMap->SetComment(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::COPYRIGHT_ELEMENT)
            {
               mMap->SetCopyright(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::CREATE_TIMESTAMP_ELEMENT)
            {
               mMap->SetCreateDateTime(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::LAST_UPDATE_TIMESTAMP_ELEMENT)
            {
               //ignored for now
            }
            else if (topEl == MapXMLConstants::EDITOR_VERSION_ELEMENT)
            {
               //ignored for now
            }
            else if (topEl == MapXMLConstants::SCHEMA_VERSION_ELEMENT)
            {
               //ignored - the schema checks this value
            }
         }
         else if (mInLibraries)
         {
            if (topEl == MapXMLConstants::LIBRARY_NAME_ELEMENT)
            {
               mLibName = dtUtil::XMLStringConverter(chars).ToString();
            }
            else if (topEl == MapXMLConstants::LIBRARY_VERSION_ELEMENT)
            {
               mLibVersion = dtUtil::XMLStringConverter(chars).ToString();
            }
         }
         else if (mInActors)
         {
            if(mInActor)
            {
               if(!mIgnoreCurrentActor)
                  ActorCharacters(chars);
            }
            else
            {
               if(topEl == MapXMLConstants::ACTOR_ENVIRONMENT_ACTOR_ELEMENT)
                  mEnvActorId = dtUtil::XMLStringConverter(chars).ToString();
            }
         }
      }

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                             "Found characters for element \"%s\" \"%s\"", dtUtil::XMLStringConverter(topEl.c_str()).c_str(), dtUtil::XMLStringConverter(chars).c_str());
      }
   }

   void MapContentHandler::ActorCharacters(const XMLCh* const chars)
   {
      xmlCharString& topEl = mElements.top();
      if (mInActorProperty)
      {
         if (mActorProxy == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                "Actor proxy is NULL, but code has entered the actor property section");

         }
         else
         {
            if (topEl == MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT)
            {
               std::string propName = dtUtil::XMLStringConverter(chars).ToString();
               mActorProperty = mActorProxy->GetProperty(propName);
               if (mActorProperty == NULL)
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                      "In actor property section, actor property object for name \"%s\" was not found, proxy exists.",
                                      propName.c_str());

            }
            else if (mActorProperty != NULL)
            {
               if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT)
               {
                  std::string resourceTypeString = dtUtil::XMLStringConverter(chars).ToString();
                  for (std::vector<dtUtil::Enumeration*>::const_iterator i = DataType::Enumerate().begin();
                       i != DataType::Enumerate().end(); i++)
                  {
                     DataType* dt = (DataType*)*i;
                     if (dt->GetName() == resourceTypeString)
                        mActorPropertyType = dt;

                  }
                  if (mActorPropertyType == NULL)
                     mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                         "No resource type found for resource type in mMap xml \"%s.\"",
                                         resourceTypeString.c_str());
               }
               else if (mActorPropertyType != NULL)
               {
                  std::string dataValue = dtUtil::XMLStringConverter(chars).ToString();

                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                         "Setting value of property %s, property type %s, datatype %s, value %s, element name %s.",
                                         mActorProperty->GetName().c_str(),
                                         mActorProperty->GetPropertyType().GetName().c_str(),
                                         mActorPropertyType->GetName().c_str(),
                                         dataValue.c_str(), dtUtil::XMLStringConverter(topEl.c_str()).c_str());

                  //we now have the property, the type, and the data.
                  ParseData(dataValue);
               }
            }
         }
      }
      else if (topEl == MapXMLConstants::ACTOR_NAME_ELEMENT)
      {
         if (mActorProxy == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                "Encountered the actor name element with value \"%s\", but actor is NULL.",
                                dtUtil::XMLStringConverter(chars).c_str());
         }
         else
         {
            mActorProxy->SetName(dtUtil::XMLStringConverter(chars).ToString());
         }
      }
      else if (topEl == MapXMLConstants::ACTOR_ID_ELEMENT)
      {
         if (mActorProxy == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                "Encountered the actor id element with value \"%s\", but actor is NULL.",
                                dtUtil::XMLStringConverter(chars).c_str());
         }
         else
            mActorProxy->SetId(dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString()));
      }
      else if (topEl == MapXMLConstants::ACTOR_TYPE_ELEMENT)
      {
         std::string actorTypeFullName = dtUtil::XMLStringConverter(chars).ToString();
         size_t index = actorTypeFullName.find_last_of('.');

         std::string actorTypeCategory;
         std::string actorTypeName;

         if (index == actorTypeFullName.length())
         {
            actorTypeName = actorTypeFullName;
            actorTypeCategory.clear();
         }
         else
         {
            actorTypeName = actorTypeFullName.substr(index + 1);
            actorTypeCategory = actorTypeFullName.substr(0, index);
         }

         //Make sure we have not tried to load this actor type already and failed.
         if (mMissingActorTypes.find(actorTypeFullName) == mMissingActorTypes.end())
         {
            dtCore::RefPtr<ActorType> actorType =
               LibraryManager::GetInstance().FindActorType(actorTypeCategory, actorTypeName);
            if (actorType == NULL)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                                   "ActorType \"%s\" not found.", actorTypeFullName.c_str());
               mMissingActorTypes.insert(actorTypeFullName);
               mIgnoreCurrentActor = true;
            }
            else
            {
               mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                   "Creating actor proxy %s with category %s.",
                                   actorTypeName.c_str(), actorTypeCategory.c_str());

               mActorProxy = LibraryManager::GetInstance().CreateActorProxy(*actorType).get();
               if (mActorProxy == NULL)
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                                      "mActorProxy could not be created for ActorType \"%s\" not found.",
                                      actorTypeFullName.c_str());

            }
         }
         else
         {
            mIgnoreCurrentActor = true;
         }
      }

   }

   void MapContentHandler::ParseData(std::string& dataValue)
   {
      if (!IsPropertyCorrectType())
         return;

      xmlCharString& topEl = mElements.top();

      if (*mActorPropertyType == DataType::BOOLEAN)
      {
         static_cast<BooleanActorProperty&>(*mActorProperty).SetValue(dataValue == "1"
                                                                      || dataValue == "true");
         mActorPropertyType = NULL;
      }
      else if (*mActorPropertyType == DataType::FLOAT)
      {
         mActorProperty->SetStringValue(dataValue.c_str());
         mActorPropertyType = NULL;
      }
      else if (*mActorPropertyType == DataType::DOUBLE)
      {
         mActorProperty->SetStringValue(dataValue.c_str());
         mActorPropertyType = NULL;
      }
      else if (*mActorPropertyType == DataType::INT)
      {
         mActorProperty->SetStringValue(dataValue.c_str());
         mActorPropertyType = NULL;
      }
      else if (*mActorPropertyType == DataType::LONGINT)
      {
         mActorProperty->SetStringValue(dataValue.c_str());
         mActorPropertyType = NULL;
      }
      else if (*mActorPropertyType == DataType::STRING)
      {
         static_cast<StringActorProperty&>(*mActorProperty).SetValue(dataValue);
         mActorPropertyType = NULL;
      }
      else if (*mActorPropertyType == DataType::VEC2)
      {
         DataType &actualType = mActorProperty->GetPropertyType();
         if(actualType == DataType::VEC2)
         {
            Vec2ActorProperty& p = static_cast<Vec2ActorProperty&>(*mActorProperty);

            osg::Vec2 vec = p.GetValue();
            if (topEl == MapXMLConstants::ACTOR_VEC_1_ELEMENT)
            {
               vec[0] = atof(dataValue.c_str());
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_2_ELEMENT)
            {
               vec[1] = atof(dataValue.c_str());
               mActorPropertyType = NULL;
            }
            p.SetValue(vec);
         }
         else if(actualType == DataType::VEC2F)
         {
            Vec2fActorProperty& p = static_cast<Vec2fActorProperty&>(*mActorProperty);
            osg::Vec2f vec = p.GetValue();
            if (topEl == MapXMLConstants::ACTOR_VEC_1_ELEMENT)
            {
               vec[0] = atof(dataValue.c_str());
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_2_ELEMENT)
            {
               vec[1] = atof(dataValue.c_str());
               mActorPropertyType = NULL;
            }
            p.SetValue(vec);
         }
         else if(actualType == DataType::VEC2D)
         {
            Vec2dActorProperty& p = static_cast<Vec2dActorProperty&>(*mActorProperty);
            osg::Vec2d vec = p.GetValue();
            if (topEl == MapXMLConstants::ACTOR_VEC_1_ELEMENT)
            {
               vec[0] = atof(dataValue.c_str());
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_2_ELEMENT)
            {
               vec[1] = atof(dataValue.c_str());
               mActorPropertyType = NULL;
            }
            p.SetValue(vec);
         }
      }
      else if (*mActorPropertyType == DataType::VEC3)
      {
         DataType &actualType = mActorProperty->GetPropertyType();
         if(actualType == DataType::VEC3)
         {
            Vec3ActorProperty& p = static_cast<Vec3ActorProperty&>(*mActorProperty);
            osg::Vec3 vec = p.GetValue();
            char* endMarker;
            if (topEl == MapXMLConstants::ACTOR_VEC_1_ELEMENT)
            {
               vec[0] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_2_ELEMENT)
            {
               vec[1] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_3_ELEMENT)
            {
               vec[2] = strtod(dataValue.c_str(), &endMarker);
               mActorPropertyType = NULL;
            }
            p.SetValue(vec);
         }
         else if(actualType == DataType::VEC3F)
         {
            Vec3fActorProperty& p = static_cast<Vec3fActorProperty&>(*mActorProperty);
            osg::Vec3f vec = p.GetValue();
            char* endMarker;
            if (topEl == MapXMLConstants::ACTOR_VEC_1_ELEMENT)
            {
               vec[0] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_2_ELEMENT)
            {
               vec[1] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_3_ELEMENT)
            {
               vec[2] = strtod(dataValue.c_str(), &endMarker);
               mActorPropertyType = NULL;
            }
            p.SetValue(vec);
         }
         else if(actualType == DataType::VEC3D)
         {
            Vec3dActorProperty& p = static_cast<Vec3dActorProperty&>(*mActorProperty);
            osg::Vec3d vec = p.GetValue();
            char* endMarker;
            if (topEl == MapXMLConstants::ACTOR_VEC_1_ELEMENT)
            {
               vec[0] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_2_ELEMENT)
            {
               vec[1] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_3_ELEMENT)
            {
               vec[2] = strtod(dataValue.c_str(), &endMarker);
               mActorPropertyType = NULL;
            }
            p.SetValue(vec);
         }
      }
      else if (*mActorPropertyType == DataType::VEC4)
      {
         DataType &actualType = mActorProperty->GetPropertyType();

         if(actualType == DataType::VEC4)
         {
            Vec4ActorProperty& p = static_cast<Vec4ActorProperty&>(*mActorProperty);
            osg::Vec4 vec = p.GetValue();
            char* endMarker;
            if (topEl == MapXMLConstants::ACTOR_VEC_1_ELEMENT)
            {
               vec[0] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_2_ELEMENT)
            {
               vec[1] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_3_ELEMENT)
            {
               vec[2] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_4_ELEMENT)
            {
               vec[3] = strtod(dataValue.c_str(), &endMarker);
               mActorPropertyType = NULL;
            }
            p.SetValue(vec);
         }
         else if(actualType == DataType::VEC4F)
         {
            Vec4fActorProperty& p = static_cast<Vec4fActorProperty&>(*mActorProperty);
            osg::Vec4f vec = p.GetValue();
            char* endMarker;
            if (topEl == MapXMLConstants::ACTOR_VEC_1_ELEMENT)
            {
               vec[0] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_2_ELEMENT)
            {
               vec[1] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_3_ELEMENT)
            {
               vec[2] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_4_ELEMENT)
            {
               vec[3] = strtod(dataValue.c_str(), &endMarker);
               mActorPropertyType = NULL;
            }
            p.SetValue(vec);
         }
         else if(actualType == DataType::VEC4D)
         {
            Vec4dActorProperty& p = static_cast<Vec4dActorProperty&>(*mActorProperty);
            osg::Vec4d vec = p.GetValue();
            char* endMarker;
            if (topEl == MapXMLConstants::ACTOR_VEC_1_ELEMENT)
            {
               vec[0] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_2_ELEMENT)
            {
               vec[1] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_3_ELEMENT)
            {
               vec[2] = strtod(dataValue.c_str(), &endMarker);
            }
            else if (topEl == MapXMLConstants::ACTOR_VEC_4_ELEMENT)
            {
               vec[3] = strtod(dataValue.c_str(), &endMarker);
               mActorPropertyType = NULL;
            }
            p.SetValue(vec);
         }
      }
      else if (*mActorPropertyType == DataType::RGBACOLOR)
      {
         osg::Vec4 vec;
         ColorRgbaActorProperty& p = static_cast<ColorRgbaActorProperty&>(*mActorProperty);
         vec = p.GetValue();
         char* endMarker;
         if (topEl == MapXMLConstants::ACTOR_COLOR_R_ELEMENT)
         {
            vec[0] = strtod(dataValue.c_str(), &endMarker);
         }
         else if (topEl == MapXMLConstants::ACTOR_COLOR_G_ELEMENT)
         {
            vec[1] = strtod(dataValue.c_str(), &endMarker);
         }
         else if (topEl == MapXMLConstants::ACTOR_COLOR_B_ELEMENT)
         {
            vec[2] = strtod(dataValue.c_str(), &endMarker);
         }
         else if (topEl == MapXMLConstants::ACTOR_COLOR_A_ELEMENT)
         {
            vec[3] = strtod(dataValue.c_str(), &endMarker);
            mActorPropertyType = NULL;
         }
         p.SetValue(vec);
      }
      else if (*mActorPropertyType == DataType::ENUMERATION)
      {
         if (!mActorProperty->SetStringValue(dataValue.c_str()))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                "Failed Setting value %s for enumerated property type named %s on actor named %s",
                                dataValue.c_str(), mActorProperty->GetName().c_str(), mActorProxy->GetName().c_str());
         }

         mActorPropertyType = NULL;
      }
      else if (*mActorPropertyType == DataType::ACTOR)
      {
         //insert the data into this map to make it accessible to assign once the parsing is done.
         dtUtil::trim(dataValue);
         if (!dataValue.empty() && dataValue != "NULL")
         {
            mActorLinking.insert(std::make_pair(mActorProxy->GetId(), std::make_pair(mActorProperty->GetName(), dtCore::UniqueId(dataValue))));

         }
         mActorPropertyType = NULL;
      }
      else if (*mActorPropertyType == DataType::GAME_EVENT)
      {
         mActorProperty->SetStringValue(dataValue);
         mActorPropertyType = NULL;
      }
      else if (mActorPropertyType->IsResource())
      {
         ResourceActorProperty& p = static_cast<ResourceActorProperty&>(*mActorProperty);
         if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT)
         {
            if (dataValue != p.GetPropertyType().GetName())
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                   "Save file expected resource property %s on actor named %s to have type %s, but it is %s.",
                                   mActorProperty->GetName().c_str(), mActorProxy->GetName().c_str(),
                                   dataValue.c_str(), p.GetPropertyType().GetName().c_str());
               //make it ignore the rest of the mElements.
               p.SetValue(NULL);
               mActorPropertyType = NULL;
            }
         }
         else if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT)
         {
            mDescriptorDisplayName = dataValue;
         }
         else if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT)
         {
            //if the value is null, then both strings will be empty.
            if (dataValue != "" && mDescriptorDisplayName != "")
            {
               ResourceDescriptor rd(mDescriptorDisplayName, dataValue);
               p.SetValue(&rd);
            }
            else
            {
               p.SetValue(NULL);
            }
            mActorPropertyType = NULL;
         }
      }

   }

   void MapContentHandler::startDocument()
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing Map Document Started.\n");
      Reset();
      mMap = new Map("","");
   }

   void MapContentHandler::startElement (
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname,
      const xerces_dt::Attributes& attrs)
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "Found element %s", dtUtil::XMLStringConverter(localname).c_str());
      }

      if (mInMap)
      {
         if (mInLibraries)
         {
            if (XMLString::compareString(localname, MapXMLConstants::LIBRARY_ELEMENT) == 0)
            {
               ClearLibraryValues();
            }
         }
         else if (mInActors)
         {
            if (mInActor)
            {
               if (mInActorProperty)
               {
                  if (mActorProperty != NULL && mActorPropertyType == NULL)
                  {
                     if (XMLString::compareString(localname,
                                                  MapXMLConstants::ACTOR_PROPERTY_BOOLEAN_ELEMENT) == 0)
                     {

                        mActorPropertyType = &DataType::BOOLEAN;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_FLOAT_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::FLOAT;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_DOUBLE_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::DOUBLE;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_INTEGER_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::INT;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_LONG_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::LONGINT;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_VEC2_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::VEC2;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_VEC3_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::VEC3;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_VEC4_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::VEC4;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_STRING_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::STRING;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_COLOR_RGBA_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::RGBACOLOR;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_COLOR_RGB_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::RGBCOLOR;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_ENUM_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::ENUMERATION;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_ACTOR_ID_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::ACTOR;
                     }
                     else if (XMLString::compareString(localname,
                              MapXMLConstants::ACTOR_PROPERTY_GAMEEVENT_ELEMENT) == 0)
                     {
                        mActorPropertyType = &DataType::GAME_EVENT;
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT) == 0)
                     {
                        //Need the character contents to know the type, so this will be
                        //handled later.
                     }
                     else
                     {
                        mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                            "Found actor property data element with name %s, but this does not map to a known type.\n",
                                            dtUtil::XMLStringConverter(localname).c_str());

                     }

                  }
               }
               else if (XMLString::compareString(localname,
                                                 MapXMLConstants::ACTOR_PROPERTY_ELEMENT) == 0)
                  mInActorProperty = true;
            }
            else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_ELEMENT) == 0)
            {
               mInActor = true;
               ClearActorValues();
            }
         }
         else if (!mInHeader && !mInLibraries && !mInActors)
         {
            if (XMLString::compareString(localname, MapXMLConstants::HEADER_ELEMENT) == 0)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Header\n");
               mInHeader = true;
            }
            else if (XMLString::compareString(localname, MapXMLConstants::LIBRARIES_ELEMENT) == 0)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Libraries\n");
               mInLibraries = true;
            }
            else if (XMLString::compareString(localname, MapXMLConstants::ACTORS_ELEMENT) == 0)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Actors\n");
               mInActors = true;
            }
         }

      }
      else if (XMLString::compareString(localname, MapXMLConstants::MAP_ELEMENT) == 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Map\n");
         mInMap = true;
      }
      mElements.push(xmlCharString(localname));
   }

   void MapContentHandler::endElement(
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname) {

      if (mElements.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Attempting to pop elements off of stack and the stack is empty."
                             "it should at least contain element %s.",
                             dtUtil::XMLStringConverter(localname).c_str());
         return;
      }

      const XMLCh* lname = mElements.top().c_str();

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                             "Ending element: \"%s\"", dtUtil::XMLStringConverter(lname).c_str());
      }


      if (0 != XMLString::compareString(lname, localname))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Attempting to pop mElements off of stack and the element "
                             "at the top (%s) is not the same as the element ending (%s).",
                             dtUtil::XMLStringConverter(lname).c_str(), dtUtil::XMLStringConverter(localname).c_str());
      }

      if (mInHeader)
         if (XMLString::compareString(localname, MapXMLConstants::HEADER_ELEMENT) == 0)
            mInHeader = false;
      if (mInLibraries)
      {
         if (XMLString::compareString(localname, MapXMLConstants::LIBRARIES_ELEMENT) == 0)
         {
            mInLibraries = false;
         }
         else if (XMLString::compareString(localname, MapXMLConstants::LIBRARY_ELEMENT) == 0)
         {

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
                                   "Attempting to add library %s version %s to the library manager.",
                                   mLibName.c_str(),
                                   mLibVersion.c_str());
            }

            try
            {
               if (LibraryManager::GetInstance().GetRegistry(mLibName) == NULL)
               {
                  LibraryManager::GetInstance().LoadActorRegistry(mLibName);
               }
               mMap->AddLibrary(mLibName, mLibVersion);
               ClearLibraryValues();
            }
            catch (const dtUtil::Exception& e)
            {
               mMissingLibraries.push_back(mLibName);
               if (dtDAL::ExceptionEnum::ProjectResourceError == e.TypeEnum())
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                      "Error loading library %s version %s in the library manager.  Exception message to follow.",
                                      mLibName.c_str(), mLibVersion.c_str());

               }
               else
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                      "Unknown exception loading library %s version %s in the library manager.  Exception message to follow.",
                                      mLibName.c_str(), mLibVersion.c_str());
               }
               e.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
            }

         }
      }
      else if (mInActors)
      {
         if (XMLString::compareString(localname, MapXMLConstants::ACTORS_ELEMENT) == 0)
         {
            mInActors = false;
            if (mInActor)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                   "Found the closing actors section tag, but the content handler thinks it's still parsing an actor");
               mInActor = false;
            }
         }
         else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_ELEMENT) == 0)
         {
            if (mActorProxy != NULL)
               mMap->AddProxy(*mActorProxy);
            mActorProxy = NULL;
            mInActor = false;
            ClearActorValues();
         }
         else if (mInActor)
         {
            if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_ELEMENT) == 0)
            {
               mInActorProperty = false;
               mActorProperty = NULL;
               mActorPropertyType = NULL;
            }
         }
      }

      mElements.pop();
   }

   void MapContentHandler::endDocument()
   {
      LinkActors();

      if(!mEnvActorId.ToString().empty())
      {
         try
         {
            osg::ref_ptr<ActorProxy> proxy = mMap->GetProxyById(mEnvActorId);
            if(!proxy.valid())
            {
               if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
                  "No environment actor was located in the map.");
               return;
            }
            else
            {
               if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
                  "An environment actor was located in the map.");
            }

            EnvironmentActor *ea = dynamic_cast<EnvironmentActor*>(proxy->GetActor());
            if(ea == NULL)
            {
               EXCEPT(ExceptionEnum::InvalidActorException, "The environment actor proxy's actor should be an environment, but a dynamic_cast failed");
            }
            mMap->SetEnvironmentActor(proxy.get());
         }
         catch(dtUtil::Exception &e)
         {
            LOG_ERROR("Exception caught: " + e.What());
            throw e;
         }
         
      }

      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                          "Parsing Map Document Ended.\n");
   }

   void MapContentHandler::error(const xerces_dt::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                          "ERROR %d:%d - %s:%s - %s", exc.getLineNumber(),
                          exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getMessage()).c_str());
      throw exc;
   }

   void MapContentHandler::fatalError(const xerces_dt::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                          "FATAL-ERROR %d:%d - %s:%s - %s", exc.getLineNumber(),
                          exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getMessage()).c_str());
      throw exc;
   }

   void MapContentHandler::warning(const xerces_dt::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                          "WARNING %d:%d - %s:%s - %s", exc.getLineNumber(),
                          exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getMessage()).c_str());
   }

   void MapContentHandler::resetDocument()
   {
      Reset();
   }

   void MapContentHandler::resetErrors()
   {
      mErrorCount = 0;
      mFatalErrorCount = 0;
      mWarningCount = 0;
      mMissingLibraries.clear();
      mMissingActorTypes.clear();
   }

   void MapContentHandler::Reset()
   {
      mMap = NULL;
      mInMap = false;
      mInHeader = false;
      mInLibraries = false;
      mInActors = false;
      mInActorProperty = false;
      mInActor = false;

      mEnvActorId = "";

      while (!mElements.empty()) mElements.pop();
      mMissingActorTypes.clear();

      ClearLibraryValues();
      ClearActorValues();
      //This should NOT be done in the Actor Value because this should
      //be cleared at the start and finish of a document, not between each actor.
      mActorLinking.clear();

      resetErrors();

      mFoundMapName = false;
   }

   void MapContentHandler::ClearLibraryValues()
   {
      mLibName.clear();
      mLibVersion.clear();
   }

   void MapContentHandler::ClearActorValues()
   {
      mCurrentPropName.clear();
      mActorProxy = NULL;
      mActorPropertyType = NULL;
      mActorProperty = NULL;
      mIgnoreCurrentActor = false;
      mDescriptorDisplayName.clear();
   }

   void MapContentHandler::LinkActors()
   {
      for (std::multimap<dtCore::UniqueId, std::pair<std::string, dtCore::UniqueId> >::iterator i = mActorLinking.begin();
           i != mActorLinking.end(); ++i)
      {
         dtCore::UniqueId id = i->first;
         ActorProxy* proxyToModify = mMap->GetProxyById(id);
         if (proxyToModify == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have an actor property set, but the proxy does not exist in the new map.",
                                id.ToString().c_str());
            continue;
         }
         std::pair<std::string, dtCore::UniqueId> data = i->second;
         std::string& propertyName = data.first;
         dtCore::UniqueId& propValueId = data.second;
         if (propValueId.ToString().empty())
         {
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have actor property %s set, but the id is empty.",
                                id.ToString().c_str(), propertyName.c_str(), propValueId.ToString().c_str());

         }
         ActorProxy* valueProxy = mMap->GetProxyById(propValueId);
         if (valueProxy == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have actor property %s set with actor %s, but the proxy does not exist in the new map.",
                                id.ToString().c_str(), propertyName.c_str(), propValueId.ToString().c_str());
            continue;
         }
         ActorProperty* property = proxyToModify->GetProperty(propertyName);
         if (property == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have actor property %s set with actor %s, but the property does not exist on the proxy.",
                                id.ToString().c_str(), propertyName.c_str(), propValueId.ToString().c_str());
            continue;
         }
         ActorActorProperty* aap = dynamic_cast<ActorActorProperty*>(property);
         if (aap == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have actor property %s set with actor %s, but the property is not an ActorActorProperty.",
                                id.ToString().c_str(), propertyName.c_str(), propValueId.ToString().c_str());
            continue;
         }
         aap->SetValue(valueProxy);
      }

   }


   MapContentHandler::MapContentHandler() : mActorProxy(NULL), mActorPropertyType(NULL), mActorProperty(NULL)
   {
      mLogger = &dtUtil::Log::GetInstance();
      //mLogger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Creating Map Content Handler.\n");
      
      mEnvActorId = "";
   }

   MapContentHandler::~MapContentHandler() {}

   MapContentHandler::MapContentHandler(const MapContentHandler&) {}
   MapContentHandler& MapContentHandler::operator=(const MapContentHandler&) { return *this;}

   void MapContentHandler::startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri) {}

   void MapContentHandler::endPrefixMapping(const XMLCh* const prefix) {}

   void MapContentHandler::skippedEntity(const XMLCh* const name) {}

   void MapContentHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}

   void MapContentHandler::processingInstruction(const XMLCh* const target, const XMLCh* const data) {}


   void MapContentHandler::setDocumentLocator(const xerces_dt::Locator* const locator) {}

   InputSource* MapContentHandler::resolveEntity(const XMLCh* const publicId, const XMLCh* const systemId)
   {
      return NULL;
   }

   /////////////////////////////////////////////////////

   MapWriter::MapFormatTarget::MapFormatTarget(): mOutFile(NULL)
   {
      mLogger = &dtUtil::Log::GetInstance(logName);
   }

   MapWriter::MapFormatTarget::~MapFormatTarget()
   {
      SetOutputFile(NULL);
   }

   void MapWriter::MapFormatTarget::SetOutputFile(FILE* newFile)
   {
      if (mOutFile != NULL)
         fclose(mOutFile);

      mOutFile = newFile;
   }

   void MapWriter::MapFormatTarget::writeChars(
      const XMLByte* const toWrite,
      const unsigned int count,
      xerces_dt::XMLFormatter* const formatter)
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

   MapWriter::~MapWriter()
   {
   }

   void MapWriter::Save(Map& map, const std::string& filePath)
   {
      FILE* outfile = fopen(filePath.c_str(), "w");

      if (outfile == NULL)
      {
         EXCEPT(dtDAL::ExceptionEnum::MapSaveError, std::string("Unable to open map file \"") + filePath + "\" for writing.");
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
         {  BeginElement(MapXMLConstants::WAYPOINT_FILENAME_ELEMENT);
            AddCharacters(map.GetPathNodeFileName());
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
         for (std::vector<std::string>::const_iterator i = libs.begin(); i != libs.end(); i++)
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

         BeginElement(MapXMLConstants::ACTORS_ELEMENT);

         if(map.GetEnvironmentActor() != NULL)
         {
            ActorProxy &proxy = *map.GetEnvironmentActor();
            BeginElement(MapXMLConstants::ACTOR_ENVIRONMENT_ACTOR_ELEMENT);
            AddCharacters(proxy.GetId().ToString());
            EndElement();
         }

         const std::map<dtCore::UniqueId, osg::ref_ptr<ActorProxy> >& proxies = map.GetAllProxies();
         for (std::map<dtCore::UniqueId, osg::ref_ptr<ActorProxy> >::const_iterator i = proxies.begin();
              i != proxies.end(); i++)
         {
            const ActorProxy& proxy = *i->second.get();
            //printf("Proxy pointer %x\n", &proxy);
            //printf("Actor pointer %x\n", proxy.getActor());
            BeginElement(MapXMLConstants::ACTOR_ELEMENT);
            BeginElement(MapXMLConstants::ACTOR_TYPE_ELEMENT);
            AddCharacters(proxy.GetActorType().GetCategory()
                          + '.' + proxy.GetActorType().GetName());
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

               //ghost proxies arent saved
               //added 7/10/06 -banderegg
               if(proxy.IsGhostProxy()) 
                  continue;

               BeginElement(MapXMLConstants::ACTOR_PROPERTY_ELEMENT);
               BeginElement(MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT);
               AddCharacters(property.GetName());
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                      "Found Property Named: %s", property.GetName().c_str());
               }
               EndElement();

               char numberConversionBuffer[512];
               DataType& propertyType = property.GetPropertyType();
               if (propertyType.IsResource())
               {
                  const ResourceActorProperty& p =
                     static_cast<const ResourceActorProperty&>(property);

                  ResourceDescriptor* rd = p.GetValue();

                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT);
                  AddCharacters(property.GetPropertyType().GetName());
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
               else if (propertyType == DataType::BOOLEAN)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_BOOLEAN_ELEMENT);
                  AddCharacters(property.GetStringValue());
                  EndElement();
               }
               else if (propertyType == DataType::DOUBLE)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_DOUBLE_ELEMENT);
                  AddCharacters(property.GetStringValue());
                  EndElement();
               }
               else if (propertyType == DataType::STRING)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_STRING_ELEMENT);
                  AddCharacters(property.GetStringValue());
                  EndElement();
               }
               else if (propertyType == DataType::FLOAT)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_FLOAT_ELEMENT);
                  AddCharacters(property.GetStringValue());
                  EndElement();
               }
               else if (propertyType == DataType::INT)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_INTEGER_ELEMENT);
                  AddCharacters(property.GetStringValue());
                  EndElement();
               }
               else if (propertyType == DataType::LONGINT)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_LONG_ELEMENT);
                  AddCharacters(property.GetStringValue());
                  EndElement();
               }
               else if (propertyType == DataType::VEC2)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC2_ELEMENT);
                  const Vec2ActorProperty& p =
                     static_cast<const Vec2ActorProperty&>(property);
                  osg::Vec2 val = p.GetValue();
                  BeginElement(MapXMLConstants::ACTOR_VEC_1_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[0]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_2_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[1]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  EndElement();
               }
               else if (propertyType == DataType::VEC2F)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC2_ELEMENT);
                  const Vec2fActorProperty& p =
                     static_cast<const Vec2fActorProperty&>(property);
                  osg::Vec2f val = p.GetValue();
                  BeginElement(MapXMLConstants::ACTOR_VEC_1_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[0]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_2_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[1]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  EndElement();
               }
               else if (propertyType == DataType::VEC2D)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC2_ELEMENT);
                  const Vec2dActorProperty& p =
                     static_cast<const Vec2dActorProperty&>(property);
                  osg::Vec2d val = p.GetValue();
                  BeginElement(MapXMLConstants::ACTOR_VEC_1_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[0]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_2_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[1]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  EndElement();
               }
               else if (propertyType == DataType::VEC3)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC3_ELEMENT);
                  const Vec3ActorProperty& p =
                     static_cast<const Vec3ActorProperty&>(property);
                  osg::Vec3 val = p.GetValue();
                  BeginElement(MapXMLConstants::ACTOR_VEC_1_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[0]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_2_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[1]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_3_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[2]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  EndElement();
               }
               else if (propertyType == DataType::VEC3F)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC3_ELEMENT);
                  const Vec3fActorProperty& p =
                     static_cast<const Vec3fActorProperty&>(property);
                  osg::Vec3f val = p.GetValue();
                  BeginElement(MapXMLConstants::ACTOR_VEC_1_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[0]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_2_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[1]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_3_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[2]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  EndElement();
               }
               else if (propertyType == DataType::VEC3D)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC3_ELEMENT);
                  const Vec3dActorProperty& p =
                     static_cast<const Vec3dActorProperty&>(property);
                  osg::Vec3d val = p.GetValue();
                  BeginElement(MapXMLConstants::ACTOR_VEC_1_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[0]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_2_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[1]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_3_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[2]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  EndElement();
               }
               else if (propertyType == DataType::VEC4)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC4_ELEMENT);
                  const Vec4ActorProperty& p =
                     static_cast<const Vec4ActorProperty&>(property);
                  osg::Vec4 val = p.GetValue();
                  BeginElement(MapXMLConstants::ACTOR_VEC_1_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[0]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_2_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[1]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_3_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[2]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_4_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[3]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  EndElement();
               }
               else if (propertyType == DataType::VEC4F)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC4_ELEMENT);
                  const Vec4fActorProperty& p =
                     static_cast<const Vec4fActorProperty&>(property);
                  osg::Vec4f val = p.GetValue();
                  BeginElement(MapXMLConstants::ACTOR_VEC_1_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[0]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_2_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[1]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_3_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[2]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_4_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[3]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  EndElement();
               }
               else if (propertyType == DataType::VEC4D)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC4_ELEMENT);
                  const Vec4dActorProperty& p =
                     static_cast<const Vec4dActorProperty&>(property);
                  osg::Vec4d val = p.GetValue();
                  BeginElement(MapXMLConstants::ACTOR_VEC_1_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[0]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_2_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[1]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_3_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[2]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_VEC_4_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%lf", val[3]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  EndElement();
               }
               else if (propertyType == DataType::RGBACOLOR)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_COLOR_RGBA_ELEMENT);
                  const ColorRgbaActorProperty& p =
                     static_cast<const ColorRgbaActorProperty&>(property);
                  osg::Vec4f val = p.GetValue();
                  BeginElement(MapXMLConstants::ACTOR_COLOR_R_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%f", val[0]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_COLOR_G_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%f", val[1]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_COLOR_B_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%f", val[2]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  BeginElement(MapXMLConstants::ACTOR_COLOR_A_ELEMENT);
                  snprintf(numberConversionBuffer, 512, "%f", val[3]);
                  AddCharacters(numberConversionBuffer);
                  EndElement();
                  EndElement();
               }
               else if (propertyType == DataType::ENUMERATION)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_ENUM_ELEMENT);
                  //hack to work around strange g++ bug.  Adding this line fixes the problem.
                  dynamic_cast<const AbstractEnumActorProperty&>(property);
                  AddCharacters(property.GetStringValue());
                  EndElement();
               }
               else if (propertyType == DataType::ACTOR)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_ACTOR_ID_ELEMENT);
                  AddCharacters(property.GetStringValue());
                  EndElement();
               }
               else if (propertyType == DataType::GAME_EVENT)
               {
                  BeginElement(MapXMLConstants::ACTOR_PROPERTY_GAMEEVENT_ELEMENT);
                  AddCharacters(property.GetStringValue());
                  EndElement();
               }
               else
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                      "Unhandled datatype in MapWriter: %s.",
                                      propertyType.GetName().c_str());
               }
               EndElement();
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
         EXCEPT(dtDAL::ExceptionEnum::MapSaveError, std::string("Unknown exception saving map \"") + map.GetName() + ("\"."));
      }
   }

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

   void MapWriter::EndElement()
   {
      const xmlCharString& name = mElements.top();
      if (mLastCharWasLF)
         AddIndent();

      mFormatter << MapXMLConstants::END_XML_ELEMENT << name.c_str() << chCloseAngle << chLF;
      mLastCharWasLF = true;
      mElements.pop();
   }

   void MapWriter::AddIndent()
   {
      if (!mLastCharWasLF)
         mFormatter << chLF;

      mLastCharWasLF = false;

      size_t indentCount = mElements.size() - 1;
      for (size_t x = 0; x < indentCount; x++)
      {
         for (int y = 0; y < MapWriter::indentSize; y++)
            mFormatter << chSpace;
      }
   }

   void MapWriter::AddCharacters(const xmlCharString& string)
   {
      mLastCharWasLF = false;
      mFormatter << string.c_str();
   }

   void MapWriter::AddCharacters(const std::string& string)
   {
      mLastCharWasLF = false;
      XMLCh * stringX = XMLString::transcode(string.c_str());
      mFormatter << stringX;
      XMLString::release(&stringX);
   }

   /////////////////////////////////////////////////////
   //// Constant Initialization ////////////////////////
   /////////////////////////////////////////////////////


   const char* const MapXMLConstants::EDITOR_VERSION = "1.1.0";
   const char* const MapXMLConstants::SCHEMA_VERSION = "1.0";

   XMLCh* MapXMLConstants::END_XML_ELEMENT = NULL;
   XMLCh* MapXMLConstants::BEGIN_XML_DECL = NULL;
   XMLCh* MapXMLConstants::END_XML_DECL = NULL;

   XMLCh* MapXMLConstants::MAP_ELEMENT = NULL;
   XMLCh* MapXMLConstants::MAP_NAMESPACE = NULL;

   XMLCh* MapXMLConstants::HEADER_ELEMENT = NULL;
   XMLCh* MapXMLConstants::MAP_NAME_ELEMENT = NULL;
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
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_VECTOR_ELEMENT = NULL;
   XMLCh* MapXMLConstants::ACTOR_PROPERTY_GAMEEVENT_ELEMENT = NULL;

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
      END_XML_ELEMENT = XMLString::transcode("</");
      BEGIN_XML_DECL = XMLString::transcode("<?xml version=\"1.0\" encoding=\"");
      END_XML_DECL = XMLString::transcode("\"?>");

      MAP_ELEMENT = XMLString::transcode("map");
      MAP_NAMESPACE = XMLString::transcode("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"delta3dMap\" xsi:schemaLocation=\"delta3dMap map.xsd\"");

      HEADER_ELEMENT = XMLString::transcode("header");
      MAP_NAME_ELEMENT = XMLString::transcode("name");
      WAYPOINT_FILENAME_ELEMENT = XMLString::transcode("waypointFileName");
      DESCRIPTION_ELEMENT = XMLString::transcode("description");
      AUTHOR_ELEMENT = XMLString::transcode("author");
      COMMENT_ELEMENT = XMLString::transcode("comment");
      COPYRIGHT_ELEMENT = XMLString::transcode("copyright");
      CREATE_TIMESTAMP_ELEMENT = XMLString::transcode("createTimestamp");
      LAST_UPDATE_TIMESTAMP_ELEMENT = XMLString::transcode("lastUpdateTimestamp");
      EDITOR_VERSION_ELEMENT = XMLString::transcode("editorVersion");
      SCHEMA_VERSION_ELEMENT = XMLString::transcode("schemaVersion");

      LIBRARIES_ELEMENT = XMLString::transcode("libraries");
      LIBRARY_ELEMENT = XMLString::transcode("library");
      LIBRARY_NAME_ELEMENT = XMLString::transcode("name");
      LIBRARY_VERSION_ELEMENT = XMLString::transcode("version");

      ACTORS_ELEMENT = XMLString::transcode("actors");
      ACTOR_ELEMENT = XMLString::transcode("actor");
      ACTOR_TYPE_ELEMENT = XMLString::transcode("type");
      ACTOR_ID_ELEMENT = XMLString::transcode("id");
      ACTOR_NAME_ELEMENT = XMLString::transcode("name");
      ACTOR_ENVIRONMENT_ACTOR_ELEMENT = XMLString::transcode("environmentActor");

      ACTOR_PROPERTY_ELEMENT = XMLString::transcode("property");
      ACTOR_PROPERTY_NAME_ELEMENT = XMLString::transcode("name");
      ACTOR_PROPERTY_STRING_ELEMENT = XMLString::transcode("string");
      ACTOR_PROPERTY_ENUM_ELEMENT = XMLString::transcode("enumerated");
      ACTOR_PROPERTY_GAMEEVENT_ELEMENT = XMLString::transcode("gameevent");
      ACTOR_PROPERTY_FLOAT_ELEMENT = XMLString::transcode("float");
      ACTOR_PROPERTY_DOUBLE_ELEMENT = XMLString::transcode("double");
      ACTOR_PROPERTY_INTEGER_ELEMENT = XMLString::transcode("integer");
      ACTOR_PROPERTY_LONG_ELEMENT = XMLString::transcode("long");
      ACTOR_PROPERTY_VEC2_ELEMENT = XMLString::transcode("vec2");
      ACTOR_PROPERTY_VEC3_ELEMENT = XMLString::transcode("vec3");
      ACTOR_PROPERTY_VEC4_ELEMENT = XMLString::transcode("vec4");
      ACTOR_PROPERTY_BOOLEAN_ELEMENT = XMLString::transcode("boolean");
      ACTOR_PROPERTY_COLOR_RGB_ELEMENT = XMLString::transcode("colorRGB");
      ACTOR_PROPERTY_COLOR_RGBA_ELEMENT = XMLString::transcode("colorRGBA");
      ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT = XMLString::transcode("resourceType");
      ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT = XMLString::transcode("resourceDisplayName");
      ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT = XMLString::transcode("resource");
      ACTOR_PROPERTY_ACTOR_ID_ELEMENT = XMLString::transcode("actorId");
      ACTOR_PROPERTY_VECTOR_ELEMENT = XMLString::transcode("vector");

      ACTOR_VEC_1_ELEMENT = XMLString::transcode("value1");
      ACTOR_VEC_2_ELEMENT = XMLString::transcode("value2");
      ACTOR_VEC_3_ELEMENT = XMLString::transcode("value3");
      ACTOR_VEC_4_ELEMENT = XMLString::transcode("value4");

      ACTOR_COLOR_R_ELEMENT = XMLString::transcode("valueR");
      ACTOR_COLOR_G_ELEMENT = XMLString::transcode("valueG");
      ACTOR_COLOR_B_ELEMENT = XMLString::transcode("valueB");
      ACTOR_COLOR_A_ELEMENT = XMLString::transcode("valueA");
   }

   void MapXMLConstants::StaticShutdown()
   {
      XMLString::release(&END_XML_ELEMENT);
      XMLString::release(&BEGIN_XML_DECL);
      XMLString::release(&END_XML_DECL);

      XMLString::release(&MAP_ELEMENT);
      XMLString::release(&MAP_NAMESPACE);

      XMLString::release(&HEADER_ELEMENT);
      XMLString::release(&MAP_NAME_ELEMENT);
      XMLString::release(&WAYPOINT_FILENAME_ELEMENT);
      XMLString::release(&DESCRIPTION_ELEMENT);
      XMLString::release(&AUTHOR_ELEMENT);
      XMLString::release(&COMMENT_ELEMENT);
      XMLString::release(&COPYRIGHT_ELEMENT);
      XMLString::release(&CREATE_TIMESTAMP_ELEMENT);
      XMLString::release(&LAST_UPDATE_TIMESTAMP_ELEMENT);
      XMLString::release(&EDITOR_VERSION_ELEMENT);
      XMLString::release(&SCHEMA_VERSION_ELEMENT);

      XMLString::release(&LIBRARIES_ELEMENT);
      XMLString::release(&LIBRARY_ELEMENT);
      XMLString::release(&LIBRARY_NAME_ELEMENT);
      XMLString::release(&LIBRARY_VERSION_ELEMENT);

      XMLString::release(&ACTORS_ELEMENT);
      XMLString::release(&ACTOR_ELEMENT);
      XMLString::release(&ACTOR_TYPE_ELEMENT);
      XMLString::release(&ACTOR_ID_ELEMENT);
      XMLString::release(&ACTOR_NAME_ELEMENT);
      XMLString::release(&ACTOR_ENVIRONMENT_ACTOR_ELEMENT);

      XMLString::release(&ACTOR_PROPERTY_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_NAME_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_STRING_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_ENUM_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_FLOAT_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_DOUBLE_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_INTEGER_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_LONG_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_VEC2_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_VEC3_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_VEC4_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_BOOLEAN_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_COLOR_RGB_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_COLOR_RGBA_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_ACTOR_ID_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_VECTOR_ELEMENT);
      XMLString::release(&ACTOR_PROPERTY_GAMEEVENT_ELEMENT);

      XMLString::release(&ACTOR_VEC_1_ELEMENT);
      XMLString::release(&ACTOR_VEC_2_ELEMENT);
      XMLString::release(&ACTOR_VEC_3_ELEMENT);
      XMLString::release(&ACTOR_VEC_4_ELEMENT);

      XMLString::release(&ACTOR_COLOR_R_ELEMENT);
      XMLString::release(&ACTOR_COLOR_G_ELEMENT);
      XMLString::release(&ACTOR_COLOR_B_ELEMENT);
      XMLString::release(&ACTOR_COLOR_A_ELEMENT);
   }

   MapXMLConstants::MapXMLConstants() {}
   MapXMLConstants::~MapXMLConstants() {}
   MapXMLConstants::MapXMLConstants(const MapXMLConstants&) {}
   MapXMLConstants& MapXMLConstants::operator=(const MapXMLConstants&) {return *this;}


}
