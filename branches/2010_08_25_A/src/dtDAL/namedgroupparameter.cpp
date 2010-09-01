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
#include <dtDAL/namedgroupparameter.h>

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
#include <dtDAL/namedvectorparameters.h>

#include <dtDAL/groupactorproperty.h>

namespace dtDAL
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedGroupParameter::NamedGroupParameter(const dtUtil::RefString& name) :
      NamedParameter(dtDAL::DataType::GROUP, name, false)
   {}

   void NamedGroupParameter::ToDataStream(dtUtil::DataStream& stream) const
   {
      // Write out the size of the list so we know how many times to loop in FromDataStream
      stream << (unsigned int)mParameterList.size();

      NamedGroupParameter::ParameterList::const_iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
      for (; i != end; ++i)
      {
         stream << i->second->GetDataType().GetTypeId();
         stream << i->second->GetName();
         stream << i->second->IsList();
         i->second->ToDataStream(stream);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGroupParameter::FromDataStream(dtUtil::DataStream& stream)
   {
      bool okay = true;

      // Read in the size of the stream
      unsigned int size;
      stream >> size;

      for (unsigned short int i = 0; i < size; i++)
      {
         unsigned char id;
         stream >> id;
         dtDAL::DataType* type = NULL;

         for (unsigned int j = 0; j < dtDAL::DataType::EnumerateType().size(); j++)
         {
            dtDAL::DataType* d = dtDAL::DataType::EnumerateType()[j];
            if (d->GetTypeId() == id)
            {
               type = d;
               break;
            }
         }
         if (type == NULL) //|| type == &dtDAL::DataType::UNKNOWN)
         {
            throw dtDAL::BaseException( "The datatype was not found in the stream", __FILE__, __LINE__);
            okay = false;
         }

         std::string name;
         stream >> name;

         bool isList;
         stream >> isList;

         dtCore::RefPtr<NamedParameter> param = GetParameter(name);
         if (param == NULL)
         {
            if (isList)
            {
               param = CreateFromType(*type, name, true);
               AddParameter(*param);
            }
            else
            {
               param = AddParameter(name, *type);
            }
         }

         okay = okay && param->FromDataStream(stream);
      }

      return okay;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedGroupParameter::ToString() const
   {
      std::string toFill;
      NamedGroupParameter::ParameterList::const_iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
      for (; i!= end; ++i)
      {
         NamedParameter& param = *i->second;
         toFill.append(param.GetName());
         toFill.append(" ");
         toFill.append(dtUtil::ToString(param.GetDataType().GetName()));
         toFill.append(" ");
         // output this boolean as "true" or "false" in the string
         bool isList = param.IsList();
         if (isList)
            toFill.append("true");
         else
            toFill.append("false");

         toFill.append(" ");
         toFill.append(param.ToString());
         toFill.append(1, '\n');
      }
      return toFill;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGroupParameter::FromString(const std::string& value)
   {
      std::istringstream iss(value);

      std::string paramName;
      std::string typeString;
      std::string isListString;
      std::string paramValue;

      // get values
      std::getline(iss, paramName, ' ');
      std::getline(iss, typeString, ' ');
      std::getline(iss, isListString, ' ');
      std::getline(iss, paramValue);

      dtDAL::DataType *type = dtDAL::DataType::GetValueForName(typeString);

      if (type == NULL)
         return false;

      // isList
      bool isList = isListString == "true";

      // try and retrieve the parameter
      dtCore::RefPtr<NamedParameter> param = GetParameter(paramName);

      if (param == NULL)
      { // add it if it does not exist
         if (isList)
         {
            param = CreateFromType(*type, paramName, true);
            AddParameter(*param);
         }
         else
         {
            param = AddParameter(paramName, *type);
         }
      }

      param->FromString(paramValue);
      return true;

   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::CopyFrom(const NamedParameter& otherParam)
   {
      if (otherParam.GetDataType() != GetDataType())
         throw dtDAL::InvalidParameterException(
         "The msg parameter must be of type GROUP.", __FILE__, __LINE__);

      const NamedGroupParameter& gpm = static_cast<const NamedGroupParameter&>(otherParam);

      //wipe out any existing parameters.  It's easier to just recreate them.
      mParameterList.clear();

      //copy parameters
      NamedGroupParameter::ParameterList::const_iterator i = gpm.mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = gpm.mParameterList.end();
      for (; i != end; ++i)
      {
         dtCore::RefPtr<NamedParameter> newParameter = NULL;
         if (i->second->IsList())
         {
            newParameter = CreateFromType(i->second->GetDataType(), i->first, true);
            AddParameter(*newParameter);
         }
         else
         {
            newParameter = AddParameter(i->first, i->second->GetDataType());
         }
         if (newParameter == NULL)
            //This case should not happen, the method above should throw an exception if it doesn't work, but
            //this is a case of paranoid programming.
            throw dtDAL::BaseException(
            "Unable to create parameter of type " + i->second->GetDataType().GetName(),
            __FILE__, __LINE__);

         newParameter->CopyFrom(*i->second);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedParameter* NamedGroupParameter::AddParameter(const dtUtil::RefString& name,
      dtDAL::DataType& type)
   {
      dtCore::RefPtr<NamedParameter> param = CreateFromType(type, name);
      if (param.valid())
      {
         AddParameter(*param);
         return param.get();
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<NamedParameter> NamedGroupParameter::RemoveParameter(const dtUtil::RefString& name)
   {
      NamedGroupParameter::ParameterList::iterator itor = mParameterList.find(name);
      if (itor != mParameterList.end())
      {
         dtCore::RefPtr<NamedParameter> param = itor->second;
         mParameterList.erase(itor);
         return param;
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::AddParameter(NamedParameter& newParam)
   {
      if (!mParameterList.insert(std::make_pair(newParam.GetName(), &newParam)).second)
      {
         throw dtDAL::InvalidParameterException(
         "Could not add new parameter: "+ newParam.GetName() +
         ". A parameter with that name already exists.", __FILE__, __LINE__);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedParameter* NamedGroupParameter::GetParameter(const dtUtil::RefString& name)
   {
      NamedGroupParameter::ParameterList::iterator itor = mParameterList.find(name);
      return itor == mParameterList.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const NamedParameter* NamedGroupParameter::GetParameter(const dtUtil::RefString& name) const
   {
      NamedGroupParameter::ParameterList::const_iterator itor = mParameterList.find(name);
      return itor == mParameterList.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::GetParameters(std::vector<NamedParameter*>& toFill)
   {
      toFill.clear();
      toFill.reserve(mParameterList.size());
      NamedGroupParameter::ParameterList::iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::iterator end = mParameterList.end();
      for (;i != end; ++i)
         toFill.push_back(i->second.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::GetParameters(std::vector<const NamedParameter*>& toFill) const
   {
      toFill.clear();
      toFill.reserve(mParameterList.size());
      NamedGroupParameter::ParameterList::const_iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
      for (;i != end; ++i)
         toFill.push_back(i->second.get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int NamedGroupParameter::GetParameterCount() const
   {
      return mParameterList.size();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::GroupActorProperty *gap = static_cast<const dtDAL::GroupActorProperty*>(&property);
      if (gap->GetValue().valid())
      {
         CopyFrom(*gap->GetValue());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::GroupActorProperty *gap = static_cast<dtDAL::GroupActorProperty*>(&property);
      gap->SetValue(*this);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGroupParameter::operator==(const NamedParameter& toCompare) const
   {
      if (GetDataType() == toCompare.GetDataType())
      {
         const NamedGroupParameter& groupToCompare = static_cast<const NamedGroupParameter&>(toCompare);
         //if the size doesn't match. it's not equal.
         if (mParameterList.size() != groupToCompare.mParameterList.size())
            return false;
         NamedGroupParameter::ParameterList::const_iterator itor = mParameterList.begin();
         NamedGroupParameter::ParameterList::const_iterator itorComp = groupToCompare.mParameterList.begin();
         NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
         for (; itor != end; ++itor)
         {
            //spin through the props and return false if one is not equal.
            if (*itor->second != *itorComp->second)
               return false;

            ++itorComp;
         }
         //all props are equal.
         return true;
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<NamedParameter> NamedGroupParameter::CreateFromType(
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

   ///////////////////////////////////////////////////////////////////////////////
}
