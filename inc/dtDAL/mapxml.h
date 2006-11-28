/* -*-c++-*-
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

#ifndef DELTA_MAPXML
#define DELTA_MAPXML

#include <string>
#include <stack>
#include <map>
#include <set>
#include <vector>

#include <osg/Referenced>

#include <dtCore/refptr.h>
#include <dtDAL/export.h>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>

namespace dtUtil
{
   class Log;
}

namespace dtDAL
{
   class Map;
   class ActorProxy;
   class AbstractParameter;
   class ActorProperty;
   class DataType;
   class NamedParameter;
   class MapContentHandler;
   
   /**
    * @class MapParser
    * @brief front end class for converting an XML map into a map instance.
    */
   class DT_DAL_EXPORT MapParser: public osg::Referenced
   {
      public:

         /**
          * Completely parses a map file.  Be sure store an dtCore::RefPtr to the map immediately, otherwise
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

         ///@return true if the map is currently being parsed.
         bool IsParsing() const { return mParsing; }

         /**
          * This method exists to allow actors being parsed to access their map.  It's to help 
          * with certain features of the loading process.  Using the meth
          * @return the map instance that the parser is currently populating.
          */
         Map* GetMapBeingParsed();
         
         ///@see #GetMapBeingParsed
         const Map* GetMapBeingParsed() const;

         const std::set<std::string>& GetMissingActorTypes(); 
         const std::vector<std::string>& GetMissingLibraries();

         MapParser();

   protected:

         virtual ~MapParser();

   public:

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
         dtCore::RefPtr<MapContentHandler> mHandler;
         xercesc::SAX2XMLReader* mXercesParser;
         dtUtil::Log* mLogger;
         bool mParsing;
   };


   /**
    * @class MapWriter
    * @brief Writes a map out to an XML file
    */
   class DT_DAL_EXPORT MapWriter: public osg::Referenced
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

         
      protected:
         virtual ~MapWriter(); ///Protected destructor so that this could be subclassed.
      private:

         class MapFormatTarget: public xercesc::XMLFormatTarget
         {
            public:
               MapFormatTarget();
               virtual ~MapFormatTarget();

               void SetOutputFile(FILE* newFile);
               const FILE* GetOutputFile(FILE* newFile) const { return mOutFile; }

               virtual void writeChars(
                  const XMLByte* const toWrite,
                  const unsigned int count,
                  xercesc::XMLFormatter* const formatter);

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
         xercesc::XMLFormatter mFormatter;

         //writes out the open tags for a new element including indentation.
         void BeginElement(const XMLCh* const name, const XMLCh* const attributes = NULL);
         //writes out the end element tag including indentation if necessary.
         void EndElement();

         void AddCharacters(const xmlCharString& string);
         void AddCharacters(const std::string& string);

         void AddIndent();

         template <typename VecType>
         void WriteVec(const VecType& vec, size_t vecSize, char* numberConversionBuffer, const size_t bufferMax);
            
         template <typename Type>
         void WriteColorRGBA(const Type& holder, char* numberConversionBuffer, size_t bufferMax);
         
         void WriteSimple(const AbstractParameter& holder);

         void WriteProperty(const ActorProperty& property);
         void WriteParameter(const NamedParameter& param);

         //disable copy constructor
         MapWriter(const MapWriter& toCopy): mFormatter("UTF-8", NULL, &mFormatTarget, xercesc::XMLFormatter::NoEscapes, xercesc::XMLFormatter::DefaultUnRep) {}
         //disable operator =
         MapWriter& operator=(const MapWriter& assignTo) { return *this;}


   };

}

#endif
