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

#include <prefix/dtcoreprefix.h>

#include <dtCore/deltadrawable.h>

#include <dtCore/actorcomponentcontainer.h>
#include <dtCore/actorhierarchynode.h>
#include <dtCore/actorproperty.h>
#include <dtCore/actorproxy.h>
#include <dtCore/arrayactorpropertybase.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/environmentactor.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/groupactorproperty.h>
#include <dtCore/actorfactory.h>
#include <dtCore/mapcontenthandler.h>
#include <dtCore/map.h>
#include <dtCore/mapxmlconstants.h>
#include <dtCore/namedparameter.h>
#include <dtUtil/tree.h>
#include <dtUtil/xercesutils.h>


#include <dtUtil/deprecationmgr.h>
DT_DISABLE_WARNING_ALL_START

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

DT_DISABLE_WARNING_END

#include <osg/Vec2f>
#include <osg/Vec2d>
#include <osg/Vec3f>
#include <osg/Vec3d>

#include <sstream>
#include <typeinfo>

XERCES_CPP_NAMESPACE_USE

namespace  dtCore
{
   /////////////////////////////////////////////////////////////////
   MapContentHandler::MapContentHandler()
      : BaseXMLHandler()
      , mInMap(false)
      , mInPrefab(false)
      , mInHeader(false)
      , mInLibraries(false)
      , mInEvents(false)
      , mInActors(false)
      , mInActor(false)
      , mInGroup(false)
      , mInPresetCameras(false)
      , mIgnoreCurrentActor(false)
      , mActorDepth(-1)
      , mEnvActorId(false)
      , mParentId(false)
      , mPropSerializer(nullptr)
      , mBaseActorObject(NULL)
      , mIgnoreActorDepth(-1)
      , mGroupIndex(-1)
      , mFinishedHeader()
      , mLoadingPrefab()
      , mPresetCameraIndex()
      , mPresetCameraView()
      , mCurrentHierNode(NULL)
   {
      mPropSerializer = new ActorPropertySerializer(this);
   }

   /////////////////////////////////////////////////////////////////
   MapContentHandler::~MapContentHandler()
   {
      delete mPropSerializer;
      mPropSerializer = NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   MapContentHandler::MapContentHandler(const MapContentHandler&)
   : mInMap(false)
   , mInPrefab(false)
   , mInHeader(false)
   , mInLibraries(false)
   , mInEvents(false)
   , mInActors(false)
   , mInActor(false)
   , mInGroup(false)
   , mInPresetCameras(false)
   , mIgnoreCurrentActor(false)
   , mActorDepth(-1)
   , mEnvActorId(false)
   , mParentId(false)
   , mPropSerializer(nullptr)
   , mBaseActorObject(NULL)
   , mIgnoreActorDepth(-1)
   , mGroupIndex(-1)
   , mFinishedHeader()
   , mLoadingPrefab()
   , mPresetCameraIndex()
   , mPresetCameraView()
   , mCurrentHierNode(NULL)
   {
      mPropSerializer = new ActorPropertySerializer(this);
   }

   //////////////////////////////////////////////////////////////////////////
   MapContentHandler& MapContentHandler::operator=(const MapContentHandler&) { return *this;}

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::startDocument()
   {
      BaseXMLHandler::startDocument();

      mMap = new Map("","");
      mPropSerializer->SetMap(mMap.get());
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
            dtCore::RefPtr<BaseActorObject> proxy = mMap->GetProxyById(mEnvActorId);
            if (!proxy.valid())
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "No environment actor was located in the map.");
               }
               return;
            }
            else
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "An environment actor was located in the map.");
               }
            }

            IEnvironmentActor *ea = dynamic_cast<IEnvironmentActor*>(proxy->GetDrawable());
            if (ea == NULL)
            {
               throw dtCore::InvalidActorException(
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
            bool isActorElement = XMLString::compareString(localname, MapXMLConstants::ACTOR_ELEMENT) == 0;

            if (mInActor)
            {
               if (!mIgnoreCurrentActor)
               {
                  // Determine if this a non-child actor element, such as a property element.
                  if ( ! isActorElement)
                  {
                     mPropSerializer->ElementStarted(localname);
                  }
               }
            }
            // Starting fresh with a root level actor...
            else if (isActorElement)
            {
               mInActor = true;
               ClearActorValues();
            }

            if (isActorElement)
            {
               ++mActorDepth;
            }
         }
         else if (mInGroup)
         {
            if (XMLString::compareString(localname, MapXMLConstants::ACTOR_GROUP_ELEMENT) == 0)
            {
               mGroupIndex = mMap->GetGroupCount();
            }
         }
         else if (XMLString::compareString(localname, MapXMLConstants::HIERARCHY_ELEMENT) == 0)
         {
            mCurrentHierNode = mMap->GetDrawableActorHierarchy();
         }
         else if (XMLString::compareString(localname, MapXMLConstants::HIERARCHY_ELEMENT_NODE) == 0)
         {
            dtCore::UniqueId id(dtUtil::XMLStringConverter(attrs.getValue(XMLString::transcode("actorID"))).ToString());
                           
            dtCore::ActorHierarchyNode* nextNode = 
               new ActorHierarchyNode(mMap->GetProxyById(id));

            mCurrentHierNode->AddChild(nextNode);
            mCurrentHierNode = nextNode;
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
      else if (XMLString::compareString(localname, MapXMLConstants::HIERARCHY_ELEMENT_NODE) == 0)
      {
         mCurrentHierNode = mCurrentHierNode->GetParent();
      }
      else if (mInPresetCameras)
      {
         EndPresetCameraSection(localname);
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
   void MapContentHandler::CombinedCharacters(const XMLCh* const chars, size_t length)
   {
      BaseXMLHandler::CombinedCharacters(chars, length);

      xmlCharString& topEl = mElements.top();
      if (mInMap || mInPrefab)
      {
         if (mInHeader)
         {
            if (topEl == MapXMLConstants::DESCRIPTION_ELEMENT)
            {
               mMap->SetDescription(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::CREATE_TIMESTAMP_ELEMENT)
            {
               mMap->SetCreateDateTime(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::LAST_UPDATE_TIMESTAMP_ELEMENT)
            {
               //ignored for now
            }
            else if (!mLoadingPrefab)
            {
               if (topEl == MapXMLConstants::NAME_ELEMENT)
               {
                  mMap->SetName(dtUtil::XMLStringConverter(chars).ToString());
               }
               else if (topEl == MapXMLConstants::DESCRIPTION_ELEMENT)
               {
                  mMap->SetDescription(dtUtil::XMLStringConverter(chars).ToString());
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
               else if (topEl == MapXMLConstants::EDITOR_VERSION_ELEMENT)
               {
                  //ignored for now
               }
               else if (topEl == MapXMLConstants::SCHEMA_VERSION_ELEMENT)
               {
                  //ignored - the schema checks this value
               }
            }
            else if (topEl == MapXMLConstants::ICON_ELEMENT)
            {
               mMap->SetIconFile(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == MapXMLConstants::PREFAB_ACTOR_TYPE_ELEMENT)
            {
               std::string actorTypeFullName = dtUtil::XMLStringConverter(chars).ToString();
               std::pair<std::string, std::string> typeCatPair = ActorType::ParseNameAndCategory(actorTypeFullName);

               MapContentHandler::FindActorType(typeCatPair.second, typeCatPair.first);
               //mMap->SetPrefabActorType();
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

               dtCore::BaseActorObject* proxy = NULL;
               dtCore::UniqueId id = dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString());
               mMap->GetProxyById(id, proxy);
               if (proxy)
               {
                  mMap->AddActorToGroup(mGroupIndex, *proxy);
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
      if (mPropSerializer->Characters(topEl, chars))
      {
      }
      else if (topEl == MapXMLConstants::ACTOR_NAME_ELEMENT)
      {
         if (mBaseActorObject == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                "Encountered the actor name element with value \"%s\", but actor is NULL.",
                                dtUtil::XMLStringConverter(chars).c_str());
         }
         else
         {
            mBaseActorObject->SetName(dtUtil::XMLStringConverter(chars).ToString());
         }
      }
      else if (topEl == MapXMLConstants::ACTOR_ID_ELEMENT)
      {
         if (mBaseActorObject == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                "Encountered the actor id element with value \"%s\", but actor is NULL.",
                                dtUtil::XMLStringConverter(chars).c_str());
         }
         else if (!mLoadingPrefab)
         {
            mBaseActorObject->SetId(dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString()));
         }
      }
      else if (topEl == MapXMLConstants::ACTOR_PARENT_ID_ELEMENT)
      {
         mParentId = dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString());
      }
      else if (topEl == MapXMLConstants::ACTOR_TYPE_ELEMENT)
      {
         std::string actorTypeFullName = dtUtil::XMLStringConverter(chars).ToString();
         std::pair<std::string, std::string> typeCatPair = ActorType::ParseNameAndCategory(actorTypeFullName);

         std::string& actorTypeCategory = typeCatPair.second;
         std::string& actorTypeName = typeCatPair.first;


         // Make sure we have not tried to load this actor type already and failed.
         if (mMissingActorTypes.find(actorTypeFullName) == mMissingActorTypes.end())
         {
            ActorTypePtr actorType = FindActorType(actorTypeCategory, actorTypeName);

            dtCore::ActorComponentContainer* compContainer
               = dynamic_cast<dtCore::ActorComponentContainer*>(mBaseActorObject.get());

            if (compContainer && actorType == nullptr)
            {
               ActorPtrVector existingComponents;
               ActorTypePtr tempType = new dtCore::ActorType(actorTypeName, actorTypeCategory, std::string());
               compContainer->GetComponents(tempType, existingComponents);
               if (!existingComponents.empty())
               {
                  actorType = &existingComponents[0]->GetActorType();
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                                      "ActorComponent actorType \"%s\" was not found in the registry, but it was found as an existing component."
                                      "Please register this type with an actor plugin registry or register the approprate registry to avoid this problem.",
                                      actorTypeFullName.c_str());
               }
            }

            if (actorType == NULL)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                                   "ActorType \"%s\" not found.", actorTypeFullName.c_str());
               mMissingActorTypes.insert(actorTypeFullName);
               mIgnoreCurrentActor = true;

               // Set the level in the actor-to-actor hierarchy to ignore.
               if (mIgnoreActorDepth < 0)
               {
                  mIgnoreActorDepth = mActorDepth;
               }
            }
            else
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "Creating actor proxy %s with category %s.",
                                   actorTypeName.c_str(), actorTypeCategory.c_str());

               if (compContainer != NULL)
               {
                  ActorPtrVector existingComponents;
                  compContainer->GetComponents(actorType, existingComponents);
                  if (!existingComponents.empty())
                  {
                     // TODO This is actually pretty bad because it means you can't load multiple of the same
                     // type.
                     mBaseActorObject = existingComponents[0];
                     // Actor components created in code won't have their defaults initialized unless the developer
                     // created it through the factory.
                     mBaseActorObject->InitDefaults();
                  }
                  else
                  {
                     mBaseActorObject = NULL;
                  }
               }
               else
               {
                  mBaseActorObject = NULL;
               }

               bool newActorComponent = mBaseActorObject == NULL;
               if (mBaseActorObject == NULL)
               {
                  mBaseActorObject = ActorFactory::GetInstance().CreateActor(*actorType).get();
               }

               if (mBaseActorObject == NULL)
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                     "mActorProxy could not be created for ActorType \"%s\" not found.",
                     actorTypeFullName.c_str());
                  mMissingActorTypes.insert(actorTypeFullName);
                  mIgnoreCurrentActor = true;

                  // Set the level in the actor-to-actor hierarchy to ignore.
                  if (mIgnoreActorDepth < 0)
                  {
                     mIgnoreActorDepth = mActorDepth;
                  }
               }
               else
               {

                  mActorStack.push(mBaseActorObject);
                  mPropSerializer->PushPropertyContainer(*mBaseActorObject);

                  // Notify the actor that it is being loaded.
                  mBaseActorObject->OnMapLoadBegin();

                  // If a previous actor was set, use it as a parent actor.
                  if (compContainer != NULL && newActorComponent)
                  {
                     compContainer->AddComponent(*mBaseActorObject);
                  }

               }
            }
         }
         else
         {
            mIgnoreCurrentActor = true;
            
            // Set the level in the actor-to-actor hierarchy to ignore.
            if (mIgnoreActorDepth < 0)
            {
               mIgnoreActorDepth = mActorDepth;
            }
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
   void MapContentHandler::SetPrefabMode()
   {
      mLoadingPrefab = true;
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
      mActorDepth = -1;
      mIgnoreActorDepth = -1;

      mPresetCameraIndex = -1;
      mPresetCameraData.isValid = false;
      mPresetCameraView = 0;

      mPropSerializer->Reset();
      mEnvActorId = "";

      ClearLibraryValues();
      ClearActorValues();
      mMissingLibraries.clear();
      mMissingActorTypes.clear();

      mFinishedHeader = false;
      mGameEvent = NULL;
      mPrevActorObject = NULL;
      mBaseActorObject = NULL;
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
      mBaseActorObject = NULL;
      mPropSerializer->SetCurrentPropertyContainer(NULL);
      mIgnoreCurrentActor = false;
      mIgnoreActorDepth = -1;
      mActorDepth = -1;
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndHeaderElement(const XMLCh* const localname)
   {
      if (XMLString::compareString(localname, MapXMLConstants::HEADER_ELEMENT) == 0)
      {
         mInHeader = false;
         mFinishedHeader = true;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndActorElement()
   {
      if (mBaseActorObject != nullptr && !mIgnoreCurrentActor)
      {
         if (! mBaseActorObject->IsActorComponent())
         {
            // Determine if the actor has a parent.
            BaseActorObject* parent = NULL;
            if ( ! mParentId.IsNull())
            {
               // Due to the order that the map is written and read,
               // parent actors should load before any children and
               // thus should be accessible via the previously
               // processed actor object.
               parent = FindActorById(mParentId);

               // Reset the id now so that subsequent read actor definitions are not affected.
               mParentId = dtCore::UniqueId("");
            }

            ActorComponentContainer* extendedActor = dynamic_cast<ActorComponentContainer*>(mBaseActorObject.get());

            // Determine if the current actor can link to a found parent.
            if (parent != NULL && extendedActor != NULL)
            {
               extendedActor->SetParentBaseActor(parent);
            }
            
            mMap->AddProxy(*mBaseActorObject);

            // Keep track of the previous actor.
            mPrevActorObject = mBaseActorObject.get();
         }
         mBaseActorObject->OnMapLoadEnd(); //notify BaseActorObject we're done loading it
      }

      // Determine if the current actor was being ignored and that
      // the end element for the actor is for the proper level in
      // the actor-to-actor hierarchy.
      bool wasIgnored = mIgnoreCurrentActor;
      if (mIgnoreCurrentActor)
      {
         if (mActorDepth == 0 || mIgnoreActorDepth == mActorDepth)
         {
            mIgnoreCurrentActor = false;
            mIgnoreActorDepth = -1;
         }
      }
      else // Actor was valid and needs to be popped off the stack.
      {
         mBaseActorObject = NULL;

         // Set the current actor pointer to the previous parent actor.
         if ( ! mActorStack.empty())
         {
            mActorStack.pop();
            if ( ! mActorStack.empty())
            {
               mBaseActorObject = mActorStack.top();
            }
         }
         else
         {
            LOG_WARNING("Actor stack was already empty.");
         }
      }

      --mActorDepth;

      mInActor = mActorDepth >= 0;

      if ( ! wasIgnored)
      {
         mPropSerializer->PopPropertyContainer();
      }
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
      else if (mInActor && !mIgnoreCurrentActor)
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
      mPropSerializer->ElementEnded(localname);
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
         if (ActorFactory::GetInstance().GetRegistry(mLibName) == NULL)
         {
            ActorFactory::GetInstance().LoadActorRegistry(mLibName);
         }
         if (mMap.valid()) mMap->AddLibrary(mLibName, mLibVersion);
         ClearLibraryValues();
      }
      catch (const dtCore::ProjectResourceErrorException& e)
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

   //////////////////////////////////////////////////////////////////////////
   bool MapContentHandler::HasDeprecatedProperty() const
   {
      return mPropSerializer->HasDeprecatedProperty();
   }

   //////////////////////////////////////////////////////////////////////////
   ActorTypePtr MapContentHandler::FindActorType(const std::string& actorTypeCategory, const std::string& actorTypeName)
   {
      ActorTypePtr actorType =
         ActorFactory::GetInstance().FindActorType(actorTypeCategory, actorTypeName);

      if (actorType == nullptr)
      {
         if (!ActorFactory::GetInstance().IsInRegistry(ActorFactory::DEFAULT_ACTOR_LIBRARY) && actorTypeCategory.find("dt") == 0)
         {
            try
            {
               //Load old default registries
               ActorFactory::GetInstance().LoadActorRegistry(ActorFactory::DEFAULT_ACTOR_LIBRARY);
               ActorFactory::GetInstance().LoadOptionalActorRegistry("dtAnim");
               ActorFactory::GetInstance().LoadOptionalActorRegistry("dtAudio");
               //try again
               actorType = ActorFactory::GetInstance().FindActorType(actorTypeCategory, actorTypeName);
               if (actorType != nullptr)
               {
                  LOGN_WARNING("actorfactory.cpp", "ActorType \"" + actorType->GetFullName() +
                        "\" was not found in any open libraries, but it was found in the " + ActorFactory::DEFAULT_ACTOR_LIBRARY +
                        " or another old, default loaded one. Auto-loaded libraries are no longer supported. If you resave the map, it will correct the library list.");

               }
            }
            catch (const dtUtil::Exception&)
            {
               // if dtActors isn't available, the application just may not be using it, so ignore.
            }
         }
      }

      if (actorType == nullptr)
      {
         actorType = ActorFactory::GetInstance().FindActorTypeReplacement(actorTypeCategory, actorTypeName);
      }
      return actorType;
   }

   //////////////////////////////////////////////////////////////////////////
   BaseActorObject* MapContentHandler::FindActorById(const dtCore::UniqueId& id) const
   {
      BaseActorObject* foundActor = NULL;

      if (mPrevActorObject.valid())
      {
         BaseActorObject* actor = mPrevActorObject.get();

         while (actor != NULL)
         {
            if (actor->GetId() == id)
            {
               foundActor = actor;
               break;
            }

            // Attempt access of the next parent.
            dtCore::ActorComponentContainer* compContainer
               = dynamic_cast<dtCore::ActorComponentContainer*>(actor);
            if (compContainer != NULL)
            {
               actor = compContainer->GetParentBaseActor();
            }
            else // Cannot get to the next parent so exit loop.
            {
               //LOG_ERROR("Could not access the parent for actor \"" + actor->GetName()
               //   + "\" (id " + actor->GetId().ToString() + ")");
               break;
            }
         }
      }

      return foundActor;
   }

}
