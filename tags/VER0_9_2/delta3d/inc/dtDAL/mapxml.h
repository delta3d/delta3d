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

#ifndef DELTA_MAPXML
#define DELTA_MAPXML

#include <string>
#include <stack>

#include <xercesc/sax2/ContentHandler.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/EntityResolver.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <dtCore/export.h>
#include <dtUtil/log.h>

#include <dtDAL/map.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/datatype.h>

XERCES_CPP_NAMESPACE_USE;

namespace dtDAL
{
    /**
     * @class MapXMLConstants
     * @brief Holds constants for all of the element names in the map XML.
     * <p>This class defines constants and handles the memory allocation and deallocation
     * for the values.</p>
     * <p>The constants are the names of the elements and any necessary
     * strings used when parsing or generating the map XML.  Since they are used with
     * the xerces parser, they strings are in stored in the multibyte format used with Xerces.
     * Since, however, the strings have to be created at startup, they are not const.  Do not
     * changes the value of these.  They are assumed to be allocated by the memory allocator
     * used internally to xerces and changing them will likely cause memory errors.</p>
     */
    class DT_EXPORT MapXMLConstants {
    public:

        static const char* const EDITOR_VERSION;
        static const char* const SCHEMA_VERSION;

        static XMLCh* END_XML_ELEMENT;
        static XMLCh* BEGIN_XML_DECL;
        static XMLCh* END_XML_DECL;

        static XMLCh* MAP_ELEMENT;
        static XMLCh* MAP_NAMESPACE;

        static XMLCh* HEADER_ELEMENT;
        static XMLCh* MAP_NAME_ELEMENT;
        static XMLCh* DESCRIPTION_ELEMENT;
        static XMLCh* AUTHOR_ELEMENT;
        static XMLCh* COMMENT_ELEMENT;
        static XMLCh* COPYRIGHT_ELEMENT;
        static XMLCh* CREATE_TIMESTAMP_ELEMENT;
        static XMLCh* LAST_UPDATE_TIMESTAMP_ELEMENT;
        static XMLCh* EDITOR_VERSION_ELEMENT;
        static XMLCh* SCHEMA_VERSION_ELEMENT;

        static XMLCh* LIBRARIES_ELEMENT;
        static XMLCh* LIBRARY_ELEMENT;
        static XMLCh* LIBRARY_NAME_ELEMENT;
        static XMLCh* LIBRARY_VERSION_ELEMENT;

        static XMLCh* ACTORS_ELEMENT;
        static XMLCh* ACTOR_ELEMENT;
        static XMLCh* ACTOR_TYPE_ELEMENT;
        static XMLCh* ACTOR_ID_ELEMENT;
        static XMLCh* ACTOR_NAME_ELEMENT;

        static XMLCh* ACTOR_PROPERTY_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_NAME_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_STRING_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_ENUM_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_FLOAT_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_DOUBLE_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_INTEGER_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_LONG_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_VEC2_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_VEC3_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_VEC4_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_BOOLEAN_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_COLOR_RGB_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_COLOR_RGBA_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_ACTOR_ID_ELEMENT;
        static XMLCh* ACTOR_PROPERTY_VECTOR_ELEMENT;

        static XMLCh* ACTOR_VEC_1_ELEMENT;
        static XMLCh* ACTOR_VEC_2_ELEMENT;
        static XMLCh* ACTOR_VEC_3_ELEMENT;
        static XMLCh* ACTOR_VEC_4_ELEMENT;

        static XMLCh* ACTOR_COLOR_R_ELEMENT;
        static XMLCh* ACTOR_COLOR_G_ELEMENT;
        static XMLCh* ACTOR_COLOR_B_ELEMENT;
        static XMLCh* ACTOR_COLOR_A_ELEMENT;

        /**
         * this is called automatically on startup.
         */
        static void StaticInit();
        /**
         * this is called automatically on shutdown.
         */
        static void StaticShutdown();
    private:
        //these are private because on one should cerate an instance of this class.
        MapXMLConstants();
        ~MapXMLConstants();
        MapXMLConstants(const MapXMLConstants& handler);
        MapXMLConstants& operator=(const MapXMLConstants& handler);

    };

    /**
     * @class MapContentHandler
     * @brief The SAX2 content handler for loading maps.
     */
    class DT_EXPORT MapContentHandler: public ContentHandler, public ErrorHandler,
        public EntityResolver, public osg::Referenced
    {
    public:

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
        virtual void setDocumentLocator(const Locator* const locator);

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
            const   XMLCh*  const  uri,
            const   XMLCh*  const  localname,
            const   XMLCh*  const  qname,
            const Attributes& attrs
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
        virtual InputSource* resolveEntity(
            const XMLCh* const publicId,
            const XMLCh* const systemId);

        //@}

        /** @name Default implementation of the ErrorHandler interface */
        //@{
        /**
        * @see ErrorHandler#warning
        * @see SAXParseException#SAXParseException
        */
        virtual void error(const SAXParseException& exc);

        /**
        * @see ErrorHandler#fatalError
        * @see SAXParseException#SAXParseException
        */
        virtual void fatalError(const SAXParseException& exc);

        /**
        * @see ErrorHandler#warning
        * @see SAXParseException#SAXParseException
        */
        virtual void warning(const SAXParseException& exc);

        /**
        * @see ErrorHandler#resetErrors
        */
        virtual void resetErrors();


        /**
        * @return true if the map parsing has come across the map name yet.
        */
        bool HasFoundMapName() const { return mFoundMapName; };

        /**
         * note: store a ref_ptr to this map immediately because reparsing with this handler
         * would otherwise cause it to be deleted.
         * @return a handle to the map parsed, or NULL if no map has been created.
         */
        Map* GetMap() { return mMap.get(); }

        /**
         * This causes the parser to release its reference to the map.
         */
        void ClearMap() { mMap = NULL; }

        MapContentHandler();
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

        osg::ref_ptr<Map> mMap;

        bool mInMap;
        bool mInHeader;
        bool mInLibraries;
        bool mInActors;
        bool mInActor;
        bool mIgnoreCurrentActor;
        bool mInActorProperty;
        bool mPreparingProp;

        std::string mLibName;
        std::string mLibVersion;

        std::stack<xmlCharString> mElements;

        std::vector<std::string> mMissingLibraries;
        std::set<std::string> mMissingActorTypes;

        std::string mCurrentPropName;
        bool mCurrentPropIsVector;

        std::string mDescriptorDisplayName;

        osg::ref_ptr<ActorProxy> mActorProxy;
        DataType* mActorPropertyType;
        osg::ref_ptr<ActorProperty> mActorProperty;

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
        //returns true if a property in the actor is the same as the XML expects.
        bool IsPropertyCorrectType();
        //Called from characters when the state says we are inside an actor element.
        void ActorCharacters(const XMLCh* const chars);
        //parses the text data from the xml and stores it in the property.
        void ParseData(std::string& dataValue);

    };

    /**
     * @class MapParser
     * @brief front end class for converting an XML map into a map instance.
     */
    class DT_EXPORT MapParser: public osg::Referenced
    {
    public:

        /**
         * Completely parses a map file.  Be sure store an osg::ref_ptr to the map immediately, otherwise
         * if the parser is deleted or another map file is parse, the map will get deleted.
         * @param path The file path to the map.
         * @param handler The content handler to be used when parsing.
         * @return A pointer to the loaded map.
         * @throws MapLoadParseError if a fatal error occurs in the parsing.
         */
        Map* Parse(const std::string& path);

        /**
         * Reads the assigned name from the map path given.
         * @param path the file path to the map.
         * @return the name of the map from the file.
         * @throws MapLoadParseError if any errors occurs in the parsing.
         */
        const std::string ParseMapName(const std::string& path);

        const std::set<std::string>& GetMissingActorTypes() { return mHandler.GetMissingActorTypes(); };
        const std::vector<std::string>& GetMissingLibraries() { return mHandler.GetMissingLibraries(); };

        MapParser();
        ~MapParser();

        /**
         * this is called automatically on startup.
         */
        static void StaticInit();
        /**
         * this is called automatically on shutdown.
         */
        static void StaticShutdown();

    private:
        MapParser(const MapParser& copyParser);
        MapParser& operator=(const MapParser& assignParser);
        MapContentHandler mHandler;
        SAX2XMLReader* mXercesParser;
        dtUtil::Log* mLogger;
    };


    /**
     * @class MapWriter
     * @brief Writes a map out to an XML file
     */
    class DT_EXPORT MapWriter: public osg::Referenced
    {
    public:
        MapWriter();
        /**
         * Saves the map to an XML file.
         * The create time will be set on the map if this is the first time it has been saved.
         * @param map the map to save.
         * @param filePath the path to the file to save.  The map has a file name property,
         *  but it does not include any directories needed or the extension.
         * @throws ExceptionEnum::MapSaveError if any errors occur saving the file.
         */
        void Save(Map& map, const std::string& filePath);

        /**
         * @param time the time in seconds since the epoch to convert.
         * @return the given time as a UTC formatted string.
         */
        static const std::string TimeAsUTC(time_t time);
    protected:
            virtual ~MapWriter(); ///Protected destructor so that this could be subclassed.
    private:

        class MapFormatTarget: public XMLFormatTarget
        {
        public:
            MapFormatTarget();
            virtual ~MapFormatTarget();

            void SetOutputFile(FILE* newFile);
            const FILE* GetOutputFile(FILE* newFile) const { return mOutFile; }

            virtual void writeChars(
                                    const XMLByte* const toWrite,
                                    const unsigned int count,
                                    XMLFormatter* const formatter);

            virtual void flush();

        private:
            FILE* mOutFile;
            dtUtil::Log*  mLogger;
        };

        typedef std::basic_string<XMLCh> xmlCharString;
        static const int indentSize = 4;
        dtUtil::Log* mLogger;

        bool mLastCharWasLF;
        std::stack<xmlCharString> mElements;

        MapFormatTarget mFormatTarget;
        XMLFormatter mFormatter;

        //writes out the open tags for a new element including indentation.
        void BeginElement(const XMLCh* const name, const XMLCh* const attributes = NULL);
        //writes out the end element tag including indentation if necessary.
        void EndElement();

        void AddCharacters(const xmlCharString& string);
        void AddCharacters(const std::string& string);

        void AddIndent();

    };

}

#endif
