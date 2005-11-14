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
* @author Matthew W. Campbell and David Guthrie
*/
#ifndef DELTA_MESSAGEPARAMETER
#define DELTA_MESSAGEPARAMETER

#include <string>
#include <sstream>
#include <osg/Referenced>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Vec2f>
#include <osg/Vec3f>
#include <osg/Vec4f>
#include <osg/Vec2d>
#include <osg/Vec3d>
#include <osg/Vec4d>
#include <dtUtil/stringutils.h>
#include <dtUtil/exception.h>
#include <dtCore/refptr.h>
#include <dtDAL/datatype.h>
#include <dtDAL/resourcedescriptor.h>
#include "dtGame/datastream.h"

namespace dtGame
{
   
   /**
    * @class MessageParameter
    */
   class DT_GAMEMANAGER_EXPORT MessageParameter : public osg::Referenced
   {
      public:
      
         MessageParameter(const std::string &name) : mName(name) { }         
        
         const std::string &GetName() const { return mName; }
        
         virtual const dtDAL::DataType &GetDataType() const = 0;
        
         virtual void ToDataStream(DataStream &stream) const = 0;
        
         virtual void FromDataStream(DataStream &stream) = 0;
        
         virtual const std::string ToString() const = 0;
        
         /**
          * @return true if it was able to assign the value based on the string or false if not.
          * @param value the string value to assign the parameter to.
          */
         virtual bool FromString(const std::string &value) = 0;

         /**
          * @param otherParam the message parameter to make this one match.
          */        
         virtual void CopyFrom(const MessageParameter &otherParam) = 0;
         
         /**
          * Creates a message parameter that will hold the given type.
          * @param type the datatype the parameter should hold.
          * @param name the name of the parameter to create.
          * @throw dtUtil::Exception with dtGame::ExceptionEnum::INVALID_PARAMETER if the type is unsupported.
          */
         static dtCore::RefPtr<MessageParameter> CreateFromType(const dtDAL::DataType& type, const std::string& name) throw(dtUtil::Exception);
      protected:
        
         virtual ~MessageParameter() { }
        
      private:
         std::string mName;         
   };

   /**
    * The GenericActorProperty class implements the set and get functionality
    * which defines the primary behavior of the ActorProperty.  The class is
    * templated such that it has a set type and a get type which correspond to
    * the set and get method signatures.
    * @note
    *      The set method signature excepts one parameter of any type and may
    *      or may not return a value.
    *  @note
    *      The get method returns a value of any type and takes no parameters.
    */
   template <class ParamType>
   class GenericMessageParameter : public MessageParameter
   {
      public:
         /**
          * Constructs the actor property.  Note, that functor objects must be
          * specified and match the set and get types of the GenericActorProperty.
          */
         GenericMessageParameter(const std::string &name) : MessageParameter(name) { }         
        
         /**
          * This method allows a generic property of any type to be copied from
          * one to the other.
          * @param otherProp The property to copy from.
          * @note
          *     This method will only allow generic properties of the same
          *     type to be copied.  For example, a FloatActorProperty cannot
          *     be copied to an IntActorProperty.
          */
         virtual void CopyFrom(const MessageParameter &otherParam)
         {
            const GenericMessageParameter<ParamType> *param =
                dynamic_cast<const GenericMessageParameter<ParamType> *>(&otherParam);
         
            if (param != NULL)
               SetValue(param->GetValue());
            else
            {
               if (!FromString(otherParam.ToString()))
                  LOGN_ERROR("MessageParameter", "Parameter types are incompatible. Cannot copy the value.");
            }
         }
            
      
         /**
          * Sets the value of this property by calling the set functor
          * assigned to this property.
          */
         virtual void SetValue(const ParamType& value) { mValue = value; }
      
         /**
          * Gets the value of this property be calling the get functor
          * assigned to this property.
          */
         virtual const ParamType& GetValue() const { return mValue; }        
        
         virtual void ToDataStream(DataStream &stream) const { stream.Write(mValue); };
        
         virtual void FromDataStream(DataStream &stream) { stream.Read(mValue); };         
        
      protected:
         ///Keep destructors protected to ensure property smart pointer management.
         virtual ~GenericMessageParameter() { }
      
      private:
         ParamType mValue;    	
   };

   /**
    * @class PODMessageParameter
    * This is a template for the single piece of data types to make reading data to and from a string easier. 
    */
   template <class ParamType>
   class PODMessageParameter: public GenericMessageParameter<ParamType>
   {
      public:
         PODMessageParameter(const std::string &name) : GenericMessageParameter<ParamType>(name) {}
         virtual const std::string ToString() const 
         {
            std::ostringstream stream;
            stream << GenericMessageParameter<ParamType>::GetValue();
            return stream.str();
            
         }
        
         virtual bool FromString(const std::string &value) 
         {  
            ParamType newValue;
            std::istringstream stream;
            stream.str(value);
            stream >> newValue;
            GenericMessageParameter<ParamType>::SetValue(newValue);
            return true;
         }
      
      protected:
         virtual ~PODMessageParameter() {}   
   };

   /**
    * @class BooleanMessageParameter 
    */
   class BooleanMessageParameter: public PODMessageParameter<bool>
   {
      public:
         BooleanMessageParameter(const std::string &name) : PODMessageParameter<bool>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::BOOLEAN; }
      protected:
         virtual ~BooleanMessageParameter() {}   
   };

   /**
    * @class UnsignedCharMessageParameter 
    */
   class UnsignedCharMessageParameter: public PODMessageParameter<unsigned char>
   {
      public:
         UnsignedCharMessageParameter(const std::string &name) : PODMessageParameter<unsigned char>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::INT; }
      protected:
         virtual ~UnsignedCharMessageParameter() {}   
   };

   /**
    * @class ShortIntMessageParameter 
    */
   class ShortIntMessageParameter: public PODMessageParameter<short>
   {
      public:
         ShortIntMessageParameter(const std::string &name) : PODMessageParameter<short>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::INT; }
      protected:
         virtual ~ShortIntMessageParameter() {}   
   };

   /**
    * @class UnsignedShortIntMessageParameter 
    */
   class UnsignedShortIntMessageParameter: public PODMessageParameter<unsigned short>
   {
      public:
         UnsignedShortIntMessageParameter(const std::string &name) : PODMessageParameter<unsigned short>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::INT; }
      protected:
         virtual ~UnsignedShortIntMessageParameter() {}   
   };

   /**
    * @class UnsignedIntMessageParameter 
    */
   class UnsignedIntMessageParameter: public PODMessageParameter<unsigned int>
   {
      public:
         UnsignedIntMessageParameter(const std::string &name) : PODMessageParameter<unsigned int>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::INT; }
      protected:
         virtual ~UnsignedIntMessageParameter() {}   
   };

   /**
    * @class IntMessageParameter 
    */
   class IntMessageParameter: public PODMessageParameter<int>
   {
      public:
         IntMessageParameter(const std::string &name) : PODMessageParameter<int>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::INT; }
      protected:
         virtual ~IntMessageParameter() {} 	
   };
  
   /**
    * @class UnsignedLongIntMessageParameter 
    */
   class UnsignedLongIntMessageParameter: public PODMessageParameter<unsigned long>
   {
      public:
         UnsignedLongIntMessageParameter(const std::string &name) : PODMessageParameter<unsigned long>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::LONGINT; }
      protected:
         virtual ~UnsignedLongIntMessageParameter() {}   
   };

   /**
    * @class LongIntMessageParameter 
    */
   class LongIntMessageParameter: public PODMessageParameter<long>
   {
      public:
         LongIntMessageParameter(const std::string &name) : PODMessageParameter<long>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::LONGINT; }
      protected:
         virtual ~LongIntMessageParameter() {}   
   };

   /**
    * @class FloatMessageParameter 
    */
   class FloatMessageParameter: public PODMessageParameter<float>
   {
   public:
      FloatMessageParameter(const std::string &name) : PODMessageParameter<float>(name) {}
      virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::FLOAT; }
   protected:
      virtual ~FloatMessageParameter() {}   
   };

   /**
    * @class DoubleMessageParameter 
    */
   class DoubleMessageParameter: public PODMessageParameter<double>
   {
   public:
      DoubleMessageParameter(const std::string &name) : PODMessageParameter<double>(name) {}
      virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::DOUBLE; }
   protected:
      virtual ~DoubleMessageParameter() {}   
   };

   /**
    * @class StringMessageParameter 
    */
   class StringMessageParameter: public GenericMessageParameter<std::string>
   {
      public:
         StringMessageParameter(const std::string& name) : GenericMessageParameter<std::string>(name) {}
         virtual const dtDAL::DataType& GetDataType() const { return dtDAL::DataType::STRING; }
         
         virtual const std::string ToString() const 
         {
            return GetValue();  
         }
        
         virtual bool FromString(const std::string& value) 
         {
            SetValue(value);
            return true;  
         }
      protected:
         virtual ~StringMessageParameter() {}   
   };

   /**
    * @class EnumMessageParameter
    * The EnumMessageParameter works like the string message parameter because there is no way for the message to 
    * efficiently know how to fetch proper enumeration object when the data is populated from a string or stream.
    *  
    */
   class EnumMessageParameter: public GenericMessageParameter<std::string>
   {
      public:
         EnumMessageParameter(const std::string& name) : GenericMessageParameter<std::string>(name) {}
         virtual const dtDAL::DataType& GetDataType() const { return dtDAL::DataType::ENUMERATION; }
         
         virtual const std::string ToString() const 
         {
            return GetValue();  
         }
        
         virtual bool FromString(const std::string& value) 
         {
            SetValue(value);
            return true;  
         }
      protected:
         virtual ~EnumMessageParameter() {}   
   };

   /**
    * @class ActorMessageParameter
    * The ActorMessageParameter stores a unique id that can be used to retrieve the necessary
    * GameActorProxy from the game manager.
    */
   class ActorMessageParameter: public GenericMessageParameter<dtCore::UniqueId>
   {
      public:
         ActorMessageParameter(const std::string& name) : GenericMessageParameter<dtCore::UniqueId>(name) {}
         virtual const dtDAL::DataType& GetDataType() const { return dtDAL::DataType::ACTOR; }
         
         virtual const std::string ToString() const 
         {
            return GetValue().ToString();  
         }
        
         virtual bool FromString(const std::string& value) 
         {
            SetValue(dtCore::UniqueId(value));
            return true;  
         }
      protected:
         virtual ~ActorMessageParameter() {}   
   };


   /**
    * @class VecMessageParameter
    * This is a template for the vector types to make reading data from a string easier. 
    */
   template <class ParamType>
   class VecMessageParameter: public GenericMessageParameter<ParamType>
   {
      protected:
         VecMessageParameter(const std::string &name) : GenericMessageParameter<ParamType>(name) {}
         virtual ~VecMessageParameter() {}   
  
         bool InternalFromString(const std::string &value, unsigned size) 
         {  
            ParamType newValue;
   
            if (dtUtil::ParseVec<ParamType>(value, newValue, size))
            {
               SetValue(newValue);
               return true;
            }
            else return false;
         }
   };

   /**
    * @class Vec2MessageParameter
    */
   class Vec2MessageParameter: public VecMessageParameter<osg::Vec2>
   {
      public:
         Vec2MessageParameter(const std::string &name) : VecMessageParameter<osg::Vec2>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC2; }
         
         virtual const std::string ToString() const 
         {
            std::ostringstream ss;
            osg::Vec2 vec = GetValue();
            ss << vec.x() << ' ' << vec.y();
            return ss.str();  
         }
        
         virtual bool FromString(const std::string &value) 
         {
            return InternalFromString(value, 2);  
         }
      protected:
         virtual ~Vec2MessageParameter() {}   
   };

   /**
    * @class Vec2fMessageParameter
    */
   class Vec2fMessageParameter: public VecMessageParameter<osg::Vec2f>
   {
      public:
         Vec2fMessageParameter(const std::string &name) : VecMessageParameter<osg::Vec2f>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC2F; }
         
         virtual const std::string ToString() const 
         {
            std::ostringstream ss;
            osg::Vec2f vec = GetValue();
            ss << vec.x() << ' ' << vec.y();
            return ss.str();  
         }
        
         virtual bool FromString(const std::string &value) 
         {
            return InternalFromString(value, 2);  
         }
      protected:
         virtual ~Vec2fMessageParameter() {}   
   };

   /**
    * @class Vec2dMessageParameter
    */
   class Vec2dMessageParameter: public VecMessageParameter<osg::Vec2d>
   {
      public:
         Vec2dMessageParameter(const std::string &name) : VecMessageParameter<osg::Vec2d>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC2D; }
         
         virtual const std::string ToString() const 
         {
            std::ostringstream ss;
            osg::Vec2d vec = GetValue();
            ss << vec.x() << ' ' << vec.y();
            return ss.str();  
         }
        
         virtual bool FromString(const std::string &value) 
         {
            return InternalFromString(value, 2);  
         }
      protected:
         virtual ~Vec2dMessageParameter() {}   
   };


   /**
    * @class Vec3MessageParameter
    */
   class Vec3MessageParameter: public VecMessageParameter<osg::Vec3>
   {
      public:
         Vec3MessageParameter(const std::string &name) : VecMessageParameter<osg::Vec3>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC3; }
         
         virtual const std::string ToString() const 
         {
            std::ostringstream ss;
            osg::Vec3 vec = GetValue();
            ss << vec.x() << ' ' << vec.y() << ' ' << vec.z();
            return ss.str();  
         }
        
         virtual bool FromString(const std::string &value) 
         {
            return InternalFromString(value, 3);  
         }
      protected:
         virtual ~Vec3MessageParameter() {}   
   };

   /**
    * @class Vec3fMessageParameter
    */
   class Vec3fMessageParameter: public VecMessageParameter<osg::Vec3f>
   {
      public:
         Vec3fMessageParameter(const std::string &name) : VecMessageParameter<osg::Vec3f>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC3F; }
         
         virtual const std::string ToString() const 
         {
            std::ostringstream ss;
            osg::Vec3f vec = GetValue();
            ss << vec.x() << ' ' << vec.y() << ' ' << vec.z();
            return ss.str();  
         }
        
         virtual bool FromString(const std::string &value) 
         {
            return InternalFromString(value, 3);  
         }
      protected:
         virtual ~Vec3fMessageParameter() {}   
   };
   
   /**
    * @class Vec3dMessageParameter
    */
   class Vec3dMessageParameter: public VecMessageParameter<osg::Vec3d>
   {
      public:
         Vec3dMessageParameter(const std::string &name) : VecMessageParameter<osg::Vec3d>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC3D; }
         
         virtual const std::string ToString() const 
         {
            std::ostringstream ss;
            osg::Vec3d vec = GetValue();
            ss << vec.x() << ' ' << vec.y() << ' ' << vec.z();
            return ss.str();  
         }
        
         virtual bool FromString(const std::string &value) 
         {
            return InternalFromString(value, 3);  
         }
      protected:
         virtual ~Vec3dMessageParameter() {}   
   };
 
   /**
    * @class Vec4MessageParameter
    */
   class Vec4MessageParameter: public VecMessageParameter<osg::Vec4>
   {
      public:
         Vec4MessageParameter(const std::string &name) : VecMessageParameter<osg::Vec4>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC4; }
         
         virtual const std::string ToString() const 
         {
            std::ostringstream ss;
            osg::Vec4 vec = GetValue();
            ss << vec.x() << ' ' << vec.y() << ' ' << vec.z() << ' ' << vec.w();
            return ss.str();  
         }
        
         virtual bool FromString(const std::string &value) 
         {
            return InternalFromString(value, 4);  
         }
      protected:
         virtual ~Vec4MessageParameter() {}   
   };
   
   /**
    * @class Vec4fMessageParameter
    */
   class Vec4fMessageParameter: public VecMessageParameter<osg::Vec4f>
   {
      public:
         Vec4fMessageParameter(const std::string &name) : VecMessageParameter<osg::Vec4f>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC4F; }
         
         virtual const std::string ToString() const 
         {
            std::ostringstream ss;
            osg::Vec4f vec = GetValue();
            ss << vec.x() << ' ' << vec.y() << ' ' << vec.z() << ' ' << vec.w();
            return ss.str();  
         }
        
         virtual bool FromString(const std::string &value) 
         {
            return InternalFromString(value, 4);  
         }
      protected:
         virtual ~Vec4fMessageParameter() {}   
   };

   /**
    * @class Vec4dMessageParameter
    */
   class Vec4dMessageParameter: public VecMessageParameter<osg::Vec4d>
   {
      public:
         Vec4dMessageParameter(const std::string &name) : VecMessageParameter<osg::Vec4d>(name) {}
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC4D; }
         
         virtual const std::string ToString() const 
         {
            std::ostringstream ss;
            osg::Vec4d vec = GetValue();
            ss << vec.x() << ' ' << vec.y() << ' ' << vec.z() << ' ' << vec.w();
            return ss.str();  
         }
        
         virtual bool FromString(const std::string &value) 
         {
            return InternalFromString(value, 4);  
         }
      protected:
         virtual ~Vec4dMessageParameter() {}   
   };

   /**
    * @class ResourceMessageParameter
    */
   class DT_GAMEMANAGER_EXPORT ResourceMessageParameter: public MessageParameter
   {
      public:
         ResourceMessageParameter(const dtDAL::DataType& type, const std::string &name): 
            MessageParameter(name), mDataType(&type) {}
         
         virtual const dtDAL::DataType& GetDataType() const { return *mDataType; }
         
         virtual void ToDataStream(DataStream &stream) const;
        
         virtual void FromDataStream(DataStream &stream);
        
         virtual const std::string ToString() const;
        
         virtual bool FromString(const std::string &value);

         virtual void CopyFrom(const MessageParameter &otherParam);
         
         void SetValue(const dtDAL::ResourceDescriptor* descriptor);
            
         const dtDAL::ResourceDescriptor* GetValue() const; 

      protected:
         virtual ~ResourceMessageParameter() {}
      private:
         const dtDAL::DataType* mDataType;
         dtDAL::ResourceDescriptor mDescriptor;   
   };


}

#endif
