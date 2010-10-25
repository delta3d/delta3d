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

#include <dtDAL/export.h>
#include <dtDAL/basexml.h>

namespace dtDAL
{
   class Map;
   class MapContentHandler;
   class BaseActorObject;
   class ActorPropertySerializer;
   
   /**
    * @class MapParser
    * @brief front end class for converting an XML map into a map instance.
    */
   class DT_DAL_EXPORT MapParser: public BaseXMLParser
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
         bool Parse(const std::string& path, Map** map);

         /**
         * Parses a prefab resource and places it in the given map
         * at a given location.
         *
         * @param[in]  path       The prefab filepath.
         * @param[in]  proxyList  The list of proxies loaded from the prefab.
         * @param[in]  map        The map we are loading the prefab for.
         */
         bool ParsePrefab(const std::string& path, std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> >& actorList, dtDAL::Map* map = NULL);

         /**
         * Parses only the header of a prefab's xml file and extracts the icon
         * file name.  Returns "" if no icon element is found in the header.        
         */
         const std::string GetPrefabIconFileName(const std::string& path);

         /**
          * Reads the assigned name from the map path given.
          * @param path the file path to the map.
          * @return the name of the map from the file.
          * @throws MapLoadParseError if any errors occurs in the parsing.
          */
         const std::string ParseMapName(const std::string& path);

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


   /**
    * @class MapWriter
    * @brief Writes a map out to an XML file
    */
   class DT_DAL_EXPORT MapWriter: public BaseXMLWriter
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
       * @throws ExceptionEnum::MapSaveError if any errors occur saving the file.
       */
      void Save(Map& map, const std::string& filePath);

      /**
       * Saves a number of given actor proxies into a prefab resource.
       */
      void SavePrefab(std::vector<dtCore::RefPtr<BaseActorObject> > actorList,
                      const std::string& filePath, const std::string& description,
                      const std::string& iconFile = "");
         
   protected:
      virtual ~MapWriter(); ///Protected destructor so that this could be subclassed.

   private:
         
      //disable copy constructor
      MapWriter(const MapWriter& toCopy): BaseXMLWriter(toCopy) {}
      //disable operator =
      MapWriter& operator=(const MapWriter& assignTo) { return *this;}

      ActorPropertySerializer* mPropSerializer;
   };
}

#endif
