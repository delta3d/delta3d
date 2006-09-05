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
 */
#include <prefix/dtgameprefix-src.h>
#include <string>
#include "dtGame/exceptionenum.h"
#include "dtGame/messageparameter.h"
#include <dtDAL/enginepropertytypes.h>

namespace dtGame
{


   ///////////////////////////////////////////////////////////////////////////////
   const char MessageParameter::DEFAULT_DELIMETER = '|';

   ///////////////////////////////////////////////////////////////////////////////
   void MessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      throw dtUtil::Exception(dtGame::ExceptionEnum::INVALID_PARAMETER, 
         "Message parameter[" + GetName() + "] of type[" + GetDataType().GetName() +
         "] does not have an associated actor property type in SetFromProperty()", __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      throw dtUtil::Exception(dtGame::ExceptionEnum::INVALID_PARAMETER, 
         "Message parameter[" + GetName() + "] of type[" + GetDataType().GetName() +
         "] does not have an associated actor property type in ApplyValueToProperty()", __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MessageParameter::ValidatePropertyType(const dtDAL::ActorProperty &property) const 
   {
      if (property.GetPropertyType() != GetDataType())
      {
         throw dtUtil::Exception(dtGame::ExceptionEnum::INVALID_PARAMETER, 
            "Actor Property [" + property.GetName() + "] with Data Type [" + property.GetPropertyType().GetName() +
            "] does not match the Message Parameter [" + GetName() + 
            "] with Data Type [" + GetDataType().GetName() + "]", __FILE__, __LINE__);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<MessageParameter> MessageParameter::CreateFromType(
   		const dtDAL::DataType& type, const std::string& name, bool isList)
   {
      dtCore::RefPtr<MessageParameter> param;

      switch (type.GetTypeId()) 
      {
      case dtDAL::DataType::CHAR_ID:
      case dtDAL::DataType::UCHAR_ID:
         param = new UnsignedCharMessageParameter(name,0,isList);
         break;
      case dtDAL::DataType::FLOAT_ID:
         param = new FloatMessageParameter(name,0.0f,isList);
         break;
      case dtDAL::DataType::DOUBLE_ID:
         param = new DoubleMessageParameter(name,0.0,isList);
         break;
      case dtDAL::DataType::INT_ID:
         param = new IntMessageParameter(name,0,isList);
         break;
      case dtDAL::DataType::UINT_ID:
         param = new UnsignedIntMessageParameter(name,0,isList);
         break;
      case dtDAL::DataType::ULONGINT_ID:
         param = new UnsignedLongIntMessageParameter(name,0,isList);
         break;
      case dtDAL::DataType::LONGINT_ID:
         param = new LongIntMessageParameter(name,0,isList);
         break;
      case dtDAL::DataType::SHORTINT_ID:
         param = new ShortIntMessageParameter(name,0,isList);
         break;
      case dtDAL::DataType::USHORTINT_ID:
         param = new UnsignedShortIntMessageParameter(name,0,isList);
         break;
      case dtDAL::DataType::STRING_ID:
         param = new StringMessageParameter(name,"",isList);
         break;
      case dtDAL::DataType::BOOLEAN_ID:
         param = new BooleanMessageParameter(name,false,isList);
         break;
      case dtDAL::DataType::VEC2_ID:
         param = new Vec2MessageParameter(name,osg::Vec2(0,0),isList);
         break;
      case dtDAL::DataType::VEC3_ID:
         param = new Vec3MessageParameter(name,osg::Vec3(0,0,0),isList);
         break;
      case dtDAL::DataType::VEC4_ID:
         param = new Vec4MessageParameter(name,osg::Vec4(0,0,0,0),isList);
         break;
      case dtDAL::DataType::VEC2F_ID:
         param = new Vec2fMessageParameter(name,osg::Vec2f(0,0),isList);
         break;
      case dtDAL::DataType::VEC3F_ID:
         param = new Vec3fMessageParameter(name,osg::Vec3f(0,0,0),isList);
         break;
      case dtDAL::DataType::VEC4F_ID:
         param = new Vec4fMessageParameter(name,osg::Vec4f(0,0,0,0),isList);
         break;
      case dtDAL::DataType::VEC2D_ID:
         param = new Vec2dMessageParameter(name,osg::Vec2d(0,0),isList);
         break;
      case dtDAL::DataType::VEC3D_ID:
         param = new Vec3dMessageParameter(name,osg::Vec3d(0,0,0),isList);
         break;
      case dtDAL::DataType::VEC4D_ID:
         param = new Vec4dMessageParameter(name,osg::Vec4d(0,0,0,0),isList);
         break;
      case dtDAL::DataType::RGBCOLOR_ID:
         param = new RGBColorMessageParameter(name,osg::Vec3(0,0,0),isList);
         break;
      case dtDAL::DataType::RGBACOLOR_ID:
         param = new RGBAColorMessageParameter(name,osg::Vec4(0,0,0,0),isList);
         break;
      case dtDAL::DataType::ENUMERATION_ID:
         param = new EnumMessageParameter(name,"",isList);
         break;
      case dtDAL::DataType::GAMEEVENT_ID:
         param = new GameEventMessageParameter(name,dtCore::UniqueId(""),isList);
         break;
      case dtDAL::DataType::ACTOR_ID:
         param = new ActorMessageParameter(name,dtCore::UniqueId(""),isList);
         break;
      case dtDAL::DataType::GROUP_ID:
         param = new GroupMessageParameter(name);
         break;
      case dtDAL::DataType::STATICMESH_ID:
      case dtDAL::DataType::TEXTURE_ID:
      case dtDAL::DataType::CHARACTER_ID:
      case dtDAL::DataType::TERRAIN_ID:
      case dtDAL::DataType::SOUND_ID:
      case dtDAL::DataType::PARTICLESYSTEM_ID:
         param = new ResourceMessageParameter(type,name,isList);
         break;
      default:
         throw dtUtil::Exception(dtGame::ExceptionEnum::INVALID_PARAMETER, "Type " + type.GetName() + " is not supported by the MessageParameter class.", __FILE__, __LINE__);
         break;
      }

      return param;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MessageParameter::WriteToLog(dtUtil::Log &logger, dtUtil::Log::LogMessageType level) const
   {
      if(logger.IsLevelEnabled(level))
      {
         std::ostringstream oss("");
         oss << "Message Parameter is: \"" << GetName() << ".\" ";
         oss << "Its message type is: \""  << GetDataType() << ".\" ";
         oss << "Its value is: \"" << ToString() << ".\"";
         
         logger.LogMessage(level, __FUNCTION__, __LINE__, oss.str().c_str());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////

   void GroupMessageParameter::ToDataStream(DataStream& stream) const 
   {
      // Write out the size of the list so we know how many times to loop in FromDataStream
      stream << (unsigned int)mParameterList.size();

      for(std::map<std::string,dtCore::RefPtr<MessageParameter> >::const_iterator i = mParameterList.begin(); 
         i != mParameterList.end(); ++i)
      {
         stream << i->second->GetDataType().GetTypeId();
         stream << i->second->GetName();
         i->second->ToDataStream(stream);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////

   void GroupMessageParameter::FromDataStream(DataStream& stream) 
   {
      // Read in the size of the stream
      unsigned int size;
      stream >> size;

      for(unsigned short int i = 0; i < size; i++)
      {
         unsigned char id;
         stream >> id;
         dtDAL::DataType *type = NULL;

         for(unsigned int j = 0; j < dtDAL::DataType::Enumerate().size(); j++)
         {
            if(static_cast<dtDAL::DataType*>(dtDAL::DataType::Enumerate()[j])->GetTypeId() == id)
            {
               type = static_cast<dtDAL::DataType*>(dtDAL::DataType::Enumerate()[j]);
               break;
            }
         }
         if(type == NULL) //|| type == &dtDAL::DataType::UNKNOWN)
            throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, "The datatype was not found in the stream", __FILE__, __LINE__);

         std::string name;
         stream >> name;
         
         dtCore::RefPtr<MessageParameter> param = GetParameter(name);
         if (param == NULL)
            param = AddParameter(name, *type);

         param->FromDataStream(stream);
      }      
   }

   ///////////////////////////////////////////////////////////////////////////////

   const std::string GroupMessageParameter::ToString() const 
   {
      std::string toFill;
      for(std::map<std::string,dtCore::RefPtr<MessageParameter> >::const_iterator i = mParameterList.begin(); 
         i != mParameterList.end(); ++i)
      {
         toFill.append(i->second->ToString());
         toFill.append(1, '\n');
      }
      return toFill;
   }

   ///////////////////////////////////////////////////////////////////////////////

   bool GroupMessageParameter::FromString(const std::string& value) 
   {
      std::istringstream iss(value);
      //this is wrong.
      for(std::map<std::string,dtCore::RefPtr<MessageParameter> >::iterator i = mParameterList.begin(); 
         i != mParameterList.end(); ++i)
      {
         std::string line;
         std::getline(iss, line);
         i->second->FromString(line);
      }
      return true; 
   }

   ///////////////////////////////////////////////////////////////////////////////

   void GroupMessageParameter::CopyFrom(const MessageParameter& otherParam) 
   {
      if (otherParam.GetDataType() != GetDataType())
         throw dtUtil::Exception(ExceptionEnum::INVALID_PARAMETER, 
            "The msg parameter must be of type GROUP.", __FILE__, __LINE__);
      
      const GroupMessageParameter& gpm = static_cast<const GroupMessageParameter&>(otherParam);
      
      //wipe out any existing parameters.  It's easier to just recreate them.
      mParameterList.clear(); 
      
      //copy parameters
      for (std::map<std::string, dtCore::RefPtr<MessageParameter> >::const_iterator i = gpm.mParameterList.begin();
            i != gpm.mParameterList.end(); ++i)
      {
         MessageParameter* newParameter = AddParameter(i->first, i->second->GetDataType());
         if (newParameter == NULL)
            //This case should not happen, the method above should throw an exception if it doesn't work, but
            //this is a case of paranoid programming.
            throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, 
               "Unable to create parameter of type " + i->second->GetDataType().GetName(), __FILE__, __LINE__);
         
         newParameter->CopyFrom(*i->second);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////

   MessageParameter* GroupMessageParameter::AddParameter(const std::string& name, 
                                                         const dtDAL::DataType& type) 
   {
      dtCore::RefPtr<MessageParameter> param = MessageParameter::CreateFromType(type, name);
      if (param.valid())
      {
         AddParameter(*param);
         return param.get();
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////

   void GroupMessageParameter::AddParameter(MessageParameter& newParam) 
   {
      if (!mParameterList.insert(std::make_pair(newParam.GetName(), &newParam)).second)
         throw dtUtil::Exception(dtGame::ExceptionEnum::INVALID_PARAMETER, "Could not add new parameter: " + newParam.GetName() + 
            ". A parameter with that name already exists.", __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////

   MessageParameter* GroupMessageParameter::GetParameter(const std::string& name) 
   {
      std::map<std::string, dtCore::RefPtr<MessageParameter> >::iterator itor = mParameterList.find(name);
      return itor == mParameterList.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////

   const MessageParameter* GroupMessageParameter::GetParameter(const std::string& name) const 
   {
      std::map<std::string, dtCore::RefPtr<MessageParameter> >::const_iterator itor = mParameterList.find(name);
      return itor == mParameterList.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////

   void GroupMessageParameter::GetParameters(std::vector<MessageParameter*>& toFill) 
   {
      toFill.clear();
      toFill.reserve(mParameterList.size());
      for(std::map<std::string, dtCore::RefPtr<MessageParameter> >::iterator itor = mParameterList.begin();
          itor != mParameterList.end(); ++itor)
          toFill.push_back(itor->second.get());
   }

   ///////////////////////////////////////////////////////////////////////////////

   void GroupMessageParameter::GetParameters(std::vector<const MessageParameter*>& toFill) const 
   {
      toFill.clear();
      toFill.reserve(mParameterList.size());
      for(std::map<std::string, dtCore::RefPtr<MessageParameter> >::const_iterator itor = mParameterList.begin();
          itor != mParameterList.end(); ++itor)
          toFill.push_back(itor->second.get());
   }


   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   BooleanMessageParameter::BooleanMessageParameter(const std::string &name, bool defaultValue,
      bool isList) : PODMessageParameter<bool>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   BooleanMessageParameter::~BooleanMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string BooleanMessageParameter::ToString() const
   {
      if (IsList())
      {
         std::ostringstream stream;
         const std::vector<bool> &values = GetValueList();

         if (values[0])
            stream << "true";
         else
            stream << "false";
         for (unsigned int i=1; i<values.size(); i++)
         {
            stream << GetParamDelimeter();
            if (values[i])
               stream << "true";
            else
               stream << "false";
         }

         return stream.str();
      }
      else
      {
         const char* result = GetValue() ? "true" : "false";
         return result;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool BooleanMessageParameter::FromString(const std::string &value)
   {
      if (IsList())
      {
         std::vector<std::string> result;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         GetValueList().clear();
         for (unsigned int i=0; i<result.size(); i++)
         {
            const std::string &str = result[i];
            if (str == "true" || str == "True" || str =="1" || str == "TRUE")
               GetValueList().push_back(true);
            else
               GetValueList().push_back(false);
         }
      }
      else
      {
         bool result = false;
         if (value == "true" || value == "True" || value == "1" || value == "TRUE")
            result = true;
         SetValue(result);
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BooleanMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property)
   {
      ValidatePropertyType(property);

      const dtDAL::BooleanActorProperty *bap = static_cast<const dtDAL::BooleanActorProperty*> (&property);
      SetValue(bap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BooleanMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::BooleanActorProperty *bap = static_cast<dtDAL::BooleanActorProperty*> (&property);
      bap->SetValue(GetValue());
   }


   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   UnsignedCharMessageParameter::UnsignedCharMessageParameter(const std::string &name,
      unsigned char defaultValue, bool isList) : PODMessageParameter<unsigned char>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   UnsignedCharMessageParameter::~UnsignedCharMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   ShortIntMessageParameter::ShortIntMessageParameter(const std::string &name,
      short defaultValue, bool isList) : PODMessageParameter<short>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShortIntMessageParameter::~ShortIntMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   UnsignedShortIntMessageParameter::UnsignedShortIntMessageParameter(const std::string &name,
      unsigned short defaultValue, bool isList) : PODMessageParameter<unsigned short>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   UnsignedShortIntMessageParameter::~UnsignedShortIntMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   UnsignedIntMessageParameter::UnsignedIntMessageParameter(const std::string &name,
      unsigned int defaultValue,	 bool isList)  : PODMessageParameter<unsigned int>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   UnsignedIntMessageParameter::~UnsignedIntMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   IntMessageParameter::IntMessageParameter(const std::string &name, int defaultValue,	bool isList) :
      PODMessageParameter<int>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   IntMessageParameter::~IntMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void IntMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property)
   {
      ValidatePropertyType(property);

      const dtDAL::IntActorProperty *ap = static_cast<const dtDAL::IntActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void IntMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::IntActorProperty *ap = static_cast<dtDAL::IntActorProperty*> (&property);
      ap->SetValue(GetValue());
   }


   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   UnsignedLongIntMessageParameter::UnsignedLongIntMessageParameter(const std::string &name,
      unsigned long defaultValue, bool isList) : PODMessageParameter<unsigned long>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   UnsignedLongIntMessageParameter::~UnsignedLongIntMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   LongIntMessageParameter::LongIntMessageParameter(const std::string &name, long defaultValue,
      bool isList) : PODMessageParameter<long>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   LongIntMessageParameter::~LongIntMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LongIntMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::LongActorProperty *ap = static_cast<const dtDAL::LongActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LongIntMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::LongActorProperty *ap = static_cast<dtDAL::LongActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   FloatMessageParameter::FloatMessageParameter(const std::string &name, float defaultValue,
         bool isList) : PODMessageParameter<float>(name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   FloatMessageParameter::~FloatMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void FloatMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::FloatActorProperty *ap = static_cast<const dtDAL::FloatActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void FloatMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::FloatActorProperty *ap = static_cast<dtDAL::FloatActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   DoubleMessageParameter::DoubleMessageParameter(const std::string &name, double defaultValue,
      bool isList) : PODMessageParameter<double>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   DoubleMessageParameter::~DoubleMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DoubleMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property)
   {
      ValidatePropertyType(property);

      const dtDAL::DoubleActorProperty *ap = static_cast<const dtDAL::DoubleActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DoubleMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::DoubleActorProperty *ap = static_cast<dtDAL::DoubleActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   StringMessageParameter::StringMessageParameter(const std::string& name, const std::string& defaultValue,
      bool isList) : GenericMessageParameter<std::string>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   StringMessageParameter::~StringMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StringMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::StringActorProperty *ap = static_cast<const dtDAL::StringActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StringMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const
   {
      ValidatePropertyType(property);

      dtDAL::StringActorProperty *ap = static_cast<dtDAL::StringActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string StringMessageParameter::ToString() const
   {
      if (IsList())
      {
         const std::vector<std::string> &values = GetValueList();
         std::ostringstream stream;

         if (values[0].empty())
            stream << "null";
         else
            stream << values[0];

         for (unsigned int i=1; i<values.size(); i++)
         {
            if (values[i].empty())
               stream << GetParamDelimeter() << "null";
            else
               stream << GetParamDelimeter() << values[i];
         }

         return stream.str();
      }
      else
      {
         return GetValue();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool StringMessageParameter::FromString(const std::string& value)
   {
      if (IsList())
      {
         std::vector<std::string> result;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         GetValueList().clear();
         for (unsigned int i=0; i<result.size(); i++)
         {
            if (result[i] == "null")
               GetValueList().push_back("");
            else
               GetValueList().push_back(result[i]);
         }
      }
      else
      {
         SetValue(value);
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   EnumMessageParameter::EnumMessageParameter(const std::string& name, const std::string& defaultValue,
      bool isList) : StringMessageParameter(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   EnumMessageParameter::~EnumMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EnumMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property)
   {
      ValidatePropertyType(property);

      SetValue(property.GetStringValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EnumMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      if (!property.SetStringValue(GetValue()))
         LOG_ERROR(("Failed to set the enum value on property \"" + GetName() + "\".").c_str());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   ActorMessageParameter::ActorMessageParameter(const std::string& name,
      const dtCore::UniqueId& defaultValue, bool isList) :
        GenericMessageParameter<dtCore::UniqueId>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorMessageParameter::~ActorMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::ActorActorProperty *ap = static_cast<const dtDAL::ActorActorProperty*> (&property);
      SetValue(ap->GetValue()->GetId());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      throw dtUtil::Exception(dtGame::ExceptionEnum::INVALID_PARAMETER, 
         "Cannot call ApplyValueToProperty()on an ActorMessageParameter.  See the GameActor::ApplyActorUpdate() for an example of how to do this.",
         __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string ActorMessageParameter::ToString() const
   {
      if (IsList())
      {
         std::ostringstream stream;
         const std::vector<dtCore::UniqueId> &values = GetValueList();

         if (values[0].ToString().empty())
            stream << "null";
         else
            stream << values[0].ToString();

         for (unsigned int i=1; i<values.size(); i++)
         {
            if (values[i].ToString().empty())
               stream << GetParamDelimeter() << "null";
            else
               stream << GetParamDelimeter() << values[i].ToString();
         }

         return stream.str();
      }
      else
         return GetValue().ToString();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool ActorMessageParameter::FromString(const std::string& value)
   {
      if (IsList())
      {
         std::vector<std::string> result;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         GetValueList().clear();
         for (unsigned int i=0; i<result.size(); i++)
         {
            if (result[i] == "null")
               GetValueList().push_back(dtCore::UniqueId(""));
            else
               GetValueList().push_back(dtCore::UniqueId(result[i]));
         }
      }
      else
      {
         SetValue(dtCore::UniqueId(value));
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   GameEventMessageParameter::GameEventMessageParameter(const std::string& name,
      const dtCore::UniqueId& defaultValue, bool isList) :
      GenericMessageParameter<dtCore::UniqueId>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameEventMessageParameter::~GameEventMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameEventMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      FromString(property.GetStringValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameEventMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      property.SetStringValue(ToString());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string GameEventMessageParameter::ToString() const
   {
      if (IsList())
      {
         std::ostringstream stream;
         const std::vector<dtCore::UniqueId> &values = GetValueList();

         if (values[0].ToString().empty())
            stream << "null";
         else
            stream << values[0].ToString();

         for (unsigned int i=1; i<values.size(); i++)
         {
            if (values[i].ToString().empty())
               stream << GetParamDelimeter() << "null";
            else
               stream << GetParamDelimeter() << values[i].ToString();
         }

         return stream.str();
      }
      else
         return GetValue().ToString();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool GameEventMessageParameter::FromString(const std::string& value)
   {
      if (IsList())
      {
         std::vector<std::string> result;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         GetValueList().clear();
         for (unsigned int i=0; i<result.size(); i++)
         {
            if (result[i] == "null")
               GetValueList().push_back(dtCore::UniqueId(""));
            else
               GetValueList().push_back(dtCore::UniqueId(result[i]));
         }
      }
      else
      {
         SetValue(dtCore::UniqueId(value));
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   Vec2MessageParameter::Vec2MessageParameter(const std::string &name,
      const osg::Vec2& defaultValue, bool isList) : VecMessageParameter<osg::Vec2>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   Vec2MessageParameter::~Vec2MessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec2MessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::Vec2ActorProperty *ap = static_cast<const dtDAL::Vec2ActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec2MessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::Vec2ActorProperty *ap = static_cast<dtDAL::Vec2ActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   Vec2fMessageParameter::Vec2fMessageParameter(const std::string &name,
      const osg::Vec2f& defaultValue, bool isList) : VecMessageParameter<osg::Vec2f>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   Vec2fMessageParameter::~Vec2fMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec2fMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::Vec2fActorProperty *ap = static_cast<const dtDAL::Vec2fActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec2fMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::Vec2fActorProperty *ap = static_cast<dtDAL::Vec2fActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   Vec2dMessageParameter::Vec2dMessageParameter(const std::string &name,
      const osg::Vec2d& defaultValue, bool isList) : VecMessageParameter<osg::Vec2d>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   Vec2dMessageParameter::~Vec2dMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec2dMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::Vec2dActorProperty *ap = static_cast<const dtDAL::Vec2dActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec2dMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::Vec2dActorProperty *ap = static_cast<dtDAL::Vec2dActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   Vec3MessageParameter::Vec3MessageParameter(const std::string &name,
      const osg::Vec3& defaultValue, bool isList) : VecMessageParameter<osg::Vec3>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   Vec3MessageParameter::~Vec3MessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec3MessageParameter::SetFromProperty(const dtDAL::ActorProperty &property)
   {
      ValidatePropertyType(property);

      const dtDAL::Vec3ActorProperty *ap = static_cast<const dtDAL::Vec3ActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec3MessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::Vec3ActorProperty *ap = static_cast<dtDAL::Vec3ActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   RGBColorMessageParameter::RGBColorMessageParameter(const std::string &name,
      const osg::Vec3& defaultValue, bool isList) : Vec3MessageParameter(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   RGBColorMessageParameter::~RGBColorMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   Vec3fMessageParameter::Vec3fMessageParameter(const std::string &name,
      const osg::Vec3f& defaultValue, bool isList) : VecMessageParameter<osg::Vec3f>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   Vec3fMessageParameter::~Vec3fMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec3fMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::Vec3fActorProperty *ap = static_cast<const dtDAL::Vec3fActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec3fMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::Vec3fActorProperty *ap = static_cast<dtDAL::Vec3fActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   Vec3dMessageParameter::Vec3dMessageParameter(const std::string &name,
      const osg::Vec3d& defaultValue, bool isList) : VecMessageParameter<osg::Vec3d>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   Vec3dMessageParameter::~Vec3dMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec3dMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::Vec3dActorProperty *ap = static_cast<const dtDAL::Vec3dActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec3dMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::Vec3dActorProperty *ap = static_cast<dtDAL::Vec3dActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   Vec4MessageParameter::Vec4MessageParameter(const std::string &name,
      const osg::Vec4& defaultValue, bool isList) : VecMessageParameter<osg::Vec4>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   Vec4MessageParameter::~Vec4MessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec4MessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::Vec4ActorProperty *ap = static_cast<const dtDAL::Vec4ActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec4MessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const
   {
      ValidatePropertyType(property);

      dtDAL::Vec4ActorProperty *ap = static_cast<dtDAL::Vec4ActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   RGBAColorMessageParameter::RGBAColorMessageParameter(const std::string &name,
      const osg::Vec4& defaultValue, bool isList) : Vec4MessageParameter(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   RGBAColorMessageParameter::~RGBAColorMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   Vec4fMessageParameter::Vec4fMessageParameter(const std::string &name,
      const osg::Vec4f& defaultValue, bool isList) : VecMessageParameter<osg::Vec4f>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   Vec4fMessageParameter::~Vec4fMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec4fMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::Vec4fActorProperty *ap = static_cast<const dtDAL::Vec4fActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec4fMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::Vec4fActorProperty *ap = static_cast<dtDAL::Vec4fActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   Vec4dMessageParameter::Vec4dMessageParameter(const std::string &name,
      const osg::Vec4d& defaultValue, bool isList) : VecMessageParameter<osg::Vec4d>(name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   Vec4dMessageParameter::~Vec4dMessageParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec4dMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property)
   {
      ValidatePropertyType(property);

      const dtDAL::Vec4dActorProperty *ap = static_cast<const dtDAL::Vec4dActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Vec4dMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::Vec4dActorProperty *ap = static_cast<dtDAL::Vec4dActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   ResourceMessageParameter::ResourceMessageParameter(const dtDAL::DataType& type,  const std::string &name,
      bool isList) : MessageParameter(name,isList), mDataType(&type)
   {
     if (IsList())
        mValueList = new std::vector<dtDAL::ResourceDescriptor>();
     else
        mValueList = NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceMessageParameter::SetFromProperty(const dtDAL::ActorProperty &property) 
   {
      ValidatePropertyType(property);

      const dtDAL::ResourceActorProperty *ap = static_cast<const dtDAL::ResourceActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceMessageParameter::ApplyValueToProperty(dtDAL::ActorProperty &property) const 
   {
      ValidatePropertyType(property);

      dtDAL::ResourceActorProperty *vap = static_cast<dtDAL::ResourceActorProperty*> (&property);
      if (GetValue() != NULL)
      {
         dtDAL::ResourceDescriptor newValue(*GetValue());
         vap->SetValue(&newValue);
      }
      else 
      {
         vap->SetValue(NULL);
      }

      //dtDAL::ResourceActorProperty *ap = static_cast<dtDAL::ResourceActorProperty*> (&property);
      //ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ResourceMessageParameter::~ResourceMessageParameter()
   {
     delete mValueList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceMessageParameter::ToDataStream(DataStream &stream) const
   {
      if (IsList())
      {
         stream << mValueList->size();
         for (unsigned int i=0; i<mValueList->size(); i++)
         {
            stream << (*mValueList)[i].GetResourceIdentifier();
            stream << (*mValueList)[i].GetDisplayName();
         }
      }
      else
      {
         stream << mDescriptor.GetResourceIdentifier();
         stream << mDescriptor.GetDisplayName();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceMessageParameter::FromDataStream(DataStream &stream)
   {
      std::string resourceId;
      std::string resourceDisplayName;

      if (IsList())
      {
         mValueList->clear();

         unsigned int listSize;
         stream >> listSize;
         for (unsigned int i=0; i<listSize; i++)
         {
            stream >> resourceId;
            stream >> resourceDisplayName;
            mValueList->push_back(dtDAL::ResourceDescriptor(resourceId,resourceDisplayName));
         }
      }
      else
      {
         stream >> resourceId;
         stream >> resourceDisplayName;
         mDescriptor = dtDAL::ResourceDescriptor(resourceId,resourceDisplayName);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string ResourceMessageParameter::ToString() const
   {
      std::ostringstream stream;

      if (IsList())
      {
         const std::vector<dtDAL::ResourceDescriptor> &values =
            GetValueList();

         stream << values[0].GetDisplayName() << "/" << values[0].GetResourceIdentifier();
         for (unsigned int i=1; i<values.size(); i++)
            stream << GetParamDelimeter() << values[i].GetDisplayName() <<
               "/" << values[i].GetResourceIdentifier();
      }
      else
      {
         const dtDAL::ResourceDescriptor* r = GetValue();
         if (r != NULL)
            stream << r->GetDisplayName() << "/" << r->GetResourceIdentifier();
      }

      return stream.str();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool ResourceMessageParameter::FromString(const std::string& value)
   {
      bool result = true;
      if (value.empty() || value == "NULL")
      {
         if (IsList())
            mValueList->clear();
         else
            SetValue(NULL);

         return true;
      }

      std::string displayName;
      std::string identifier;
      dtUtil::StringTokenizer<dtUtil::IsSlash> stok;
      std::vector<std::string> tokens;
      if (IsList())
      {
         std::vector<std::string> result;
         unsigned int i;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         mValueList->clear();
         for (i=0; i<result.size(); i++)
         {
            tokens.clear();

            #if (defined WIN32 || defined __WIN32__)
            stok = stok;
            #endif
            stok.tokenize(tokens,result[i]);
            if (tokens.size() == 2)
            {
               displayName = tokens[0];
               identifier = tokens[1];
            }
            else
            {
               //assume the value is a descriptor and use it for both the
               //data and the display name.
               displayName = tokens[0];
               identifier = tokens[0];
            }

            dtUtil::trim(identifier);
            dtUtil::trim(displayName);
            mValueList->push_back(dtDAL::ResourceDescriptor(displayName, identifier));
         }
      }
      else
      {
         stok.tokenize(tokens,value);
         if (tokens.size() == 2)
         {
            displayName = tokens[0];
            identifier = tokens[1];
         }
         else
         {
            //assume the value is a descriptor and use it for both the
            //data and the display name.
            displayName = tokens[0];
            identifier = tokens[0];
         }

         dtUtil::trim(identifier);
         dtUtil::trim(displayName);

         dtDAL::ResourceDescriptor descriptor(displayName,identifier);
         SetValue(&descriptor);
      }

      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceMessageParameter::CopyFrom(const MessageParameter& otherParam)
   {
      const ResourceMessageParameter *param =
          dynamic_cast<const ResourceMessageParameter*>(&otherParam);


      //First make sure this parameter does not have a list if the
      //other parameter does and vice versa.
      if ((IsList() && !otherParam.IsList()) ||(!IsList() && otherParam.IsList()))
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot assign two parameters with one being a list of values and the other not.",
            __FILE__, __LINE__);

      if (param != NULL)
      {
         if (!IsList())
           SetValue(param->GetValue());
         else
            SetValueList(param->GetValueList());
      }
      else
      {
         if (!FromString(otherParam.ToString()))
            LOGN_ERROR("MessageParameter", "Parameter types are incompatible. Cannot copy the value.");
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceMessageParameter::SetValue(const dtDAL::ResourceDescriptor* descriptor)
   {
      if (IsList())
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot call SetValue() on message parameter with a list of values.", __FILE__, __LINE__);

      mDescriptor = descriptor == NULL ? dtDAL::ResourceDescriptor("","") : *descriptor;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtDAL::ResourceDescriptor* ResourceMessageParameter::GetValue() const
   {
      if (IsList())
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot call GetValue() on message parameter with a list of values.", __FILE__, __LINE__);

      if (mDescriptor.GetResourceIdentifier().empty())
         return NULL;
      else
         return &mDescriptor;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::vector<dtDAL::ResourceDescriptor> &ResourceMessageParameter::GetValueList() const
   {
      if (!IsList())
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot retrieve the parameters value list.  Parameter does not contain a list.", __FILE__, __LINE__);
      return *mValueList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::vector<dtDAL::ResourceDescriptor> &ResourceMessageParameter::GetValueList()
   {
      if (!IsList())
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot retrieve the parameters value list.  Parameter does not contain a list.", __FILE__, __LINE__);
      return *mValueList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceMessageParameter::SetValueList(const std::vector<dtDAL::ResourceDescriptor> &newValues)
   {
      if (!IsList())
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot set a list of new values on a parameter that is not a list.", __FILE__, __LINE__);

      *mValueList = newValues;
   }
}
