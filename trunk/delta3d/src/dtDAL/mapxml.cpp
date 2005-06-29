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

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/internal/XMLGrammarPoolImpl.hpp>

//#include <dtCore/globals.h>
#include <dtCore/scene.h>

#include <osg/Vec2f>
#include <osg/Vec2d>
#include <osg/Vec3f>
#include <osg/Vec3d>

#include "dtDAL/mapxml.h"
#include "dtDAL/log.h"
#include "dtDAL/map.h"
#include "dtDAL/librarymanager.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/fileutils.h"

XERCES_CPP_NAMESPACE_USE;

namespace dtDAL
{

    const std::string logName("MapXML.cpp");

    /**
     *  This is a simple class that lets us do easy (though not terribly efficient)
     *  trancoding of XMLCh data to local code page for display.  This code was take from
     *  the xerces-c 2.6 samples
     *  <p>
     *  It's main reason for existing is to allow short and quick translations for printing out debugging info.
     *  </p>
     */
    class XMLStringConverter
    {
    public :
        XMLStringConverter(const XMLCh* const charData): mData(NULL), mLocalForm(NULL)
        {
            if (charData != NULL)
            {
                mData = new XMLCh[XMLString::stringLen(charData) + 1];
                XMLString::copyString(mData, charData);
            }
        }

        ~XMLStringConverter()
        {
            delete[] mData;
            if (mLocalForm != NULL)
                XMLString::release(&mLocalForm);
        }

        /**
        * returns the XMLCh string as a char*
         */
        const char* c_str()
        {
            if (mData == NULL)
                return "";

            if (mLocalForm == NULL)
                mLocalForm = XMLString::transcode(mData);

            return mLocalForm;
        }

        const std::string ToString() { return std::string(c_str()); }
    private :
        XMLCh* mData;
        char*   mLocalForm;
        XMLStringConverter(const XMLStringConverter&) {}
        XMLStringConverter& operator=(const XMLStringConverter&) { return *this;}
    };


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
            Log::GetInstance(logName).LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__,
                "Error during parser initialization! %s :\n", XMLStringConverter(toCatch.getMessage()).c_str());
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
            mXercesParser->setContentHandler(&mHandler);
            mXercesParser->setErrorHandler(&mHandler);
            mXercesParser->parse(path.c_str());
            mLogger->LogMessage(dtDAL::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing complete.\n");
            osg::ref_ptr<Map> mapRef = mHandler.GetMap();
            mHandler.ClearMap();
            return mapRef.release();
        }
        catch (const OutOfMemoryException&)
        {
            mLogger->LogMessage(dtDAL::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
            EXCEPT(ExceptionEnum::MapLoadParsingError, "Ran out of memory parsing save file.");
        }
        catch (const XMLException& toCatch)
        {
            mLogger->LogMessage(dtDAL::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %ls :\n",
                toCatch.getMessage());
            EXCEPT(ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.");
        }
        catch (const SAXParseException& toCatch)
        {
            //this will already by logged by the
            EXCEPT(ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.");
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
            mXercesParser->setContentHandler(&mHandler);
            mXercesParser->setErrorHandler(&mHandler);

            if (mXercesParser->parseFirst(path.c_str(), token))
            {
                parserNeedsReset = true;

                bool cont = mXercesParser->parseNext(token);
                while (cont && !mHandler.HasFoundMapName())
                {
                    cont = mXercesParser->parseNext(token);
                }

                parserNeedsReset = false;
                //reSet the parser and close the file handles.
                mXercesParser->parseReset(token);

                if (mHandler.HasFoundMapName())
                {
                    mLogger->LogMessage(dtDAL::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing complete.");
                    std::string name = mHandler.GetMap()->GetName();
                    mHandler.ClearMap();
                    return name;
                }
                else
                {
                    EXCEPT(ExceptionEnum::MapLoadParsingError, "Parser stopped without finding the map name.");
                }
            }
            else
            {
                EXCEPT(ExceptionEnum::MapLoadParsingError, "Parsing to find the map name did not begin.");
            }
        }
        catch (const OutOfMemoryException&)
        {
            if (parserNeedsReset)
                mXercesParser->parseReset(token);

            mLogger->LogMessage(dtDAL::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
            EXCEPT(ExceptionEnum::MapLoadParsingError, "Ran out of memory parsing save file.");
        }
        catch (const XMLException& toCatch)
        {
            if (parserNeedsReset)
                mXercesParser->parseReset(token);

            mLogger->LogMessage(dtDAL::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %ls :\n",
                toCatch.getMessage());
            EXCEPT(ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.");
        }
        catch (const SAXParseException& toCatch)
        {
            if (parserNeedsReset)
                mXercesParser->parseReset(token);

            //this will already by logged by the content handler
            EXCEPT(ExceptionEnum::MapLoadParsingError, "Error while parsing map file. See log for more information.");
        }
    }

    MapParser::MapParser()
    {
        mLogger = &Log::GetInstance(logName);

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

        if (!FileUtils::GetInstance().FileExists(schemaFileName))
        {
            mLogger->LogMessage(dtDAL::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                "Error, unable to load required file \"map.xsd\".  Aborting.");
            EXCEPT(ExceptionEnum::ProjectException, "Error, unable to load required file \"map.xsd\".  Aborting.");
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
            mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__, "Call made to isPropertyCorrectType with content handler in incorrect state.");
            return false;
        }

        bool result = mActorProperty->GetPropertyType() == *mActorPropertyType;
        if (!result)
        {
            mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__, __LINE__,
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
                    mMap->SetName(XMLStringConverter(chars).ToString());
                    //this flag is only used when the parser is just looking for the map name.
                    mFoundMapName = true;
                }
                else if (topEl == MapXMLConstants::DESCRIPTION_ELEMENT)
                {
                    mMap->SetDescription(XMLStringConverter(chars).ToString());
                }
                else if (topEl == MapXMLConstants::AUTHOR_ELEMENT)
                {
                    mMap->SetAuthor(XMLStringConverter(chars).ToString());
                }
                else if (topEl == MapXMLConstants::COMMENT_ELEMENT)
                {
                    mMap->SetComment(XMLStringConverter(chars).ToString());
                }
                else if (topEl == MapXMLConstants::COPYRIGHT_ELEMENT)
                {
                    mMap->SetCopyright(XMLStringConverter(chars).ToString());
                }
                else if (topEl == MapXMLConstants::CREATE_TIMESTAMP_ELEMENT)
                {
                    mMap->SetCreateDateTime(XMLStringConverter(chars).ToString());
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
                    mLibName = XMLStringConverter(chars).ToString();
                }
                else if (topEl == MapXMLConstants::LIBRARY_VERSION_ELEMENT)
                {
                    mLibVersion = XMLStringConverter(chars).ToString();
                }
            }
            else if (mInActors && mInActor && !mIgnoreCurrentActor)
            {
                ActorCharacters(chars);
            }
        }

        if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
        {
            mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                               "Found characters for element \"%s\" \"%s\"", XMLStringConverter(topEl.c_str()).c_str(), XMLStringConverter(chars).c_str());
        }
    }

    void MapContentHandler::ActorCharacters(const XMLCh* const chars)
    {
        xmlCharString& topEl = mElements.top();
        if (mInActorProperty)
        {
            if (mActorProxy == NULL)
            {
                mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__, __LINE__,
                    "Actor proxy is NULL, but code has entered the actor property section");

            }
            else
            {
                if (topEl == MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT)
                {
                    std::string propName = XMLStringConverter(chars).ToString();
                    mActorProperty = mActorProxy->GetProperty(propName);
                    if (mActorProperty == NULL)
                        mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__, __LINE__,
                            "In actor property section, actor property object for name \"%s\" was not found, proxy exists.",
                            propName.c_str());

                }
                else if (mActorProperty != NULL)
                {
                    if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT)
                    {
                        std::string resourceTypeString = XMLStringConverter(chars).ToString();
                        for (std::vector<dtUtil::Enumeration*>::const_iterator i = DataType::Enumerate().begin();
                                i != DataType::Enumerate().end(); i++)
                        {
                            DataType* dt = (DataType*)*i;
                            if (dt->GetName() == resourceTypeString)
                                mActorPropertyType = dt;

                        }
                        if (mActorPropertyType == NULL)
                            mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                                "No resource type found for resource type in mMap xml \"%s.\"",
                                                resourceTypeString.c_str());
                    }
                    else if (mActorPropertyType != NULL)
                    {
                        std::string dataValue = XMLStringConverter(chars).ToString();

                        if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
                            mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Setting value of property %s, property type %s, datatype %s, value %s, element name %s.",
                                mActorProperty->GetName().c_str(),
                                mActorProperty->GetPropertyType().GetName().c_str(),
                                mActorPropertyType->GetName().c_str(),
                                dataValue.c_str(), XMLStringConverter(topEl.c_str()).c_str());

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
                mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                    "Encountered the actor name element with value \"%s\", but actor is NULL.",
                    XMLStringConverter(chars).c_str());
            }
            else
            {
                mActorProxy->SetName(XMLStringConverter(chars).ToString());
            }
        }
        else if (topEl == MapXMLConstants::ACTOR_ID_ELEMENT)
        {
            if (mActorProxy == NULL)
            {
                mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                    "Encountered the actor id element with value \"%s\", but actor is NULL.",
                                    XMLStringConverter(chars).c_str());
            }
            else
                mActorProxy->SetId(dtCore::UniqueId(XMLStringConverter(chars).ToString()));
        }
        else if (topEl == MapXMLConstants::ACTOR_TYPE_ELEMENT)
        {
            std::string actorTypeFullName = XMLStringConverter(chars).ToString();
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
                osg::ref_ptr<ActorType> actorType =
                    LibraryManager::GetInstance().FindActorType(actorTypeCategory, actorTypeName);
                if (actorType == NULL)
                {
                    mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                                        "ActorType \"%s\" not found.", actorTypeFullName.c_str());
                    mMissingActorTypes.insert(actorTypeFullName);
                    mIgnoreCurrentActor = true;
                }
                else
                {
                    mLogger->LogMessage(dtDAL::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                        "Creating actor proxy %s with category %s.",
                                        actorTypeName.c_str(), actorTypeCategory.c_str());

                    mActorProxy = LibraryManager::GetInstance().CreateActorProxy(actorType);
                    if (mActorProxy == NULL)
                        mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__,  __LINE__,
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
        else if (*mActorPropertyType == DataType::VEC3)
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
        else if (*mActorPropertyType == DataType::VEC4)
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
                mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__, __LINE__,
                    "Failed Setting value %s for enumerated property type named %s on actor named %s",
                     dataValue.c_str(), mActorProperty->GetName().c_str(), mActorProxy->GetName().c_str());
            }

            mActorPropertyType = NULL;
        }
        else if (mActorPropertyType->IsResource())
        {
            ResourceActorProperty& p = static_cast<ResourceActorProperty&>(*mActorProperty);
            if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT)
            {
                if (dataValue != p.GetPropertyType().GetName())
                {
                    mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__, __LINE__,
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
        mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing Map Document Started.\n");
        Reset();
        mMap = new Map("","");
    }

    void MapContentHandler::startElement (
         const XMLCh* const uri,
         const XMLCh* const localname,
         const XMLCh* const qname,
         const Attributes& attrs)
    {
        if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
        {
            mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                "Found element %s", XMLStringConverter(localname).c_str());
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
                        if (mActorProperty != NULL)
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
                                                                MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT) == 0)
                            {
                                //Need the character contents to know the type.
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
                    mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Header\n");
                    mInHeader = true;
                }
                else if (XMLString::compareString(localname, MapXMLConstants::LIBRARIES_ELEMENT) == 0)
                {
                    mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Libraries\n");
                    mInLibraries = true;
                }
                else if (XMLString::compareString(localname, MapXMLConstants::ACTORS_ELEMENT) == 0)
                {
                    mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Actors\n");
                    mInActors = true;
                }
            }

        }
        else if (XMLString::compareString(localname, MapXMLConstants::MAP_ELEMENT) == 0)
        {
            mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Map\n");
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
            mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__,
                               "Attempting to pop elements off of stack and the stack is empty."
                               "it should at least contain element %s.",
                               XMLStringConverter(localname).c_str());
            return;
        }

        const XMLCh* lname = mElements.top().c_str();

        if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
        {
            mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                               "Ending element: \"%s\"", XMLStringConverter(lname).c_str());
        }


        if (0 != XMLString::compareString(lname, localname))
        {
            mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__,
                "Attempting to pop mElements off of stack and the element "
                "at the top (%s) is not the same as the element ending (%s).",
                XMLStringConverter(lname).c_str(), XMLStringConverter(localname).c_str());
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

                if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
                {
                    mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__,
                        "Attempting to add library %s version %s to the library manager.",
                        mLibName.c_str(),
                        mLibVersion.c_str());
                }

                try
                {
                    LibraryManager::GetInstance().LoadActorRegistry(mLibName);
                    mMap->AddLibrary(mLibName, mLibVersion);
                    ClearLibraryValues();
                }
                catch (dtDAL::Exception& e)
                {
                    mMissingLibraries.push_back(mLibName);
                    if (ExceptionEnum::ProjectResourceError == e.TypeEnum())
                    {
                        mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__,
                            "Error loading library %s version %s in the library manager.  Exception message to follow.",
                            mLibName.c_str(), mLibVersion.c_str());

                    }
                    else
                    {
                        mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__,
                            "Unknown exception loading library %s version %s in the library manager.  Exception message to follow.",
                            mLibName.c_str(), mLibVersion.c_str());
                    }
                    e.LogException(Log::LOG_ERROR, *mLogger);
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
                    mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__,
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
        mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                           "Parsing Map Document Ended.\n");
    }

    void MapContentHandler::error(const SAXParseException& exc)
    {
        mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__,  __LINE__,
            "ERROR %d:%d - %s:%s - %s", exc.getLineNumber(),
            exc.getColumnNumber(), XMLStringConverter(exc.getPublicId()).c_str(),
            XMLStringConverter(exc.getSystemId()).c_str(),
            XMLStringConverter(exc.getMessage()).c_str());
        throw exc;
    }

    void MapContentHandler::fatalError(const SAXParseException& exc)
    {
        mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__,  __LINE__,
            "FATAL-ERROR %d:%d - %s:%s - %s", exc.getLineNumber(),
            exc.getColumnNumber(), XMLStringConverter(exc.getPublicId()).c_str(),
            XMLStringConverter(exc.getSystemId()).c_str(),
            XMLStringConverter(exc.getMessage()).c_str());
        throw exc;
    }

    void MapContentHandler::warning(const SAXParseException& exc)
    {
        mLogger->LogMessage(Log::LOG_WARNING, __FUNCTION__,  __LINE__,
            "WARNING %d:%d - %s:%s - %s", exc.getLineNumber(),
            exc.getColumnNumber(), XMLStringConverter(exc.getPublicId()).c_str(),
            XMLStringConverter(exc.getSystemId()).c_str(),
            XMLStringConverter(exc.getMessage()).c_str());
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

        while (!mElements.empty()) mElements.pop();
            mMissingActorTypes.clear();

        ClearLibraryValues();
        ClearActorValues();

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


    MapContentHandler::MapContentHandler() : mActorProxy(NULL), mActorPropertyType(NULL), mActorProperty(NULL)
    {
        mLogger = &Log::GetInstance(logName);
        //mLogger->SetLogLevel(Log::LOG_DEBUG);
        mLogger->LogMessage(Log::LOG_INFO, __FUNCTION__,  __LINE__, "Creating Map Content Handler.\n");

    }

    MapContentHandler::~MapContentHandler() {}

    MapContentHandler::MapContentHandler(const MapContentHandler&) {}
    MapContentHandler& MapContentHandler::operator=(const MapContentHandler&) { return *this;}

    void MapContentHandler::startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri) {}

    void MapContentHandler::endPrefixMapping(const XMLCh* const prefix) {}

    void MapContentHandler::skippedEntity(const XMLCh* const name) {}

    void MapContentHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}

    void MapContentHandler::processingInstruction(const XMLCh* const target, const XMLCh* const data) {}


    void MapContentHandler::setDocumentLocator(const Locator* const locator) {}

    InputSource* MapContentHandler::resolveEntity(const XMLCh* const publicId, const XMLCh* const systemId)
    {
        return NULL;
    }

    /////////////////////////////////////////////////////

    MapWriter::MapFormatTarget::MapFormatTarget(): mOutFile(NULL)
    {
        mLogger = &Log::GetInstance(logName);
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
                                                XMLFormatter* const formatter)
    {

        if (mOutFile != NULL)
        {
            size_t size = fwrite((char *) toWrite, sizeof(char), (size_t)count, mOutFile);
            if (size < (size_t)count)
            {
                mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__,
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
        mLogger = &Log::GetInstance(logName);
    }

    MapWriter::~MapWriter()
    {
    }

    void MapWriter::Save(Map& mMap, const std::string& filePath)
    {
        FILE* outfile = fopen(filePath.c_str(), "w");

        if (outfile == NULL)
        {
            EXCEPT(ExceptionEnum::MapSaveError, std::string("Unable to open map file \"") + filePath + "\" for writing.");
        }

        mFormatTarget.SetOutputFile(outfile);

        mFormatter << MapXMLConstants::BEGIN_XML_DECL << mFormatter.getEncodingName() << MapXMLConstants::END_XML_DECL << chLF;

        time_t currTime;
        time(&currTime);
        const std::string& utcTime = TimeAsUTC(currTime);

        BeginElement(MapXMLConstants::MAP_ELEMENT, MapXMLConstants::MAP_NAMESPACE);
        BeginElement(MapXMLConstants::HEADER_ELEMENT);
        BeginElement(MapXMLConstants::MAP_NAME_ELEMENT);
        AddCharacters(mMap.GetName());
        EndElement();
        BeginElement(MapXMLConstants::DESCRIPTION_ELEMENT);
        AddCharacters(mMap.GetDescription());
        EndElement();
        BeginElement(MapXMLConstants::AUTHOR_ELEMENT);
        AddCharacters(mMap.GetAuthor());
        EndElement();
        BeginElement(MapXMLConstants::COMMENT_ELEMENT);
        AddCharacters(mMap.GetComment());
        EndElement();
        BeginElement(MapXMLConstants::COPYRIGHT_ELEMENT);
        AddCharacters(mMap.GetCopyright());
        EndElement();
        BeginElement(MapXMLConstants::CREATE_TIMESTAMP_ELEMENT);
        if (mMap.GetCreateDateTime().length() == 0)
        {
            mMap.SetCreateDateTime(utcTime);
        }
        AddCharacters(mMap.GetCreateDateTime());
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
        const std::vector<std::string>& libs = mMap.GetAllLibraries();
        for (std::vector<std::string>::const_iterator i = libs.begin(); i != libs.end(); i++)
        {
            BeginElement(MapXMLConstants::LIBRARY_ELEMENT);
            BeginElement(MapXMLConstants::LIBRARY_NAME_ELEMENT);
            AddCharacters(*i);
            EndElement();
            BeginElement(MapXMLConstants::LIBRARY_VERSION_ELEMENT);
            AddCharacters(mMap.GetLibraryVersion(*i));
            EndElement();
            EndElement();
        }
        EndElement();

        BeginElement(MapXMLConstants::ACTORS_ELEMENT);
        const std::map<dtCore::UniqueId, osg::ref_ptr<ActorProxy> >& proxies = mMap.GetAllProxies();
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
            if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
            {
                mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__, __LINE__,
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
                BeginElement(MapXMLConstants::ACTOR_PROPERTY_ELEMENT);
                BeginElement(MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT);
                AddCharacters(property.GetName());
                if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
                {
                    mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__, __LINE__,
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
                else
                {
                    mLogger->LogMessage(Log::LOG_ERROR, __FUNCTION__, __LINE__,
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

#ifdef __APPLE__
    long GetTimeZone(struct tm& timeParts)
    {
        return timeParts.tm_gmtoff;
    }
#else
    long GetTimeZone(struct tm& timeParts)
    {
        tzset();
        return timezone * -1;
    }
#endif


    const std::string MapWriter::TimeAsUTC(time_t time)
    {
        char data[28];
        std::string result;
        struct tm timeParts;
        struct tm* tp = localtime(&time);
        timeParts = *tp;

        long tz = GetTimeZone(timeParts);

        int tzHour = (int)floor(fabs((double)tz / 3600));
        int tzMin = (int)floor(fabs((double)tz / 60) - (60 * tzHour));

        //since the function of getting hour does fabs,
        //this needs to check the sign of tz.
        if (tz < 0)
            tzHour *= -1;

        snprintf(data, 28, "%04d-%02d-%02dT%02d:%02d:%02d.0%+03d:%02d",
                 timeParts.tm_year + 1900, timeParts.tm_mon + 1, timeParts.tm_mday,
                 timeParts.tm_hour, timeParts.tm_min, timeParts.tm_sec,
                 tzHour, tzMin);

        result.assign(data);
        return result;
    }

    /////////////////////////////////////////////////////
    //// Constant Initialization ////////////////////////
    /////////////////////////////////////////////////////


    const char* const MapXMLConstants::EDITOR_VERSION = "0.1";
    const char* const MapXMLConstants::SCHEMA_VERSION = "1.0";

    XMLCh* MapXMLConstants::END_XML_ELEMENT = NULL;
    XMLCh* MapXMLConstants::BEGIN_XML_DECL = NULL;
    XMLCh* MapXMLConstants::END_XML_DECL = NULL;

    XMLCh* MapXMLConstants::MAP_ELEMENT = NULL;
    XMLCh* MapXMLConstants::MAP_NAMESPACE = NULL;

    XMLCh* MapXMLConstants::HEADER_ELEMENT = NULL;
    XMLCh* MapXMLConstants::MAP_NAME_ELEMENT = NULL;
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

        ACTOR_PROPERTY_ELEMENT = XMLString::transcode("property");
        ACTOR_PROPERTY_NAME_ELEMENT = XMLString::transcode("name");
        ACTOR_PROPERTY_STRING_ELEMENT = XMLString::transcode("string");
        ACTOR_PROPERTY_ENUM_ELEMENT = XMLString::transcode("enumerated");
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
