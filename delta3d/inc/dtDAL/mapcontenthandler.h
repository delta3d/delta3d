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

#include <dtDAL/basexmlhandler.h>
#include <dtDAL/map.h>
#include <dtDAL/actorpropertyserializer.h>

#include <dtCore/uniqueid.h>

namespace dtDAL
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
   class DT_DAL_EXPORT MapContentHandler: public BaseXMLHandler
   {
      public:

         enum PrefabReadMode {
            PREFAB_READ_ALL,
            PREFAB_ICON_ONLY            
         };

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
         virtual void ElementStarted( const XMLCh*  const  uri,
                                    const XMLCh*  const  localname,
                                    const XMLCh*  const  qname,
                                    const xercesc::Attributes& attrs );

         /**
          * @see DocumentHandler#endElement
          */
         virtual void ElementEnded(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);

         /**
          * @see DocumentHandler#characters
          */
#if XERCES_VERSION_MAJOR < 3
         virtual void characters(const XMLCh* const chars, const unsigned int length);
         virtual void ignorableWhitespace(const XMLCh* const, const unsigned int) {}
#else
         virtual void characters(const XMLCh* const chars, const XMLSize_t length);
         virtual void ignorableWhitespace(const XMLCh* const, const XMLSize_t) {}
#endif
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
         bool HasFoundMapName() const { return mFoundMapName; };

         /**
         * Initializes the content handler to load a map.
         */
         void SetMapMode() {mLoadingPrefab = false; mPrefabReadMode = PREFAB_READ_ALL;}

         /**
         * Initializes the content handler to load a prefab.
         */
         void SetPrefabMode(std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> >& proxyList,
               PrefabReadMode readMode = PREFAB_READ_ALL, dtDAL::Map* map = NULL);

         /**
         * Get prefab icon file name.  If there isn't one, returns ""
         */
         const std::string GetPrefabIconFileName();

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
         bool HasDeprecatedProperty() const { return mPropSerializer->HasDeprecatedProperty(); }

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

         std::string mLibName;
         std::string mLibVersion;

         dtCore::UniqueId mEnvActorId;

         std::vector<std::string> mMissingLibraries;
         std::set<std::string> mMissingActorTypes;

         ActorPropertySerializer* mPropSerializer;

         dtCore::RefPtr<GameEvent> mGameEvent;

         dtCore::RefPtr<BaseActorObject> mBaseActorObject;

         int mGroupIndex;

         bool mFoundMapName;

         bool mLoadingPrefab;
         PrefabReadMode mPrefabReadMode;
         std::string mPrefabIconFileName;
         std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> >* mPrefabProxyList;

         int                   mPresetCameraIndex;
         Map::PresetCameraData mPresetCameraData;
         int                   mPresetCameraView;

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
