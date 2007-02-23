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
 * William E. Johnson II
 */

#include <dtHLAGM/hlafomconfigxml.h>

#include <sstream>

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

#include <dtUtil/stringutils.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

#include <dtDAL/librarymanager.h>
#include <dtGame/gamemanager.h>

#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtHLAGM/attributetype.h>
#include <dtHLAGM/parametertranslator.h>
#include <dtHLAGM/exceptionenum.h>

namespace dtHLAGM
{

   const std::string HLAFOMConfigContentHandler::LOG_NAME("hlafomconfigxml.cpp");

   const std::string HLAFOMConfigContentHandler::HLA_FOM_ELEMENT("hla");

   const std::string HLAFOMConfigContentHandler::HEADER_ELEMENT("header");
   const std::string HLAFOMConfigContentHandler::HEADER_NAME_ELEMENT("name");
   const std::string HLAFOMConfigContentHandler::HEADER_USE_DIS_ENTITIES_ELEMENT("disEntityTypes");
   const std::string HLAFOMConfigContentHandler::HEADER_DIS_ENTITIES_ATTR_ELEMENT("disEntityTypeAttribute");
   const std::string HLAFOMConfigContentHandler::HEADER_DESCRIPTION_ELEMENT("description");
   const std::string HLAFOMConfigContentHandler::HEADER_AUTHOR_ELEMENT("author");
   const std::string HLAFOMConfigContentHandler::HEADER_COMMENT_ELEMENT("comment");
   const std::string HLAFOMConfigContentHandler::HEADER_COPYRIGHT_ELEMENT("copyright");
   const std::string HLAFOMConfigContentHandler::HEADER_SCHEMA_VERSION_ELEMENT("schemaVersion");

   const std::string HLAFOMConfigContentHandler::LIBRARIES_ELEMENT("libraries");
   const std::string HLAFOMConfigContentHandler::LIBRARY_ACTOR_ELEMENT("actorLibrary");
   const std::string HLAFOMConfigContentHandler::LIBRARY_TRANSLATOR_ELEMENT("translatorLibrary");
   const std::string HLAFOMConfigContentHandler::LIBRARY_NAME_ELEMENT("name");
   const std::string HLAFOMConfigContentHandler::LIBRARY_VERSION_ELEMENT("version");

   const std::string HLAFOMConfigContentHandler::OBJECTS_ELEMENT("objects");
   const std::string HLAFOMConfigContentHandler::OBJECT_ELEMENT("object");
   const std::string HLAFOMConfigContentHandler::OBJECT_KEYNAME_ATTRIBUTE("name");
   const std::string HLAFOMConfigContentHandler::OBJECT_EXTENDS_ATTRIBUTE("extends");
   const std::string HLAFOMConfigContentHandler::OBJECT_ABSTRACT_ELEMENT("abstract");
   const std::string HLAFOMConfigContentHandler::OBJECT_CLASS_ELEMENT("objectClass");
   const std::string HLAFOMConfigContentHandler::OBJECT_ACTOR_TYPE_ELEMENT("actorType");
   const std::string HLAFOMConfigContentHandler::OBJECT_ENTITY_ID_ELEMENT("entityIdAttributeName");
   const std::string HLAFOMConfigContentHandler::OBJECT_DIS_ENTITY_ELEMENT("disEntityEnum");
   const std::string HLAFOMConfigContentHandler::OBJECT_ATTR_TO_PROP_ELEMENT("attrToProp");
   const std::string HLAFOMConfigContentHandler::OBJECT_REMOTE_ONLY_ELEMENT("remoteOnly");

   const std::string HLAFOMConfigContentHandler::OBJECT_HANDLER_ELEMENT("objectHandler");
   const std::string HLAFOMConfigContentHandler::OBJECT_HANDLER_OBJECT_CLASS_ELEMENT("objectClass");
   const std::string HLAFOMConfigContentHandler::OBJECT_HANDLER_DIS_ENTITY_ELEMENT("disEntityEnum");
   const std::string HLAFOMConfigContentHandler::OBJECT_HANDLER_HANDLER_NAME_ELEMENT("handler");

   const std::string HLAFOMConfigContentHandler::ACTORTYPE_HANDLER_ELEMENT("actorTypeHandler");
   const std::string HLAFOMConfigContentHandler::ACTORTYPE_HANDLER_ACTOR_TYPE_ELEMENT("actorType");
   const std::string HLAFOMConfigContentHandler::ACTORTYPE_HANDLER_HANDLER_ELEMENT("handler");

   const std::string HLAFOMConfigContentHandler::INTERACTIONS_ELEMENT("interactions");
   const std::string HLAFOMConfigContentHandler::INTERACTION_ELEMENT("interaction");
   const std::string HLAFOMConfigContentHandler::INTERACTION_KEYNAME_ATTRIBUTE("name");
   const std::string HLAFOMConfigContentHandler::INTERACTION_EXTENDS_ATTRIBUTE("extends");
   const std::string HLAFOMConfigContentHandler::INTERACTION_ABSTRACT_ELEMENT("abstract");
   const std::string HLAFOMConfigContentHandler::INTERACTION_CLASS_ELEMENT("interactionClass");
   const std::string HLAFOMConfigContentHandler::INTERACTION_MESSAGE_TYPE_ELEMENT("messageType");
   const std::string HLAFOMConfigContentHandler::INTERACTION_PARAM_TO_PARAM_ELEMENT("paramToParam");

   const std::string HLAFOMConfigContentHandler::INTERACTION_HANDLER_ELEMENT("interactionHandler");
   const std::string HLAFOMConfigContentHandler::INTERACTION_HANDLER_INTERACTION_CLASS_ELEMENT("interactionClass");
   const std::string HLAFOMConfigContentHandler::INTERACTION_HANDLER_HANDLER_NAME_ELEMENT("handler");

   const std::string HLAFOMConfigContentHandler::MESSAGE_HANDLER_ELEMENT("messageHandler");
   const std::string HLAFOMConfigContentHandler::MESSAGE_HANDLER_MESSAGETYPE_ELEMENT("messageType");
   const std::string HLAFOMConfigContentHandler::MESSAGE_HANDLER_HANDLER_NAME_ELEMENT("handler");

   const std::string HLAFOMConfigContentHandler::ONE_TO_MANY_HLA_NAME_ELEMENT("hlaName");
   const std::string HLAFOMConfigContentHandler::ONE_TO_MANY_GAME_NAME_ELEMENT("gameName");
   const std::string HLAFOMConfigContentHandler::ONE_TO_MANY_HLA_DATATYPE_ELEMENT("hlaDataType");
   const std::string HLAFOMConfigContentHandler::ONE_TO_MANY_GAME_DATATYPE_ELEMENT("gameDataType");
   const std::string HLAFOMConfigContentHandler::ONE_TO_MANY_HLA_REQUIRED_ELEMENT("hlaRequired");
   const std::string HLAFOMConfigContentHandler::ONE_TO_MANY_GAME_REQUIRED_ELEMENT("gameRequired");
   const std::string HLAFOMConfigContentHandler::ONE_TO_MANY_DEFAULT_ELEMENT("default");
   const std::string HLAFOMConfigContentHandler::ONE_TO_MANY_ENUM_MAPPING_ELEMENT("enumerationMapping");
   const std::string HLAFOMConfigContentHandler::ONE_TO_MANY_ENUM_HLA_ID_ATTRIBUTE("id");
   const std::string HLAFOMConfigContentHandler::ONE_TO_MANY_ENUM_VALUE_ELEMENT("value");

   const std::string HLAFOMConfigContentHandler::DIS_ENTITY_KIND_ELEMENT("kind");
   const std::string HLAFOMConfigContentHandler::DIS_ENTITY_DOMAIN_ELEMENT("domain");
   const std::string HLAFOMConfigContentHandler::DIS_ENTITY_COUNTRY_ELEMENT("country");
   const std::string HLAFOMConfigContentHandler::DIS_ENTITY_CATEGORY_ELEMENT("category");
   const std::string HLAFOMConfigContentHandler::DIS_ENTITY_SUBCATEGORY_ELEMENT("subcategory");
   const std::string HLAFOMConfigContentHandler::DIS_ENTITY_SPECIFIC_ELEMENT("specific");
   const std::string HLAFOMConfigContentHandler::DIS_ENTITY_EXTRA_ELEMENT("extra");

   HLAFOMConfigContentHandler::HLAFOMConfigContentHandler()
   {
      mLogger = &dtUtil::Log::GetInstance(LOG_NAME);
   }

   HLAFOMConfigContentHandler::~HLAFOMConfigContentHandler()
   {
   }

   void HLAFOMConfigContentHandler::startDocument()
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
         "Parsing HLA FOM Config Document Started.\n");
      resetDocument();
   }

   void HLAFOMConfigContentHandler::startElement
       (
       const XMLCh*  const  uri,
       const XMLCh*  const  localname,
       const XMLCh*  const  qname,
       const xercesc_dt::Attributes& attrs
       )
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                            "Found element \"%s\" ", dtUtil::XMLStringConverter(localname).c_str());
      }

      dtUtil::XMLStringConverter localNameConverter(localname);
      std::string sLocalName(localNameConverter.c_str());

      mElements.push(sLocalName);

      if (mInHLAConfig)
      {
         //flag checking is based on the amount of time spent in section
         //not the order in the XML.
         if (mInObjects)
         {
            if (mInObject)
            {
               if (sLocalName == OBJECT_ABSTRACT_ELEMENT)
               {
                  mCurrentObjectToActorIsAbstract = true;
               }
               else if (sLocalName == OBJECT_DIS_ENTITY_ELEMENT)
               {
                  //still mark this as parsing a DIS ID even if mUsingDisID is false
                  //so the parser has no way of getting confused.
                  mParsingDISID = true;

                  if (mCurrentObjectToActor.valid() && mUsingDisID)
                  {
                     //assign an empty entity type object to the mapping
                     //that the parser can fill.
                     EntityType et;
                     mCurrentObjectToActor->SetDisID(&et);
                  }
               }
               //if we have an attr to prop construct and a valid object to actor
               else if (sLocalName == OBJECT_ATTR_TO_PROP_ELEMENT && mCurrentObjectToActor.valid())
               {
                  mCurrentObjectToActor->GetOneToManyMappingVector().push_back(AttributeToPropertyList());
                  mCurrentAttrToProp = &mCurrentObjectToActor->GetOneToManyMappingVector().back();
                  mCurrentAttrToProp->GetParameterDefinitions().push_back(OneToManyMapping::ParameterDefinition());
               }
               else if (mCurrentAttrToProp != NULL)
               {
                  if (mParsingEnumMapping && sLocalName == ONE_TO_MANY_ENUM_VALUE_ELEMENT)
                  {
                     StartElementEnumMapping(sLocalName, attrs);
                  }
                  else if (sLocalName == ONE_TO_MANY_ENUM_MAPPING_ELEMENT)
                  {
                     mParsingEnumMapping = true;
                  }
               }
            }
            else if (mInObjectHandler)
            {
               if (sLocalName == OBJECT_HANDLER_DIS_ENTITY_ELEMENT)
                  mParsingDISID = true;
            }
            else if (sLocalName == OBJECT_ELEMENT)
            {
               mInObject = true;
               mCurrentObjectToActor = new ObjectToActor;
               dtUtil::AttributeSearch as;
               dtUtil::AttributeSearch::ResultMap rMap = as(attrs);
               //see if this object mapping extends another, i.e. inherits all attribute mappings.
               dtUtil::AttributeSearch::ResultMap::iterator itor = rMap.find(OBJECT_EXTENDS_ATTRIBUTE);
               if (itor != rMap.end())
               {
                  const std::string& keyName = itor->second;
                  std::map<std::string, dtCore::RefPtr<ObjectToActor> >::iterator superItor = mNamedObjectToActors.find(keyName);
                  if (superItor != mNamedObjectToActors.end())
                  {
                     dtCore::RefPtr<ObjectToActor> super = superItor->second;

                     //Set Entity Id field name
                     mCurrentObjectToActor->SetEntityIdAttributeName(super->GetEntityIdAttributeName());

                     //copy all data from the one Attr to Prop vector to the other.
                     mCurrentObjectToActor->GetOneToManyMappingVector().insert(mCurrentObjectToActor->GetOneToManyMappingVector().end(),
                        super->GetOneToManyMappingVector().begin(), super->GetOneToManyMappingVector().end());

                     if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                        mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                           "Added field mappings from object mapping \"%s\" to the current mapping.",
                           keyName.c_str());

                  }
                  else
                  {
                     std::ostringstream ss;
                     ss << "Unable to extend from mapping named " << keyName << " because no such mapping has been found in the XML."; 
                     throw dtUtil::Exception(ExceptionEnum::XML_CONFIG_EXCEPTION, ss.str(), __FILE__, __LINE__);
                  }

               }

               //see if this object mapping has a key name so that others may inherit all attribute mappings.
               itor = rMap.find(OBJECT_KEYNAME_ATTRIBUTE);
               if (itor != rMap.end())
               {
                  const std::string& newKeyName = itor->second;
                  mNamedObjectToActors.insert(std::make_pair(newKeyName, mCurrentObjectToActor));
                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                        "Setting mapping name for current object mapping to \"%s\".",
                        newKeyName.c_str());
               }
            }
            else if (sLocalName == OBJECT_HANDLER_ELEMENT)
            {
               mInObjectHandler = true;
            }
            else if (sLocalName == ACTORTYPE_HANDLER_ELEMENT)
            {
               mInActorTypeHandler = true;
            }

         }
         else if (mInInteractions)
         {
            if (mInInteraction)
            {
               if (sLocalName == INTERACTION_ABSTRACT_ELEMENT)
               {
                  mCurrentInteractionToMessageIsAbstract = true;
               }
               else if (sLocalName == INTERACTION_PARAM_TO_PARAM_ELEMENT && mCurrentInteractionToMessage != NULL)
               {
                  mCurrentInteractionToMessage->GetOneToManyMappingVector().push_back(ParameterToParameterList());
                  mCurrentParamToParam = &mCurrentInteractionToMessage->GetOneToManyMappingVector().back();
                  mCurrentParamToParam->GetParameterDefinitions().push_back(OneToManyMapping::ParameterDefinition());
               }
               else if (mCurrentParamToParam != NULL)
               {
                  if (mParsingEnumMapping)
                  {
                     StartElementEnumMapping(sLocalName, attrs);
                  }
                  else if (sLocalName == ONE_TO_MANY_ENUM_MAPPING_ELEMENT)
                  {
                     mParsingEnumMapping = true;
                  }
               }
            }
            else if (sLocalName == INTERACTION_ELEMENT)
            {
               mInInteraction = true;
               mCurrentInteractionToMessage = new InteractionToMessage;
               dtUtil::AttributeSearch as;
               dtUtil::AttributeSearch::ResultMap rMap = as(attrs);
               //see if this interaction mapping extends another, i.e. inherits all attribute mappings.
               dtUtil::AttributeSearch::ResultMap::iterator itor = rMap.find(INTERACTION_EXTENDS_ATTRIBUTE);
               if (itor != rMap.end())
               {
                  const std::string& keyName = itor->second;
                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                        "Current mapping extends interaction mapping \"%s\".",
                        keyName.c_str());

                  std::map<std::string, dtCore::RefPtr<InteractionToMessage> >::iterator superItor = mNamedInteractionToMessages.find(keyName);
                  if (superItor != mNamedInteractionToMessages.end())
                  {
                     //copy all data from the one vector to the other.
                     dtCore::RefPtr<InteractionToMessage> super = superItor->second;
                     mCurrentInteractionToMessage->GetOneToManyMappingVector().insert(mCurrentInteractionToMessage->GetOneToManyMappingVector().end(),
                        super->GetOneToManyMappingVector().begin(), super->GetOneToManyMappingVector().end());

                     if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                        mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                           "Added parameter mappings from interaction mapping \"%s\" to the current mapping.",
                           keyName.c_str());

                  }
                  else
                  {
                     std::ostringstream ss;
                     ss << "Unable to extend from mapping named " << keyName << " because no such mapping has been found in the XML."; 
                     throw dtUtil::Exception(ExceptionEnum::XML_CONFIG_EXCEPTION, ss.str(), __FILE__, __LINE__);
                  }

               }

               //see if this interaction mapping has a key name so that others may inherit all attribute mappings.
               itor = rMap.find(INTERACTION_KEYNAME_ATTRIBUTE);
               if (itor != rMap.end())
               {
                  const std::string& newKeyName = itor->second;
                  mNamedInteractionToMessages.insert(make_pair(newKeyName, mCurrentInteractionToMessage));
                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                        "Setting mapping name for current interaction mapping to \"%s\".",
                        newKeyName.c_str());
               }
            }
            else if (sLocalName == INTERACTION_HANDLER_ELEMENT)
               mInInteractionHandler = true;
            else if (sLocalName == MESSAGE_HANDLER_ELEMENT)
               mInMessageHandler = true;
         }
         else if (mInLibraries)
         {
            if (sLocalName == LIBRARY_ACTOR_ELEMENT)
            {
               mInActorLibrary = true;
            }
            else if (sLocalName == LIBRARY_TRANSLATOR_ELEMENT)
            {
               mInTranslatorLibrary = false;
            }
         }
         else if (!mInHeader)
         {
            if (sLocalName == OBJECTS_ELEMENT)
               mInObjects = true;
            else if (sLocalName == INTERACTIONS_ELEMENT)
               mInInteractions = true;
            else if (sLocalName == HEADER_ELEMENT)
               mInHeader = true;
            else if (sLocalName == LIBRARIES_ELEMENT)
            {
               ClearLibraryValues();
               mInLibraries = true;
            }
         }
      }
      else if (sLocalName == HLA_FOM_ELEMENT)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found main element.");
         mInHLAConfig = true;
      }
   }

   void HLAFOMConfigContentHandler::StartElementEnumMapping(const std::string& elementName, const xercesc_dt::Attributes& attrs)
   {
      dtUtil::AttributeSearch as;
      dtUtil::AttributeSearch::ResultMap rMap = as(attrs);
      //see if this object mapping extends another, i.e. inherits all attribute mappings.
      dtUtil::AttributeSearch::ResultMap::iterator itor = rMap.find(ONE_TO_MANY_ENUM_HLA_ID_ATTRIBUTE);
      if (itor != rMap.end())
      {
         mCurrentEnumHLAID = itor->second;
      }
   }

   void HLAFOMConfigContentHandler::characters(const XMLCh* const chars, const unsigned int length)
   {
      dtUtil::XMLStringConverter charsConverter(chars);
      const std::string sChars(charsConverter.c_str());
      const std::string& topElement = mElements.top();

      if (!mInHLAConfig)
         return;

      //flag checking is based on the amount of time spent in section
      //not the order in the XML.
      if (mInObjects)
      {
         if (mInObject)
         {
            ObjectToActorCharacters(topElement, sChars);
         }
         else if (mInObjectHandler)
         {
         }
         else if (mInActorTypeHandler)
         {
         }
      }
      else if (mInInteractions)
      {
         if (mInInteraction)
         {
            InteractionToMessageCharacters(topElement, sChars);
         }
         else if (mInInteractionHandler)
         {
         }
         else if (mInMessageHandler)
         {
         }
      }
      else if (mInHeader)
      {
         if (topElement == HEADER_ELEMENT)
         {
         }
         else if (topElement == HEADER_NAME_ELEMENT)
         {
         }
         else if (topElement == HEADER_DESCRIPTION_ELEMENT)
         {
         }
         else if (topElement == HEADER_USE_DIS_ENTITIES_ELEMENT)
         {
            // the translator needs to be able to handle this.
            //mTargetTranslator->
            mUsingDisID = sChars == "true" || sChars == "1";
         }
         else if (topElement == HEADER_DIS_ENTITIES_ATTR_ELEMENT)
         {
            // the translator needs to be able to handle this.
         }
         else if (topElement == HEADER_AUTHOR_ELEMENT)
         {
         }
         else if (topElement == HEADER_COMMENT_ELEMENT)
         {
         }
         else if (topElement == HEADER_COPYRIGHT_ELEMENT)
         {
         }
         else if (topElement == HEADER_SCHEMA_VERSION_ELEMENT)
         {
         }
      }
      else if (mInLibraries)
      {
         if (topElement == LIBRARY_NAME_ELEMENT)
         {
            mLibName = sChars;
         }
         else if (topElement == LIBRARY_VERSION_ELEMENT)
         {
            mLibVersion = sChars;
         }
      }

   }

   void HLAFOMConfigContentHandler::ObjectToActorCharacters(const std::string& elementName, const std::string& characters)
   {
      if (mCurrentObjectToActor == NULL)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
            "mCurrentObjectToActor is NULL, so it must be invalid.");
         return;
      }

      if (mParsingDISID)
      {
         if (!mUsingDisID)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                "DIS ID found, but dis id usage is turned off for this mapping.");
            return;
         }

         if (mCurrentObjectToActor->GetDisID() != NULL)
            DISIDCharacters(*mCurrentObjectToActor->GetDisID(), elementName, characters);
         return;
      }
      else if (mCurrentAttrToProp != NULL)
      {
         OneToManyCharacters(elementName, characters, *mCurrentAttrToProp);
         return;
      }

      if (elementName == OBJECT_CLASS_ELEMENT)
      {
         mCurrentObjectToActor->SetObjectClassName(characters);
      }
      else if (elementName == OBJECT_ACTOR_TYPE_ELEMENT)
      {
         dtDAL::ActorType* type = FindActorType(characters);
         if (type == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                                "ActorType \"%s\" not found. The current object to actor mapping will be ignored.", characters.c_str());
            mCurrentObjectToActor = NULL;
         }
         else
         {
            mCurrentObjectToActor->SetActorType(*type);
         }

      }
      else if (elementName == OBJECT_ENTITY_ID_ELEMENT)
      {
         if (mCurrentObjectToActor->GetEntityIdAttributeName() != characters
            && mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                               "Overriding the Entity Id field name in a subclassed object mapping.  The old value was \"%s\", the new value is \"%s\".",
                               mCurrentObjectToActor->GetEntityIdAttributeName().c_str(),
                               characters.c_str());
         }
         mCurrentObjectToActor->SetEntityIdAttributeName(characters);
      }
      else if (elementName == OBJECT_REMOTE_ONLY_ELEMENT)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                               "Setting Object To Actor with Object Class Type \"%s\" Remote Only value to \"%s\".",
                               mCurrentObjectToActor->GetObjectClassName().c_str(),
                               characters.c_str());
         mCurrentObjectToActor->SetRemoteOnly(characters == "true" || characters == "1");
      }
   }

   void HLAFOMConfigContentHandler::InteractionToMessageCharacters(const std::string& elementName, const std::string& characters)
   {
      if (mCurrentInteractionToMessage == NULL)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
               "mCurrentInteractionToMessage is NULL, so it must be invalid.");
         return;
      }

      if (mCurrentParamToParam != NULL)
      {
         OneToManyCharacters(elementName, characters, *mCurrentParamToParam);
         return;
      }

      if (elementName == INTERACTION_CLASS_ELEMENT)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Interaction class is being set to %s.", characters.c_str());

         mCurrentInteractionToMessage->SetInteractionName(characters);
      }
      else if (elementName == INTERACTION_MESSAGE_TYPE_ELEMENT)
      {
         if (mTargetTranslator->GetGameManager() == NULL)
         {
            std::ostringstream ss;
            ss << "Unable to lookup message type " << characters << ".  No GameManager assigned to the target translator."; 
            throw dtUtil::Exception(ExceptionEnum::XML_CONFIG_EXCEPTION, ss.str(), __FILE__, __LINE__);
         }
         else
         {
            const dtGame::MessageType* messageType =
               mTargetTranslator->GetGameManager()->GetMessageFactory().GetMessageTypeByName(characters);

            if (messageType == NULL)
            {
               std::ostringstream ss;
               ss << "Message type " << characters << " does not exist."; 
               throw dtUtil::Exception(ExceptionEnum::XML_CONFIG_EXCEPTION, ss.str(), __FILE__, __LINE__);
            }
            else
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                     "Message type %s found.", characters.c_str());
               mCurrentInteractionToMessage->SetMessageType(*messageType);
            }
         }

      }
   }

   void HLAFOMConfigContentHandler::OneToManyCharacters(const std::string& elementName, const std::string& characters, OneToManyMapping& mapping)
   {
      if (mParsingEnumMapping)
      {
         if (elementName == ONE_TO_MANY_ENUM_VALUE_ELEMENT)
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Adding Enum mapping of \"%s\" to \"%s\" to parameter def 0.", 
                  mCurrentEnumHLAID.c_str(), characters.c_str());
            mapping.GetParameterDefinitions()[0].AddEnumerationMapping(mCurrentEnumHLAID, characters);
         }
      }
      else if (elementName == ONE_TO_MANY_HLA_NAME_ELEMENT)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Setting HLA parameter/attribute name to \"%s\".", 
               characters.c_str());
               
         mapping.SetHLAName(characters);
      }
      else if (elementName == ONE_TO_MANY_HLA_DATATYPE_ELEMENT)
      {
         const std::vector<dtCore::RefPtr<ParameterTranslator> >& parameterTranslators = mTargetTranslator->GetParameterTranslators();
         
         for (unsigned i = 0; i < parameterTranslators.size(); ++i)
         { 
            const AttributeType& dt = parameterTranslators[i]->GetAttributeTypeForName(characters);
            if (dt != AttributeType::UNKNOWN)
            {
               mapping.SetHLAType(dt);
               break;
            }
         }
         
         if (mapping.GetHLAType() == AttributeType::UNKNOWN)
         {
            std::ostringstream ss;
            ss << "No dtHLAGM::AttributeType with name " << characters << " exists."; 
            throw dtUtil::Exception(ExceptionEnum::XML_CONFIG_EXCEPTION, ss.str(), __FILE__, __LINE__);
         }
         else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Setting HLA Attribute type to \"%s\".", 
               characters.c_str());
         }

      }
      else if (elementName == ONE_TO_MANY_HLA_REQUIRED_ELEMENT)
      {
         bool value = characters == "true" || characters == "1";
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::string sValue = value ? "true" : "false";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Setting required flag for HLA to \"%s\".", 
               sValue.c_str());
         }

         mapping.SetRequiredForHLA(value);
      }
      else if (elementName == ONE_TO_MANY_GAME_NAME_ELEMENT)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Setting game parameter/property name to \"%s\" on parameter def 0.", 
               characters.c_str());
               
         mapping.GetParameterDefinitions()[0].SetGameName(characters);
      }
      else if (elementName == ONE_TO_MANY_GAME_DATATYPE_ELEMENT)
      {
         const dtDAL::DataType* dt = static_cast<const dtDAL::DataType*>(dtDAL::DataType::GetValueForName(characters));
         if (dt != NULL)
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Setting game datatype to \"%s\" on parameter def 0.", 
                  dt->GetName().c_str());
            mapping.GetParameterDefinitions()[0].SetGameType(*dt);
         }
         else
         {
            std::ostringstream ss;
            ss << "No dtDAL::DataType with name " << characters << " exists."; 
            throw dtUtil::Exception(ExceptionEnum::XML_CONFIG_EXCEPTION, ss.str(), __FILE__, __LINE__);
         }
      }
      else if (elementName == ONE_TO_MANY_GAME_REQUIRED_ELEMENT)
      {
         bool value = characters == "true" || characters == "1";
                  
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::string sValue = value ? "true" : "false";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Setting required for game flag to \"%s\" on parameter def 0.", 
               sValue.c_str());
         }

         mapping.GetParameterDefinitions()[0].SetRequiredForGame(value);
      }
      else if (elementName == ONE_TO_MANY_DEFAULT_ELEMENT)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "The default value for parameter def 0 is %s", 
               characters.c_str());

         mapping.GetParameterDefinitions()[0].SetDefaultValue(characters);
      }

   }

   void HLAFOMConfigContentHandler::DISIDCharacters(EntityType& entityType, const std::string& elementName, const std::string& characters)
   {
      std::istringstream ss;
      ss.str(characters);

      //all of the values are no larger than a short.
      unsigned short value;
      ss >> value;

      if (elementName == DIS_ENTITY_KIND_ELEMENT)
         entityType.SetKind((unsigned char)value);
      else if (elementName == DIS_ENTITY_DOMAIN_ELEMENT)
         entityType.SetDomain((unsigned char)value);
      else if (elementName == DIS_ENTITY_CATEGORY_ELEMENT)
         entityType.SetCategory((unsigned char)value);
      else if (elementName == DIS_ENTITY_SUBCATEGORY_ELEMENT)
         entityType.SetSubcategory((unsigned char)value);
      else if (elementName == DIS_ENTITY_COUNTRY_ELEMENT)
         entityType.SetCountry(value);
      else if (elementName == DIS_ENTITY_SPECIFIC_ELEMENT)
         entityType.SetSpecific((unsigned char)value);
      else if (elementName == DIS_ENTITY_EXTRA_ELEMENT)
         entityType.SetExtra((unsigned char)value);
   }

   dtDAL::ActorType* HLAFOMConfigContentHandler::FindActorType(const std::string& actorTypeFullName)
   {
      size_t index = actorTypeFullName.find_last_of('.');

      std::string actorTypeCategory;
      std::string actorTypeName;

      if (index == actorTypeFullName.npos)
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
         dtCore::RefPtr<dtDAL::ActorType> actorType =
            mTargetTranslator->GetGameManager()->FindActorType(actorTypeCategory, actorTypeName);
         if (actorType == NULL)
         {
            mMissingActorTypes.insert(actorTypeFullName);
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                "Actor Type %s with category %s found.",
                                actorTypeName.c_str(), actorTypeCategory.c_str());

            return actorType.get();
         }
      }
      return NULL;
   }

   void HLAFOMConfigContentHandler::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
   {
      if (mElements.empty())
      {
         std::ostringstream ss;
         ss << "Attempting to pop elements off of stack and the stack is empty."
            << "It should at least contain element " 
            << dtUtil::XMLStringConverter(localname).c_str();  
         throw dtUtil::Exception(ExceptionEnum::XML_CONFIG_EXCEPTION, ss.str(), __FILE__, __LINE__);
      }

      dtUtil::XMLStringConverter localNameConverter(localname);

      const std::string& lname = mElements.top();
      const std::string sLocalName(localNameConverter.c_str());

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                            "Ending element: \"%s\"", lname.c_str());
      }

      if (lname != sLocalName)
      {
         std::ostringstream ss;
         ss << "Attempting to pop mElements off of stack and the element "
            << "at the top (" << lname << ") is not the same as the element ending ("
            << sLocalName << ").";
         throw dtUtil::Exception(ExceptionEnum::XML_CONFIG_EXCEPTION, ss.str(), __FILE__, __LINE__);
      }

      if (mInHLAConfig)
      {
         //flag checking is based on the amount of time spent in section
         //not the order in the XML.
         if (mInObjects)
         {
            if (mInObject)
            {
               if (sLocalName == OBJECT_DIS_ENTITY_ELEMENT)
               {
                  mParsingDISID = false;
               }
               else if (sLocalName == OBJECT_ELEMENT)
               {
                  mInObject = false;
                  if (mCurrentObjectToActor != NULL)
                  {
                     mObjectToActors.push_back(mCurrentObjectToActor);

                     if (!mCurrentObjectToActorIsAbstract)
                     {
                        if (mCurrentObjectToActor->GetOneToManyMappingVector().size() == 0)
                        {
                           std::ostringstream ss;
                           ss << "Non-Abstract ObjectToActor mapping HLA object class \"" 
                              << mCurrentObjectToActor->GetObjectClassName() 
                              << "\" to Actor Type \"" << mCurrentObjectToActor->GetActorType() 
                              << "\" is invalid because it has no attribute mappings.";
                           throw dtUtil::Exception(ExceptionEnum::XML_CONFIG_EXCEPTION, ss.str(), __FILE__, __LINE__);         
                        }
                        mTargetTranslator->RegisterActorMapping(*mCurrentObjectToActor);
                     }

                     ClearObjectValues();
                  }
               }
               else if (sLocalName == OBJECT_ATTR_TO_PROP_ELEMENT)
               {
                  //the pointer points to a value IN the vector on the object to actor, so
                  //setting it to NULL is sufficient.
                  mCurrentAttrToProp = NULL;
               }
               else if (sLocalName == ONE_TO_MANY_ENUM_MAPPING_ELEMENT)
               {
                  mParsingEnumMapping = false;
                  //set to -1 so that it will be 0 if it gets incremented
                  mCurrentEnumHLAID = -1;
               }
               else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
                        "Ending handled element %s within the context of the %s element.",
                        sLocalName.c_str(), OBJECT_ELEMENT.c_str()
                        );
               }
            }
            else if (mInObjectHandler)
            {
               if (sLocalName == OBJECT_HANDLER_DIS_ENTITY_ELEMENT)
               {
                  mParsingDISID = false;
               }
               else if (sLocalName == OBJECT_HANDLER_ELEMENT)
               {
                  mInObjectHandler = false;
               }
            }

            else if (sLocalName == ACTORTYPE_HANDLER_ELEMENT)
            {
               mInActorTypeHandler = false;
            }
            else if (sLocalName == OBJECTS_ELEMENT)
            {
               mInObjects = false;
            }
            else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
                     "Ending handled element %s within the context of the %s element.",
                     sLocalName.c_str(), OBJECTS_ELEMENT.c_str()
                     );
            }

         }
         else if (mInInteractions)
         {
            if (mInInteraction)
            {
               if (sLocalName == INTERACTION_ELEMENT)
               {
                  mInInteraction = false;
                  if (mCurrentInteractionToMessage != NULL)
                  {
                     mInteractionToMessages.push_back(mCurrentInteractionToMessage);

                     if (!mCurrentInteractionToMessageIsAbstract)
                     {
                        if (mCurrentInteractionToMessage->GetOneToManyMappingVector().size() == 0)
                        {
                           std::ostringstream ss;
                           ss << "Non-Abstract InteractionToMessage mapping HLA interaction class \"" 
                              << mCurrentInteractionToMessage->GetInteractionName() 
                              << "\" to Message Type \"" << mCurrentInteractionToMessage->GetMessageType().GetName() 
                              << "\" is invalid because has it no parameter mappings.";
                           throw dtUtil::Exception(ExceptionEnum::XML_CONFIG_EXCEPTION, ss.str(), __FILE__, __LINE__);         
                        }
                        mTargetTranslator->RegisterMessageMapping(*mCurrentInteractionToMessage);
                     }
                     ClearInteractionValues();
                  }
               }
               else if (sLocalName == INTERACTION_PARAM_TO_PARAM_ELEMENT)
               {
                  //the pointer points to a value IN the vector on the object to actor, so
                  //setting it to NULL is sufficient.
                  mCurrentParamToParam = NULL;
               }
               else if (sLocalName == ONE_TO_MANY_ENUM_MAPPING_ELEMENT)
               {
                  mParsingEnumMapping = false;
                  //set to -1 so that it will be 0 if it gets incremented
                  mCurrentEnumHLAID = -1;
               }
               else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
                        "Ending handled element %s within the context of the %s element.",
                        sLocalName.c_str(), INTERACTION_ELEMENT.c_str()
                        );
               }

            }
            else if (sLocalName == INTERACTION_HANDLER_ELEMENT)
            {
               mInInteractionHandler = false;
            }
            else if (sLocalName == MESSAGE_HANDLER_ELEMENT)
            {
               mInMessageHandler = false;
            }
            else if (sLocalName == INTERACTIONS_ELEMENT)
            {
               mInInteractions = false;
            }
            else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
                     "Ending element %s within the context of the %s element.",
                     sLocalName.c_str(), INTERACTIONS_ELEMENT.c_str()
                     );
            }
         }
         else if (mInActorLibrary)
         {
            if (sLocalName == LIBRARY_ACTOR_ELEMENT)
            {
               LoadActorLibrary();
               ClearLibraryValues();
            }
            else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
                     "Ending element %s within the context of the %s element.",
                     sLocalName.c_str(), LIBRARY_ACTOR_ELEMENT.c_str()
                     );
            }
         }
         else if (mInTranslatorLibrary)
         {
            if (sLocalName == LIBRARY_TRANSLATOR_ELEMENT)
            {
               LoadTranslatorLibrary();
               ClearLibraryValues();
            }
            else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
                     "Ending element %s within the context of the %s element.",
                     sLocalName.c_str(), LIBRARY_TRANSLATOR_ELEMENT.c_str()
                     );
            }
         }
         else
         {
            if (sLocalName == HEADER_ELEMENT)
            {
               mInHeader = false;
            }
            else if (sLocalName == LIBRARIES_ELEMENT)
            {
               mInLibraries = false;
            }
            else if (sLocalName == HLA_FOM_ELEMENT)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Closing main element.");
               mInHLAConfig = false;
            }
            else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
                     "Ending element %s within the context of the %s element.",
                     sLocalName.c_str(), HLA_FOM_ELEMENT.c_str()
                     );
            }
         }
      }
      else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
               "Ending unknown element %s within the global context.",
               sLocalName.c_str()
               );
      }


      mElements.pop();
   }

   void HLAFOMConfigContentHandler::LoadActorLibrary()
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
            "Attempting to add library %s version %s to the library manager.",
            mLibName.c_str(),
            mLibVersion.c_str());
      }

      if (mLibName.empty())
         return;

      try
      {
         if (mTargetTranslator->GetGameManager()->GetRegistry(mLibName) == NULL)
         {
            mTargetTranslator->GetGameManager()->LoadActorRegistry(mLibName);
         }
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
         throw e;
      }
   }

   void HLAFOMConfigContentHandler::LoadTranslatorLibrary()
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
            "Attempting to add library %s version %s to the translator registry.",
            mLibName.c_str(),
            mLibVersion.c_str());
      }

      if (mLibName.empty())
         return;

      try
      {
         dtUtil::LibrarySharingManager& lsm = dtUtil::LibrarySharingManager::GetInstance();
         dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> lib;

         lib = lsm.LoadSharedLibrary(mLibName);

         dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS createFn;
         createFn = lib->FindSymbol("CreateParameterTranslator");

         //Make sure the plugin actually implemented these functions and they
         //have been exported.
         std::ostringstream msg;

         if (!createFn)
         {
            msg.clear();
            msg.str("");
            msg << "Actor plugin libraries must implement the function " <<
               " CreateParameterTranslator.";
            throw dtUtil::Exception(dtDAL::ExceptionEnum::ProjectResourceError, msg.str(), __FILE__, __LINE__);
         }

          mTargetTranslator->AddParameterTranslator(*((ParameterTranslator*(*)())createFn)());
        
      }
      catch (const dtUtil::Exception& e)
      {
         mMissingLibraries.push_back(mLibName);
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Unknown exception loading library %s version %s in the translator registry.  Exception message to follow.",
             mLibName.c_str(), mLibVersion.c_str());
         e.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
         throw e;
      }
   }

   void HLAFOMConfigContentHandler::endDocument()
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                         "Parsing HLA FOM Configuration Document Ended.\n");
   }

   void HLAFOMConfigContentHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length)
   {
      //okay, so I'm ignoring it.
   }

   void HLAFOMConfigContentHandler::processingInstruction(const XMLCh* const target, const XMLCh* const   data)
   {
   }

   void HLAFOMConfigContentHandler::setDocumentLocator(const xercesc_dt::Locator* const locator)
   {
   }

   void HLAFOMConfigContentHandler::startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri)
   {
   }

   void HLAFOMConfigContentHandler::endPrefixMapping(const XMLCh* const prefix)
   {
   }

   void HLAFOMConfigContentHandler::skippedEntity(const XMLCh* const name)
   {
   }

   xercesc_dt::InputSource* HLAFOMConfigContentHandler::resolveEntity(
      const XMLCh* const publicId,
      const XMLCh* const systemId)
   {
      return NULL;
   }

   void HLAFOMConfigContentHandler::error(const xercesc_dt::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
         "ERROR %d:%d - %s:%s - %s", exc.getLineNumber(),
         exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
         dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
         dtUtil::XMLStringConverter(exc.getMessage()).c_str());
      mTargetTranslator = NULL;
      throw exc;
   }

   void HLAFOMConfigContentHandler::fatalError(const xercesc_dt::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
         "FATAL-ERROR %d:%d - %s:%s - %s", exc.getLineNumber(),
         exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
         dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
         dtUtil::XMLStringConverter(exc.getMessage()).c_str());
      mTargetTranslator = NULL;
      throw exc;
   }

   void HLAFOMConfigContentHandler::warning(const xercesc_dt::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
         "WARNING %d:%d - %s:%s - %s", exc.getLineNumber(),
         exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
         dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
         dtUtil::XMLStringConverter(exc.getMessage()).c_str());
   }

   void HLAFOMConfigContentHandler::resetDocument()
   {
      while (!mElements.empty()) mElements.pop();
         mMissingActorTypes.clear();

      mUsingDisID = false;

      mInHLAConfig = false;
      mInHeader = false;
      mInLibraries = false;
      mInObjects = false;
      mInInteractions = false;

      ClearLibraryValues();

      mInObject = false;
      ClearObjectValues();
      mNamedObjectToActors.clear();
      mObjectToActors.clear();

      mInInteraction = false;
      ClearInteractionValues();
      mNamedInteractionToMessages.clear();
      mInteractionToMessages.clear();

      resetErrors();
   }

   void HLAFOMConfigContentHandler::ClearLibraryValues()
   {
      mInActorLibrary = false;
      mInTranslatorLibrary = false;

      mLibName.clear();
      mLibVersion.clear();
   }

   void HLAFOMConfigContentHandler::ClearObjectValues()
   {
      mInObjectHandler = false;
      mInActorTypeHandler = false;
      mCurrentObjectToActor = NULL;
      mCurrentObjectToActorIsAbstract = false;
      mCurrentAttrToProp = NULL;
      mParsingDISID = false;
      mParsingEnumMapping = false;
      mCurrentEnumHLAID = -1;
   }

   void HLAFOMConfigContentHandler::ClearInteractionValues()
   {
      mInInteractionHandler = false;
      mInMessageHandler = false;
      mCurrentInteractionToMessage = NULL;
      mCurrentInteractionToMessageIsAbstract = false;
      mCurrentParamToParam = NULL;
      mParsingEnumMapping = false;
      mCurrentEnumHLAID = -1;
   }

   void HLAFOMConfigContentHandler::resetErrors()
   {
      mMissingLibraries.clear();
      mMissingActorTypes.clear();
   }


}
