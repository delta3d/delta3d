/*
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
 * Extracted actor property functionality from mapxml by Jeff P. Houde
 */

#include <prefix/dtcoreprefix.h>
#include <dtCore/actorpropertyserializer.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtCore/actoractorproperty.h>
#include <dtCore/actoridactorproperty.h>
#include <dtCore/actorproxy.h>
#include <dtCore/actortype.h>
#include <dtCore/arrayactorpropertybase.h>
#include <dtCore/colorrgbaactorproperty.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/containerselectoractorproperty.h>
#include <dtCore/propertycontaineractorproperty.h>
#include <dtCore/datatype.h>

#include <dtCore/gameevent.h>
#include <dtCore/gameeventactorproperty.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/groupactorproperty.h>
#include <dtCore/mapcontenthandler.h>
#include <dtCore/mapxmlconstants.h>
#include <dtCore/namedactorparameter.h>
#include <dtCore/namedgameeventparameter.h>
#include <dtCore/namedresourceparameter.h>
#include <dtCore/namedrgbacolorparameter.h>
#include <dtCore/namedvectorparameters.h>
#include <dtCore/project.h>
#include <dtCore/propertycontainer.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/transformableactorproxy.h>
#include <dtCore/vectoractorproperties.h>

#include <dtUtil/xercesutils.h>
#include <dtUtil/mathdefines.h>

#include <osgDB/FileNameUtils>
#include <typeinfo> //for std:bad_cast
#include <limits> // for std::numeric_limits

XERCES_CPP_NAMESPACE_USE

#define NEST_ARRAY 0
#define NEST_CONTAINER 1

namespace dtCore
{
   static const std::string logName("actorpropertyserializer.cpp");

   //////////////////////////////////////////////////////////////////////////
   SerializerRuntimeData::SerializerRuntimeData()
   : mActorPropertyType(NULL)
   , mNestedPropertyType(NULL)
   , mParameterTypeToCreate(NULL)
   , mHasDeprecatedProperty(false)
   , mInActorProperty(false)
   , mInGroupProperty(false)
   , mInActorComponents(false)
   {
   }

   SerializerRuntimeData::~SerializerRuntimeData()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   ActorPropertySerializer::ActorPropertySerializer(BaseXMLWriter* writer)
   : mWriter(writer)
   , mParser(NULL)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   ActorPropertySerializer::ActorPropertySerializer(BaseXMLHandler* parser)
   : mWriter(NULL)
   , mParser(parser)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   ActorPropertySerializer::~ActorPropertySerializer()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   SerializerRuntimeData& ActorPropertySerializer::Top()
   {
      if (mData.empty())
      {
         dtCore::SerializerRuntimeData data;
         mData.push(data);
      }

      return  mData.top();
   }


   //////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::SetMap(Map* map)
   {
      mMap = map;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::SetCurrentPropertyContainer(dtCore::PropertyContainer* pc)
   {
      Top().mPropertyContainer = pc;
      Top().ClearParameterValues();
   }

   //////////////////////////////////////////////////////////////////////////
   bool ActorPropertySerializer::HasPropertyContainer()
   {
      return Top().mPropertyContainer.valid();
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::PushPropertyContainer(dtCore::PropertyContainer& pc)
   {
      dtCore::SerializerRuntimeData data;
      data.mPropertyContainer = &pc;
      mData.push(data);
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::PropertyContainer* ActorPropertySerializer::PopPropertyContainer()
   {
      dtCore::PropertyContainer* pc = NULL;

      if ( ! mData.empty())
      {
         pc = mData.top().mPropertyContainer.get();
         mData.pop();
      }

      return pc;
   }

   //////////////////////////////////////////////////////////////////////////
   int ActorPropertySerializer::GetPropertyContainerStackSize() const
   {
      return (int)mData.size();
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::Reset()
   {
      while (!mData.empty())
      {
         mData.pop();
      }
      mGroupParameters.clear();
      //This should NOT be done in the Actor Value because this should
      //be cleared at the start and finish of a document, not between each actor.
      mActorLinking.clear();
      mData.push(dtCore::SerializerRuntimeData());
      mData.top().Reset();
   }

   /////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::WriteProperty(const ActorProperty& property)
   {
      if (mWriter == NULL)
      {
         return;
      }

      // Only save properties that should be saved.
      dtCore::SerializerRuntimeData& data = Top();
      if (!data.mPropertyContainer.valid() || !data.mPropertyContainer->ShouldPropertySave(property))
      {
         return;
      }

      const size_t bufferMax = 512;
      char numberConversionBuffer[bufferMax];
      const DataType& propertyType = property.GetDataType();

      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_ELEMENT);

      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT);
      mWriter->AddCharacters(property.GetName());
      if (mWriter->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mWriter->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Found Property Named: %s", property.GetName().c_str());
      }
      mWriter->EndElement();

      switch (propertyType.GetTypeId())
      {
      case DataType::FLOAT_ID:
      case DataType::DOUBLE_ID:
      case DataType::INT_ID:
      case DataType::LONGINT_ID:
      case DataType::BOOLEAN_ID:
      case DataType::ACTOR_ID:
      case DataType::GAMEEVENT_ID:
      case DataType::ENUMERATION_ID:
      case DataType::BIT_MASK_ID:
         WriteSimple(property);
         break;
      case DataType::STRING_ID:
         WriteString(property.ToString());
         break;
      case DataType::VEC2_ID:
         WriteVec(static_cast<const Vec2ActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
         break;
      case DataType::VEC2F_ID:
         WriteVec(static_cast<const Vec2fActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
         break;
      case DataType::VEC2D_ID:
         WriteVec(static_cast<const Vec2dActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
         break;
      case DataType::VEC3_ID:
         WriteVec(static_cast<const Vec3ActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
         break;
      case DataType::VEC3F_ID:
         WriteVec(static_cast<const Vec3fActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
         break;
      case DataType::VEC3D_ID:
         WriteVec(static_cast<const Vec3dActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
         break;
      case DataType::VEC4_ID:
         WriteVec(static_cast<const Vec4ActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
         break;
      case DataType::VEC4F_ID:
         WriteVec(static_cast<const Vec4fActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
         break;
      case DataType::VEC4D_ID:
         WriteVec(static_cast<const Vec4dActorProperty&>(property).GetValue(), numberConversionBuffer, bufferMax);
         break;
      case DataType::RGBACOLOR_ID:
         WriteColorRGBA(static_cast<const ColorRgbaActorProperty&>(property), numberConversionBuffer, bufferMax);
         break;
      case DataType::GROUP_ID:
         {
            mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_GROUP_ELEMENT);
            dtCore::RefPtr<NamedGroupParameter> gp = static_cast<const GroupActorProperty&>(property).GetValue();
            if (gp.valid())
            {
               std::vector<const NamedParameter*> parameters;
               gp->GetParameters(parameters);
               for (size_t i = 0; i < parameters.size(); ++i)
               {
                  WriteParameter(*parameters[i]);
               }
            }
            mWriter->EndElement();
            break;
         }
      case DataType::ARRAY_ID:
         {
            WriteArray(static_cast<const ArrayActorPropertyBase&>(property), numberConversionBuffer, bufferMax);
            break;
         }
      case DataType::CONTAINER_ID:
         {
            WriteContainer(static_cast<const ContainerActorProperty&>(property), numberConversionBuffer, bufferMax);
            break;
         }
      case DataType::CONTAINER_SELECTOR_ID:
         {
            WriteContainerSelector(static_cast<const ContainerSelectorActorProperty&>(property), numberConversionBuffer, bufferMax);
            break;
         }
      case DataType::PROPERTY_CONTAINER_ID:
         {
            WritePropertyContainer(static_cast<const BasePropertyContainerActorProperty&>(property));
            break;
         }
      default:
         {
            if (propertyType.IsResource())
            {
               const ResourceActorProperty& p =
                  static_cast<const ResourceActorProperty&>(property);

               ResourceDescriptor rd = p.GetValue();

               mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT);
               mWriter->AddCharacters(property.GetDataType().GetName());
               mWriter->EndElement();

               mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT);
               if (!rd.IsEmpty())
                  mWriter->AddCharacters(rd.GetDisplayName());
               mWriter->EndElement();
               mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT);
               if (!rd.IsEmpty())
                  mWriter->AddCharacters(rd.GetResourceIdentifier());
               mWriter->EndElement();
            }
            else
            {
               mWriter->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Unhandled datatype in ActorPropertySerializer: %s.",
                  propertyType.GetName().c_str());
            }
         }
      }

      //end property element
      mWriter->EndElement();
   }

   //////////////////////////////////////////////////////////////////////////
   bool ActorPropertySerializer::ElementStarted(const XMLCh* const localname)
   {
      if (!mParser) return false;

      if (mData.empty())
      {
         mData.push(SerializerRuntimeData());
         mData.top().Reset();
      }

      SerializerRuntimeData& data = Top();

      if (data.mInActorProperty)
      {
         if (data.mActorProperty != NULL)
         {
            if (data.mActorPropertyType == NULL)
            {
               data.mActorPropertyType = ParsePropertyType(localname);
               // If the property value was read from the map, that means we want to resave it.
               // This makes it so that default values can be forced to save, or not forced to save and that decision
               // will be retained.
               data.mActorProperty->SetAlwaysSave(true);
            }

            if (data.mInGroupProperty)
            {
               if (!data.mParameterNameToCreate.empty())
               {
                  data.mParameterTypeToCreate = ParsePropertyType(localname);
                  //It will be null if it's a resource property.  The value will be parsed later.
                  if (data.mParameterTypeToCreate != NULL)
                     CreateAndPushParameter();
               }
            }
            else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_GROUP_ELEMENT) == 0)
            {
               data.ClearParameterValues();
               data.mInGroupProperty = true;
               data.mParameterStack.push(new NamedGroupParameter(data.mActorProperty->GetName()));

            }
            else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_ARRAY_ELEMENT) == 0)
            {
               data.mNestedTypes.push_back(NEST_ARRAY);
            }
            else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT) == 0 ||
               XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT) == 0)
            {
               data.mNestedTypes.push_back(NEST_CONTAINER);
            }
            else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_PROPERTY_CONTAINER_ELEMENT) == 0)
            {
               EnterPropertyContainer();
            }
         }
         // Even if the property is NULL, we still need to recurse because otherwise the context gets broken.
         else if (XMLString::compareString(localname,
            MapXMLConstants::ACTOR_PROPERTY_PROPERTY_CONTAINER_ELEMENT) == 0)
         {
            EnterPropertyContainer();
         }

         return true;
      }
      else if (XMLString::compareString(localname,
         MapXMLConstants::ACTOR_PROPERTY_ELEMENT) == 0)
      {
         data.mInActorProperty = true;
         return true;
      }
      else if (XMLString::compareString(localname,
         MapXMLConstants::ACTOR_COMPONENTS_ELEMENT) == 0)
      {
         data.mInActorComponents = true;
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ActorPropertySerializer::ElementEnded(const XMLCh* const localname)
   {
      if (mData.empty())
      {
         LOG_ERROR(std::string("Data stack is empty, can't end element: ") + dtUtil::XMLStringConverter(localname).ToString());
         return false;
      }

      SerializerRuntimeData& data = Top();

      if (data.mNestedTypes.empty() && XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_ELEMENT) == 0)
      {
         EndActorPropertyElement();
         return true;
      }
      else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_ARRAY_ELEMENT) == 0)
      {
         if (!data.mNestedTypes.empty())
         {
            if (data.mNestedTypes.back() == NEST_ARRAY)
            {
               data.mNestedTypes.pop_back();
            }
            else
            {
               LOG_ERROR("Attempted to end an Array element when the current nested type is not an array.");
            }
         }
         else
         {
            LOG_ERROR("Attempted to end an Array element when there are no nested types.");
         }
         return true;
      }
      else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT) == 0 ||
         XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT) == 0)
      {
         if (!data.mNestedTypes.empty())
         {
            if (data.mNestedTypes.back() == NEST_CONTAINER)
            {
               data.mNestedTypes.pop_back();
            }
            else
            {
               LOG_ERROR("Attempted to end a Container element when the current nested type is not a container.");
            }
         }
         else
         {
            LOG_ERROR("Attempted to end a Container element when there are no nested types.");
         }
         return true;
      }
      else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_PROPERTY_CONTAINER_ELEMENT) == 0)
      {
         if (!mData.empty())
         {
            mData.pop();
         }

         if (mData.empty())
         {
            LOG_ERROR("Data stack is empty when completing a property container element in map \"" + mMap->GetName() + "\". This stack should never be empty.");
         }
      }
      else if (data.mInGroupProperty)
      {
         if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_PARAMETER_ELEMENT) == 0)
         {
            EndActorPropertyParameterElement();
            return true;
         }
         else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_GROUP_ELEMENT) == 0)
         {
            EndActorPropertyGroupElement();
            return true;
         }
         else if (!data.mParameterStack.empty())
         {
            // parse the end element into a data type to see if it's an end param element.
            dtCore::DataType* d = ParsePropertyType(localname, false);
            // The parameter has ended, so pop.
            if (d != NULL)
            {
               data.mParameterStack.pop();
            }
            return true;
         }
      }
      else if (data.mInActorProperty)
      {
         if (data.mActorProperty != NULL && data.mActorPropertyType != NULL && data.mNestedTypes.empty())
         {
            // parse the end element into a data type to see if it's an end property element.
            dtCore::DataType* d = ParsePropertyType(localname, false);
            // The property has ended, so in case the property type has not
            // been unset, it is now.
            if (d != NULL)
            {
               //This works here because the actor types referenced here all set
               // their property type to NULL when the value is set.
               NonEmptyDefaultWorkaround();
               data.mActorPropertyType = NULL;
               return true;
            }
         }
      }
      else if (data.mInActorComponents)
      {
         if (XMLString::compareString(localname, MapXMLConstants::ACTOR_COMPONENTS_ELEMENT) == 0)
         {
            data.mInActorComponents = false;
            return true;
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ActorPropertySerializer::Characters(BaseXMLHandler::xmlCharString& topEl, const XMLCh* const chars)
   {
      if (mData.empty())
      {
         std::string data = dtUtil::XMLStringConverter(chars).ToString();
         dtUtil::Trim(data);
         LOG_ERROR(std::string("Data stack is empty, can't handle characters: \"") + data + "\"");
         return false;
      }

      SerializerRuntimeData& data = Top();

      if (!data.mPropertyContainer.valid())
      {
         return false;
      }

      if (data.mInActorProperty)
      {
         if (data.mInGroupProperty)
         {
            ParameterCharacters(topEl, chars);
         }
         else
         {
            // Make sure we don't try and change the current property if we are loading properties from an array.
            if (data.mNestedTypes.empty() && topEl == MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT)
            {
               std::string propName = dtUtil::XMLStringConverter(chars).ToString();
               data.mActorProperty = data.mPropertyContainer->GetProperty(propName);

               // If the property was not found, attempt to get a temporary one instead.
               if (!data.mActorProperty.valid())
               {
                  data.mActorProperty = data.mPropertyContainer->GetDeprecatedProperty(propName).get();
                  if (data.mActorProperty.valid())
                  {
                     data.mHasDeprecatedProperty = true;
                  }
                  else
                  {
                     mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                        "No property found for \"(%s, %s).\"", data.mPropertyContainer->GetObjectType().GetFullName().c_str(), propName.c_str());
                  }
               }
            }
            else if (data.mActorProperty != NULL)
            {
               // Make sure we don't try and change the current property if we are loading properties from an array.
               if (data.mNestedTypes.empty() && topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT)
               {
                  std::string resourceTypeString = dtUtil::XMLStringConverter(chars).ToString();
                  data.mActorPropertyType = DataType::GetValueForName(resourceTypeString);

                  if (data.mActorPropertyType == NULL)
                  {
                     mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                        "No resource type found for type specified in xml \"%s.\"",
                        resourceTypeString.c_str());
                  }
               }
               else if (GetNestedType() != NULL)
               {
                  std::string dataValue = dtUtil::XMLStringConverter(chars).ToString();

                  ActorProperty* nestedProperty = GetNestedProperty();
                  if (mParser->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  {
                     mParser->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                        "Setting value of property %s, property type %s, datatype %s, value %s, element name %s.",
                        nestedProperty->GetName().c_str(),
                        nestedProperty->GetDataType().GetName().c_str(),
                        GetNestedType()->GetName().c_str(),
                        dataValue.c_str(), dtUtil::XMLStringConverter(topEl.c_str()).c_str());
                  }

                  //we now have the property, the type, and the data.
                  //WARNING:  GetNestedType() must be IN the function call because
                  // the function actually sets the value, which is returned by reference by GetNestedType()
                  // This is an ugly evolution from an older version of the code that used to set the datatype back to null
                  // so that the later code would know it was set and not an empty value.
                  ParsePropertyData(topEl, dataValue, GetNestedType(), nestedProperty);
               }
            }
         }

         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   template <typename VecType>
   void ActorPropertySerializer::WriteVec(const VecType& vec, char* numberConversionBuffer, const size_t bufferMax)
   {
      enum NumComponents
      {
         TWO = 2, THREE, FOUR
      };
      switch (NumComponents(VecType::num_components)) {
      case TWO:
         mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC2_ELEMENT);
         break;
      case THREE:
         mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC3_ELEMENT);
         break;
      case FOUR:
         mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC4_ELEMENT);
         break;
      default:
         //LOG error
         return;
      }

      mWriter->BeginElement(MapXMLConstants::ACTOR_VEC_1_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%f", vec[0]);
      mWriter->AddCharacters(numberConversionBuffer);
      mWriter->EndElement();

      mWriter->BeginElement(MapXMLConstants::ACTOR_VEC_2_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%f", vec[1]);
      mWriter->AddCharacters(numberConversionBuffer);
      mWriter->EndElement();

      if (VecType::num_components > 2)
      {
         mWriter->BeginElement(MapXMLConstants::ACTOR_VEC_3_ELEMENT);
         snprintf(numberConversionBuffer, bufferMax, "%f", vec[2]);
         mWriter->AddCharacters(numberConversionBuffer);
         mWriter->EndElement();
      }

      if (VecType::num_components > 3)
      {
         mWriter->BeginElement(MapXMLConstants::ACTOR_VEC_4_ELEMENT);
         snprintf(numberConversionBuffer, bufferMax, "%f", vec[3]);
         mWriter->AddCharacters(numberConversionBuffer);
         mWriter->EndElement();
      }

      mWriter->EndElement();
   }

   /////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::WriteParameter(const NamedParameter& parameter)
   {
      const size_t bufferMax = 512;
      char numberConversionBuffer[bufferMax];
      const DataType& dataType = parameter.GetDataType();

      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_PARAMETER_ELEMENT);

      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT);
      mWriter->AddCharacters(parameter.GetName());
      if (mWriter->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mWriter->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                             "Found Parameter of GroupActorProperty Named: %s", parameter.GetName().c_str());
      }
      mWriter->EndElement();

      switch (dataType.GetTypeId())
      {
         case DataType::FLOAT_ID:
         case DataType::DOUBLE_ID:
         case DataType::INT_ID:
         case DataType::LONGINT_ID:
         case DataType::BOOLEAN_ID:
         case DataType::ACTOR_ID:
         case DataType::GAMEEVENT_ID:
         case DataType::ENUMERATION_ID:
         case DataType::BIT_MASK_ID:
            WriteSimple(parameter);
            break;
         case DataType::STRING_ID:
            WriteString(parameter.ToString());
            break;
         case DataType::VEC2_ID:
            WriteVec(static_cast<const NamedVec2Parameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC2F_ID:
            WriteVec(static_cast<const NamedVec2fParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC2D_ID:
            WriteVec(static_cast<const NamedVec2dParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC3_ID:
            WriteVec(static_cast<const NamedVec3Parameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC3F_ID:
            WriteVec(static_cast<const NamedVec3fParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC3D_ID:
            WriteVec(static_cast<const NamedVec3dParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC4_ID:
            WriteVec(static_cast<const NamedVec4Parameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC4F_ID:
            WriteVec(static_cast<const NamedVec4fParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::VEC4D_ID:
            WriteVec(static_cast<const NamedVec4dParameter&>(parameter).GetValue(), numberConversionBuffer, bufferMax);
            break;
         case DataType::RGBACOLOR_ID:
            WriteColorRGBA(static_cast<const NamedRGBAColorParameter&>(parameter), numberConversionBuffer, bufferMax);
            break;
         case DataType::GROUP_ID:
         {
            mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_GROUP_ELEMENT);
            std::vector<const NamedParameter*> parameters;
            static_cast<const NamedGroupParameter&>(parameter).GetParameters(parameters);
            for (size_t i = 0; i < parameters.size(); ++i)
            {
               WriteParameter(*parameters[i]);
            }
            mWriter->EndElement();
            break;
         }
         case DataType::ARRAY_ID:
         {
            // mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_ARRAY_ELEMENT);
            // TODO ARRAY: Save an array that was part of a group.
            break;
         }
         case DataType::CONTAINER_ID:
         {
            //mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT);
            // TODO CONTAINER: Save a container that was part of a group.
            break;
         }
         case DataType::CONTAINER_SELECTOR_ID:
         {
            //mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT);
            // TODO CONTAINER SELECTOR: Save a container selector that was part of a group.
            break;
         }
         default:
         {
            if (dataType.IsResource())
            {
               const NamedResourceParameter& p =
                  static_cast<const NamedResourceParameter&>(parameter);

               const ResourceDescriptor rd = p.GetValue();

               mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT);
               mWriter->AddCharacters(parameter.GetDataType().GetName());
               mWriter->EndElement();

               mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT);
               if (!rd.IsEmpty())
                  mWriter->AddCharacters(rd.GetDisplayName());
               mWriter->EndElement();
               mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT);
               if (!rd.IsEmpty())
                  mWriter->AddCharacters(rd.GetResourceIdentifier());
               mWriter->EndElement();
            }
            else
            {
               mWriter->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                    "Unhandled datatype in ActorPropertySerializer: %s.",
                                    dataType.GetName().c_str());
            }
         }
      }
      //End the parameter element.
      mWriter->EndElement();
   }

   /////////////////////////////////////////////////////////////////
   template <typename Type>
   void ActorPropertySerializer::WriteColorRGBA(const Type& holder, char* numberConversionBuffer, size_t bufferMax)
   {
      osg::Vec4f val = holder.GetValue();

      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_COLOR_RGBA_ELEMENT);

      mWriter->BeginElement(MapXMLConstants::ACTOR_COLOR_R_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%f", val[0]);
      mWriter->AddCharacters(numberConversionBuffer);
      mWriter->EndElement();

      mWriter->BeginElement(MapXMLConstants::ACTOR_COLOR_G_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%f", val[1]);
      mWriter->AddCharacters(numberConversionBuffer);
      mWriter->EndElement();

      mWriter->BeginElement(MapXMLConstants::ACTOR_COLOR_B_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%f", val[2]);
      mWriter->AddCharacters(numberConversionBuffer);
      mWriter->EndElement();

      mWriter->BeginElement(MapXMLConstants::ACTOR_COLOR_A_ELEMENT);
      snprintf(numberConversionBuffer, bufferMax, "%f", val[3]);
      mWriter->AddCharacters(numberConversionBuffer);
      mWriter->EndElement();

      mWriter->EndElement();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::WriteArray(const ArrayActorPropertyBase& arrayProp, char* numberConversionBuffer, size_t bufferMax)
   {
      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_ARRAY_ELEMENT);

      mWriter->BeginElement(MapXMLConstants::ACTOR_ARRAY_SIZE_ELEMENT);
      int arraySize = arrayProp.GetArraySize();
      snprintf(numberConversionBuffer, bufferMax, "%d", arraySize);
      mWriter->AddCharacters(numberConversionBuffer);
      mWriter->EndElement();

      // Save out the data for each index.
      for (int index = 0; index < arraySize; index++)
      {
         // Save out the index number.
         mWriter->BeginElement(MapXMLConstants::ACTOR_ARRAY_ELEMENT);

         mWriter->BeginElement(MapXMLConstants::ACTOR_ARRAY_INDEX_ELEMENT);
         snprintf(numberConversionBuffer, bufferMax, "%d", index);
         mWriter->AddCharacters(numberConversionBuffer);
         mWriter->EndElement();

         // Write the data for the current property.
         arrayProp.SetIndex(index);
         WriteProperty(*arrayProp.GetArrayProperty());

         mWriter->EndElement();
      }

      mWriter->EndElement();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::WriteContainer(const ContainerActorProperty& containerProp, char* numberConversionBuffer, size_t bufferMax)
   {
      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT);

      // Save out the data for each index.
      for (int index = 0; index < containerProp.GetPropertyCount(); index++)
      {
         // Write the data for the current property.
         WriteProperty(*containerProp.GetProperty(index));
      }

      mWriter->EndElement();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::WriteContainerSelector(const ContainerSelectorActorProperty& containerProp, char* numberConversionBuffer, size_t bufferMax)
   {
      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT);

      // Save out the container type name.
      mWriter->BeginElement(MapXMLConstants::ACTOR_TYPE_ELEMENT);
      mWriter->AddCharacters(containerProp.GetValue());
      mWriter->EndElement();

      // Save out each property contained within the selected container.
      dtCore::PropertyContainer* propCon = containerProp.GetContainer();
      if (propCon != NULL)
      {
         std::vector<dtCore::ActorProperty*> properties;
         propCon->GetPropertyList(properties);

         dtCore::SerializerRuntimeData data;
         data = Top();
         data.mPropertyContainer = propCon;
         mData.push(data);

         std::vector<dtCore::ActorProperty*>::const_iterator i, iend;
         i = properties.begin();
         iend = properties.end();
         // Save out the data for each index.
         for (; i != iend; ++i)
         {
            data.mActorProperty = *i;
            data.mActorPropertyType = &(*i)->GetDataType();
            // Write the data for the current property.
            WriteProperty(**i);
         }

         mData.pop();
      }
      mWriter->EndElement();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::WritePropertyContainer(const BasePropertyContainerActorProperty& propConProp)
   {
      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_PROPERTY_CONTAINER_ELEMENT);

      dtCore::PropertyContainer* propCon = propConProp.GetValue();
      // TODO What to write if NULL?
      if (propCon != NULL)
      {
         std::vector<dtCore::ActorProperty*> properties;
         propCon->GetPropertyList(properties);

         dtCore::SerializerRuntimeData data;
         data = Top();
         data.mPropertyContainer = propCon;
         mData.push(data);

         std::vector<dtCore::ActorProperty*>::const_iterator i, iend;
         i = properties.begin();
         iend = properties.end();
         // Save out the data for each index.
         for (; i != iend; ++i)
         {
            data.mActorProperty = *i;
            data.mActorPropertyType = &(*i)->GetDataType();
            // Write the data for the current property.
            WriteProperty(**i);
         }

         mData.pop();
      }
      mWriter->EndElement();
   }

   /////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::WriteSimple(const AbstractParameter& holder)
   {
      switch (holder.GetDataType().GetTypeId())
      {
         case DataType::FLOAT_ID:
            mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_FLOAT_ELEMENT);
            break;
         case DataType::DOUBLE_ID:
            mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_DOUBLE_ELEMENT);
            break;
         case DataType::INT_ID:
            mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_INTEGER_ELEMENT);
            break;
         case DataType::LONGINT_ID:
            mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_LONG_ELEMENT);
            break;
         case DataType::BOOLEAN_ID:
            mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_BOOLEAN_ELEMENT);
            break;
         case DataType::ACTOR_ID:
            mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_ACTOR_ID_ELEMENT);
            break;
         case DataType::GAMEEVENT_ID:
            mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_GAMEEVENT_ELEMENT);
            break;
         case DataType::ENUMERATION_ID:
            mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_ENUM_ELEMENT);
            break;
         case DataType::BIT_MASK_ID:
            mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_BIT_MASK_ELEMENT);
            break;
         default:
            //LOG ERROR
            return;
      }
      mWriter->AddCharacters(holder.ToString());
      mWriter->EndElement();
   }

   /////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::AssignGroupProperties()
   {
      for (std::multimap<PropertyContainer*, std::pair<std::string, dtCore::RefPtr<dtCore::NamedGroupParameter> > >::iterator i
         = mGroupParameters.begin();
         i != mGroupParameters.end(); ++i)
      {
         PropertyContainer* propContainer = i->first;
         if (propContainer == NULL)
         {
            continue;
         }
         std::pair<std::string, dtCore::RefPtr<dtCore::NamedGroupParameter> >& param = i->second;
         std::string& propertyName = param.first;
         dtCore::RefPtr<dtCore::NamedGroupParameter>& propValue = param.second;

         dtCore::RefPtr<ActorProperty> property = propContainer->GetProperty(propertyName);
         if (!property.valid())
         {
            property = propContainer->GetDeprecatedProperty(propertyName);
         }

         if (!property.valid())
         {
            mParser->mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__,
               "Group property %s set with actor %s does not exist on the property container.",
               propertyName.c_str(), propValue->ToString().c_str());
            continue;
         }

         GroupActorProperty* gap = dynamic_cast<GroupActorProperty*>(property.get());
         if (gap == NULL)
         {
            mParser->mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__,
               "Actor property %s set with actor %s is not a GroupActorProperty.",
               propertyName.c_str(), propValue->ToString().c_str());
            continue;
         }
         gap->SetValue(*propValue);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::EndActorPropertyGroupElement()
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      SerializerRuntimeData& data = Top();

      dtCore::RefPtr<NamedGroupParameter> topParam = static_cast<NamedGroupParameter*>(data.mParameterStack.top().get());
      data.mParameterStack.pop();
      //The only way we know we have completed a group actor property is that the
      //stack of parameters has been emptied since they can nest infinitely.
      if (data.mParameterStack.empty())
      {
         data.mInGroupProperty = false;
         mGroupParameters.insert(std::make_pair(data.mPropertyContainer.get(),
            std::make_pair(data.mActorProperty->GetName(), topParam)));
      }
   }

   /////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::ParameterCharacters(BaseXMLHandler::xmlCharString& topEl, const XMLCh* const chars)
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      SerializerRuntimeData& data = Top();

      if (topEl == MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT)
      {
         data.mParameterNameToCreate = dtUtil::XMLStringConverter(chars).ToString();
         if (data.mParameterNameToCreate.empty())
            mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "In named parameter section, found a named parameter with an empty name.");

      }
      //Resource datatypes are held in a string value inside the resource element.
      //This means creating the parameter has to be here, when the type element is found.
      else if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT)
      {
         std::string resourceTypeString = dtUtil::XMLStringConverter(chars).ToString();
         data.mParameterTypeToCreate = static_cast<DataType*>(DataType::GetValueForName(resourceTypeString));

         if (data.mParameterTypeToCreate == NULL)
         {
            mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
               "No resource type found for type specified in xml \"%s.\"",
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
      else if (!data.mParameterStack.empty() && data.mParameterStack.top()->GetDataType() != DataType::GROUP)
      {
         std::string dataValue = dtUtil::XMLStringConverter(chars).ToString();

         if (mParser->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mParser->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Setting value of parameter %s, parameter datatype %s, value %s, element name %s.",
            data.mParameterStack.top()->GetName().c_str(),
            data.mParameterStack.top()->GetDataType().GetName().c_str(),
            dataValue.c_str(), dtUtil::XMLStringConverter(topEl.c_str()).c_str());
         }

         //we now have the property, the type, and the data.
         ParseParameterData(topEl, dataValue);
      }
   }

   /////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::CreateAndPushParameter()
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      SerializerRuntimeData& data = Top();

      try
      {
         NamedGroupParameter& gp = dynamic_cast<NamedGroupParameter&>(*data.mParameterStack.top());
         data.mParameterStack.push(gp.AddParameter(data.mParameterNameToCreate, *data.mParameterTypeToCreate));
         data.mParameterTypeToCreate = NULL;
         data.mParameterNameToCreate.clear();
      }
      catch (const std::bad_cast&)
      {
         mParser->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Tried to add a new parameter \"%s\" with type \"%s\", but failed",
            data.mParameterNameToCreate.c_str(), data.mParameterTypeToCreate->GetName().c_str());
      }
   }

   /////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::ParsePropertyData(BaseXMLHandler::xmlCharString& topEl, std::string& dataValue, dtCore::DataType*& dataType, dtCore::ActorProperty* actorProperty)
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      SerializerRuntimeData& data = Top();

      if (!IsPropertyCorrectType(dataType, actorProperty))
      {
         return;
      }

      switch ((dataType)->GetTypeId())
      {
      case DataType::FLOAT_ID:
      case DataType::DOUBLE_ID:
      case DataType::INT_ID:
      case DataType::LONGINT_ID:
      case DataType::STRING_ID:
      case DataType::BOOLEAN_ID:
      case DataType::ENUMERATION_ID:
      case DataType::BIT_MASK_ID:
         {
            if (!actorProperty->FromString(dataValue))
            {
               mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                  "Failed Setting value %s for property type %s named %s",
                  dataValue.c_str(), (dataType)->GetName().c_str(),
                  actorProperty->GetName().c_str());
            }
            (dataType) = NULL;
            break;
         }
      case DataType::GAMEEVENT_ID:
         {
            GameEventActorProperty& geProp = static_cast<GameEventActorProperty&>(*actorProperty);
            if (!dtUtil::Trim(dataValue).empty())
            {
               GameEvent *e = NULL;
               dtCore::UniqueId id = dtCore::UniqueId(dataValue);

               if (mMap.valid())
               {
                  e = mMap->GetEventManager().FindEvent(id);
               }

               if (e == NULL)
               {
                  e = Project::GetInstance().GetGameEvent(id);
               }

               if (e != NULL)
               {
                  geProp.SetValue(e);
               }
               else
               {
                  geProp.SetValue(NULL);
                  mParser->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                     "Game Event referenced in actor property %s was not found.",
                     actorProperty->GetName().c_str());
               }
            }
            else
            {
               geProp.SetValue(NULL);
            }
            dataType = NULL;
            break;
         }
      case DataType::VEC2_ID:
         {
            if (ParseVec(topEl, dataValue, 2))
            {
               Vec2ActorProperty& p = static_cast<Vec2ActorProperty&>(*actorProperty);
               p.SetValue(osg::Vec2(mVecBeingParsed.x(), mVecBeingParsed.y()));
            }
            break;
         }
      case DataType::VEC2F_ID:
         {
            if (ParseVec(topEl, dataValue, 2))
            {
               Vec2fActorProperty& p = static_cast<Vec2fActorProperty&>(*actorProperty);
               p.SetValue(osg::Vec2f(mVecBeingParsed.x(), mVecBeingParsed.y()));
            }
            break;
         }
      case DataType::VEC2D_ID:
         {
            if (ParseVec(topEl, dataValue, 2))
            {
               Vec2dActorProperty& p = static_cast<Vec2dActorProperty&>(*actorProperty);
               p.SetValue(osg::Vec2d(mVecBeingParsed.x(), mVecBeingParsed.y()));
            }
            break;
         }
      case DataType::VEC3_ID:
         {
            if (ParseVec(topEl, dataValue, 3))
            {
               Vec3ActorProperty& p = static_cast<Vec3ActorProperty&>(*actorProperty);
               p.SetValue(osg::Vec3(mVecBeingParsed.x(), mVecBeingParsed.y(), mVecBeingParsed.z()));
            }
            break;
         }
      case DataType::VEC3F_ID:
         {
            if (ParseVec(topEl, dataValue, 3))
            {
               Vec3fActorProperty& p = static_cast<Vec3fActorProperty&>(*actorProperty);
               p.SetValue(osg::Vec3f(mVecBeingParsed.x(), mVecBeingParsed.y(), mVecBeingParsed.z()));
            }
            break;
         }
      case DataType::VEC3D_ID:
         {
            if (ParseVec(topEl, dataValue, 3))
            {
               Vec3dActorProperty& p = static_cast<Vec3dActorProperty&>(*actorProperty);
               p.SetValue(osg::Vec3d(mVecBeingParsed.x(), mVecBeingParsed.y(), mVecBeingParsed.z()));
            }
            break;
         }
      case DataType::VEC4_ID:
         {
            if (ParseVec(topEl, dataValue, 4))
            {
               Vec4ActorProperty& p = static_cast<Vec4ActorProperty&>(*actorProperty);
               p.SetValue(mVecBeingParsed);
            }
            break;
         }
      case DataType::VEC4F_ID:
         {
            if (ParseVec(topEl, dataValue, 4))
            {
               Vec4fActorProperty& p = static_cast<Vec4fActorProperty&>(*actorProperty);
               p.SetValue(osg::Vec4f(mVecBeingParsed));
            }
            break;
         }
      case DataType::VEC4D_ID:
         {
            if (ParseVec(topEl, dataValue, 4))
            {
               Vec4dActorProperty& p = static_cast<Vec4dActorProperty&>(*actorProperty);
               p.SetValue(osg::Vec4d(mVecBeingParsed));
            }
            break;
         }
      case DataType::RGBACOLOR_ID:
         {
            if (ParseVec(topEl, dataValue, 4))
            {
               ColorRgbaActorProperty& p = static_cast<ColorRgbaActorProperty&>(*actorProperty);
               p.SetValue(mVecBeingParsed);
            }
            break;
         }
      case DataType::ACTOR_ID:
         {
            //insert the data into this map to make it accessible to assign once the parsing is done.
            dtUtil::Trim(dataValue);
            if (!dataValue.empty() && dataValue != "NULL")
            {
               ActorIDActorProperty* p = dynamic_cast<ActorIDActorProperty*>(actorProperty);
               if (p)
               {
                  if (!p->FromString(dataValue))
                  {
                     mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                        "Failed Setting value %s for property type %s named %s",
                        dataValue.c_str(), dataType->GetName().c_str(),
                        actorProperty->GetName().c_str());
                  }
               }
               else
               {
                  mActorLinking.insert(std::make_pair(data.mPropertyContainer.get(), std::make_pair(actorProperty->GetName(), dtCore::UniqueId(dataValue))));
               }
            }
            dataType = NULL;
            break;
         }
      case DataType::GROUP_ID:
         {
            ///Nothing Useful happens here.
            break;
         }
      case DataType::ARRAY_ID:
         {
            ArrayActorPropertyBase* arrayProp = dynamic_cast<ArrayActorPropertyBase*>(actorProperty);
            if (arrayProp != NULL)
            {
               ParseArray(topEl, dataValue, arrayProp);
            }
            break;
         }
      case DataType::PROPERTY_CONTAINER_ID:
         {
            break;
         }
      case DataType::CONTAINER_ID:
         {
            ContainerActorProperty* containerProp = static_cast<ContainerActorProperty*>(actorProperty);
            ParseContainer(topEl, dataValue, containerProp);
            break;
         }
      case DataType::CONTAINER_SELECTOR_ID:
         {
            ContainerSelectorActorProperty* containerProp = static_cast<ContainerSelectorActorProperty*>(actorProperty);
            ParseContainerSelector(topEl, dataValue, containerProp);
            break;
         }
      default:
         {
            if (dataType->IsResource())
            {
               ResourceActorProperty& p = static_cast<ResourceActorProperty&>(*actorProperty);
               if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT)
               {
                  if (dataValue != p.GetDataType().GetName())
                  {
                     mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                        "Save file expected resource property %s to have type %s, but it is %s.",
                        actorProperty->GetName().c_str(),
                        dataValue.c_str(), p.GetDataType().GetName().c_str());
                     p.SetValue(dtCore::ResourceDescriptor::NULL_RESOURCE);
                     dataType = NULL;
                  }
               }
               else if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_DISPLAY_ELEMENT)
               {
                  data.mDescriptorDisplayName = dataValue;
               }
               else if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT)
               {
                  //if the value is null, then both strings will be empty.
                  if (!dataValue.empty() && !data.mDescriptorDisplayName.empty())
                  {
                     ResourceDescriptor rd(data.mDescriptorDisplayName, dataValue);
                     p.SetValue(rd);
                  }
                  else
                  {
                     p.SetValue(dtCore::ResourceDescriptor::NULL_RESOURCE);
                  }
               }
            }
            else
            {
               mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                  "DataType \"%s\" is not supported in the map loading code.  The data has been ignored.",
                  dataType->GetName().c_str());
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::ParseParameterData(BaseXMLHandler::xmlCharString& topEl, std::string& dataValue)
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      SerializerRuntimeData& data = Top();

      if (data.mParameterStack.empty())
      {
         mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Unable to set the value \"%s\" without a valid parameter for group actor property \"%s\"",
            dataValue.c_str(), data.mActorProperty->GetName().c_str());
         return;
      }

      ///Optimization
      if (topEl == MapXMLConstants::ACTOR_PROPERTY_PARAMETER_ELEMENT)
      {
         return;
      }

      NamedParameter& np = *data.mParameterStack.top();

      switch (np.GetDataType().GetTypeId())
      {
      case DataType::FLOAT_ID:
      case DataType::DOUBLE_ID:
      case DataType::INT_ID:
      case DataType::LONGINT_ID:
      case DataType::STRING_ID:
      case DataType::BOOLEAN_ID:
      case DataType::ENUMERATION_ID:
      case DataType::BIT_MASK_ID:
         {
            if (!np.FromString(dataValue))
            {
               mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                  "Failed Setting value \"%s\" for parameter type \"%s\" named \"%s\" in property \"%s\".",
                  dataValue.c_str(), data.mActorPropertyType->GetName().c_str(),
                  data.mActorProperty->GetName().c_str());
            }
            break;
         }
      case DataType::GAMEEVENT_ID:
         {
            NamedGameEventParameter& geParam = static_cast<NamedGameEventParameter&>(np);
            if (!dtUtil::Trim(dataValue).empty())
            {
               GameEvent *e = NULL;
               dtCore::UniqueId id = dtCore::UniqueId(dataValue);

               if (mMap.valid())
               {
                  e = mMap->GetEventManager().FindEvent(id);
               }

               if (e == NULL)
               {
                  e = Project::GetInstance().GetGameEvent(id);
               }

               if (e != NULL)
               {
                  geParam.SetValue(dtCore::UniqueId(dataValue));
               }
               else
               {
                  geParam.SetValue(dtCore::UniqueId(""));
                  mParser->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                     "Game Event referenced in named parameter \"%s\" in property \"%s\" "
                     "was not found.",
                     np.GetName().c_str(), data.mActorProperty->GetName().c_str());
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
            if (ParseVec(topEl, dataValue, 2))
            {
               NamedVec2Parameter& p = static_cast<NamedVec2Parameter&>(np);
               p.SetValue(osg::Vec2(mVecBeingParsed.x(), mVecBeingParsed.y()));
            }
            break;
         }
      case DataType::VEC2F_ID:
         {
            if (ParseVec(topEl, dataValue, 2))
            {
               NamedVec2fParameter& p = static_cast<NamedVec2fParameter&>(np);
               p.SetValue(osg::Vec2f(mVecBeingParsed.x(), mVecBeingParsed.y()));
            }
            break;
         }
      case DataType::VEC2D_ID:
         {
            if (ParseVec(topEl, dataValue, 2))
            {
               NamedVec2dParameter& p = static_cast<NamedVec2dParameter&>(np);
               p.SetValue(osg::Vec2d(mVecBeingParsed.x(), mVecBeingParsed.y()));
            }
            break;
         }
      case DataType::VEC3_ID:
         {
            if (ParseVec(topEl, dataValue, 3))
            {
               NamedVec3Parameter& p = static_cast<NamedVec3Parameter&>(np);
               p.SetValue(osg::Vec3(mVecBeingParsed.x(), mVecBeingParsed.y(), mVecBeingParsed.z()));
            }
            break;
         }
      case DataType::VEC3F_ID:
         {
            if (ParseVec(topEl, dataValue, 3))
            {
               NamedVec3fParameter& p = static_cast<NamedVec3fParameter&>(np);
               p.SetValue(osg::Vec3f(mVecBeingParsed.x(), mVecBeingParsed.y(), mVecBeingParsed.z()));
            }
            break;
         }
      case DataType::VEC3D_ID:
         {
            if (ParseVec(topEl, dataValue, 3))
            {
               NamedVec3dParameter& p = static_cast<NamedVec3dParameter&>(np);
               p.SetValue(osg::Vec3d(mVecBeingParsed.x(), mVecBeingParsed.y(), mVecBeingParsed.z()));
            }
            break;
         }
      case DataType::VEC4_ID:
         {
            if (ParseVec(topEl, dataValue, 4))
            {
               NamedVec4Parameter& p = static_cast<NamedVec4Parameter&>(np);
               p.SetValue(mVecBeingParsed);
            }
            break;
         }
      case DataType::VEC4F_ID:
         {
            if (ParseVec(topEl, dataValue, 4))
            {
               NamedVec4fParameter& p = static_cast<NamedVec4fParameter&>(np);
               p.SetValue(osg::Vec4f(mVecBeingParsed));
            }
            break;
         }
      case DataType::VEC4D_ID:
         {
            if (ParseVec(topEl, dataValue, 4))
            {
               NamedVec4dParameter& p = static_cast<NamedVec4dParameter&>(np);
               p.SetValue(osg::Vec4d(mVecBeingParsed));
            }
            break;
         }
      case DataType::RGBACOLOR_ID:
         {
            if (ParseVec(topEl, dataValue, 4))
            {
               NamedRGBAColorParameter& p = static_cast<NamedRGBAColorParameter&>(np);
               p.SetValue(mVecBeingParsed);
            }
            break;
         }
      case DataType::ACTOR_ID:
         {
            //insert the data into this map to make it accessible to assign once the parsing is done.
            dtUtil::Trim(dataValue);
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
                  data.mDescriptorDisplayName = dataValue;
               }
               else if (topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_IDENTIFIER_ELEMENT)
               {
                  //if the value is null, then both strings will be empty.
                  if (!dataValue.empty() && !data.mDescriptorDisplayName.empty())
                  {
                     ResourceDescriptor rd(data.mDescriptorDisplayName, dataValue);
                     p.SetValue(rd);
                  }
                  else
                  {
                     p.SetValue(ResourceDescriptor::NULL_RESOURCE);
                  }
               }
            }
            else
            {
               mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                  "DataType \"%s\" is not supported in the map loading code.  Ignoring parameter \"%s\".",
                  np.GetDataType().GetName().c_str(), np.GetName().c_str());

            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool ActorPropertySerializer::ParseVec(BaseXMLHandler::xmlCharString& topEl, const std::string& dataValue, size_t vecSize)
   {
      // Bail out if this is a property element
      if (topEl == MapXMLConstants::ACTOR_PROPERTY_VEC2_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_VEC3_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_VEC4_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_COLOR_RGBA_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_COLOR_RGBA_ELEMENT)
      {
         return false;
      }

      char* endMarker;
      double value = strtod(dataValue.c_str(), &endMarker);
      bool isVecComplete = false;

      if (topEl == MapXMLConstants::ACTOR_VEC_1_ELEMENT || topEl == MapXMLConstants::ACTOR_COLOR_R_ELEMENT)
      {
         // Since this is our first element, clear out any previous values with NaNs
         // so that if one of the values is not in the xml file we are parsing
         // it will be obvious
         osg::Vec4::value_type nan = std::numeric_limits<osg::Vec4::value_type>::quiet_NaN();
         mVecBeingParsed = osg::Vec4(nan, nan, nan, nan);

         mVecBeingParsed[0] = value;
      }
      else if (topEl == MapXMLConstants::ACTOR_VEC_2_ELEMENT || topEl == MapXMLConstants::ACTOR_COLOR_G_ELEMENT)
      {
         mVecBeingParsed[1] = value;

         // If we are a Vec2, then we are complete
         isVecComplete = vecSize == 2;
      }
      else if (vecSize >= 3 && (topEl == MapXMLConstants::ACTOR_VEC_3_ELEMENT || topEl == MapXMLConstants::ACTOR_COLOR_B_ELEMENT))
      {
         mVecBeingParsed[2] = value;

         // If we are a Vec3, then we are complete
         isVecComplete = vecSize == 3;
      }
      else if (vecSize == 4 && (topEl == MapXMLConstants::ACTOR_VEC_4_ELEMENT || topEl == MapXMLConstants::ACTOR_COLOR_A_ELEMENT))
      {
         mVecBeingParsed[3] = value;
         isVecComplete = true;
      }
      else
      {
         mParser->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Got an invalid element for a Vec%u: %s", unsigned(vecSize), dtUtil::XMLStringConverter(topEl.c_str()).c_str());
      }

      return isVecComplete;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::EnterPropertyContainer()
   {
      //SerializerRuntimeData& data = Top();

      dtCore::ActorProperty* actorProperty = GetNestedProperty();

      dtCore::ArrayActorPropertyBase* arrayProp = dynamic_cast<dtCore::ArrayActorPropertyBase*>(actorProperty);
      if (arrayProp)
      {
         actorProperty = arrayProp->GetArrayProperty();
      }
      else
      {
         dtCore::ContainerActorProperty* containerProp = dynamic_cast<dtCore::ContainerActorProperty*>(actorProperty);
         if (containerProp)
         {
            actorProperty = containerProp->GetCurrentProperty();
         }
      }

      mData.push(dtCore::SerializerRuntimeData());
      mData.top().Reset();
      BasePropertyContainerActorProperty* pcProp = dynamic_cast<BasePropertyContainerActorProperty*>(actorProperty);
      if (pcProp != NULL)
      {

         dtCore::PropertyContainer* pc = pcProp->GetValue();
         if (pc == NULL)
         {
            // TODO figure out a way to pass on the type.
            pcProp->CreateNew();
            pc = pcProp->GetValue();
         }

         // This works even if it's NULL, but will the rest of the code?
         SetCurrentPropertyContainer(pc);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::ParseArray(BaseXMLHandler::xmlCharString& topEl, std::string& dataValue, ArrayActorPropertyBase* arrayProp)
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      //SerializerRuntimeData& data = Top();

      //arrayProp = dynamic_cast<ArrayActorPropertyBase*>(GetNestedProperty());
      //if (!arrayProp)
      //{
      //   return;
      //}

      // Array Size
      if (topEl == MapXMLConstants::ACTOR_ARRAY_SIZE_ELEMENT)
      {
         int arraySize;
         std::istringstream stream;
         stream.str(dataValue);
         stream >> arraySize;

         int min = arrayProp->GetMinArraySize();
         int max = arrayProp->GetMaxArraySize();
         min = min < 0 ? 0 : min;
         max = max < 0 ? arraySize : max;
         dtUtil::Clamp(arraySize, min, max);

         // Add more elements.
         arrayProp->SetIndex(0);
         while (arraySize > arrayProp->GetArraySize())
         {
            arrayProp->PushBack();
         }

         // Remove extra elements.
         while (arraySize < arrayProp->GetArraySize())
         {
            arrayProp->PopBack();
         }
      }
      // Set current Array Index
      else if (topEl == MapXMLConstants::ACTOR_ARRAY_INDEX_ELEMENT)
      {
         int index;
         std::istringstream stream;
         stream.str(dataValue);
         stream >> index;
         arrayProp->SetIndex(index);
      }
      // Skipped elements.
      else if (topEl == MapXMLConstants::ACTOR_ARRAY_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_ARRAY_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_ELEMENT)
      {
      }
      // Unrecognized elements are checked with the array's property type
      else
      {
         ActorProperty* prop = arrayProp->GetArrayProperty();
         if (prop != NULL)
         {
            DataType* propType = &prop->GetDataType();

            // If the element is the name of the property, we only care
            // about it if we are parsing container elements.
            if (topEl != MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT ||
               (propType == &DataType::ARRAY || propType == &DataType::CONTAINER ||
               propType == &DataType::CONTAINER_SELECTOR))
            {
               ParsePropertyData(topEl, dataValue, propType, prop);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::ParseContainer(BaseXMLHandler::xmlCharString& topEl, std::string& dataValue, ContainerActorProperty* containerProp)
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      //SerializerRuntimeData& data = Top();

      // Find the property we want to edit based on how deep the container is nested.
      //containerProp = dynamic_cast<ContainerActorProperty*>(GetNestedProperty());
      //if (!containerProp)
      //{
      //   return;
      //}

      // Skipped elements.
      if (topEl == MapXMLConstants::ACTOR_ARRAY_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_ARRAY_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_ELEMENT)
      {
      }
      else if (topEl == MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT)
      {
         // Find the property in the container with the given name.
         for (int index = 0; index < containerProp->GetPropertyCount(); index++)
         {
            ActorProperty* prop = containerProp->GetProperty(index);
            if (prop)
            {
               if (prop->GetName() == dataValue)
               {
                  containerProp->SetCurrentPropertyIndex(index);
                  break;
               }
            }
         }
      }
      // Unrecognized elements are checked with the array's property type
      else
      {
         int index = containerProp->GetCurrentPropertyIndex();
         ActorProperty* prop = containerProp->GetProperty(index);
         if (prop)
         {
            DataType* propType = &prop->GetDataType();
            ParsePropertyData(topEl, dataValue, propType, prop);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::ParseContainerSelector(BaseXMLHandler::xmlCharString& topEl, std::string& dataValue, ContainerSelectorActorProperty* containerProp)
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      // Skipped elements.
      if (topEl == MapXMLConstants::ACTOR_ARRAY_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_ARRAY_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_ELEMENT)
      {
      }
      // The container type name.
      else if (topEl == MapXMLConstants::ACTOR_TYPE_ELEMENT)
      {
         containerProp->SetValue(dataValue);
      }
      else if (topEl == MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT)
      {
         // Find the property in the container with the given name.
         dtCore::PropertyContainer* propCon = containerProp->GetContainer();
         if (propCon)
         {
            std::vector<const dtCore::ActorProperty*> propList;
            propCon->GetPropertyList(propList);
            int count = (int)propList.size();
            for (int index = 0; index < count; ++index)
            {
               if (propList[index]->GetName() == dataValue)
               {
                  containerProp->SetCurrentPropertyIndex(index);
                  break;
               }
            }
         }
      }
      // Unrecognized elements are checked with the contained property type
      else
      {
         int index = containerProp->GetCurrentPropertyIndex();
         dtCore::PropertyContainer* propCon = containerProp->GetContainer();

         if (index > -1 && propCon)
         {
            std::vector<dtCore::ActorProperty*> propList;
            propCon->GetPropertyList(propList);

            if (index < (int)propList.size())
            {
               ActorProperty* prop = propList[index];
               if (prop)
               {
                  dtCore::SerializerRuntimeData data;
                  data = Top();
                  data.mPropertyContainer = propCon;
                  data.mActorProperty = prop;
                  data.mActorPropertyType = &prop->GetDataType();
                  mData.push(data);

                  DataType* propType = &prop->GetDataType();
                  ParsePropertyData(topEl, dataValue, propType, prop);

                  mData.pop();
               }
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   DataType* ActorPropertySerializer::ParsePropertyType(const XMLCh* const localname, bool errorIfNotFound)
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
         MapXMLConstants::ACTOR_PROPERTY_ARRAY_ELEMENT) == 0)
      {
         return &DataType::ARRAY;
      }
      else if (XMLString::compareString(localname,
         MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT) == 0)
      {
         return &DataType::CONTAINER;
      }
      else if (XMLString::compareString(localname,
         MapXMLConstants::ACTOR_PROPERTY_CONTAINER_SELECTOR_ELEMENT) == 0)
      {
         return &DataType::CONTAINER_SELECTOR;
      }
      else if (XMLString::compareString(localname,
         MapXMLConstants::ACTOR_PROPERTY_PROPERTY_CONTAINER_ELEMENT) == 0)
      {
         return &DataType::PROPERTY_CONTAINER;
      }
      else if (XMLString::compareString(localname,
         MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT) == 0)
      {
         //Need the character contents to know the type, so this will be
         //handled later.
      }
      else if (XMLString::compareString(localname,
         MapXMLConstants::ACTOR_PROPERTY_BIT_MASK_ELEMENT) == 0)
      {
         return &DataType::BIT_MASK;
      }
      else if (errorIfNotFound)
      {
         mParser->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
            "Found property data element with name '%s', but this does not map to a known type.\n",
            dtUtil::XMLStringConverter(localname).c_str());
      }
      return NULL;
   }


   /////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::NonEmptyDefaultWorkaround()
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      SerializerRuntimeData& data = Top();

      // We don't have control over what the string or actor
      // property default value is, but if the data in the
      // xml is empty string, no event is generated.  Thus,
      // this preemptively sets this string to "" so that
      // empty data will work.
      if (data.mActorPropertyType != NULL && !data.mActorProperty->IsReadOnly())
      {
         if (*data.mActorPropertyType == DataType::STRING
            || *data.mActorPropertyType == DataType::ACTOR)
         {
            data.mActorProperty->FromString(std::string());
         }
         else if (*data.mActorPropertyType == DataType::GAME_EVENT)
         {
            static_cast<GameEventActorProperty*>(data.mActorProperty.get())->SetValue(NULL);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::EndActorPropertyParameterElement()
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      SerializerRuntimeData& data = Top();

      ///We pop if the element was never filled in.  This happens if the value is empty for
      ///parameter data.  We don't pop for a group because that is handled separately below.
      if (!data.mParameterStack.empty() && data.mParameterStack.top()->GetDataType() != DataType::GROUP)
      {
         data.mParameterStack.pop();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::EndActorPropertyElement()
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      SerializerRuntimeData& data = Top();

      data.mInActorProperty = false;
      data.mActorProperty = NULL;
      data.mActorPropertyType = NULL;
   }

   /////////////////////////////////////////////////////////////////
   bool ActorPropertySerializer::IsPropertyCorrectType(dtCore::DataType*& dataType, dtCore::ActorProperty* actorProperty)
   {
      if (actorProperty == NULL || (dataType) == NULL)
      {
         mParser->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Call made to %s with content handler in incorrect state.", __FUNCTION__);
         return false;
      }
      bool result = actorProperty->GetDataType() == *(dataType);
      if (!result)
      {
         if ( (*(dataType) == DataType::VEC2 && (actorProperty->GetDataType() == DataType::VEC2D || actorProperty->GetDataType() == DataType::VEC2F)) ||
            (*(dataType) == DataType::VEC3 && (actorProperty->GetDataType() == DataType::VEC3D || actorProperty->GetDataType() == DataType::VEC3F)) ||
            (*(dataType) == DataType::VEC4 && (actorProperty->GetDataType() == DataType::VEC4D || actorProperty->GetDataType() == DataType::VEC4F)) )
         {
            mParser->mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
               "Differentiating between a osg::vecf/osg::vecd and a osg::vec. Correcting.");
            (dataType) = &actorProperty->GetDataType();
            return true;
         }

         bool isSimple1, isSimple2;
         switch (dataType->GetTypeId())
         {
         case DataType::FLOAT_ID:
         case DataType::DOUBLE_ID:
         case DataType::INT_ID:
         case DataType::LONGINT_ID:
         case DataType::STRING_ID:
         case DataType::BOOLEAN_ID:
         case DataType::ENUMERATION_ID:
         case DataType::BIT_MASK_ID:
            isSimple1 = true;
            break;
         default:
            isSimple1 = false;
         }

         switch (actorProperty->GetDataType().GetTypeId())
         {
         case DataType::FLOAT_ID:
         case DataType::DOUBLE_ID:
         case DataType::INT_ID:
         case DataType::LONGINT_ID:
         case DataType::STRING_ID:
         case DataType::BOOLEAN_ID:
         case DataType::ENUMERATION_ID:
         case DataType::BIT_MASK_ID:
            isSimple2 = true;
            break;
         default:
            isSimple2 = false;
         }

         if (isSimple1 && isSimple2)
         {
            mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
               "Allowing simple datatypes to attempt to set the value from string.  It may convert. %s %s %s",
               actorProperty->GetName().c_str(), dataType->GetName().c_str(), actorProperty->GetDataType().GetName().c_str());
            (dataType) = &actorProperty->GetDataType();
            return true;
         }


         mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Parameter/Property '%s' was saved as type %s, but is now of type %s. Data will be ignored",
            actorProperty->GetName().c_str(),
            (dataType)->GetName().c_str(), actorProperty->GetDataType().GetName().c_str());
      }
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ActorProperty* ActorPropertySerializer::GetNestedProperty()
   {
      SerializerRuntimeData& data = Top();

      dtCore::ActorProperty* prop = data.mActorProperty.get();
      int count = (int)data.mNestedTypes.size()-1;
      for (int index = 0; index < count; ++index)
      {
         int type = data.mNestedTypes[index];

         switch (type)
         {
         case NEST_ARRAY:
            {
               ArrayActorPropertyBase* arrayActorProp = dynamic_cast<ArrayActorPropertyBase*>(prop);
               if (arrayActorProp)
               {
                  prop = arrayActorProp->GetArrayProperty();
               }
            }
            break;

         case NEST_CONTAINER:
            {
               ContainerActorProperty* contActorProp = dynamic_cast<ContainerActorProperty*>(prop);
               if (contActorProp)
               {
                  prop = contActorProp->GetCurrentProperty();
               }
               else
               {
                  ContainerSelectorActorProperty* contSelActorProp = dynamic_cast<ContainerSelectorActorProperty*>(prop);
                  if (contSelActorProp)
                  {
                     int propIndex = contSelActorProp->GetCurrentPropertyIndex();
                     dtCore::PropertyContainer* propCon = contSelActorProp->GetContainer();
                     if (propCon)
                     {
                        std::vector<dtCore::ActorProperty*> propList;
                        propCon->GetPropertyList(propList);

                        if (propIndex > -1 && propIndex < (int)propList.size())
                        {
                           prop = propList[propIndex];
                        }
                     }
                  }
               }
            }
            break;
         }
      }

      return prop;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::DataType*& ActorPropertySerializer::GetNestedType()
   {
      SerializerRuntimeData& data = Top();

      dtCore::ActorProperty* prop = GetNestedProperty();
      if (prop && prop != data.mActorProperty.get())
      {
         data.mNestedPropertyType = DataType::GetValueForName(prop->GetDataType().GetName());
         return data.mNestedPropertyType;
      }

      return data.mActorPropertyType;
   }

   /////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::LinkActors()
   {
//      if (mData.empty())
//      {
//         LOG_ERROR("Data stack is empty.");
//         return;
//      }
//
//      SerializerRuntimeData& data = Top();

      // Can't link actors if we can't find them using the map.
      if (!mMap.valid()) return;

      for (PropContainerToNameIdMultimap::iterator i = mActorLinking.begin();
         i != mActorLinking.end(); ++i)
      {
         PropertyContainer* propContainer = i->first.get();
         if (propContainer == NULL)
         {
            continue;
         }
         NameIdPair& data = i->second;
         std::string& propertyName = data.first;
         dtCore::UniqueId& propValueId = data.second;
         if (propValueId.ToString().empty())
         {
            mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
               "Container was defined to have actor property %s set, but the id is empty.",
               propertyName.c_str(), propValueId.ToString().c_str());
         }
         BaseActorObject* valueProxy = mMap->GetProxyById(propValueId);
         if (valueProxy == NULL)
         {
            mParser->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
               "Property Container was defined to have actor property %s set with actor %s, but the proxy does not exist in the new map.",
               propertyName.c_str(), propValueId.ToString().c_str());
            continue;
         }
         dtCore::RefPtr<ActorProperty> property = propContainer->GetProperty(propertyName);
         if (!property.valid())
         {
            property = propContainer->GetDeprecatedProperty(propertyName);
         }
         if (!property.valid())
         {
            mParser->mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__,
               "Property Container was defined to have actor property %s set with actor %s, but the property does not exist on the proxy.",
               propertyName.c_str(), propValueId.ToString().c_str());
            continue;
         }
         ActorActorProperty* aap = dynamic_cast<ActorActorProperty*>(property.get());
         if (aap == NULL)
         {
            mParser->mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__,
               "Property Container was defined to have actor property %s set with actor %s, but the property is not an ActorActorProperty.",
               propertyName.c_str(), propValueId.ToString().c_str());
            continue;
         }
         aap->SetValue(valueProxy);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::WriteString(const std::string& str) const
   {
      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_STRING_ELEMENT);
      //mWriter->AddCharacters("<![CDATA[" + str + "]]>");
      mWriter->AddCharacters(str);
      mWriter->EndElement();
   }
}

//////////////////////////////////////////////////////////////////////////
