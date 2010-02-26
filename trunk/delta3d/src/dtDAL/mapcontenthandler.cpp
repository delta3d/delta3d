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

#include <prefix/dtdalprefix.h>

#include <typeinfo>

#include <dtDAL/mapcontenthandler.h>
#include <dtDAL/map.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/arrayactorpropertybase.h>
#include <dtDAL/containeractorproperty.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/datatype.h>
#include <dtDAL/mapxmlconstants.h>
#include <dtDAL/environmentactor.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventmanager.h>
#include <dtDAL/namedparameter.h>
#include <dtDAL/mapxmlconstants.h>

#include <dtUtil/xercesutils.h>

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable : 4267) // for warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned int', possible loss of data
#endif

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

#include <osg/Vec2f>
#include <osg/Vec2d>
#include <osg/Vec3f>
#include <osg/Vec3d>

XERCES_CPP_NAMESPACE_USE


// Default iimplementation of char_traits<XMLCh>, needed for gcc3.3
#if (__GNUC__ == 3 && __GNUC_MINOR__ <= 3)
/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace std
{
   template<>
   struct char_traits<unsigned short>
   {
      typedef unsigned short char_type;

      static void
         assign(char_type& __c1, const char_type& __c2)
      { __c1 = __c2; }

      static int
         compare(const char_type* __s1, const char_type* __s2, size_t __n)
      {
         for (;__n > 0; ++__s1, ++__s2, --__n) {
            if (*__s1 < *__s2) return -1;
            if (*__s1 > *__s2) return +1;
         }
         return 0;
      }

      static size_t
         length(const char_type* __s)
      { size_t __n = 0; while (*__s++) ++__n; return __n; }

      static char_type*
         copy(char_type* __s1, const char_type* __s2, size_t __n)
      {  return static_cast<char_type*>(memcpy(__s1, __s2, __n * sizeof(char_type))); }

   };
}
/// @endcond
#endif

namespace  dtDAL
{
   /////////////////////////////////////////////////////////////////
   MapContentHandler::MapContentHandler()
      : BaseXMLHandler()
      , mActorProxy(NULL)
      , mGroupIndex(-1)
      , mLoadingPrefab(false)
      , mPrefabReadMode(PREFAB_READ_ALL)
      , mPrefabIconFileName("")
      , mPrefabProxyList(NULL)
   {
      mPropSerializer = new ActorPropertySerializer(this);

      mEnvActorId = "";
   }

   /////////////////////////////////////////////////////////////////
   MapContentHandler::~MapContentHandler()
   {
      delete mPropSerializer;
   }

   //////////////////////////////////////////////////////////////////////////
   MapContentHandler::MapContentHandler(const MapContentHandler&) {}

   //////////////////////////////////////////////////////////////////////////
   MapContentHandler& MapContentHandler::operator=(const MapContentHandler&) { return *this;}

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::startDocument()
   {
      BaseXMLHandler::startDocument();

      if (!mLoadingPrefab)
      {
         mMap = new Map("","");
         mPropSerializer->SetMap(mMap.get());
      }
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::endDocument()
   {
      BaseXMLHandler::endDocument();

      mPropSerializer->LinkActors();
      mPropSerializer->AssignGroupProperties();

      if (!mEnvActorId.ToString().empty())
      {
         try
         {
            dtCore::RefPtr<ActorProxy> proxy = mMap->GetProxyById(mEnvActorId);
            if (!proxy.valid())
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "No environment actor was located in the map.");
               return;
            }
            else
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "An environment actor was located in the map.");
            }

            IEnvironmentActor *ea = dynamic_cast<IEnvironmentActor*>(proxy->GetActor());
            if (ea == NULL)
            {
               throw dtDAL::InvalidActorException(
                  "The environment actor proxy's actor should be an environment, but a dynamic_cast failed", __FILE__, __LINE__);
            }
            mMap->SetEnvironmentActor(proxy.get());
         }
         catch(dtUtil::Exception &e)
         {
            LOG_ERROR("Exception caught: " + e.What());
            throw e;
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::ElementStarted(
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname,
      const xercesc::Attributes& attrs)
   {
      BaseXMLHandler::ElementStarted(uri, localname, qname, attrs);

      if (mInMap || mInPrefab)
      {
         if (mInLibraries)
         {
            if (XMLString::compareString(localname, MapXMLConstants::LIBRARY_ELEMENT) == 0)
            {
               ClearLibraryValues();
            }
         }
         else if (mInActors)
         {
            if (mInActor)
            {
               mPropSerializer->ElementStarted(localname);
            }
            else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_ELEMENT) == 0)
            {
               mInActor = true;
               ClearActorValues();
            }
         }
         else if (mInGroup)
         {
            if (XMLString::compareString(localname, MapXMLConstants::ACTOR_GROUP_ELEMENT) == 0)
            {
               mGroupIndex = mMap->GetGroupCount();
            }
         }
         else if (mInPresetCameras)
         {
            if (XMLString::compareString(localname, MapXMLConstants::PRESET_CAMERA_ELEMENT) == 0)
            {
               mPresetCameraIndex = -1;
               mPresetCameraData.isValid = false;
            }
         }
         else if (mInEvents)
         {
            if (XMLString::compareString(localname, MapXMLConstants::EVENT_ELEMENT) == 0)
            {
               mGameEvent = new GameEvent();
            }
         }
         else if (!mInHeader && !mInEvents && !mInLibraries && !mInActors && !mInGroup)
         {
            if (XMLString::compareString(localname, MapXMLConstants::HEADER_ELEMENT) == 0)
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Header");
               mInHeader = true;
            }
            if (XMLString::compareString(localname, MapXMLConstants::EVENTS_ELEMENT) == 0)
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Events");
               mInEvents = true;
            }
            else if (XMLString::compareString(localname, MapXMLConstants::LIBRARIES_ELEMENT) == 0)
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Libraries");
               mInLibraries = true;
            }
            else if (XMLString::compareString(localname, MapXMLConstants::ACTORS_ELEMENT) == 0)
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Actors");
               mInActors = true;
            }
            else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_GROUPS_ELEMENT) == 0)
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Found Groups");
               mInGroup = true;
            }
            else if (XMLString::compareString(localname, MapXMLConstants::PRESET_CAMERAS_ELEMENT) == 0)
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Found Preset Cameras");
               mInPresetCameras = true;
            }
         }
      }
      else if (XMLString::compareString(localname, MapXMLConstants::MAP_ELEMENT) == 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Map");
         mInMap = true;
      }
      else if (XMLString::compareString(localname, MapXMLConstants::PREFAB_ELEMENT) == 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Prefab");
         mInPrefab = true;
      }
      else if (XMLString::compareString(localname, MapXMLConstants::ICON_ELEMENT) == 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Icon");               
      }
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::ElementEnded( const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname)
   {
      BaseXMLHandler::ElementEnded(uri, localname, qname);

      if (mInHeader)
      {
         EndHeaderElement(localname);
      }
      else if (mInEvents)
      {
         EndEventSection(localname);
      }
      else if (mInLibraries)
      {
         EndLibrarySection(localname);
      }
      else if (mInActors)
      {
         EndActorSection(localname);
      }
      else if (mInGroup)
      {
         EndGroupSection(localname);
      }
      else if (mInPresetCameras)
      {
         EndPresetCameraSection(localname);
      }

      if(mLoadingPrefab && mPrefabReadMode == PREFAB_ICON_ONLY &&
         (XMLString::compareString(localname, MapXMLConstants::ICON_ELEMENT) == 0))
      {
         //Got the icon file name -- time to stop parsing
         throw(dtUtil::Exception("Icon found", __FILE__, __LINE__));
      }
   }

   /////////////////////////////////////////////////////////////////
   Map* MapContentHandler::GetMap()
   {
      return mMap.get();
   }

   /////////////////////////////////////////////////////////////////
   const Map* MapContentHandler::GetMap() const
   {
      return mMap.get();
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::ClearMap()
   {
      mMap = NULL;
   }

   /////////////////////////////////////////////////////////////////
#if XERCES_VERSION_MAJOR < 3
   void MapContentHandler::characters(const XMLCh* const chars, const unsigned int length)
#else
   void MapContentHandler::characters(const XMLCh* const chars, const XMLSize_t length)
#endif
   {
      BaseXMLHandler::characters(chars, length);

      xmlCharString& topEl = mElements.top();
      if (mInMap || mInPrefab)
      {
         if (mInHeader)
         {
            if (topEl == MapXMLConstants::NAME_ELEMENT)
            {
               mMap->SetName(dtUtil::XMLStringConverter(chars).ToString());
               //this flag is only used when the parser is just looking for the map name.
               mFoundMapName = true;
            }            
            else if (topEl == MapXMLConstants::DESCRIPTION_ELEMENT)
            {
               // if we are loading a prefab, this description does not matter.
               if (!mLoadingPrefab)
               {
                  mMap->SetDescription(dtUtil::XMLStringConverter(chars).ToString());
               }
            }
            else if (topEl == MapXMLConstants::AUTHOR_ELEMENT)
            {
               mMap->SetAuthor(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::COMMENT_ELEMENT)
            {
               mMap->SetComment(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::COPYRIGHT_ELEMENT)
            {
               mMap->SetCopyright(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::CREATE_TIMESTAMP_ELEMENT)
            {
               mMap->SetCreateDateTime(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::LAST_UPDATE_TIMESTAMP_ELEMENT)
            {
               //ignored for now
            }
            else if (topEl == MapXMLConstants::EDITOR_VERSION_ELEMENT)
            {
               if (!mLoadingPrefab)
               {
                  //ignored for now
               }
            }
            else if (topEl == MapXMLConstants::SCHEMA_VERSION_ELEMENT)
            {
               if (!mLoadingPrefab)
               {
                  //ignored - the schema checks this value
               }
            }
            else if (topEl == MapXMLConstants::ICON_ELEMENT)
            {
               mPrefabIconFileName = dtUtil::XMLStringConverter(chars).ToString();
            }
         }
         else if (mInEvents)
         {
            if (mGameEvent.valid())
            {
               if (topEl == MapXMLConstants::EVENT_ID_ELEMENT)
               {
                  mGameEvent->SetUniqueId(dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString()));
               }
               else if (topEl == MapXMLConstants::EVENT_NAME_ELEMENT)
               {
                  mGameEvent->SetName(dtUtil::XMLStringConverter(chars).ToString());
               }
               else if (topEl == MapXMLConstants::EVENT_DESCRIPTION_ELEMENT)
               {
                  mGameEvent->SetDescription(dtUtil::XMLStringConverter(chars).ToString());
               }
            }
         }
         else if (mInLibraries)
         {
            if (topEl == MapXMLConstants::LIBRARY_NAME_ELEMENT)
            {
               mLibName = dtUtil::XMLStringConverter(chars).ToString();
            }
            else if (topEl == MapXMLConstants::LIBRARY_VERSION_ELEMENT)
            {
               mLibVersion = dtUtil::XMLStringConverter(chars).ToString();
            }
         }
         else if (mInActors)
         {
            if (mInActor)
            {
               if (!mIgnoreCurrentActor)
               {
                  ActorCharacters(chars);
               }
            }
            else
            {
               if (topEl == MapXMLConstants::ACTOR_ENVIRONMENT_ACTOR_ELEMENT)
                  mEnvActorId = dtUtil::XMLStringConverter(chars).ToString();
            }            
         }
         else if (mInGroup)
         {
            if (topEl == MapXMLConstants::ACTOR_GROUP_ACTOR_ELEMENT)
            {
               if (mGroupIndex == -1)
               {
                  mGroupIndex = 0;
               }

               dtDAL::ActorProxy* proxy = NULL;
               dtCore::UniqueId id = dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString());
               mMap->GetProxyById(id, proxy);
               if (proxy)
               {
                  mMap->AddActorToGroup(mGroupIndex, proxy);
               }
            }
         }
         else if (mInPresetCameras)
         {
            PresetCameraCharacters(chars);
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::ActorCharacters(const XMLCh* const chars)
   {
      xmlCharString& topEl = mElements.top();
      if (mPropSerializer->Characters(topEl, chars, mActorProxy.get()))
      {
      }
      else if (topEl == MapXMLConstants::ACTOR_NAME_ELEMENT)
      {
         if (mActorProxy == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                "Encountered the actor name element with value \"%s\", but actor is NULL.",
                                dtUtil::XMLStringConverter(chars).c_str());
         }
         else
         {
            mActorProxy->SetName(dtUtil::XMLStringConverter(chars).ToString());
         }
      }
      else if (topEl == MapXMLConstants::ACTOR_ID_ELEMENT)
      {
         if (mActorProxy == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                "Encountered the actor id element with value \"%s\", but actor is NULL.",
                                dtUtil::XMLStringConverter(chars).c_str());
         }
         else if (!mLoadingPrefab)
         {
            mActorProxy->SetId(dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString()));
         }
      }
      else if (topEl == MapXMLConstants::ACTOR_TYPE_ELEMENT)
      {
         std::string actorTypeFullName = dtUtil::XMLStringConverter(chars).ToString();
         size_t index = actorTypeFullName.find_last_of('.');

         std::string actorTypeCategory;
         std::string actorTypeName;

         if (index == actorTypeFullName.length())
         {
            actorTypeName = actorTypeFullName;
            actorTypeCategory.clear();
         }
         else
         {
            actorTypeName = actorTypeFullName.substr(index + 1);
            actorTypeCategory = actorTypeFullName.substr(0, index);
         }

         //Make sure we have not tried to load this actor type already and failed.
         if (mMissingActorTypes.find(actorTypeFullName) == mMissingActorTypes.end())
         {
            dtCore::RefPtr<const ActorType> actorType =
               LibraryManager::GetInstance().FindActorType(actorTypeCategory, actorTypeName);

            if (actorType == NULL)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                                   "ActorType \"%s\" not found.", actorTypeFullName.c_str());
               mMissingActorTypes.insert(actorTypeFullName);
               mIgnoreCurrentActor = true;
            }
            else
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "Creating actor proxy %s with category %s.",
                                   actorTypeName.c_str(), actorTypeCategory.c_str());

               mActorProxy = LibraryManager::GetInstance().CreateActorProxy(*actorType).get();
               if (mActorProxy == NULL)
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                     "mActorProxy could not be created for ActorType \"%s\" not found.",
                     actorTypeFullName.c_str());
               }

               // Notify the proxy that it is being loaded.
               mActorProxy->OnMapLoadBegin();

               // When loading a prefab, all actors are put into a group.
               if (mLoadingPrefab)
               {
                  //if (mGroupIndex == -1)
                  //{
                  //   mGroupIndex = mMap->GetGroupCount();
                  //}

                  //mMap->AddActorToGroup(mGroupIndex, mActorProxy.get());

                  if (mPrefabProxyList)
                  {
                     mPrefabProxyList->push_back(mActorProxy);
                  }
               }
            }
         }
         else
         {
            mIgnoreCurrentActor = true;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MapContentHandler::PresetCameraCharacters(const XMLCh* const chars)
   {
      xmlCharString& topEl = mElements.top();

      if (mInPresetCameras)
      {
         if (topEl == MapXMLConstants::PRESET_CAMERA_INDEX_ELEMENT)
         {
            std::istringstream stream;
            stream.str(dtUtil::XMLStringConverter(chars).ToString());
            stream >> mPresetCameraIndex;

            mPresetCameraData.isValid = true;
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_PERSPECTIVE_VIEW_ELEMENT)
         {
            mPresetCameraView = 0;
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_TOP_VIEW_ELEMENT)
         {
            mPresetCameraView = 1;
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_SIDE_VIEW_ELEMENT)
         {
            mPresetCameraView = 2;
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_FRONT_VIEW_ELEMENT)
         {
            mPresetCameraView = 3;
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_POSITION_X_ELEMENT)
         {
            char* endMarker;
            double value = strtod(dtUtil::XMLStringConverter(chars).ToString().c_str(), &endMarker);

            switch (mPresetCameraView)
            {
            case 0: mPresetCameraData.persPosition.x() = value; break;
            case 1: mPresetCameraData.topPosition.x() = value; break;
            case 2: mPresetCameraData.sidePosition.x() = value; break;
            case 3: mPresetCameraData.frontPosition.x() = value; break;
            }
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_POSITION_Y_ELEMENT)
         {
            char* endMarker;
            double value = strtod(dtUtil::XMLStringConverter(chars).ToString().c_str(), &endMarker);

            switch (mPresetCameraView)
            {
            case 0: mPresetCameraData.persPosition.y() = value; break;
            case 1: mPresetCameraData.topPosition.y() = value; break;
            case 2: mPresetCameraData.sidePosition.y() = value; break;
            case 3: mPresetCameraData.frontPosition.y() = value; break;
            }
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_POSITION_Z_ELEMENT)
         {
            char* endMarker;
            double value = strtod(dtUtil::XMLStringConverter(chars).ToString().c_str(), &endMarker);

            switch (mPresetCameraView)
            {
            case 0: mPresetCameraData.persPosition.z() = value; break;
            case 1: mPresetCameraData.topPosition.z() = value; break;
            case 2: mPresetCameraData.sidePosition.z() = value; break;
            case 3: mPresetCameraData.frontPosition.z() = value; break;
            }
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_ROTATION_X_ELEMENT)
         {
            char* endMarker;
            double value = strtod(dtUtil::XMLStringConverter(chars).ToString().c_str(), &endMarker);

            switch (mPresetCameraView)
            {
            case 0: mPresetCameraData.persRotation.x() = value; break;
            }
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_ROTATION_Y_ELEMENT)
         {
            char* endMarker;
            double value = strtod(dtUtil::XMLStringConverter(chars).ToString().c_str(), &endMarker);

            switch (mPresetCameraView)
            {
            case 0: mPresetCameraData.persRotation.y() = value; break;
            }
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_ROTATION_Z_ELEMENT)
         {
            char* endMarker;
            double value = strtod(dtUtil::XMLStringConverter(chars).ToString().c_str(), &endMarker);

            switch (mPresetCameraView)
            {
            case 0: mPresetCameraData.persRotation.z() = value; break;
            }
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_ROTATION_W_ELEMENT)
         {
            char* endMarker;
            double value = strtod(dtUtil::XMLStringConverter(chars).ToString().c_str(), &endMarker);

            switch (mPresetCameraView)
            {
            case 0: mPresetCameraData.persRotation.w() = value; break;
            }
         }
         else if (topEl == MapXMLConstants::PRESET_CAMERA_ZOOM_ELEMENT)
         {
            char* endMarker;
            double value = strtod(dtUtil::XMLStringConverter(chars).ToString().c_str(), &endMarker);

            switch (mPresetCameraView)
            {
            case 1: mPresetCameraData.topZoom = value; break;
            case 2: mPresetCameraData.sideZoom = value; break;
            case 3: mPresetCameraData.frontZoom = value; break;
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MapContentHandler::SetPrefabMode(std::vector<dtCore::RefPtr<dtDAL::ActorProxy> >& proxyList,
      PrefabReadMode readMode, dtDAL::Map* map)
   {
      mLoadingPrefab = true;
      mPrefabProxyList = &proxyList;
      
      mPrefabReadMode = readMode;
      mMap = map;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string MapContentHandler::GetPrefabIconFileName()
   {
      return mPrefabIconFileName;
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::Reset()
   {
      BaseXMLHandler::Reset();

      if (!mLoadingPrefab)
      {
         mMap = NULL;
      }
      mInMap = false;
      mInPrefab = false;
      mInHeader = false;
      mInLibraries = false;
      mInEvents = false;
      mInActors = false;
      mInGroup = false;
      mInPresetCameras = false;
      mInActor = false;
      mPresetCameraIndex = -1;
      mPresetCameraData.isValid = false;
      mPresetCameraView = 0;

      mPropSerializer->Reset();
      mEnvActorId = "";

      ClearLibraryValues();
      ClearActorValues();
      mMissingLibraries.clear();
      mMissingActorTypes.clear();

      mFoundMapName = false;
      mGameEvent = NULL;
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::ClearLibraryValues()
   {
      mLibName.clear();
      mLibVersion.clear();
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::ClearActorValues()
   {
      mActorProxy = NULL;
      mIgnoreCurrentActor = false;

      mPropSerializer->ClearParameterValues();
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndHeaderElement(const XMLCh* const localname)
   {
      if (XMLString::compareString(localname, MapXMLConstants::HEADER_ELEMENT) == 0)
      {
         mInHeader = false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndActorElement()
   {
      if (mActorProxy != NULL)
      {
         if (!mLoadingPrefab)
         {
            mMap->AddProxy(*mActorProxy);
         }
         mActorProxy->OnMapLoadEnd(); //notify ActorProxy we're done loading it
      }
      mActorProxy = NULL;
      mInActor = false;
      ClearActorValues();
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndActorSection(const XMLCh* const localname)
   {
      if (XMLString::compareString(localname, MapXMLConstants::ACTORS_ELEMENT) == 0)
      {
         EndActorsElement();
      }
      else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_ELEMENT) == 0)
      {
         EndActorElement();
      }
      else if (mInActor)
      {
         EndActorPropertySection(localname);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndActorsElement()
   {
      mInActors = false;
      if (mInActor)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Found the closing actors section tag, but the content handler thinks it's still parsing an actor");
         mInActor = false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndActorPropertySection(const XMLCh* const localname)
   {
      mPropSerializer->ElementEnded(localname, mActorProxy.get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndGroupSection(const XMLCh* const localname)
   {
      if (XMLString::compareString(localname, MapXMLConstants::ACTOR_GROUPS_ELEMENT) == 0)
      {
         EndGroupElement();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndGroupElement()
   {
      mInGroup = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndPresetCameraSection(const XMLCh* const localname)
   {
      if (XMLString::compareString(localname, MapXMLConstants::PRESET_CAMERAS_ELEMENT) == 0)
      {
         mInPresetCameras = false;
      }
      else if (XMLString::compareString(localname, MapXMLConstants::PRESET_CAMERA_ELEMENT) == 0)
      {
         EndPresetCameraElement();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndPresetCameraElement()
   {
      if (mPresetCameraIndex > -1 &&
         mPresetCameraData.isValid &&
         mMap.valid())
      {
         mMap->SetPresetCameraData(mPresetCameraIndex, mPresetCameraData);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndLibrarySection(const XMLCh* const localname)
   {
      if (XMLString::compareString(localname, MapXMLConstants::LIBRARIES_ELEMENT) == 0)
      {
         mInLibraries = false;
      }
      else if (XMLString::compareString(localname, MapXMLConstants::LIBRARY_ELEMENT) == 0)
      {
         EndLibraryElement();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndLibraryElement()
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
            "Attempting to add library %s version %s to the library manager.",
            mLibName.c_str(),
            mLibVersion.c_str());
      }

      try
      {
         if (LibraryManager::GetInstance().GetRegistry(mLibName) == NULL)
         {
            LibraryManager::GetInstance().LoadActorRegistry(mLibName);
         }
         if (mMap.valid()) mMap->AddLibrary(mLibName, mLibVersion);
         ClearLibraryValues();
      }
      catch (const dtDAL::ProjectResourceErrorException &e)
      {
         mMissingLibraries.push_back(mLibName);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Error loading library %s version %s in the library manager.  Exception message to follow.",
            mLibName.c_str(), mLibVersion.c_str());

         e.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
      }
      catch (const dtUtil::Exception& e)
      {
         mMissingLibraries.push_back(mLibName);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Unknown exception loading library %s version %s in the library manager.  Exception message to follow.",
            mLibName.c_str(), mLibVersion.c_str());

         e.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndEventSection(const XMLCh* const localname)
   {
      if (XMLString::compareString(localname, MapXMLConstants::EVENTS_ELEMENT) == 0)
      {
         mInEvents = false;
      }
      else if (XMLString::compareString(localname, MapXMLConstants::EVENT_ELEMENT) == 0)
      {
         if (mGameEvent.valid())
         {
            mMap->GetEventManager().AddEvent(*mGameEvent);
            mGameEvent = NULL;
         }
      }
   }
}
