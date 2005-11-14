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
#include "dtGame/exceptionenum.h"
#include "dtGame/messageparameter.h"

namespace dtGame {
   
   dtCore::RefPtr<MessageParameter> MessageParameter::CreateFromType(const dtDAL::DataType& type, const std::string& name)
      throw(dtUtil::Exception)
   {
      dtCore::RefPtr<MessageParameter> param;
      if(type == dtDAL::DataType::CHAR)
      {
         param = new UnsignedCharMessageParameter(name);
      }
      else if(type == dtDAL::DataType::UCHAR)
      {
         param = new UnsignedCharMessageParameter(name);
      }
      else if(type == dtDAL::DataType::SHORTINT)
      {
         param = new ShortIntMessageParameter(name);
      }
      else if(type == dtDAL::DataType::USHORTINT)
      {
         param = new UnsignedShortIntMessageParameter(name);
      }
      else if(type == dtDAL::DataType::INT)
      {
         param = new IntMessageParameter(name);
      }
      else if(type == dtDAL::DataType::UINT)
      {
         param = new UnsignedIntMessageParameter(name);
      }
      else if(type == dtDAL::DataType::LONGINT)
      {
         param = new LongIntMessageParameter(name);
      }
      else if(type == dtDAL::DataType::ULONGINT)
      {
         param = new UnsignedLongIntMessageParameter(name);
      }
      else if(type == dtDAL::DataType::FLOAT)
      {
         param = new FloatMessageParameter(name);
      }
      else if(type == dtDAL::DataType::DOUBLE)
      {
         param = new DoubleMessageParameter(name);
      }
      else if(type == dtDAL::DataType::BOOLEAN)
      {
         param = new BooleanMessageParameter(name);
      }
      else if(type == dtDAL::DataType::ENUMERATION)
      {
         param = new EnumMessageParameter(name);
      }
      else if(type == dtDAL::DataType::STRING)
      {
         param = new StringMessageParameter(name);
      }
      else if(type == dtDAL::DataType::FLOAT)
      {
         param = new FloatMessageParameter(name);
      }
      else if(type == dtDAL::DataType::VEC2)
      {
         param = new Vec2MessageParameter(name);
      }
      else if(type == dtDAL::DataType::VEC2F)
      {
         param = new Vec2fMessageParameter(name);
      }
      else if(type == dtDAL::DataType::VEC2D)
      {
         param = new Vec2dMessageParameter(name);
      }
      else if(type == dtDAL::DataType::VEC3 || type == dtDAL::DataType::RGBCOLOR)
      {
         param = new Vec3MessageParameter(name);
      }
      else if(type == dtDAL::DataType::VEC3F)
      {
         param = new Vec3fMessageParameter(name);
      }
      else if(type == dtDAL::DataType::VEC3D)
      {
         param = new Vec3dMessageParameter(name);
      }
      else if(type == dtDAL::DataType::VEC4 || type == dtDAL::DataType::RGBACOLOR)
      {
         param = new Vec4MessageParameter(name);
      }
      else if(type == dtDAL::DataType::VEC4F)
      {
         param = new Vec4fMessageParameter(name);
      }
      else if(type == dtDAL::DataType::VEC4D)
      {
         param = new Vec4dMessageParameter(name);
      }      
      else if(type.IsResource())
      {
         param = new ResourceMessageParameter(type, name);
      }
      else if(type == dtDAL::DataType::ACTOR)
      {
         param = new ActorMessageParameter(name);
      }
      else
      {
         EXCEPT(dtGame::ExceptionEnum::INVALID_PARAMETER, "Type " + type.GetName() + " is not supported by the MessageParameter class.");
      }      
      return param;
   }
   
   void ResourceMessageParameter::ToDataStream(DataStream &stream) const 
   {
      stream.Write(mDescriptor.GetResourceIdentifier());
      stream.Write(mDescriptor.GetDisplayName());
   }
  
   void ResourceMessageParameter::FromDataStream(DataStream &stream)
   {
      std::string resourceId;
      std::string resourceDisplayName;
      stream.Read(resourceId);
      stream.Read(resourceDisplayName);
      mDescriptor = dtDAL::ResourceDescriptor(resourceId, resourceDisplayName);
   }
  
   const std::string ResourceMessageParameter::ToString() const
   {
      const dtDAL::ResourceDescriptor* r = GetValue();
      if (r == NULL)
         return "";
      else
         return r->GetDisplayName() + "/" + r->GetResourceIdentifier();
   }
  
   bool ResourceMessageParameter::FromString(const std::string& value)
   {
        bool result = true;
        if (value.empty() || value == "NULL")
        {
            SetValue(NULL);
        }
        else
        {
            std::vector<std::string> tokens;
            dtUtil::StringTokenizer<dtUtil::IsSlash> stok;

            stok.tokenize(tokens, value);

            if (tokens.size() == 2)
            {
                std::string displayName(tokens[0]);
                std::string identifier(tokens[1]);

                dtUtil::trim(identifier);
                dtUtil::trim(displayName);

                dtDAL::ResourceDescriptor descriptor(displayName, identifier);
                SetValue(&descriptor);
            }
            else
                result = false;
        }

        return result;
   }

   void ResourceMessageParameter::CopyFrom(const MessageParameter& otherParam)
   {
      const ResourceMessageParameter *param =
          dynamic_cast<const ResourceMessageParameter*>(&otherParam);
   
      if (param != NULL)
         SetValue(param->GetValue());
      else
      {
         if (!FromString(otherParam.ToString()))
            LOGN_ERROR("MessageParameter", "Parameter types are incompatible. Cannot copy the value.");
      }      
   }
   
   void ResourceMessageParameter::SetValue(const dtDAL::ResourceDescriptor* descriptor) 
   { 
      mDescriptor = descriptor == NULL ? dtDAL::ResourceDescriptor("","") : *descriptor ; 
   }
      
   const dtDAL::ResourceDescriptor* ResourceMessageParameter::GetValue() const 
   { 
      if (mDescriptor.GetResourceIdentifier().empty())
         return NULL;
      
      return &mDescriptor; 
   }        
   
}
