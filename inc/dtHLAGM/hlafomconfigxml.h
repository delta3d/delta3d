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

#ifndef DELTA_HLAFOMCONFIGXML
#define DELTA_HLAFOMCONFIGXML

#include <string>
#include <stack>
#include <vector>
#include <set>
#include <map>

#include <osg/Referenced>

#include <dtCore/refptr.h>

#include <dtHLAGM/export.h>
#include <dtHLAGM/ddmregioncalculator.h>
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/interactiontomessage.h>

#include <xercesc/sax2/ContentHandler.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/EntityResolver.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>

namespace xercesc_dt = XERCES_CPP_NAMESPACE;

namespace dtUtil
{
   class Log;
}

namespace dtCore
{
   class ActorType;
}

namespace dtHLAGM
{
   class AttributeToPropertyList;
   class AttributeType;
   class ParameterToParameterList;
   class EntityType;
   class OneToManyMapping;
   class DDMRegionCalculator;

   class DT_HLAGM_EXPORT HLAFOMConfigContentHandler : public xercesc_dt::ContentHandler, public xercesc_dt::ErrorHandler,
        public xercesc_dt::EntityResolver, public osg::Referenced
   {
      public:
      	HLAFOMConfigContentHandler();

         /**
          * @see DocumentHandler#characters
          */
#if XERCES_VERSION_MAJOR < 3
         virtual void characters(const XMLCh* const chars, const unsigned int length);
#else
         virtual void characters(const XMLCh* const chars, const XMLSize_t length);
#endif

         /**
          * @see DocumentHandler#endDocument
          */
         virtual void endDocument();

         /**
          * @see DocumentHandler#endElement
          */
         virtual void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);

         /**
          * @see DocumentHandler#ignorableWhitespace
          */
#if XERCES_VERSION_MAJOR < 3
         virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
#else
         virtual void ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length);
#endif

         /**
          * @see DocumentHandler#processingInstruction
          */
         virtual void processingInstruction(const   XMLCh* const target, const XMLCh* const   data);

         /**
          * Any previously held onto map created during parsing will be deleted.
          * @see DocumentHandler#resetDocument
          */
         virtual void resetDocument();

         /**
          * @see Locator
          */
         virtual void setDocumentLocator(const xercesc_dt::Locator* const locator);

         /**
          * Any map held onto by a previous parsing will be deleted here and new map created.
          * @see DocumentHandler#startDocument
          */
         virtual void startDocument();

         /**
          * @see DocumentHandler#startElement
          */
         virtual void startElement
             (
             const XMLCh*  const  uri,
             const XMLCh*  const  localname,
             const XMLCh*  const  qname,
             const xercesc_dt::Attributes& attrs
             );

         /**
          * @see DocumentHandler#startPrefixMapping
          */
         virtual void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri);

         /**
          * @see DocumentHandler#endPrefixMapping
          */
         virtual void endPrefixMapping(const XMLCh* const prefix);

         /**
          * @see DocumentHandler#skippedEntity
          */
         virtual void skippedEntity(const XMLCh* const name);

         //@}

         /** @name Default implementation of the EntityResolver interface. */

         //@{
         /**
          * @see EntityResolver#resolveEntity
          */
         virtual xercesc_dt::InputSource* resolveEntity(
            const XMLCh* const publicId,
            const XMLCh* const systemId);

         /**
          * @see xercesc_dt::ErrorHandler#warning
          * @see xercesc_dt::SAXParseException#SAXParseException
          */
         virtual void error(const xercesc_dt::SAXParseException& exc);

         /**
          * @see xercesc_dt::ErrorHandler#fatalError
          * @see xercesc_dt::SAXParseException#SAXParseException
          */
         virtual void fatalError(const xercesc_dt::SAXParseException& exc);

         /**
          * @see xercesc_dt::ErrorHandler#warning
          * @see xercesc_dt::SAXParseException#SAXParseException
          */
         virtual void warning(const xercesc_dt::SAXParseException& exc);

         /**
          * @see xercesc_dt::ErrorHandler#resetErrors
          */
         virtual void resetErrors();

         void SetTargetTranslator(HLAComponent& target) { mTargetTranslator = &target; }
         HLAComponent& GetTargetTranslator() { return *mTargetTranslator; }

      private:
         static const std::string LOG_NAME;
      
         static const std::string HLA_FOM_ELEMENT;

         static const std::string HEADER_ELEMENT;
         static const std::string HEADER_NAME_ELEMENT;
         static const std::string HEADER_USE_DIS_ENTITIES_ELEMENT;
         static const std::string HEADER_DIS_ENTITIES_ATTR_ELEMENT;
         static const std::string HEADER_DESCRIPTION_ELEMENT;
         static const std::string HEADER_AUTHOR_ELEMENT;
         static const std::string HEADER_COMMENT_ELEMENT;
         static const std::string HEADER_COPYRIGHT_ELEMENT;
         static const std::string HEADER_SCHEMA_VERSION_ELEMENT;

         static const std::string LIBRARIES_ELEMENT;
         static const std::string LIBRARY_ACTOR_ELEMENT;
         static const std::string LIBRARY_TRANSLATOR_ELEMENT;
         static const std::string LIBRARY_NAME_ELEMENT;
         static const std::string LIBRARY_VERSION_ELEMENT;

         static const std::string DDM_ELEMENT;
         static const std::string DDM_ENABLED_ELEMENT;
         static const std::string DDM_SPACE_ELEMENT;
         static const std::string DDM_SPACE_NAME_ATTRIBUTE;
         static const std::string DDM_PROPERTY_ELEMENT;
         static const std::string DDM_PROPERTY_NAME_ATTRIBUTE;

         static const std::string OBJECTS_ELEMENT;
         static const std::string OBJECT_ELEMENT;
         static const std::string OBJECT_KEYNAME_ATTRIBUTE;
         static const std::string OBJECT_EXTENDS_ATTRIBUTE;
         static const std::string OBJECT_ABSTRACT_ELEMENT;
         static const std::string OBJECT_CLASS_ELEMENT;
         static const std::string OBJECT_DDM_SPACE_ELEMENT;
         static const std::string OBJECT_ACTOR_TYPE_ELEMENT;
         static const std::string OBJECT_ENTITY_ID_ELEMENT;
         static const std::string OBJECT_ENTITY_TYPE_ATTR_ELEMENT;
         static const std::string OBJECT_DIS_ENTITY_ELEMENT;
         static const std::string OBJECT_ENTITY_TYPE_ELEMENT;
         static const std::string OBJECT_ATTR_TO_PROP_ELEMENT;

         static const std::string OBJECT_HANDLER_ELEMENT;
         static const std::string OBJECT_HANDLER_OBJECT_CLASS_ELEMENT;
         static const std::string OBJECT_HANDLER_DIS_ENTITY_ELEMENT;
         static const std::string OBJECT_HANDLER_HANDLER_NAME_ELEMENT;
         static const std::string OBJECT_REMOTE_ONLY_ELEMENT;
         static const std::string OBJECT_LOCAL_ONLY_ELEMENT;

         static const std::string ACTORTYPE_HANDLER_ELEMENT;
         static const std::string ACTORTYPE_HANDLER_ACTOR_TYPE_ELEMENT;
         static const std::string ACTORTYPE_HANDLER_HANDLER_ELEMENT;

         static const std::string INTERACTIONS_ELEMENT;
         static const std::string INTERACTION_ELEMENT;
         static const std::string INTERACTION_KEYNAME_ATTRIBUTE;
         static const std::string INTERACTION_EXTENDS_ATTRIBUTE;
         static const std::string INTERACTION_ABSTRACT_ELEMENT;
         static const std::string INTERACTION_DDM_SPACE_ELEMENT;
         static const std::string INTERACTION_CLASS_ELEMENT;
         static const std::string INTERACTION_MESSAGE_TYPE_ELEMENT;
         static const std::string INTERACTION_PARAM_TO_PARAM_ELEMENT;

         static const std::string INTERACTION_HANDLER_ELEMENT;
         static const std::string INTERACTION_HANDLER_INTERACTION_CLASS_ELEMENT;
         static const std::string INTERACTION_HANDLER_HANDLER_NAME_ELEMENT;

         static const std::string MESSAGE_HANDLER_ELEMENT;
         static const std::string MESSAGE_HANDLER_MESSAGETYPE_ELEMENT;
         static const std::string MESSAGE_HANDLER_HANDLER_NAME_ELEMENT;

         static const std::string ONE_TO_MANY_PARAMETER_ELEMENT;
         static const std::string ONE_TO_MANY_HLA_NAME_ELEMENT;
         static const std::string ONE_TO_MANY_GAME_NAME_ELEMENT;
         static const std::string ONE_TO_MANY_HLA_DATATYPE_ELEMENT;
         static const std::string ONE_TO_MANY_DATATYPE_ARRAY;
         static const std::string ONE_TO_MANY_GAME_DATATYPE_ELEMENT;
         static const std::string ONE_TO_MANY_HLA_REQUIRED_ELEMENT;
         static const std::string ONE_TO_MANY_GAME_REQUIRED_ELEMENT;
         static const std::string ONE_TO_MANY_DEFAULT_ELEMENT;
         static const std::string ONE_TO_MANY_ENUM_MAPPING_ELEMENT;
         static const std::string ONE_TO_MANY_ENUM_HLA_ID_ATTRIBUTE;
         static const std::string ONE_TO_MANY_ENUM_VALUE_ELEMENT;

         static const std::string DIS_ENTITY_KIND_ELEMENT;
         static const std::string DIS_ENTITY_DOMAIN_ELEMENT;
         static const std::string DIS_ENTITY_COUNTRY_ELEMENT;
         static const std::string DIS_ENTITY_CATEGORY_ELEMENT;
         static const std::string DIS_ENTITY_SUBCATEGORY_ELEMENT;
         static const std::string DIS_ENTITY_SPECIFIC_ELEMENT;
         static const std::string DIS_ENTITY_EXTRA_ELEMENT;

         dtUtil::Log* mLogger;

         dtCore::RefPtr<HLAComponent> mTargetTranslator;

         std::stack<std::string> mElements;
         std::vector<std::string> mMissingLibraries;
         std::set<std::string> mMissingActorTypes;
         
         std::string mCurrentEnumHLAID;

         std::string mCurrentDDMSpaceName;
         std::string mCurrentDDMPropertyName;
         dtCore::RefPtr<DDMRegionCalculator> mCurrentDDMSubscriptionCalculator;
         dtCore::RefPtr<DDMRegionCalculator> mCurrentDDMPublishingCalculator;
         
         std::string mLibName;
         std::string mLibVersion;

         dtCore::RefPtr<ObjectToActor> mCurrentObjectToActor;
         dtCore::RefPtr<InteractionToMessage> mCurrentInteractionToMessage;

         std::map<std::string, dtCore::RefPtr<ObjectToActor> > mNamedObjectToActors;
         std::map<std::string, dtCore::RefPtr<InteractionToMessage> > mNamedInteractionToMessages;

         AttributeToPropertyList* mCurrentAttrToProp;
         ParameterToParameterList* mCurrentParamToParam;
         bool mInMultiParam;

         std::vector<dtCore::RefPtr<ObjectToActor> > mObjectToActors;
         std::vector<dtCore::RefPtr<InteractionToMessage> > mInteractionToMessages;

         bool mInHLAConfig:1;
         bool mInHeader:1;

         bool mUsingDisID:1;

         bool mInLibraries:1;
         bool mInActorLibrary:1;
         bool mInTranslatorLibrary:1;
         
         bool mInDDM:1;
         bool mDDMEnabled:1;
         
         bool mInObjects:1;
         bool mInObject:1;
         bool mInObjectHandler:1;
         bool mInActorTypeHandler:1;

         bool mInInteractions:1;
         bool mInInteraction:1;
         bool mInInteractionHandler:1;
         bool mInMessageHandler:1;

         //DIS ID's are used in a couple places
         //this flag is used for each case.
         bool mParsingDISID:1;

         //Enum mapping is used when the game datatype is a Enumeration
         //and that needs to be mapped to the numeric values coming
         //from HLA.
         bool mParsingEnumMapping:1;

         bool mCurrentObjectToActorIsAbstract:1;
         bool mCurrentInteractionToMessageIsAbstract:1;

         void StartElementEnumMapping(const std::string& elementName, const xercesc_dt::Attributes& attrs);

         void ObjectToActorCharacters(const std::string& elementName, const std::string& characters);
         const dtCore::ActorType* FindActorType(const std::string& actorTypeFullName);

         // This parses the entity type as one string 10.1.3.... etc
         void EntityTypeCharacters(EntityType& entityType, const std::string& elementName, const std::string& characters);
         // This parses the entity type set in pieces using the separate elements.
         void DISIDCharacters(EntityType& entityType, const std::string& elementName, const std::string& characters);

         void DDMCharacters(const std::string& elementName, const std::string& characters);
         
         void InteractionToMessageCharacters(const std::string& elementName, const std::string& characters);

         ///Handles the characters for any OneToManyMapping object.  This is used by both object and interation mappings
         void OneToManyCharacters(const std::string& elementName, const std::string& characters, OneToManyMapping& mapping);

         void GetAttributeValue(const std::string& attrName, const xercesc_dt::Attributes& attrs, std::string& toFill);

         void LoadActorLibrary();
         void LoadTranslatorLibrary();

         //reset/clear all of the library data/state variables
         void ClearLibraryValues();
         //reset/clear all of the DDM data/state variables
         void ClearDDMValues();
         //reset/clear all of the Object data/state variables
         void ClearObjectValues();
         //reset/clear all of the Interation data/state variables
         void ClearInteractionValues();

         //make the copy constructor unusable
         HLAFOMConfigContentHandler(const HLAFOMConfigContentHandler& toCopy) {}
         //make operator= unusable
         HLAFOMConfigContentHandler& operator=(const HLAFOMConfigContentHandler& assignTo) { return *this;}
         virtual ~HLAFOMConfigContentHandler();
   };

}

#endif /*DELTA_HLAFOMCONFIGXML*/
