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
 * Jeff P. Houde
 */

#ifndef DIRECTOR_XML_HANDLER
#define DIRECTOR_XML_HANDLER

#include <string>
#include <vector>
#include <set>

#include <dtDirector/node.h>

#include <dtCore/basexmlhandler.h>
#include <dtCore/actorpropertyserializer.h>
#include <dtCore/map.h>

#include <dtCore/uniqueid.h>

namespace dtDirector
{
   class Director;
   class DirectorGraph;
   class InputLink;
   class OutputLink;
   class ValueLink;

   /**
    * @class DirectorXMLHandler
    *
    * @brief The SAX2 content handler for loading maps.
    */
   class DT_DIRECTOR_EXPORT DirectorXMLHandler: public dtCore::BaseXMLHandler
   {
   public:

      /**
       * Constructor.
       */
      DirectorXMLHandler();

      /**
       * Any map held onto by a previous parsing will be deleted here and new map created.
       * @see DocumentHandler#startDocument
       */
      virtual void startDocument();

      /**
       * @see DocumentHandler#endDocument
       */
      virtual void endDocument();

      /**
       * @see DocumentHandler#startElement
       */
      virtual void ElementStarted(const XMLCh* const uri,
                                  const XMLCh* const localname,
                                  const XMLCh* const qname,
                                  const xercesc::Attributes& attrs);

      /**
       * @see DocumentHandler#endElement
       */
      virtual void ElementEnded(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);

      /**
       * @see BaseXMLHandler#CombinedCharacters
       */
      virtual void CombinedCharacters(const XMLCh* const chars, size_t length);

      /**
       * Sets the Director.
       *
       * @param[in]  director  The director.
       */
      void SetDirector(Director* director) {mDirector = director;}

      /**
       * Sets the map.
       *
       * @param[in]  map  The map.
       */
      void SetMap(dtCore::Map* map) {mMap = map;}

      /**
       * Retrieves a list of missing libraries.
       */
      const std::vector<std::string>& GetMissingLibraries() {return mMissingLibraries;}

      /**
       *	Retrieves a list of missing inheritances.
       */
      const std::vector<std::string>& GetMissingImportedScripts() {return mMissingImportedScripts;}

      /**
       * Retrieves a list of missing actor types.
       */
      const std::set<std::string>& GetMissingNodeTypes() {return mMissingNodeTypes;}

      /**
       * Returns whether or not the map had a temporary property in it.
       */
      bool HasDeprecatedProperty() const {return mPropSerializer->HasDeprecatedProperty();}

      /**
       * Returns whether or not the Director script type was parsed.
       */
      bool HasParsedHeader() const {return mHasParsedHeader;}
      const std::string& GetScriptType() const {return mScriptType;}

   protected:

      virtual ~DirectorXMLHandler();

   private:

      // -----------------------------------------------------------------------
      //  Unimplemented constructors and operators
      // -----------------------------------------------------------------------
      DirectorXMLHandler(const DirectorXMLHandler&);
      DirectorXMLHandler& operator=(const DirectorXMLHandler&);

      bool mInHeaders;
      bool mInLibraries;
      bool mInValueNodes;
      bool mInEventNodes;
      bool mInActionNodes;

      int mInGraph;

      bool mInNodes;
      bool mInNode;

      bool mInLink;
      bool mInRemoveLink;
      bool mInInputLink;
      bool mInOutputLink;
      bool mInValueLink;
      bool mInLinkTo;

      InputLink*  mInputLink;
      OutputLink* mOutputLink;
      ValueLink*  mValueLink;

      Director*   mDirector;
      dtCore::RefPtr<dtCore::Map> mMap;
      std::string mScriptType;
      bool        mHasParsedHeader;
      bool        mHasImportedScripts;

      float       mSchemaVersion;

      std::string mLibName;
      std::string mLibVersion;

      std::vector<std::string> mMissingLibraries;
      std::set<std::string> mMissingNodeTypes;
      std::vector<std::string> mMissingImportedScripts;

      std::stack<dtCore::RefPtr<DirectorGraph> > mGraphs;

      dtCore::ActorPropertySerializer* mPropSerializer;

      dtCore::RefPtr<Node> mNode;
      std::string mNodeName;
      std::string mNodeCategory;

      ID           mLinkOutputOwnerID;
      std::string  mLinkOutputName;
      ID           mLinkNodeID;
      std::string  mLinkToName;

      ID           mGraphID;
      bool         mIsGraphImported;
      bool         mIsGraphReadOnly;

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

      std::vector<ToLinkData> mLinkList;
      std::vector<ToLinkData> mRemovedLinkList;

      //Reset all of the internal data/state variables
      void Reset();
      //reset/clear all of the library data/state variables
      void ClearLibraryValues();
      //reset/clear all of the node data/state variables
      void ClearNodeValues();
      //reset/clear all node link data/state variables.
      void ClearLinkValues();
      //reset/clear all node link data/state variables.
      void ClearLinkToValues();

      void EndHeaderElement(const XMLCh* const localname);
      void EndGraphSection(const XMLCh* const localname);
      void EndNodeSection(const XMLCh* const localname);
      void EndNodesElement();
      void EndNodeElement();
      void EndLibrarySection(const XMLCh* const localname);
      void EndLibraryElement();

      /**
       * Links all nodes together at the end of the load.
       */
      void LinkNodes();
      void RemoveLinkNodes();
   };
}
#endif
