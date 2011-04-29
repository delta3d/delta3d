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

#include <prefix/dtdalprefix.h>
#include <dtDAL/namedparameter.h>

#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>


#include <dtDAL/namedactorparameter.h>
#include <dtDAL/namedarrayparameter.h>
#include <dtDAL/namedbooleanparameter.h>
#include <dtDAL/namedcontainerparameter.h>
#include <dtDAL/nameddoubleparameter.h>
#include <dtDAL/namedenumparameter.h>
#include <dtDAL/namedfloatparameter.h>
#include <dtDAL/namedgameeventparameter.h>
#include <dtDAL/namedgenericparameter.h>
#include <dtDAL/namedintparameter.h>
#include <dtDAL/namedlongintparameter.h>
#include <dtDAL/namedpodparameter.h>
#include <dtDAL/namedshortintparameter.h>
#include <dtDAL/namedunsignedcharparameter.h>
#include <dtDAL/namedunsignedintparameter.h>
#include <dtDAL/namedunsignedlongintparameter.h>
#include <dtDAL/namedunsignedshortintparameter.h>
#include <dtDAL/namedresourceparameter.h>
#include <dtDAL/namedrgbcolorparameter.h>
#include <dtDAL/namedrgbacolorparameter.h>
#include <dtDAL/namedstringparameter.h>
#include <dtDAL/namedgroupparameter.h>
#include <dtDAL/namedpropertycontainerparameter.h>
#include <dtDAL/namedvectorparameters.h>

#include <sstream>

namespace dtDAL
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
   void NamedParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      throw dtDAL::InvalidParameterException(
         "Message parameter[" + GetName() + "] of type[" + GetDataType().GetName() +
         "] does not have an associated actor property type in SetFromProperty()", __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      throw dtDAL::InvalidParameterException(
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
   void NamedParameter::ValidatePropertyType(const dtDAL::ActorProperty& property) const
   {
      if (property.GetDataType() != GetDataType())
      {
         throw dtDAL::InvalidParameterException(
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
      dtDAL::DataType& type, const dtUtil::RefString& name, bool isList)
   {
      dtCore::RefPtr<NamedParameter> param;

      switch (type.GetTypeId())
      {
      case dtDAL::DataType::CHAR_ID:
      case dtDAL::DataType::UCHAR_ID:
         param = new NamedUnsignedCharParameter(name,0,isList);
         break;
      case dtDAL::DataType::FLOAT_ID:
         param = new NamedFloatParameter(name,0.0f,isList);
         break;
      case dtDAL::DataType::DOUBLE_ID:
         param = new NamedDoubleParameter(name,0.0,isList);
         break;
      case dtDAL::DataType::INT_ID:
         param = new NamedIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::UINT_ID:
         param = new NamedUnsignedIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::ULONGINT_ID:
         param = new NamedUnsignedLongIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::LONGINT_ID:
         param = new NamedLongIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::SHORTINT_ID:
         param = new NamedShortIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::USHORTINT_ID:
         param = new NamedUnsignedShortIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::STRING_ID:
         param = new NamedStringParameter(name,"",isList);
         break;
      case dtDAL::DataType::BOOLEAN_ID:
         param = new NamedBooleanParameter(name,false,isList);
         break;
      case dtDAL::DataType::VEC2_ID:
         param = new NamedVec2Parameter(name,osg::Vec2(0,0),isList);
         break;
      case dtDAL::DataType::VEC3_ID:
         param = new NamedVec3Parameter(name,osg::Vec3(0,0,0),isList);
         break;
      case dtDAL::DataType::VEC4_ID:
         param = new NamedVec4Parameter(name,osg::Vec4(0,0,0,0),isList);
         break;
      case dtDAL::DataType::VEC2F_ID:
         param = new NamedVec2fParameter(name,osg::Vec2f(0,0),isList);
         break;
      case dtDAL::DataType::VEC3F_ID:
         param = new NamedVec3fParameter(name,osg::Vec3f(0,0,0),isList);
         break;
      case dtDAL::DataType::VEC4F_ID:
         param = new NamedVec4fParameter(name,osg::Vec4f(0,0,0,0),isList);
         break;
      case dtDAL::DataType::VEC2D_ID:
         param = new NamedVec2dParameter(name,osg::Vec2d(0,0),isList);
         break;
      case dtDAL::DataType::VEC3D_ID:
         param = new NamedVec3dParameter(name,osg::Vec3d(0,0,0),isList);
         break;
      case dtDAL::DataType::VEC4D_ID:
         param = new NamedVec4dParameter(name,osg::Vec4d(0,0,0,0),isList);
         break;
      case dtDAL::DataType::RGBCOLOR_ID:
         param = new NamedRGBColorParameter(name,osg::Vec3(0,0,0),isList);
         break;
      case dtDAL::DataType::RGBACOLOR_ID:
         param = new NamedRGBAColorParameter(name,osg::Vec4(0,0,0,0),isList);
         break;
      case dtDAL::DataType::ENUMERATION_ID:
         param = new NamedEnumParameter(name,"",isList);
         break;
      case dtDAL::DataType::GAMEEVENT_ID:
         param = new NamedGameEventParameter(name,dtCore::UniqueId(""),isList);
         break;
      case dtDAL::DataType::ACTOR_ID:
         param = new NamedActorParameter(name,dtCore::UniqueId(""),isList);
         break;
      case dtDAL::DataType::GROUP_ID:
         param = new NamedGroupParameter(name);
         break;
      case dtDAL::DataType::PROPERTY_CONTAINER_ID:
         param = new NamedPropertyContainerParameter(name);
         break;
      case dtDAL::DataType::ARRAY_ID:
         param = new NamedArrayParameter(name);
         break;
      case dtDAL::DataType::CONTAINER_ID:
         param = new NamedContainerParameter(name);
         break;
      case dtDAL::DataType::STATICMESH_ID:
      case dtDAL::DataType::TEXTURE_ID:
      case dtDAL::DataType::TERRAIN_ID:
      case dtDAL::DataType::SOUND_ID:
      case dtDAL::DataType::PARTICLESYSTEM_ID:
      case dtDAL::DataType::SKELETAL_MESH_ID:
      case dtDAL::DataType::PREFAB_ID:
      case dtDAL::DataType::SHADER_ID:
      case dtDAL::DataType::DIRECTOR_ID:
         param = new NamedResourceParameter(type,name,isList);
         break;
      default:
         throw dtDAL::InvalidParameterException( "Type " + type.GetName() + " is not supported by the MessageParameter class.", __FILE__, __LINE__);
         break;
      }

      return param;
   }
}
