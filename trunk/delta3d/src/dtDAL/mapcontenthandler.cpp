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

#include <prefix/dtdalprefix-src.h>

#include <dtDAL/mapcontenthandler.h>
#include <dtDAL/map.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/groupactorproperty.h>
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

XERCES_CPP_NAMESPACE_USE;


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
   bool MapContentHandler::IsPropertyCorrectType()
   {
      if (mActorProperty == NULL || mActorPropertyType == NULL || mActorProxy == NULL)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Call made to %s with content handler in incorrect state.", __FUNCTION__);
         return false;
      }
      bool result = mActorProperty->GetDataType() == *mActorPropertyType;
      if (!result)
      {
         if ( (*mActorPropertyType == DataType::VEC2 && (mActorProperty->GetDataType() == DataType::VEC2D || mActorProperty->GetDataType() == DataType::VEC2F)) ||
              (*mActorPropertyType == DataType::VEC3 && (mActorProperty->GetDataType() == DataType::VEC3D || mActorProperty->GetDataType() == DataType::VEC3F)) ||
              (*mActorPropertyType == DataType::VEC4 && (mActorProperty->GetDataType() == DataType::VEC4D || mActorProperty->GetDataType() == DataType::VEC4F)) )
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                "Differentiating between a osg::vecf/osg::vecd and a osg::vec. Correcting.");
            mActorPropertyType = &mActorProperty->GetDataType();
            return true;
         }

         mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                             "Parameter/Property %s of actor %s was saved as type %s, but is now of type %s. Data will be ignored",
                             mActorProperty->GetName().c_str(), mActorProxy->GetName().c_str(),
                             mActorPropertyType->GetName().c_str(), mActorProperty->GetDataType().GetName().c_str());
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::characters(const XMLCh* const chars, const unsigned int length)
   {
      xmlCharString& topEl = mElements.top();
      if (mInMap)
      {
         if (mInHeader)
         {
            if (topEl == MapXMLConstants::MAP_NAME_ELEMENT)
            {
               mMap->SetName(dtUtil::XMLStringConverter(chars).ToString());
               //this flag is only used when the parser is just looking for the map name.
               mFoundMapName = true;
            }
            else if (topEl == MapXMLConstants::WAYPOINT_CREATE_NAVMESH)
            {
               mMap->SetCreateNavMesh(dtUtil::ToType<bool>(dtUtil::XMLStringConverter(chars).ToString()));
            }
            else if (topEl == MapXMLConstants::WAYPOINT_FILENAME_ELEMENT)
            {
               mMap->SetPathNodeFileName(dtUtil::XMLStringConverter(chars).ToString());
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
               //ignored for now
            }
            else if (topEl == MapXMLConstants::SCHEMA_VERSION_ELEMENT)
            {
               //ignored - the schema checks this value
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
      }

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                             "Found characters for element \"%s\" \"%s\"", dtUtil::XMLStringConverter(topEl.c_str()).c_str(), dtUtil::XMLStringConverter(chars).c_str());
      }
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::ActorCharacters(const XMLCh* const chars)
   {
      xmlCharString& topEl = mElements.top();
      if (mInActorProperty)
      {
         if (mActorProxy == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                "Actor proxy is NULL, but code has entered the actor property section");

         }
         if (mInGroupProperty)
         {
            ParameterCharacters(chars);
         }
         else
         {
            if (topEl == MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT)
            {
               std::string propName = dtUtil::XMLStringConverter(chars).ToString();
               mActorProperty = mActorProxy->GetProperty(propName);
               if (mActorProperty == NULL)
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                      "In actor property section, actor property for name \"%s\" was not found on actor proxy \"%s\".",
                                      propName.c_str(), mActorProxy->GetName().c_str());

            }
            else if (mActorProperty != NULL)
            {
               if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT)
               {
                  std::string resourceTypeString = dtUtil::XMLStringConverter(chars).ToString();
                  mActorPropertyType = static_cast<DataType*>(DataType::GetValueForName(resourceTypeString));

                  if (mActorPropertyType == NULL)
                     mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                         "No resource type found for type specified in mMap xml \"%s.\"",
                                         resourceTypeString.c_str());
               }
               else if (mActorPropertyType != NULL)
               {
                  std::string dataValue = dtUtil::XMLStringConverter(chars).ToString();

                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                         "Setting value of property %s, property type %s, datatype %s, value %s, element name %s.",
                                         mActorProperty->GetName().c_str(),
                                         mActorProperty->GetDataType().GetName().c_str(),
                                         mActorPropertyType->GetName().c_str(),
                                         dataValue.c_str(), dtUtil::XMLStringConverter(topEl.c_str()).c_str());

                  //we now have the property, the type, and the data.
                  ParsePropertyData(dataValue);
               }
            }
         }
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
         else
            mActorProxy->SetId(dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString()));
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
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                                      "mActorProxy could not be created for ActorType \"%s\" not found.",
                                      actorTypeFullName.c_str());
            }
         }
         else
         {
            mIgnoreCurrentActor = true;
         }
      }

   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::CreateAndPushParameter()
   {
      try
      {
         NamedGroupParameter& gp = dynamic_cast<NamedGroupParameter&>(*mParameterStack.top());
         mParameterStack.push(gp.AddParameter(mParameterNameToCreate, *mParameterTypeToCreate));
         mParameterTypeToCreate = NULL;
         mParameterNameToCreate.clear();
      }
      catch (const std::bad_cast&)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Tried to add a new parameter \"%s\" with type \"%s\", but failed",
                             mParameterNameToCreate.c_str(), mParameterTypeToCreate->GetName().c_str());
      }
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::ParameterCharacters(const XMLCh* const chars)
   {
      xmlCharString& topEl = mElements.top();
      if (topEl == MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT)
      {
         mParameterNameToCreate = dtUtil::XMLStringConverter(chars).ToString();
         if (mParameterNameToCreate.empty())
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                "In named parameter section, found a named parameter with an empty name.");

      }
      //Resource datatypes are held in a string value inside the resource element.
      //This means creating the parameter has to be here, when the type element is found.
      else if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT)
      {
         std::string resourceTypeString = dtUtil::XMLStringConverter(chars).ToString();
         mParameterTypeToCreate = static_cast<DataType*>(DataType::GetValueForName(resourceTypeString));

         if (mParameterTypeToCreate == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                "No resource type found for type specified in mMap xml \"%s.\"",
                                resourceTypeString.c_str());
         }
         else
         {
            //if this fails, the parameter just won't be added.
            CreateAndPushParameter();
         }
      }
      //The make sure we are parsing a parameter type other than group.  If not, then no character data
      //makes any sense to handle.
      else if (!mParameterStack.empty() && mParameterStack.top()->GetDataType() != DataType::GROUP)
      {
         std::string dataValue = dtUtil::XMLStringConverter(chars).ToString();

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Setting value of parameter %s, parameter datatype %s, value %s, element name %s.",
                                mParameterStack.top()->GetName().c_str(),
                                mParameterStack.top()->GetDataType().GetName().c_str(),
                                dataValue.c_str(), dtUtil::XMLStringConverter(topEl.c_str()).c_str());

         //we now have the property, the type, and the data.
         ParseParameterData(dataValue);
      }
   }

   /////////////////////////////////////////////////////////////////
   template <typename VecType>
   void MapContentHandler::ParseVec(const std::string& dataValue, VecType& vec, size_t vecSize)
   {
      xmlCharString& topEl = mElements.top();

      char* endMarker;
      double value = strtod(dataValue.c_str(), &endMarker);

      if (topEl == MapXMLConstants::ACTOR_VEC_1_ELEMENT || topEl == MapXMLConstants::ACTOR_COLOR_R_ELEMENT)
      {
         vec[0] = value;
      }
      else if (topEl == MapXMLConstants::ACTOR_VEC_2_ELEMENT || topEl == MapXMLConstants::ACTOR_COLOR_G_ELEMENT)
      {
         vec[1] = value;
      }
      else if (vecSize >= 3 && (topEl == MapXMLConstants::ACTOR_VEC_3_ELEMENT || topEl == MapXMLConstants::ACTOR_COLOR_B_ELEMENT))
      {
         vec[2] = value;
      }
      else if (vecSize == 4 && (topEl == MapXMLConstants::ACTOR_VEC_4_ELEMENT || topEl == MapXMLConstants::ACTOR_COLOR_A_ELEMENT))
      {
         vec[3] = value;
      }
      else
      {
         if (topEl == MapXMLConstants::ACTOR_PROPERTY_VEC2_ELEMENT ||
             topEl == MapXMLConstants::ACTOR_PROPERTY_VEC3_ELEMENT ||
             topEl == MapXMLConstants::ACTOR_PROPERTY_VEC4_ELEMENT ||
             topEl == MapXMLConstants::ACTOR_PROPERTY_COLOR_RGBA_ELEMENT ||
             topEl == MapXMLConstants::ACTOR_PROPERTY_COLOR_RGBA_ELEMENT)
            return;

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Got an invalid element for a Vec%u: %s", unsigned(vecSize), dtUtil::XMLStringConverter(topEl.c_str()).c_str());
      }
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::ParsePropertyData(std::string& dataValue)
   {
      if (!IsPropertyCorrectType())
         return;

      xmlCharString& topEl = mElements.top();

      switch (mActorPropertyType->GetTypeId())
      {
         case DataType::FLOAT_ID:
         case DataType::DOUBLE_ID:
         case DataType::INT_ID:
         case DataType::LONGINT_ID:
         case DataType::STRING_ID:
         case DataType::BOOLEAN_ID:
         case DataType::ENUMERATION_ID:
         {
            if (!mActorProperty->FromString(dataValue))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                   "Failed Setting value %s for property type %s named %s on actor named %s",
                                   dataValue.c_str(), mActorPropertyType->GetName().c_str(),
                                   mActorProperty->GetName().c_str(), mActorProxy->GetName().c_str());
            }
            mActorPropertyType = NULL;
            break;
         }
         case DataType::GAMEEVENT_ID:
         {
            GameEventActorProperty& geProp = static_cast<GameEventActorProperty&>(*mActorProperty);
            if (!dtUtil::trim(dataValue).empty())
            {
               GameEvent *e = mMap->GetEventManager().FindEvent(dtCore::UniqueId(dataValue));
               if (e != NULL)
               {
                  geProp.SetValue(e);
               }
               else
               {
                  geProp.SetValue(NULL);
                  mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                      "Game Event referenced in actor property %s on proxy of type \"%s\" was not found.",
                                      mActorProperty->GetName().c_str(), mActorProxy->GetActorType().GetFullName().c_str());
               }
            }
            else
            {
               geProp.SetValue(NULL);
            }
            mActorPropertyType = NULL;
            break;
         }
         case DataType::VEC2_ID:
         {
            Vec2ActorProperty& p = static_cast<Vec2ActorProperty&>(*mActorProperty);
            osg::Vec2 vec = p.GetValue();
            ParseVec(dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC2F_ID:
         {
            Vec2fActorProperty& p = static_cast<Vec2fActorProperty&>(*mActorProperty);
            osg::Vec2f vec = p.GetValue();
            ParseVec(dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC2D_ID:
         {
            Vec2dActorProperty& p = static_cast<Vec2dActorProperty&>(*mActorProperty);
            osg::Vec2d vec = p.GetValue();
            ParseVec(dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC3_ID:
         {
            Vec3ActorProperty& p = static_cast<Vec3ActorProperty&>(*mActorProperty);
            osg::Vec3 vec = p.GetValue();
            ParseVec(dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC3F_ID:
         {
            Vec3fActorProperty& p = static_cast<Vec3fActorProperty&>(*mActorProperty);
            osg::Vec3f vec = p.GetValue();
            ParseVec(dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC3D_ID:
         {
            Vec3dActorProperty& p = static_cast<Vec3dActorProperty&>(*mActorProperty);
            osg::Vec3d vec = p.GetValue();
            ParseVec(dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC4_ID:
         {
            Vec4ActorProperty& p = static_cast<Vec4ActorProperty&>(*mActorProperty);
            osg::Vec4 vec = p.GetValue();
            ParseVec(dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC4F_ID:
         {
            Vec4fActorProperty& p = static_cast<Vec4fActorProperty&>(*mActorProperty);
            osg::Vec4f vec = p.GetValue();
            ParseVec(dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC4D_ID:
         {
            Vec4dActorProperty& p = static_cast<Vec4dActorProperty&>(*mActorProperty);
            osg::Vec4d vec = p.GetValue();
            ParseVec(dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
         case DataType::RGBACOLOR_ID:
         {
            ColorRgbaActorProperty& p = static_cast<ColorRgbaActorProperty&>(*mActorProperty);
            osg::Vec4 vec = p.GetValue();
            ParseVec(dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
         case DataType::ACTOR_ID:
         {
            //insert the data into this map to make it accessible to assign once the parsing is done.
            dtUtil::trim(dataValue);
            if (!dataValue.empty() && dataValue != "NULL")
            {
               mActorLinking.insert(std::make_pair(mActorProxy->GetId(), std::make_pair(mActorProperty->GetName(), dtCore::UniqueId(dataValue))));

            }
            mActorPropertyType = NULL;
            break;
         }
         case DataType::GROUP_ID:
         {
            ///Nothing Useful happens here.
            break;
         }
         default:
         {
            if (mActorPropertyType->IsResource())
            {
               ResourceActorProperty& p = static_cast<ResourceActorProperty&>(*mActorProperty);
               if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT)
               {
                  if (dataValue != p.GetDataType().GetName())
                  {
                     mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                         "Save file expected resource property %s on actor named %s to have type %s, but it is %s.",
                                         mActorProperty->GetName().c_str(), mActorProxy->GetName().c_str(),
                                         dataValue.c_str(), p.GetDataType().GetName().c_str());
                     //make it ignore the rest of the mElements.
                     p.SetValue(NULL);
                     mActorPropertyType = NULL;
                  }
               }
               else if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT)
               {
                  mDescriptorDisplayName = dataValue;
               }
               else if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT)
               {
                  //if the value is null, then both strings will be empty.
                  if (dataValue != "" && mDescriptorDisplayName != "")
                  {
                     ResourceDescriptor rd(mDescriptorDisplayName, dataValue);
                     p.SetValue(&rd);
                  }
                  else
                  {
                     p.SetValue(NULL);
                  }
               }
            }
            else
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                  "DataType \"%s\" is not supported in the map loading code.  The data has been ignored.",
                  mActorPropertyType->GetName().c_str());
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::ParseParameterData(std::string& dataValue)
   {
      if (mParameterStack.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                             "Unable to set the value \"%s\" without a valid parameter for group actor property \"%s\" "
                             "on actor \"%s\" with type \"%s\".",
                             dataValue.c_str(), mActorProperty->GetName().c_str(), mActorProxy->GetName().c_str(),
                             mActorProxy->GetActorType().GetFullName().c_str());
         return;
      }

      xmlCharString& topEl = mElements.top();

      ///Optimization
      if (topEl == MapXMLConstants::ACTOR_PROPERTY_PARAMETER_ELEMENT)
         return;

      NamedParameter& np = *mParameterStack.top();

      switch (np.GetDataType().GetTypeId())
      {
         case DataType::FLOAT_ID:
         case DataType::DOUBLE_ID:
         case DataType::INT_ID:
         case DataType::LONGINT_ID:
         case DataType::STRING_ID:
         case DataType::BOOLEAN_ID:
         case DataType::ENUMERATION_ID:
         {
            if (!np.FromString(dataValue))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                   "Failed Setting value \"%s\" for parameter type \"%s\" named \"%s\" on actor named \"%s\" in property \"%s\".",
                                   dataValue.c_str(), mActorPropertyType->GetName().c_str(),
                                   mActorProperty->GetName().c_str(), mActorProxy->GetName().c_str());
            }
            break;
         }
         case DataType::GAMEEVENT_ID:
         {
            NamedGameEventParameter& geParam = static_cast<NamedGameEventParameter&>(np);
            if (!dtUtil::trim(dataValue).empty())
            {
               GameEvent *e = mMap->GetEventManager().FindEvent(dtCore::UniqueId(dataValue));
               if (e != NULL)
               {
                  geParam.SetValue(dtCore::UniqueId(dataValue));
               }
               else
               {
                  geParam.SetValue(dtCore::UniqueId(""));
                  mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                      "Game Event referenced in named parameter \"%s\" in property \"%s\" "
                                      "proxy of type \"%s\" was not found.",
                                      np.GetName().c_str(), mActorProperty->GetName().c_str(),
                                      mActorProxy->GetActorType().GetFullName().c_str());
               }
            }
            else
            {
               geParam.SetValue(dtCore::UniqueId(""));
            }
            break;
         }
         case DataType::VEC2_ID:
         {
            NamedVec2Parameter& p = static_cast<NamedVec2Parameter&>(np);
            osg::Vec2 vec = p.GetValue();
            ParseVec(dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC2F_ID:
         {
            NamedVec2fParameter& p = static_cast<NamedVec2fParameter&>(np);
            osg::Vec2f vec = p.GetValue();
            ParseVec(dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC2D_ID:
         {
            NamedVec2dParameter& p = static_cast<NamedVec2dParameter&>(np);
            osg::Vec2d vec = p.GetValue();
            ParseVec(dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC3_ID:
         {
            NamedVec3Parameter& p = static_cast<NamedVec3Parameter&>(np);
            osg::Vec3 vec = p.GetValue();
            ParseVec(dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC3F_ID:
         {
            NamedVec3fParameter& p = static_cast<NamedVec3fParameter&>(np);
            osg::Vec3f vec = p.GetValue();
            ParseVec(dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC3D_ID:
         {
            NamedVec3dParameter& p = static_cast<NamedVec3dParameter&>(np);
            osg::Vec3d vec = p.GetValue();
            ParseVec(dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC4_ID:
         {
            NamedVec4Parameter& p = static_cast<NamedVec4Parameter&>(np);
            osg::Vec4 vec = p.GetValue();
            ParseVec(dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC4F_ID:
         {
            NamedVec4fParameter& p = static_cast<NamedVec4fParameter&>(np);
            osg::Vec4f vec = p.GetValue();
            ParseVec(dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
         case DataType::VEC4D_ID:
         {
            NamedVec4dParameter& p = static_cast<NamedVec4dParameter&>(np);
            osg::Vec4d vec = p.GetValue();
            ParseVec(dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
         case DataType::RGBACOLOR_ID:
         {
            NamedRGBAColorParameter& p = static_cast<NamedRGBAColorParameter&>(np);
            osg::Vec4 vec = p.GetValue();
            ParseVec(dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
         case DataType::ACTOR_ID:
         {
            //insert the data into this map to make it accessible to assign once the parsing is done.
            dtUtil::trim(dataValue);
            if (!dataValue.empty() && dataValue != "NULL")
            {
               static_cast<NamedActorParameter&>(np).SetValue(dtCore::UniqueId(dataValue));
            }
            break;
         }
         case DataType::GROUP_ID:
         {
            break;
         }
         default:
         {
            if (np.GetDataType().IsResource())
            {
               NamedResourceParameter& p = static_cast<NamedResourceParameter&>(np);

               if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT)
               {
                  mDescriptorDisplayName = dataValue;
               }
               else if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT)
               {
                  //if the value is null, then both strings will be empty.
                  if (dataValue != "" && mDescriptorDisplayName != "")
                  {
                     ResourceDescriptor rd(mDescriptorDisplayName, dataValue);
                     p.SetValue(&rd);
                  }
                  else
                  {
                     p.SetValue(NULL);
                  }
               }
            }
            else
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                  "DataType \"%s\" is not supported in the map loading code.  Ignoring parameter \"%s\".",
                  np.GetDataType().GetName().c_str(), np.GetName().c_str());

            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::startDocument()
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
            "Parsing Map Document Started.");

      Reset();
      mMap = new Map("","");
   }

   /////////////////////////////////////////////////////////////////
   DataType* MapContentHandler::ParsePropertyType(const XMLCh* const localname, bool errorIfNotFound)
   {
      if (XMLString::compareString(localname,
          MapXMLConstants::ACTOR_PROPERTY_BOOLEAN_ELEMENT) == 0)
      {
         return &DataType::BOOLEAN;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_FLOAT_ELEMENT) == 0)
      {
         return &DataType::FLOAT;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_DOUBLE_ELEMENT) == 0)
      {
         return &DataType::DOUBLE;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_INTEGER_ELEMENT) == 0)
      {
         return &DataType::INT;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_LONG_ELEMENT) == 0)
      {
         return &DataType::LONGINT;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_VEC2_ELEMENT) == 0)
      {
         return &DataType::VEC2;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_VEC3_ELEMENT) == 0)
      {
         return &DataType::VEC3;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_VEC4_ELEMENT) == 0)
      {
         return &DataType::VEC4;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_STRING_ELEMENT) == 0)
      {
         return &DataType::STRING;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_COLOR_RGBA_ELEMENT) == 0)
      {
         return &DataType::RGBACOLOR;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_COLOR_RGB_ELEMENT) == 0)
      {
         return &DataType::RGBCOLOR;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_ENUM_ELEMENT) == 0)
      {
         return &DataType::ENUMERATION;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_ACTOR_ID_ELEMENT) == 0)
      {
         return &DataType::ACTOR;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_GAMEEVENT_ELEMENT) == 0)
      {
         return &DataType::GAME_EVENT;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_GROUP_ELEMENT) == 0)
      {
         return &DataType::GROUP;
      }
      else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT) == 0)
      {
         //Need the character contents to know the type, so this will be
         //handled later.
      }
      else if (errorIfNotFound)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                             "Found property data element with name %s, but this does not map to a known type.\n",
                             dtUtil::XMLStringConverter(localname).c_str());
      }
      return NULL;
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::NonEmptyDefaultWorkaround()
   {
      // We don't have control over what the string or actor
      // property default value is, but if the data in the
      // xml is empty string, no event is generated.  Thus,
      // this preemptively set this string to "" so that
      // empty data will work.
      if (mActorPropertyType != NULL && !mActorProperty->IsReadOnly())
      {
         if (*mActorPropertyType == DataType::STRING
                || *mActorPropertyType == DataType::ACTOR)
         {
            mActorProperty->FromString("");
         }
         else if (*mActorPropertyType == DataType::GAME_EVENT)
         {
            static_cast<GameEventActorProperty*>(mActorProperty.get())->SetValue(NULL);
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::startElement (
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname,
      const xercesc::Attributes& attrs)
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "Found element %s", dtUtil::XMLStringConverter(localname).c_str());
      }

      if (mInMap)
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
               if (mInActorProperty)
               {
                  if (mActorProperty != NULL)
                  {
                     if (mActorPropertyType == NULL)
                     {
                        mActorPropertyType = ParsePropertyType(localname);
                     }

                     if (mInGroupProperty)
                     {
                        if (!mParameterNameToCreate.empty())
                        {
                           mParameterTypeToCreate = ParsePropertyType(localname);
                           //It will be null if it's a resource property.  The value will be parsed later.
                           if (mParameterTypeToCreate != NULL)
                              CreateAndPushParameter();
                        }
                     }
                     else if (XMLString::compareString(localname,
                                                       MapXMLConstants::ACTOR_PROPERTY_GROUP_ELEMENT) == 0)
                     {
                         mInGroupProperty = true;
                         ClearParameterValues();
                         mParameterStack.push(new NamedGroupParameter(mActorProperty->GetName()));

                     }
                  }
               }
               else if (XMLString::compareString(localname,
                                                 MapXMLConstants::ACTOR_PROPERTY_ELEMENT) == 0)
               {
                  mInActorProperty = true;
               }
            }
            else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_ELEMENT) == 0)
            {
               mInActor = true;
               ClearActorValues();
            }
         }
         else if (mInEvents)
         {
            if (XMLString::compareString(localname, MapXMLConstants::EVENT_ELEMENT) == 0)
            {
               mGameEvent = new GameEvent();
            }
         }
         else if (!mInHeader && !mInEvents && !mInLibraries && !mInActors)
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
         }
      }
      else if (XMLString::compareString(localname, MapXMLConstants::MAP_ELEMENT) == 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Map");
         mInMap = true;
      }
      mElements.push(xmlCharString(localname));
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::endElement( const XMLCh* const uri,
                                       const XMLCh* const localname,
                                       const XMLCh* const qname) 
   {
      if (mElements.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Attempting to pop elements off of stack and the stack is empty."
                             "it should at least contain element %s.",
                             dtUtil::XMLStringConverter(localname).c_str());
         return;
      }

      const XMLCh* lname = mElements.top().c_str();

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                             "Ending element: \"%s\"", dtUtil::XMLStringConverter(lname).c_str());
      }

      if (XMLString::compareString(lname, localname) != 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Attempting to pop mElements off of stack and the element "
                             "at the top (%s) is not the same as the element ending (%s).",
                             dtUtil::XMLStringConverter(lname).c_str(), dtUtil::XMLStringConverter(localname).c_str());
      }

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
      mElements.pop();
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::endDocument()
   {
      LinkActors();
      AssignGroupProperties();

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
               throw dtUtil::Exception(ExceptionEnum::InvalidActorException,
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

      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                          "Parsing Map Document Ended.\n");
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::error(const xercesc::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                          "ERROR %d:%d - %s:%s - %s", exc.getLineNumber(),
                          exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getMessage()).c_str());
      throw exc;
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::fatalError(const xercesc::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                          "FATAL-ERROR %d:%d - %s:%s - %s", exc.getLineNumber(),
                          exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getMessage()).c_str());
      throw exc;
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::warning(const xercesc::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                          "WARNING %d:%d - %s:%s - %s", exc.getLineNumber(),
                          exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getMessage()).c_str());
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::resetDocument()
   {
      Reset();
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::resetErrors()
   {
      mErrorCount = 0;
      mFatalErrorCount = 0;
      mWarningCount = 0;
      mMissingLibraries.clear();
      mMissingActorTypes.clear();
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::Reset()
   {
      mMap = NULL;
      mInMap = false;
      mInHeader = false;
      mInLibraries = false;
      mInEvents = false;
      mInActors = false;
      mInActorProperty = false;
      mInActor = false;

      mEnvActorId = "";

      while (!mElements.empty()) mElements.pop();
      mMissingActorTypes.clear();

      ClearLibraryValues();
      ClearActorValues();
      //This should NOT be done in the Actor Value because this should
      //be cleared at the start and finish of a document, not between each actor.
      mActorLinking.clear();

      mGroupParameters.clear();

      resetErrors();

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
      mCurrentPropName.clear();
      mActorProxy = NULL;
      mActorPropertyType = NULL;
      mActorProperty = NULL;
      mIgnoreCurrentActor = false;
      mDescriptorDisplayName.clear();

      mInGroupProperty = false;
      while (!mParameterStack.empty())
      {
         mParameterStack.pop();
      }

      ClearParameterValues();
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::ClearParameterValues()
   {
      mParameterNameToCreate.clear();
      mParameterTypeToCreate = NULL;
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::AssignGroupProperties()
   {
      for (std::multimap<dtCore::UniqueId, std::pair<std::string, dtCore::RefPtr<dtDAL::NamedGroupParameter> > >::iterator i
           = mGroupParameters.begin();
           i != mGroupParameters.end(); ++i)
      {
         dtCore::UniqueId id = i->first;
         ActorProxy* proxyToModify = mMap->GetProxyById(id);
         if (proxyToModify == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have a group property set, but the proxy does not exist in the new map.",
                                id.ToString().c_str());
            continue;
         }
         std::pair<std::string, dtCore::RefPtr<dtDAL::NamedGroupParameter> >& data = i->second;
         std::string& propertyName = data.first;
         dtCore::RefPtr<dtDAL::NamedGroupParameter>& propValue = data.second;

         ActorProperty* property = proxyToModify->GetProperty(propertyName);
         if (property == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have group property %s set with actor %s, but the property does not exist on the proxy.",
                                id.ToString().c_str(), propertyName.c_str(), propValue->ToString().c_str());
            continue;
         }

         GroupActorProperty* gap = dynamic_cast<GroupActorProperty*>(property);
         if (gap == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have actor property %s set with actor %s, but the property is not a GroupActorProperty.",
                                id.ToString().c_str(), propertyName.c_str(), propValue->ToString().c_str());
            continue;
         }
         gap->SetValue(*propValue);
      }
   }

   /////////////////////////////////////////////////////////////////
   void MapContentHandler::LinkActors()
   {
      for (std::multimap<dtCore::UniqueId, std::pair<std::string, dtCore::UniqueId> >::iterator i = mActorLinking.begin();
           i != mActorLinking.end(); ++i)
      {
         dtCore::UniqueId id = i->first;
         ActorProxy* proxyToModify = mMap->GetProxyById(id);
         if (proxyToModify == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have an actor property set, but the proxy does not exist in the new map.",
                                id.ToString().c_str());
            continue;
         }
         std::pair<std::string, dtCore::UniqueId>& data = i->second;
         std::string& propertyName = data.first;
         dtCore::UniqueId& propValueId = data.second;
         if (propValueId.ToString().empty())
         {
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have actor property %s set, but the id is empty.",
                                id.ToString().c_str(), propertyName.c_str(), propValueId.ToString().c_str());

         }
         ActorProxy* valueProxy = mMap->GetProxyById(propValueId);
         if (valueProxy == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have actor property %s set with actor %s, but the proxy does not exist in the new map.",
                                id.ToString().c_str(), propertyName.c_str(), propValueId.ToString().c_str());
            continue;
         }
         ActorProperty* property = proxyToModify->GetProperty(propertyName);
         if (property == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have actor property %s set with actor %s, but the property does not exist on the proxy.",
                                id.ToString().c_str(), propertyName.c_str(), propValueId.ToString().c_str());
            continue;
         }
         ActorActorProperty* aap = dynamic_cast<ActorActorProperty*>(property);
         if (aap == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__,
                                "Proxy with ID %s was defined to have actor property %s set with actor %s, but the property is not an ActorActorProperty.",
                                id.ToString().c_str(), propertyName.c_str(), propValueId.ToString().c_str());
            continue;
         }
         aap->SetValue(valueProxy);
      }

   }


   /////////////////////////////////////////////////////////////////
   MapContentHandler::MapContentHandler() : mActorProxy(NULL), mActorPropertyType(NULL), mActorProperty(NULL)
   {
      mLogger = &dtUtil::Log::GetInstance();
      //mLogger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Creating Map Content Handler.\n");

      mEnvActorId = "";
   }

   /////////////////////////////////////////////////////////////////
   MapContentHandler::~MapContentHandler() {}

   MapContentHandler::MapContentHandler(const MapContentHandler&) {}
   MapContentHandler& MapContentHandler::operator=(const MapContentHandler&) { return *this;}

   void MapContentHandler::startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri) {}

   void MapContentHandler::endPrefixMapping(const XMLCh* const prefix) {}

   void MapContentHandler::skippedEntity(const XMLCh* const name) {}

   void MapContentHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}

   void MapContentHandler::processingInstruction(const XMLCh* const target, const XMLCh* const data) {}


   void MapContentHandler::setDocumentLocator(const xercesc::Locator* const locator) {}

   InputSource* MapContentHandler::resolveEntity(const XMLCh* const publicId, const XMLCh* const systemId)
   {
      return NULL;
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
         mMap->AddProxy(*mActorProxy);
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
      if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_ELEMENT) == 0)
      {
         EndActorPropertyElement();
      }
      else if (mInGroupProperty)
      {
         if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_PARAMETER_ELEMENT) == 0)
         {
            EndActorPropertyParameterElement();
         }
         else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_GROUP_ELEMENT) == 0)
         {
            EndActorPropertyGroupElement();
         }
         else if (!mParameterStack.empty())
         {
            // parse the end element into a data type to see if it's an end param element.
            dtDAL::DataType* d = ParsePropertyType(localname, false);
            // The parameter has ended, so pop.
            if (d != NULL)
            {
               mParameterStack.pop();
            }
         }
      }
      else if (mInActorProperty)
      {
         if (mActorProperty != NULL)
         {
            if (mActorPropertyType != NULL)
            {
               // parse the end element into a data type to see if it's an end property element.
               dtDAL::DataType* d = ParsePropertyType(localname, false);
               // The property has ended, so in case the property type has not
               // been unset, it is now.
               if (d != NULL)
               {
                  //This works here because the actor types referenced here all set
                  // their property type to NULL when the value is set.
                  NonEmptyDefaultWorkaround();
                  mActorPropertyType = NULL;
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndActorPropertyGroupElement()
   {
      dtCore::RefPtr<NamedGroupParameter> topParam = static_cast<NamedGroupParameter*>(mParameterStack.top().get());
      mParameterStack.pop();
      //The only way we know we have completed a group actor property is that the
      //stack of parameters has been emptied since they can nest infinitely.
      if (mParameterStack.empty())
      {
         mInGroupProperty = false;
         mGroupParameters.insert(std::make_pair(mActorProxy->GetId(),
            std::make_pair(mActorProperty->GetName(), topParam)));
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndActorPropertyParameterElement()
   {
      ///We pop if the element was never filled in.  This happens if the value is empty for
      ///parameter data.  We don't pop for a group because that is handled separately below.
      if (!mParameterStack.empty() && mParameterStack.top()->GetDataType() != DataType::GROUP)
      {
         mParameterStack.pop();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MapContentHandler::EndActorPropertyElement()
   {
      mInActorProperty = false;
      mActorProperty = NULL;
      mActorPropertyType = NULL;
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
         mMap->AddLibrary(mLibName, mLibVersion);
         ClearLibraryValues();
      }
      catch (const dtUtil::Exception& e)
      {
         mMissingLibraries.push_back(mLibName);
         if (dtDAL::ExceptionEnum::ProjectResourceError == e.TypeEnum())
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Error loading library %s version %s in the library manager.  Exception message to follow.",
               mLibName.c_str(), mLibVersion.c_str());
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unknown exception loading library %s version %s in the library manager.  Exception message to follow.",
               mLibName.c_str(), mLibVersion.c_str());
         }
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
