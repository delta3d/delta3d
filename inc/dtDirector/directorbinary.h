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

#ifndef DIRECTOR_BINARY
#define DIRECTOR_BINARY

#include <dtDirector/export.h>
#include <dtDirector/directorgraph.h>

#include <osg/Referenced>

#include <string>
#include <vector>
#include <set>
#include <stdio.h>


namespace dtCore
{
   class Map;
}

namespace dtDirector
{
   class Director;

   /**
    * @class BinaryParser
    *
    * @brief front end class for loading a binary dtDirector script file.
    */
   class DT_DIRECTOR_EXPORT BinaryParser: public osg::Referenced
   {
   public:

      BinaryParser();

      /**
       * Completely parses a Director script file.
       *
       * @param[in]  director  A Reference to the director.
       * @param[in]  map       The currently loaded map.
       * @param[in]  filePath  The path to the director script file.
       *
       * @throws MapLoadParseError if a fatal error occurs in the parsing.
       */
      void Parse(Director* director, dtCore::Map* map, const std::string& filePath);

      /**
       * Parses the script type from the Director script file.
       *
       * @param[in]  filePath  The path to the director script file.
       *
       * @return     The script type.
       *
       * @throws dtUtil::Exception if a fatal error occurs in the parsing.
       */
      const std::string& ParseScriptType(const std::string& filePath);

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

      virtual ~BinaryParser();

   private:

      /**
       * Parses the header data.
       *
       * @parma[in]  director  The director script.
       * @param[in]  file      The file.
       *
       * @return     The file version.
       */
      float ParseHeader(Director* director, FILE* file);

      /**
       * Parses graph data.
       *
       * @param[in]  version   The file version.
       * @param[in]  director  The director script.
       * @param[in]  parent    The parent of the graph.
       * @param[in]  file      The file.
       */
      void ParseGraph(float version, Director* director, DirectorGraph* parent, FILE* file);

      /**
       * Parses node data.
       *
       * @param[in]  version  The file version.
       * @param[in]  graph    The parent graph.
       * @param[in]  file     The file.
       */
      void ParseNode(float version, DirectorGraph* graph, FILE* file);

      /**
       *	Parses all link connections.
       *
       * @param[in]  version  The file version.
       * @param[in]  script   The script.
       * @param[in]  file     The file.
       */
      void ParseLinkConnections(float version, Director* script, FILE* file);

      /**
       * Parses a property container.
       *
       * @param[in]  container  The property container.
       * @param[in]  file       The file.
       */
      void ParsePropertyContainer(dtCore::PropertyContainer* container, FILE* file);

      /**
       * Parses a string value.
       *
       * @param[in]  file  The file.
       *
       * @return     The string.
       */
      std::string ParseString(FILE* file);

      /**
       * Connect all nodes together.
       *
       * @parma[in]  director  The director script.
       */
      void LinkNodes(Director* director);
      void RemoveLinkNodes(Director* director);

      BinaryParser(const BinaryParser& copyParser);
      BinaryParser& operator=(const BinaryParser& assignParser);

      struct ToLinkData
      {
         ToLinkData()
         {
            isValue = false;
         }

         ID          outputNodeID;
         ID          inputNodeID;
         std::string outputLinkName;
         std::string inputLinkName;

         bool isValue;
      };

      std::vector<ToLinkData>  mLinkList;
      std::vector<ToLinkData>  mRemovedLinkList;

      std::vector<std::string> mMissingLibraries;
      std::set<std::string>    mMissingNodeTypes;
      std::vector<std::string> mMissingImportedScripts;
      bool                     mHasDeprecatedProperty;
      std::string              mScriptType;
   };


   /**
    * @class BinaryWriter
    *
    * @brief Writes a director script out to a binary file
    */
   class DT_DIRECTOR_EXPORT BinaryWriter: public osg::Referenced
   {
   public:

      BinaryWriter();

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
      virtual ~BinaryWriter();

   private:

      struct ToLinkData
      {
         ID          outputNodeID;
         ID          inputNodeID;
         std::string outputLinkName;
         std::string inputLinkName;
      };

      /**
       * Saves the header data.
       *
       * @param[in]  director  The director script.
       * @param[in]  file      The file.
       */
      void SaveHeader(Director* director, FILE* file);

      /**
       * Saves a graph.
       *
       * @param[in]  graph  The graph.
       * @param[in]  file   The file.
       */
      void SaveGraph(DirectorGraph* graph, FILE* file);

      /**
       * Saves a node.
       *
       * @param[in]  node  The Node.
       * @param[in]  file  The file.
       */
      void SaveNode(Node* node, FILE* file);

      /**
       *	Finds and saves all link connections.
       *
       * @param[in]  script  The script to save.
       * @param[in]  file    The file.
       */
      void SaveLinkConnections(dtDirector::Director* script, FILE* file);

      /**
       * Saves a property container.
       *
       * @param[in]  container  The property container.
       * @param[in]  file       The file.
       */
      void SavePropertyContainer(dtCore::PropertyContainer* container, FILE* file);

      /**
       * Saves a string value.
       *
       * @param[in]  str   The string to save.
       * @param[in]  file  The file.
       */
      void SaveString(const std::string& str, FILE* file);

      BinaryWriter(const BinaryWriter& toCopy) {}
      BinaryWriter& operator=(const BinaryWriter& assignTo) { return *this;}
   };
}

#endif
