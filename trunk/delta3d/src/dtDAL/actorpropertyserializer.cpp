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

#include <prefix/dtdalprefix.h>
#include <dtDAL/actorpropertyserializer.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtDAL/actoractorproperty.h>
#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actortype.h>
#include <dtDAL/arrayactorpropertybase.h>
#include <dtDAL/colorrgbaactorproperty.h>
#include <dtDAL/containeractorproperty.h>
#include <dtDAL/propertycontaineractorproperty.h>
#include <dtDAL/datatype.h>

#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventactorproperty.h>
#include <dtDAL/gameeventmanager.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/mapcontenthandler.h>
#include <dtDAL/mapxmlconstants.h>
#include <dtDAL/namedactorparameter.h>
#include <dtDAL/namedgameeventparameter.h>
#include <dtDAL/namedresourceparameter.h>
#include <dtDAL/namedrgbacolorparameter.h>
#include <dtDAL/namedvectorparameters.h>
#include <dtDAL/project.h>
#include <dtDAL/propertycontainer.h>
#include <dtDAL/resourceactorproperty.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/vectoractorproperties.h>

#include <dtUtil/xercesutils.h>
#include <dtUtil/mathdefines.h>

#include <osgDB/FileNameUtils>
#include <typeinfo> //for std:bad_cast

XERCES_CPP_NAMESPACE_USE

namespace dtDAL
{
   static const std::string logName("actorpropertyserializer.cpp");

   //////////////////////////////////////////////////////////////////////////
   SerializerRuntimeData::SerializerRuntimeData()
   : mActorPropertyType(NULL)
   , mParameterTypeToCreate(NULL)
   , mHasDeprecatedProperty(false)
   , mInActorProperty(false)
   , mInGroupProperty(false)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   ActorPropertySerializer::ActorPropertySerializer(BaseXMLWriter* writer)
   : mWriter(writer)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   ActorPropertySerializer::ActorPropertySerializer(BaseXMLHandler* parser)
   : mParser(parser)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   ActorPropertySerializer::~ActorPropertySerializer()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::SetMap(Map* map)
   {
      mMap = map;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::SetCurrentPropertyContainer(dtDAL::PropertyContainer* pc)
   {
      Top().mPropertyContainer = pc;
      Top().ClearParameterValues();
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
      mData.push(dtDAL::SerializerRuntimeData());
      mData.top().Reset();
   }

   /////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::WriteProperty(const ActorProperty& property)
   {
      if (mWriter == NULL || property.IsReadOnly())
      {
         return;
      }

      // Skip properties that have not been changed from their default values.
      dtDAL::SerializerRuntimeData& data = Top();
      if (data.mPropertyContainer && data.mPropertyContainer->IsPropertyDefault(property))
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
               data.mInArrayProperty++;
            }
            else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT) == 0)
            {
               data.mInContainerProperty++;
            }
            else if (XMLString::compareString(localname,
               MapXMLConstants::ACTOR_PROPERTY_PROPERTY_CONTAINER_ELEMENT) == 0)
            {
               EnterPropertyContainer();
            }
         }

         return true;
      }
      else if (XMLString::compareString(localname,
         MapXMLConstants::ACTOR_PROPERTY_ELEMENT) == 0)
      {
         data.mInActorProperty = true;
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ActorPropertySerializer::ElementEnded(const XMLCh* const localname)
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty, can't end and element.");
         return false;
      }

      SerializerRuntimeData& data = Top();

      if (data.mInArrayProperty == 0 && data.mInContainerProperty == 0 && XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_ELEMENT) == 0)
      {
         EndActorPropertyElement();
         return true;
      }
      else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_ARRAY_ELEMENT) == 0)
      {
         data.mInArrayProperty--;
         if (data.mInArrayProperty < 0) data.mInArrayProperty = 0;
         return true;
      }
      else if (XMLString::compareString(localname, MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT) == 0)
      {
         data.mInContainerProperty--;
         if (data.mInContainerProperty < 0)
         {
            data.mInContainerProperty = 0;
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
            LOG_ERROR("Data stack is empty when completing a property container element.  This stack should never be empty.");
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
            dtDAL::DataType* d = ParsePropertyType(localname, false);
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
         if (data.mActorProperty != NULL)
         {
            if (data.mActorPropertyType != NULL && data.mInArrayProperty == 0 && data.mInContainerProperty == 0)
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
                  data.mActorPropertyType = NULL;
                  return true;
               }
            }
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ActorPropertySerializer::Characters(BaseXMLHandler::xmlCharString& topEl, const XMLCh* const chars)
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty, can't end an element.");
         return false;
      }

      SerializerRuntimeData& data = Top();

      if (data.mInActorProperty)
      {
         if (data.mInGroupProperty)
         {
            ParameterCharacters(topEl, chars);
         }
         else
         {
            // Make sure we don't try and change the current property if we are loading properties from an array.
            if (data.mInArrayProperty == 0 && data.mInContainerProperty == 0 && topEl == MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT)
            {
               std::string propName = dtUtil::XMLStringConverter(chars).ToString();
               data.mActorProperty = data.mPropertyContainer->GetProperty(propName);

               // If the property was not found, attempt to get a temporary one instead.
               if (!data.mActorProperty.valid())
               {
                  data.mActorProperty = data.mPropertyContainer->GetDeprecatedProperty(propName);
                  if (data.mActorProperty.valid())
                  {
                     data.mHasDeprecatedProperty = true;
                  }
               }
            }
            else if (data.mActorProperty != NULL)
            {
               // Make sure we don't try and change the current property if we are loading properties from an array.
               if (data.mInArrayProperty == 0 && data.mInContainerProperty == 0 && topEl == MapXMLConstants::ACTOR_PROPERTY_RESOURCE_TYPE_ELEMENT)
               {
                  std::string resourceTypeString = dtUtil::XMLStringConverter(chars).ToString();
                  data.mActorPropertyType = static_cast<DataType*>(DataType::GetValueForName(resourceTypeString));

                  if (data.mActorPropertyType == NULL)
                  {
                     mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                        "No resource type found for type specified in xml \"%s.\"",
                        resourceTypeString.c_str());
                  }
               }
               else if (data.mActorPropertyType != NULL)
               {
                  std::string dataValue = dtUtil::XMLStringConverter(chars).ToString();

                  if (mParser->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  {
                     mParser->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                        "Setting value of property %s, property type %s, datatype %s, value %s, element name %s.",
                        data.mActorProperty->GetName().c_str(),
                        data.mActorProperty->GetDataType().GetName().c_str(),
                        data.mActorPropertyType->GetName().c_str(),
                        dataValue.c_str(), dtUtil::XMLStringConverter(topEl.c_str()).c_str());
                  }

                  //we now have the property, the type, and the data.
                  ParsePropertyData(topEl, dataValue, data.mActorPropertyType, data.mActorProperty.get());
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
      switch (VecType::num_components) {
      case 2:
         mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC2_ELEMENT);
         break;
      case 3:
         mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_VEC3_ELEMENT);
         break;
      case 4:
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
   void ActorPropertySerializer::WriteContainer(const ContainerActorProperty& arrayProp, char* numberConversionBuffer, size_t bufferMax)
   {
      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT);

      // Save out the data for each index.
      for (int index = 0; index < arrayProp.GetPropertyCount(); index++)
      {
         // Write the data for the current property.
         WriteProperty(*arrayProp.GetProperty(index));
      }

      mWriter->EndElement();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::WritePropertyContainer(const BasePropertyContainerActorProperty& propConProp)
   {
      mWriter->BeginElement(MapXMLConstants::ACTOR_PROPERTY_PROPERTY_CONTAINER_ELEMENT);

      const dtDAL::PropertyContainer* propCon = propConProp.GetValue();
      // TODO What to write if NULL?
      if (propCon != NULL)
      {
         std::vector<const dtDAL::ActorProperty*> properties;
         propCon->GetPropertyList(properties);

         std::vector<const dtDAL::ActorProperty*>::const_iterator i, iend;
         i = properties.begin();
         iend = properties.end();
         // Save out the data for each index.
         for (; i != iend; ++i)
         {
            // Write the data for the current property.
            WriteProperty(**i);
         }
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
      for (std::multimap<PropertyContainer*, std::pair<std::string, dtCore::RefPtr<dtDAL::NamedGroupParameter> > >::iterator i
         = mGroupParameters.begin();
         i != mGroupParameters.end(); ++i)
      {
         PropertyContainer* propContainer = i->first;
         if (propContainer == NULL)
         {
            continue;
         }
         std::pair<std::string, dtCore::RefPtr<dtDAL::NamedGroupParameter> >& param = i->second;
         std::string& propertyName = param.first;
         dtCore::RefPtr<dtDAL::NamedGroupParameter>& propValue = param.second;

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
         mGroupParameters.insert(std::make_pair(data.mPropertyContainer,
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
   void ActorPropertySerializer::ParsePropertyData(BaseXMLHandler::xmlCharString& topEl, std::string& dataValue, dtDAL::DataType*& dataType, dtDAL::ActorProperty* actorProperty)
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
            Vec2ActorProperty& p = static_cast<Vec2ActorProperty&>(*actorProperty);
            osg::Vec2 vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC2F_ID:
         {
            Vec2fActorProperty& p = static_cast<Vec2fActorProperty&>(*actorProperty);
            osg::Vec2f vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC2D_ID:
         {
            Vec2dActorProperty& p = static_cast<Vec2dActorProperty&>(*actorProperty);
            osg::Vec2d vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC3_ID:
         {
            Vec3ActorProperty& p = static_cast<Vec3ActorProperty&>(*actorProperty);
            osg::Vec3 vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC3F_ID:
         {
            Vec3fActorProperty& p = static_cast<Vec3fActorProperty&>(*actorProperty);
            osg::Vec3f vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC3D_ID:
         {
            Vec3dActorProperty& p = static_cast<Vec3dActorProperty&>(*actorProperty);
            osg::Vec3d vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC4_ID:
         {
            Vec4ActorProperty& p = static_cast<Vec4ActorProperty&>(*actorProperty);
            osg::Vec4 vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC4F_ID:
         {
            Vec4fActorProperty& p = static_cast<Vec4fActorProperty&>(*actorProperty);
            osg::Vec4f vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC4D_ID:
         {
            Vec4dActorProperty& p = static_cast<Vec4dActorProperty&>(*actorProperty);
            osg::Vec4d vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
      case DataType::RGBACOLOR_ID:
         {
            ColorRgbaActorProperty& p = static_cast<ColorRgbaActorProperty&>(*actorProperty);
            osg::Vec4 vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 4);
            p.SetValue(vec);
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
                  mActorLinking.insert(std::make_pair(data.mPropertyContainer, std::make_pair(actorProperty->GetName(), dtCore::UniqueId(dataValue))));
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
            ContainerActorProperty& arrayProp = static_cast<ContainerActorProperty&>(*actorProperty);
            ParseContainer(topEl, dataValue, &arrayProp);
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
                     p.SetValue(dtDAL::ResourceDescriptor::NULL_RESOURCE);
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
                     p.SetValue(dtDAL::ResourceDescriptor::NULL_RESOURCE);
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
            NamedVec2Parameter& p = static_cast<NamedVec2Parameter&>(np);
            osg::Vec2 vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC2F_ID:
         {
            NamedVec2fParameter& p = static_cast<NamedVec2fParameter&>(np);
            osg::Vec2f vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC2D_ID:
         {
            NamedVec2dParameter& p = static_cast<NamedVec2dParameter&>(np);
            osg::Vec2d vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 2);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC3_ID:
         {
            NamedVec3Parameter& p = static_cast<NamedVec3Parameter&>(np);
            osg::Vec3 vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC3F_ID:
         {
            NamedVec3fParameter& p = static_cast<NamedVec3fParameter&>(np);
            osg::Vec3f vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC3D_ID:
         {
            NamedVec3dParameter& p = static_cast<NamedVec3dParameter&>(np);
            osg::Vec3d vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 3);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC4_ID:
         {
            NamedVec4Parameter& p = static_cast<NamedVec4Parameter&>(np);
            osg::Vec4 vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC4F_ID:
         {
            NamedVec4fParameter& p = static_cast<NamedVec4fParameter&>(np);
            osg::Vec4f vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
      case DataType::VEC4D_ID:
         {
            NamedVec4dParameter& p = static_cast<NamedVec4dParameter&>(np);
            osg::Vec4d vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 4);
            p.SetValue(vec);
            break;
         }
      case DataType::RGBACOLOR_ID:
         {
            NamedRGBAColorParameter& p = static_cast<NamedRGBAColorParameter&>(np);
            osg::Vec4 vec = p.GetValue();
            ParseVec(topEl, dataValue, vec, 4);
            p.SetValue(vec);
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
   template <typename VecType>
   void ActorPropertySerializer::ParseVec(BaseXMLHandler::xmlCharString& topEl, const std::string& dataValue, VecType& vec, size_t vecSize)
   {
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
         {
            return;
         }

         mParser->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Got an invalid element for a Vec%u: %s", unsigned(vecSize), dtUtil::XMLStringConverter(topEl.c_str()).c_str());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::EnterPropertyContainer()
   {
      SerializerRuntimeData& data = Top();

      dtDAL::ActorProperty* actorProperty = data.mActorProperty;
      if (data.mInArrayProperty > 0)
      {
         ArrayActorPropertyBase* arrayActorProp = dynamic_cast<ArrayActorPropertyBase*>(actorProperty);
         if (arrayActorProp != NULL)
         {
            actorProperty = arrayActorProp->GetArrayProperty();
         }
      }
      else if (data.mInContainerProperty > 0)
      {
         ContainerActorProperty* contActorProp = dynamic_cast<ContainerActorProperty*>(actorProperty);
         if (contActorProp != NULL)
         {
            actorProperty = contActorProp->GetCurrentProperty();
         }
      }
      mData.push(dtDAL::SerializerRuntimeData());
      mData.top().Reset();
      BasePropertyContainerActorProperty* pcProp = dynamic_cast<BasePropertyContainerActorProperty*>(actorProperty);
      if (pcProp != NULL)
      {
         dtDAL::PropertyContainer* pc = pcProp->GetValue();
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

      SerializerRuntimeData& data = Top();

      // Find the property we want to edit based on how deep the array is nested.
      for (int nestIndex = 1; nestIndex < data.mInArrayProperty; nestIndex++)
      {
         arrayProp = dynamic_cast<ArrayActorPropertyBase*>(arrayProp->GetArrayProperty());

         if (arrayProp == NULL)
         {
            return;
         }
      }

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
               (propType == &DataType::ARRAY || propType == &DataType::CONTAINER))
            {
               ParsePropertyData(topEl, dataValue, propType, prop);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorPropertySerializer::ParseContainer(BaseXMLHandler::xmlCharString& topEl, std::string& dataValue, ContainerActorProperty* arrayProp)
   {
      if (mData.empty())
      {
         LOG_ERROR("Data stack is empty.");
         return;
      }

      SerializerRuntimeData& data = Top();

      // Find the property we want to edit based on how deep the container is nested.
      for (int nestIndex = 1; nestIndex < data.mInContainerProperty; nestIndex++)
      {
         int index = arrayProp->GetCurrentPropertyIndex();
         arrayProp = static_cast<ContainerActorProperty*>(arrayProp->GetProperty(index));
      }

      if (arrayProp == NULL)
      {
         return;
      }

      // Skipped elements.
      if (topEl == MapXMLConstants::ACTOR_ARRAY_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_ARRAY_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_CONTAINER_ELEMENT ||
         topEl == MapXMLConstants::ACTOR_PROPERTY_ELEMENT)
      {
      }
      else if (topEl == MapXMLConstants::ACTOR_PROPERTY_NAME_ELEMENT)
      {
         arrayProp->SetCurrentPropertyIndex(-1);
         // Find the property in the container with the given name.
         for (int index = 0; index < arrayProp->GetPropertyCount(); index++)
         {
            ActorProperty* prop = arrayProp->GetProperty(index);
            if (prop)
            {
               if (prop->GetName() == dataValue)
               {
                  arrayProp->SetCurrentPropertyIndex(index);
                  break;
               }
            }
         }
      }
      // Unrecognized elements are checked with the array's property type
      else
      {
         int index = arrayProp->GetCurrentPropertyIndex();
         ActorProperty* prop = arrayProp->GetProperty(index);
         if (prop)
         {
            DataType* propType = &prop->GetDataType();
            ParsePropertyData(topEl, dataValue, propType, prop);
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
      // this preemptively set this string to "" so that
      // empty data will work.
      if (data.mActorPropertyType != NULL && !data.mActorProperty->IsReadOnly())
      {
         if (*data.mActorPropertyType == DataType::STRING
            || *data.mActorPropertyType == DataType::ACTOR)
         {
            data.mActorProperty->FromString("");
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
   bool ActorPropertySerializer::IsPropertyCorrectType(dtDAL::DataType*& dataType, dtDAL::ActorProperty* actorProperty)
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

         mParser->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Parameter/Property '%s' was saved as type %s, but is now of type %s. Data will be ignored",
            actorProperty->GetName().c_str(),
            (dataType)->GetName().c_str(), actorProperty->GetDataType().GetName().c_str());
      }
      return result;
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

      for (std::multimap<PropertyContainer*, std::pair<std::string, dtCore::UniqueId> >::iterator i = mActorLinking.begin();
         i != mActorLinking.end(); ++i)
      {
         PropertyContainer* propContainer = i->first;
         if (propContainer == NULL)
         {
            continue;
         }
         std::pair<std::string, dtCore::UniqueId>& data = i->second;
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
