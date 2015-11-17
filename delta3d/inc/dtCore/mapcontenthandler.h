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

#include <dtCore/basexmlhandler.h>
#include <dtCore/map.h>
#include <dtCore/actorpropertyserializer.h>

#include <dtCore/uniqueid.h>

namespace dtCore
{
   class BaseActorObject;
   class ActorProperty;
   class AbstractParameter;
   class GameEvent;
   class DataType;
   class Map;
   class NamedParameter;
   class NamedGroupParameter;
   class ArrayActorPropertyBase;
   class ContainerActorProperty;
   class DataType;

   /**
    * @class MapContentHandler
    * @brief The SAX2 content handler for loading maps.
    */
   class DT_CORE_EXPORT MapContentHandler: public BaseXMLHandler
   {
      public:

         ///Constructor
         MapContentHandler();

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
          * @see BaseXMLHandler::CombinedCharacters
          */
         virtual void CombinedCharacters(const XMLCh* const chars, size_t length);

         /**
          * Retrieves a list of missing libraries.
          */
         virtual const std::vector<std::string>& GetMissingLibraries() { return mMissingLibraries; }

         /**
          * Retrieves a list of missing actor types.
          */
         virtual const std::set<std::string>& GetMissingActorTypes() { return mMissingActorTypes; }

         /**
          * @return true if the map parsing has come across the map name yet.
          */
         bool HasParsedHeader() const { return mFinishedHeader; };

         /**
         * Initializes the content handler to load a map.
         */
         void SetMapMode() { mLoadingPrefab = false; }

         /**
         * Initializes the content handler to load a prefab.
         */
         void SetPrefabMode();

         /**
          * note: store a RefPtr to this map immediately because reparsing with this handler
          * would otherwise cause it to be deleted.
          * @return a handle to the map parsed, or NULL if no map has been created.
          */
         Map* GetMap();
         const Map* GetMap() const;

         /**
          * Returns whether or not the map had a temporary property in it.
          */
         bool HasDeprecatedProperty() const;

         /**
          * This causes the parser to release its reference to the map.
          */
         void ClearMap();

      protected: // This class is referenced counted, but this causes an error...

         virtual ~MapContentHandler();

      private:
         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         MapContentHandler(const MapContentHandler&);
         MapContentHandler& operator=(const MapContentHandler&);

         /**
          * Convenience method to find an ancestor actor that matches the 
          * specified id by traversing up the previously processed actor.
          */
         BaseActorObject* FindActorById(const dtCore::UniqueId& id) const;
         /**
          * Wrapper function to encapsulate deprecation functionality.
          */
         static ActorTypePtr FindActorType(const std::string& actorTypeCategory, const std::string& actorTypeName);

         dtCore::RefPtr<Map> mMap;

         bool mInMap;
         bool mInPrefab;
         bool mInHeader;
         bool mInLibraries;
         bool mInEvents;
         bool mInActors;
         bool mInActor;
         bool mInGroup;
         bool mInPresetCameras;
         bool mIgnoreCurrentActor;
         int mActorDepth;

         std::string mLibName;
         std::string mLibVersion;

         dtCore::UniqueId mEnvActorId;
         dtCore::UniqueId mParentId;

         std::vector<std::string> mMissingLibraries;
         std::set<std::string> mMissingActorTypes;

         ActorPropertySerializer* mPropSerializer;

         dtCore::RefPtr<GameEvent> mGameEvent;

         typedef dtCore::RefPtr<BaseActorObject> ActorPtr;
         typedef std::stack<ActorPtr> ActorStack;
         ActorPtr mPrevActorObject;
         ActorPtr mBaseActorObject;
         ActorStack mActorStack;

         int mIgnoreActorDepth;

         int mGroupIndex;

         bool mFinishedHeader;

         bool mLoadingPrefab;

         int                   mPresetCameraIndex;
         Map::PresetCameraData mPresetCameraData;
         int                   mPresetCameraView;

         ActorHierarchyNode*                       mCurrentHierNode;

         //Reset all of the internal data/state variables
         void Reset();
         //reset/clear all of the library data/state variables
         void ClearLibraryValues();
         //reset/clear all of the actor data/state variables
         void ClearActorValues();
         //Called from characters when the state says we are inside an actor element.
         void ActorCharacters(const XMLCh* const chars);
         //Called from characters when the state says we are inside a preset camera element.
         void PresetCameraCharacters(const XMLCh* const chars);

         void EndHeaderElement(const XMLCh* const localname);
         void EndActorSection(const XMLCh* const localname);
         void EndActorsElement();
         void EndActorElement();
         void EndActorPropertySection(const XMLCh* const localname);
         void EndGroupSection(const XMLCh* const localname);
         void EndGroupElement();
         void EndPresetCameraSection(const XMLCh* const localname);
         void EndPresetCameraElement();
         void EndLibrarySection(const XMLCh* const localname);
         void EndLibraryElement();
         void EndEventSection(const XMLCh* const localname);
   };
}
#endif
