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

#ifndef DELTA_MAP_CONTENT_HANDLER
#define DELTA_MAP_CONTENT_HANDLER

#include <vector>
#include <string>
#include <stack>
#include <map>
#include <set>

#include <dtDAL/export.h>
#include <dtCore/uniqueid.h>
#include <dtCore/refptr.h>

#include <xercesc/sax2/ContentHandler.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/EntityResolver.hpp>

namespace dtUtil
{
   class Log;
}

namespace dtDAL
{
   class ActorProxy;
   class ActorProperty;
   class AbstractParameter;
   class GameEvent;
   class DataType;
   class Map;
   class NamedParameter;
   class NamedGroupParameter;
   class DataType;

   /**
    * @class MapContentHandler
    * @brief The SAX2 content handler for loading maps.
    */
   class DT_DAL_EXPORT MapContentHandler: public xercesc::ContentHandler, public xercesc::ErrorHandler,
                                          public xercesc::EntityResolver, public osg::Referenced
   {
      public:

         ///Constructor
         MapContentHandler();

         virtual const std::vector<std::string>& GetMissingLibraries() { return mMissingLibraries; }
         virtual const std::set<std::string>& GetMissingActorTypes() { return mMissingActorTypes; }

         /**
          * @see DocumentHandler#characters
          */
         virtual void characters(const XMLCh* const chars, const unsigned int length);

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
         virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);

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
         virtual void setDocumentLocator(const xercesc::Locator* const locator);

         /**
          * Any map held onto by a previous parsing will be deleted here and new map created.
          * @see DocumentHandler#startDocument
          */
         virtual void startDocument();

         /**
          * @see DocumentHandler#startElement
          */
         virtual void startElement( const XMLCh*  const  uri,
                                    const XMLCh*  const  localname,
                                    const XMLCh*  const  qname,
                                    const xercesc::Attributes& attrs );

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
         virtual xercesc::InputSource* resolveEntity(
            const XMLCh* const publicId,
            const XMLCh* const systemId);

         //@}

         /** @name Default implementation of the ErrorHandler interface */
         //@{
         /**
          * @see xercesc::ErrorHandler#warning
          * @see xercesc::SAXParseException#SAXParseException
          */
         virtual void error(const xercesc::SAXParseException& exc);

         /**
          * @see xercesc::ErrorHandler#fatalError
          * @see xercesc::SAXParseException#SAXParseException
          */
         virtual void fatalError(const xercesc::SAXParseException& exc);

         /**
          * @see xercesc::ErrorHandler#warning
          * @see xercesc::SAXParseException#SAXParseException
          */
         virtual void warning(const xercesc::SAXParseException& exc);

         /**
          * @see xercesc::ErrorHandler#resetErrors
          */
         virtual void resetErrors();


         /**
          * @return true if the map parsing has come across the map name yet.
          */
         bool HasFoundMapName() const { return mFoundMapName; };

         /**
          * note: store a RefPtr to this map immediately because reparsing with this handler
          * would otherwise cause it to be deleted.
          * @return a handle to the map parsed, or NULL if no map has been created.
          */
         Map* GetMap();
         const Map* GetMap() const;

         /**
          * This causes the parser to release its reference to the map.
          */
         void ClearMap();

      protected: // This class is referenced counted, but this causes an error...

         virtual ~MapContentHandler();

      private:
         //A string using the xerces multibyte character.  This allows
         //for easier coding.
         typedef std::basic_string<XMLCh> xmlCharString;
         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         MapContentHandler(const MapContentHandler&);
         MapContentHandler& operator=(const MapContentHandler&);

         dtCore::RefPtr<Map> mMap;

         bool mInMap;
         bool mInHeader;
         bool mInLibraries;
         bool mInEvents;
         bool mInActors;
         bool mInActor;
         bool mIgnoreCurrentActor;
         bool mInActorProperty;
         bool mPreparingProp;
         bool mInGroupProperty;

         std::string mLibName;
         std::string mLibVersion;

         dtCore::UniqueId mEnvActorId;

         std::stack<xmlCharString> mElements;

         std::vector<std::string> mMissingLibraries;
         std::set<std::string> mMissingActorTypes;

         //data for actor linking is not completely available until all actors have been created, so it
         //is stored until the end.
         std::multimap<dtCore::UniqueId, std::pair<std::string, dtCore::UniqueId> > mActorLinking;
         //The data for group parameters is like linked actors, it needs to be set only after the map is done loading
         //so things are not in a bad state.
         std::multimap<dtCore::UniqueId, std::pair<std::string, dtCore::RefPtr<dtDAL::NamedGroupParameter> > > mGroupParameters;

         std::string mCurrentPropName;
         bool mCurrentPropIsVector;

         std::string mDescriptorDisplayName;

         dtCore::RefPtr<GameEvent> mGameEvent;

         dtCore::RefPtr<ActorProxy> mActorProxy;
         DataType* mActorPropertyType;
         dtCore::RefPtr<ActorProperty> mActorProperty;

         std::stack<dtCore::RefPtr<NamedParameter> > mParameterStack;
         std::string mParameterNameToCreate;
         DataType* mParameterTypeToCreate;

         dtUtil::Log* mLogger;

         int mErrorCount;
         int mFatalErrorCount;
         int mWarningCount;

         bool mFoundMapName;

         //Reset all of the internal data/state variables
         void Reset();
         //reset/clear all of the library data/state variables
         void ClearLibraryValues();
         //reset/clear all of the actor data/state variables
         void ClearActorValues();
         //reset/clear all of the parameter data/state variables
         void ClearParameterValues();
         //returns true if a property in the actor is the same as the XML expects and adjusts the value.
         bool IsPropertyCorrectType();
         //Called from characters when the state says we are inside an actor element.
         void ActorCharacters(const XMLCh* const chars);
         //Called from characters when the state says we are inside a parameter of a group actor property.
         void ParameterCharacters(const XMLCh* const chars);
         //parses the text data from the xml and stores it in the property.
         void ParsePropertyData(std::string& dataValue);
         //parses the text data from the xml and stores it in the property.
         void ParseParameterData(std::string& dataValue);
         //parses one item out of the xml and stores it in the proper element of the osg Vec#.
         template <typename VecType>
         void ParseVec(const std::string& dataValue, VecType& vec, size_t vecSize);
         //processes the mActorLinking multimap to set ActorActorProperties.
         void LinkActors();
         //processes the mGroupProperties multimap to set GroupActorProperties.
         void AssignGroupProperties();
         //decides on a property's datatype base on the name of the element.
         DataType* ParsePropertyType(const XMLCh* const localname, bool errorIfNotFound = true);
         //Creates a named parameter based on the name and type last parsed and pushes it to the top of the stack.
         void CreateAndPushParameter();
         // Workaround for properties where empty data would result in the property not being set.
         void NonEmptyDefaultWorkaround();

         void EndHeaderElement(const XMLCh* const localname);
         void EndActorSection(const XMLCh* const localname);
         void EndActorsElement();
         void EndActorElement();
         void EndActorPropertySection(const XMLCh* const localname);
         void EndActorPropertyGroupElement();
         void EndActorPropertyParameterElement();
         void EndActorPropertyElement();
         void EndLibrarySection(const XMLCh* const localname);
         void EndLibraryElement();
         void EndEventSection(const XMLCh* const localname);
   };
}
#endif
