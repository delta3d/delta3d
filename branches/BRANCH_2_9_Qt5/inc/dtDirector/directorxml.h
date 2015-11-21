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

#include <string>
#include <vector>
#include <set>

#include <dtDirector/export.h>

#include <dtCore/basexml.h>
#include <dtDirector/directorxmlhandler.h>
#include <dtDirector/director.h>

namespace dtCore
{
   class Map;
   class ActorPropertySerializer;
}

namespace dtDirector
{
   /**
    * @class DirectorParser
    *
    * @brief front end class for loading a dtDirector script file.
    */
   class DT_DIRECTOR_EXPORT DirectorParser: public dtCore::BaseXMLParser
   {
   public:

      /**
       * Constructor.
       */
      DirectorParser();

      /**
       * Completely parses a Director script file.
       *
       * @param[in]  director  A Reference to the director.
       * @param[in]  map       The currently loaded map.
       * @param[in]  filePath  The path to the director script file.
       *
       * @throws dtUtil::Exception if a fatal error occurs in the parsing.
       */
      bool Parse(Director* director, dtCore::Map* map, const std::string& filePath);

      /**
       * Reads the supplied filename as a Director xml file and extracts the Director
       * file's header data. Will not create a Director nor anything contained in the Director.
       * @param mapFilename The Director file to parse
       * @return The parsed Director with only the header filled out.
       * @throws dtCore::XMLLoadParsingException if file can't be found, or any parsing errors
       */
      DirectorPtr ParseDirectorHeaderData(const std::string& directorFilename) const;

      /**
       * Retrieves all missing node types.
       */
      const std::set<std::string>& GetMissingNodeTypes();

      /**
       * Retrieves all missing libraries.
       */
      const std::vector<std::string>& GetMissingLibraries();

      /**
       *	Retrieves all missing imported scripts.
       */
      const std::vector<std::string>& GetMissingImportedScripts();

      /**
       * Retrieves whether any loaded properties were deprecated.
       */
      bool HasDeprecatedProperty() const;

   protected:

      virtual ~DirectorParser();

   private:
      DirectorParser(const DirectorParser& copyParser);
      DirectorParser& operator=(const DirectorParser& assignParser);

      dtCore::RefPtr<DirectorXMLHandler> mDirectorHandler;
   };


   /**
    * @class DirectorWriter
    *
    * @brief Writes a director script out to an XML file
    */
   class DT_DIRECTOR_EXPORT DirectorWriter: public dtCore::BaseXMLWriter
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
      void Save(Director* director, const std::string& filePath);

   protected:
      virtual ~DirectorWriter(); ///Protected destructor so that this could be subclassed.

   private:

      struct ToLinkData
      {
         ID          outputNodeID;
         ID          inputNodeID;
         std::string outputLinkName;
         std::string inputLinkName;
      };

      /**
       * Saves a graph.
       *
       * @param[in]  graph  The graph.
       */
      void SaveGraphs(DirectorGraph* graph);

      /**
       * Saves a node.
       *
       * @param[in]  node  The Node.
       */
      void SaveNode(Node* node);

      /**
       *	Finds and saves all link connections.
       *
       * @param[in]  script       The script to save.
       */
      void SaveLinkConnections(dtDirector::Director* script);

      //disable copy constructor
      DirectorWriter(const DirectorWriter& toCopy): BaseXMLWriter(toCopy) {}
      //disable operator =
      DirectorWriter& operator=(const DirectorWriter& assignTo) { return *this;}

      dtCore::ActorPropertySerializer* mPropSerializer;
   };
}

#endif
