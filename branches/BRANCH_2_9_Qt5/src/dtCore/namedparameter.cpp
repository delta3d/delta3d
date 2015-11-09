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
 * @author David Guthrie
 */

#include <prefix/dtcoreprefix.h>
#include <dtCore/namedparameter.h>

#include <dtCore/actorproperty.h>
#include <dtCore/datatype.h>


#include <dtCore/namedactorparameter.h>
#include <dtCore/namedarrayparameter.h>
#include <dtCore/namedbooleanparameter.h>
#include <dtCore/namedcontainerparameter.h>
#include <dtCore/namedcontainerselectorparameter.h>
#include <dtCore/nameddoubleparameter.h>
#include <dtCore/namedenumparameter.h>
#include <dtCore/namedfloatparameter.h>
#include <dtCore/namedgameeventparameter.h>
#include <dtCore/namedgenericparameter.h>
#include <dtCore/namedintparameter.h>
#include <dtCore/namedlongintparameter.h>
#include <dtCore/namedpodparameter.h>
#include <dtCore/namedshortintparameter.h>
#include <dtCore/namedunsignedcharparameter.h>
#include <dtCore/namedunsignedintparameter.h>
#include <dtCore/namedunsignedlongintparameter.h>
#include <dtCore/namedunsignedshortintparameter.h>
#include <dtCore/namedresourceparameter.h>
#include <dtCore/namedrgbcolorparameter.h>
#include <dtCore/namedrgbacolorparameter.h>
#include <dtCore/namedstringparameter.h>
#include <dtCore/namedgroupparameter.h>
#include <dtCore/namedpropertycontainerparameter.h>
#include <dtCore/namedvectorparameters.h>
#include <dtCore/namedbitmaskparameter.h>

#include <sstream>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   const char NamedParameter::DEFAULT_DELIMETER = '|';

   ///////////////////////////////////////////////////////////////////////////////
   NamedParameter::NamedParameter(DataType& dataType, const dtUtil::RefString& name, bool isList):
      AbstractParameter(dataType, name),
      mIsList(isList)
   {
      SetParamDelimeter(DEFAULT_DELIMETER);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedParameter::IsList() const
   {
      return mIsList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   char NamedParameter::GetParamDelimeter() const
   {
      return mParamListDelimeter;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::SetParamDelimeter(char delim)
   {
      mParamListDelimeter = delim;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      throw dtCore::InvalidParameterException(
         "Message parameter[" + GetName() + "] of type[" + GetDataType().GetName() +
         "] does not have an associated actor property type in SetFromProperty()", __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      throw dtCore::InvalidParameterException(
         "Message parameter[" + GetName() + "] of type[" + GetDataType().GetName() +
         "] does not have an associated actor property type in ApplyValueToProperty()", __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         return ToString() == toCompare.ToString();
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::ValidatePropertyType(const dtCore::ActorProperty& property) const
   {
      if (property.GetDataType() != GetDataType())
      {
         throw dtCore::InvalidParameterException(
            "Actor Property [" + property.GetName() + "] with Data Type [" + property.GetDataType().GetName() +
            "] does not match the Message Parameter [" + GetName() +
            "] with Data Type [" + GetDataType().GetName() + "]", __FILE__, __LINE__);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::WriteToLog(dtUtil::Log &logger, dtUtil::Log::LogMessageType level) const
   {
      if (logger.IsLevelEnabled(level))
      {
         std::ostringstream oss("");
         oss << "Message Parameter is: \"" << GetName() << ".\" ";
         oss << "Its message type is: \""  << GetDataType() << ".\" ";
         oss << "Its value is: \"" << ToString() << ".\"";

         logger.LogMessage(level, __FUNCTION__, __LINE__, oss.str().c_str());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<NamedParameter> NamedParameter::CreateFromType(
      dtCore::DataType& type, const dtUtil::RefString& name, bool isList)
   {
      dtCore::RefPtr<NamedParameter> param;

      switch (type.GetTypeId())
      {
      case dtCore::DataType::CHAR_ID:
      case dtCore::DataType::UCHAR_ID:
         param = new NamedUnsignedCharParameter(name,0,isList);
         break;
      case dtCore::DataType::FLOAT_ID:
         param = new NamedFloatParameter(name,0.0f,isList);
         break;
      case dtCore::DataType::DOUBLE_ID:
         param = new NamedDoubleParameter(name,0.0,isList);
         break;
      case dtCore::DataType::INT_ID:
         param = new NamedIntParameter(name,0,isList);
         break;
      case dtCore::DataType::UINT_ID:
         param = new NamedUnsignedIntParameter(name,0,isList);
         break;
      case dtCore::DataType::ULONGINT_ID:
         param = new NamedUnsignedLongIntParameter(name,0,isList);
         break;
      case dtCore::DataType::LONGINT_ID:
         param = new NamedLongIntParameter(name,0,isList);
         break;
      case dtCore::DataType::SHORTINT_ID:
         param = new NamedShortIntParameter(name,0,isList);
         break;
      case dtCore::DataType::USHORTINT_ID:
         param = new NamedUnsignedShortIntParameter(name,0,isList);
         break;
      case dtCore::DataType::STRING_ID:
         param = new NamedStringParameter(name,"",isList);
         break;
      case dtCore::DataType::BOOLEAN_ID:
         param = new NamedBooleanParameter(name,false,isList);
         break;
      case dtCore::DataType::VEC2_ID:
         param = new NamedVec2Parameter(name,osg::Vec2(0,0),isList);
         break;
      case dtCore::DataType::VEC3_ID:
         param = new NamedVec3Parameter(name,osg::Vec3(0,0,0),isList);
         break;
      case dtCore::DataType::VEC4_ID:
         param = new NamedVec4Parameter(name,osg::Vec4(0,0,0,0),isList);
         break;
      case dtCore::DataType::VEC2F_ID:
         param = new NamedVec2fParameter(name,osg::Vec2f(0,0),isList);
         break;
      case dtCore::DataType::VEC3F_ID:
         param = new NamedVec3fParameter(name,osg::Vec3f(0,0,0),isList);
         break;
      case dtCore::DataType::VEC4F_ID:
         param = new NamedVec4fParameter(name,osg::Vec4f(0,0,0,0),isList);
         break;
      case dtCore::DataType::VEC2D_ID:
         param = new NamedVec2dParameter(name,osg::Vec2d(0,0),isList);
         break;
      case dtCore::DataType::VEC3D_ID:
         param = new NamedVec3dParameter(name,osg::Vec3d(0,0,0),isList);
         break;
      case dtCore::DataType::VEC4D_ID:
         param = new NamedVec4dParameter(name,osg::Vec4d(0,0,0,0),isList);
         break;
      case dtCore::DataType::RGBCOLOR_ID:
         param = new NamedRGBColorParameter(name,osg::Vec3(0,0,0),isList);
         break;
      case dtCore::DataType::RGBACOLOR_ID:
         param = new NamedRGBAColorParameter(name,osg::Vec4(0,0,0,0),isList);
         break;
      case dtCore::DataType::ENUMERATION_ID:
         param = new NamedEnumParameter(name,"",isList);
         break;
      case dtCore::DataType::GAMEEVENT_ID:
         param = new NamedGameEventParameter(name,dtCore::UniqueId(""),isList);
         break;
      case dtCore::DataType::ACTOR_ID:
         param = new NamedActorParameter(name,dtCore::UniqueId(""),isList);
         break;
      case dtCore::DataType::GROUP_ID:
         param = new NamedGroupParameter(name);
         break;
      case dtCore::DataType::PROPERTY_CONTAINER_ID:
         param = new NamedPropertyContainerParameter(name);
         break;
      case dtCore::DataType::ARRAY_ID:
         param = new NamedArrayParameter(name);
         break;
      case dtCore::DataType::CONTAINER_ID:
         param = new NamedContainerParameter(name);
         break;
      case dtCore::DataType::CONTAINER_SELECTOR_ID:
         param = new NamedContainerSelectorParameter(name);
         break;
      case dtCore::DataType::BIT_MASK_ID:
         param = new NamedBitMaskParameter(name,0,isList);
         break;
      default:
         if (type.IsResource())
         {
            param = new NamedResourceParameter(type,name,isList);
         }
         else
         {
            throw dtCore::InvalidParameterException( "Type " + type.GetName() + " is not supported by the MessageParameter class.", __FILE__, __LINE__);
         }
         break;
      }

      return param;
   }
}
