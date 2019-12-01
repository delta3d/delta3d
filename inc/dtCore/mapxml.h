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

#include <dtCore/export.h>
#include <dtCore/basexml.h>
#include <dtCore/baseactorobject.h>
#include <dtUtil/tree.h>
#include <dtCore/map.h>

namespace dtCore
{
   class MapContentHandler;
   class ActorPropertySerializer;
   class ActorHierarchyNode;
   class ActorComponentContainer;

   typedef dtUtil::Tree<dtCore::BaseActorObject*> ActorTree;

   /**
    * @class MapParser
    * @brief class for converting an XML map into a map instance.
    * @note nothing in this class is considered part of the public api.  All of this data can be read from project.h.
    * As such, functions could change, be added or removed, or this entire class could be removed with no warning.
    */
   class DT_CORE_EXPORT MapParser: public BaseXMLParser
   {
      public:

         /**
          * Constructor.
          */
         MapParser();

         /**
          * Completely parses a map file.  Be sure store an dtCore::RefPtr to the map immediately, otherwise
          * if the parser is deleted or another map file is parse, the map will get deleted.
          * @param path The file path to the map.
          * @param handler The content handler to be used when parsing.
          * @return A pointer to the loaded map.
          * @throws MapLoadParseError if a fatal error occurs in the parsing.
          */
         bool Parse(const std::string& path, Map** map, bool prefab = false);
         bool Parse(std::istream& stream, Map** map, bool prefab = false);

         /**
          * Reads the supplied filename as a Map xml file and extracts the Map
          * file's header data. Will not create a Map nor anything contained in the Map.
          * @param mapFilename The Map file to parse
          * @param prefab      if this map refers to a prefab.
          * @return The parsed MapHeaderData
          * @throws dtCore::XMLLoadParsingException if file can't be found, or any parsing errors
          */
         MapPtr ParseMapHeaderData(const std::string& mapFileName, bool prefab = false) const;
         MapPtr ParseMapHeaderData(std::istream& stream, bool prefab = false) const;

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

         bool HasDeprecatedProperty() const;

   protected:

         virtual ~MapParser();

   private:
      MapParser(const MapParser& copyParser);
      MapParser& operator=(const MapParser& assignParser);

      dtCore::RefPtr<MapContentHandler> mMapHandler;
   };
   typedef RefPtr<MapParser> MapParserPtr;


   /**
    * @class MapWriter
    * @brief Writes a map out to an XML file
    */
   class DT_CORE_EXPORT MapWriter: public BaseXMLWriter
   {
   public:

      /**
       * Constructor.
       */
      MapWriter();

      /**
       * Saves the map to an XML file.
       * The create time will be set on the map if this is the first time it has been saved.
       * @param map the map to save.
       * @param filePath the path to the file to save.  The map has a file name property,
       *  but it does not include any directories needed or the extension.
       * @throws MapSaveException if any errors occur saving the file.
       */
      void Save(Map& map, const std::string& filePath, bool prefab = false);

      /**
       * Saves the map to an XML file.
       * The create time will be set on the map if this is the first time it has been saved.
       * @param map the map to save.
       * @param stream the stream to write into.
       * @param prefab Same the map as a prefab.  This is called this way from the SavePrefab function.
       * @throws MapSaveException if any errors occur saving the file.
       */
      void Save(Map& map, std::ostream& stream, bool prefab = false);


   protected:
      virtual ~MapWriter(); ///Protected destructor so that this could be subclassed.

      /**
       * Shared method for writing out an actor definition in XML.
       * This is a recursive method which allows it to write child actor definitions.
       * @param actor Actor object with properties to be written.
       * @param allowReadOnlyProps Flag for determining if read only properties should be written.
       * @return Number of successfully written actor definitions.
       */
      int WriteActor(dtCore::BaseActorObject& actor, bool allowReadOnlyProps);

   private:

      //disable copy constructor
      MapWriter(const MapWriter& toCopy): BaseXMLWriter(toCopy), mPropSerializer(NULL) {}
      //disable operator =
      MapWriter& operator=(const MapWriter&) { return *this; }

      //void WriteHierarchyBranch(dtCore::ActorComponentContainer& actor);

      ActorPropertySerializer* mPropSerializer;
   };
   typedef RefPtr<MapWriter> MapWriterPtr;
}

#endif
