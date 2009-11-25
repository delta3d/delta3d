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
 * Jeff P. Houde
 */

#ifndef DIRECTOR_XML
#define DIRECTOR_XML

#include <dtDirector/export.h>

#include <dtDirector/director.h>

#include <dtDAL/basexml.h>


namespace dtDAL
{
   class Map;
   class MapContentHandler;
   class ActorProxy;
   class ActorPropertySerializer;
}

namespace dtDirector
{
   ///**
   // * @class DirectorParser
   // * @brief front end class for loading a dtDirector script file.
   // */
   //class DT_DIRECTOR_EXPORT DirectorParser: public dtDAL::BaseXMLParser
   //{
   //   public:

   //      /**
   //       * Constructor.
   //       */
   //      DirectorParser();

   //      /**
   //       * Completely parses a map file.  Be sure store an dtCore::RefPtr to the map immediately, otherwise
   //       * if the parser is deleted or another map file is parse, the map will get deleted.
   //       * @param path The file path to the map.
   //       * @param handler The content handler to be used when parsing.
   //       * @return A pointer to the loaded map.
   //       * @throws MapLoadParseError if a fatal error occurs in the parsing.
   //       */
   //      bool Parse(const std::string& path, Map** map);

   //      /**
   //      * Parses a prefab resource and places it in the given map
   //      * at a given location.
   //      *
   //      * @param[in]  path       The prefab filepath.
   //      * @param[in]  proxyList  The list of proxies loaded from the prefab.
   //      * @param[in]  map        The map we are loading the prefab for.
   //      */
   //      bool ParsePrefab(const std::string& path, std::vector<dtCore::RefPtr<dtDAL::ActorProxy> >& proxyList, dtDAL::Map* map = NULL);

   //      /**
   //      * Parses only the header of a prefab's xml file and extracts the icon
   //      * file name.  Returns "" if no icon element is found in the header.        
   //      */
   //      const std::string GetPrefabIconFileName(const std::string& path);

   //      /**
   //       * Reads the assigned name from the map path given.
   //       * @param path the file path to the map.
   //       * @return the name of the map from the file.
   //       * @throws MapLoadParseError if any errors occurs in the parsing.
   //       */
   //      const std::string ParseMapName(const std::string& path);

   //      /**
   //       * This method exists to allow actors being parsed to access their map.  It's to help 
   //       * with certain features of the loading process.  Using the meth
   //       * @return the map instance that the parser is currently populating.
   //       */
   //      Map* GetMapBeingParsed();
   //      
   //      ///@see #GetMapBeingParsed
   //      const Map* GetMapBeingParsed() const;

   //      const std::set<std::string>& GetMissingActorTypes(); 
   //      const std::vector<std::string>& GetMissingLibraries();

   //      bool HasDeprecatedProperty() const;

   //protected:

   //      virtual ~DirectorParser();

   //private:
   //   DirectorParser(const DirectorParser& copyParser);
   //   DirectorParser& operator=(const DirectorParser& assignParser);

   //   dtCore::RefPtr<MapContentHandler> mMapHandler;
   //};


   /**
    * @class DirectorWriter
    * @brief Writes a director script out to an XML file
    */
   class DT_DIRECTOR_EXPORT DirectorWriter: public dtDAL::BaseXMLWriter
   {
   public:

      /**
       * Constructor.
       */
      DirectorWriter();

      /**
       * Saves the director script to an XML file.
       *
       * @param[in]  director  The director to save.
       * @param[in]  filePath  The path to the file.
       *
       * @throws     ExceptionEnum::MapSaveError if any errors occur saving the file.
       */
      void Save(Director& director, const std::string& filePath);
         
   protected:
      virtual ~DirectorWriter(); ///Protected destructor so that this could be subclassed.

   private:

      /**
       * Saves a node.
       *
       * @param[in]  node      The Node.
       */
      void SaveNode(Node* node);

      //disable copy constructor
      DirectorWriter(const DirectorWriter& toCopy): BaseXMLWriter(toCopy) {}
      //disable operator =
      DirectorWriter& operator=(const DirectorWriter& assignTo) { return *this;}

      dtDAL::ActorPropertySerializer* mPropSerializer;
   };
}

#endif
