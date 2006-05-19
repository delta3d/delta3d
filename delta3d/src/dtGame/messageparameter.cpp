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
#include <string>
#include "dtGame/exceptionenum.h"
#include "dtGame/messageparameter.h"

namespace dtGame {


   ///////////////////////////////////////////////////////////////////////////////
   const char MessageParameter::DEFAULT_DELIMETER = '|';

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<MessageParameter> MessageParameter::CreateFromType(
   		const dtDAL::DataType& type, const std::string& name, bool isList)
		      throw(dtUtil::Exception)
   {
      dtCore::RefPtr<MessageParameter> param;
      if(type == dtDAL::DataType::UCHAR)
      {
         param = new UnsignedCharMessageParameter(name,0,isList);
      }
      else if(type == dtDAL::DataType::SHORTINT)
      {
         param = new ShortIntMessageParameter(name,0,isList);
      }
      else if(type == dtDAL::DataType::USHORTINT)
      {
         param = new UnsignedShortIntMessageParameter(name,0,isList);
      }
      else if(type == dtDAL::DataType::INT)
      {
         param = new IntMessageParameter(name,0,isList);
      }
      else if(type == dtDAL::DataType::UINT)
      {
         param = new UnsignedIntMessageParameter(name,0,isList);
      }
      else if(type == dtDAL::DataType::LONGINT)
      {
         param = new LongIntMessageParameter(name,0,isList);
      }
      else if(type == dtDAL::DataType::ULONGINT)
      {
         param = new UnsignedLongIntMessageParameter(name,0,isList);
      }
      else if(type == dtDAL::DataType::FLOAT)
      {
         param = new FloatMessageParameter(name,0.0f,isList);
      }
      else if(type == dtDAL::DataType::DOUBLE)
      {
         param = new DoubleMessageParameter(name,0.0,isList);
      }
      else if(type == dtDAL::DataType::BOOLEAN)
      {
         param = new BooleanMessageParameter(name,false,isList);
      }
      else if(type == dtDAL::DataType::ENUMERATION)
      {
         param = new EnumMessageParameter(name,"",isList);
      }
      else if(type == dtDAL::DataType::STRING)
      {
         param = new StringMessageParameter(name,"",isList);
      }
      else if(type == dtDAL::DataType::VEC2)
      {
         param = new Vec2MessageParameter(name,osg::Vec2(0,0),isList);
      }
      else if(type == dtDAL::DataType::VEC2F)
      {
         param = new Vec2fMessageParameter(name,osg::Vec2f(0,0),isList);
      }
      else if(type == dtDAL::DataType::VEC2D)
      {
         param = new Vec2dMessageParameter(name,osg::Vec2d(0,0),isList);
      }
      else if(type == dtDAL::DataType::VEC3 || type == dtDAL::DataType::RGBCOLOR)
      {
         param = new Vec3MessageParameter(name,osg::Vec3(0,0,0),isList);
      }
      else if(type == dtDAL::DataType::VEC3F)
      {
         param = new Vec3fMessageParameter(name,osg::Vec3f(0,0,0),isList);
      }
      else if(type == dtDAL::DataType::VEC3D)
      {
         param = new Vec3dMessageParameter(name,osg::Vec3d(0,0,0),isList);
      }
      else if(type == dtDAL::DataType::VEC4 || type == dtDAL::DataType::RGBACOLOR)
      {
         param = new Vec4MessageParameter(name,osg::Vec4(0,0,0,0),isList);
      }
      else if(type == dtDAL::DataType::VEC4F)
      {
         param = new Vec4fMessageParameter(name,osg::Vec4f(0,0,0,0),isList);
      }
      else if(type == dtDAL::DataType::VEC4D)
      {
         param = new Vec4dMessageParameter(name,osg::Vec4d(0,0,0,0),isList);
      }
      else if(type.IsResource())
      {
         param = new ResourceMessageParameter(type,name,isList);
      }
      else if(type == dtDAL::DataType::ACTOR)
      {
         param = new ActorMessageParameter(name,dtCore::UniqueId(""),isList);
      }
      else if (type == dtDAL::DataType::GAME_EVENT)
      {
         param = new GameEventMessageParameter(name,dtCore::UniqueId(""),isList);
      }
      else
      {
         EXCEPT(dtGame::ExceptionEnum::INVALID_PARAMETER, "Type " + type.GetName() + " is not supported by the MessageParameter class.");
      }

      return param;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MessageParameter::WriteToLog(dtUtil::Log &logger, dtUtil::Log::LogMessageType level) const
   {
      if(logger.IsLevelEnabled(level))
      {
         std::ostringstream oss("");
         oss << "Message Parameter is: " << GetName() << " . ";
         oss << "Its message type is: "  << GetDataType() << " . ";
         oss << "Its value is: " << ToString();
         
         logger.LogMessage(level, __FUNCTION__, __LINE__, oss.str().c_str());
      }
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
         EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot assign two parameters with one being a list of values and the other not.");

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
         EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot call SetValue() on message parameter with a list of values.");

      mDescriptor = descriptor == NULL ? dtDAL::ResourceDescriptor("","") : *descriptor;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtDAL::ResourceDescriptor* ResourceMessageParameter::GetValue() const
   {
      if (IsList())
         EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot call GetValue() on message parameter with a list of values.");

      if (mDescriptor.GetResourceIdentifier().empty())
         return NULL;
      else
         return &mDescriptor;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::vector<dtDAL::ResourceDescriptor> &ResourceMessageParameter::GetValueList() const
   {
      if (!IsList())
         EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot retrieve the parameters value list.  Parameter does not contain a list.");
      return *mValueList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::vector<dtDAL::ResourceDescriptor> &ResourceMessageParameter::GetValueList()
   {
      if (!IsList())
         EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot retrieve the parameters value list.  Parameter does not contain a list.");
      return *mValueList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceMessageParameter::SetValueList(const std::vector<dtDAL::ResourceDescriptor> &newValues)
   {
      if (!IsList())
         EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
            "Cannot set a list of new values on a parameter that is not a list.");

      *mValueList = newValues;
   }
}
