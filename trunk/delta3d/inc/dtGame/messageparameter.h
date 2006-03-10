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
#include <vector>
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
#include "dtGame/exceptionenum.h"

namespace dtGame
{
   
   /**
    * @class MessageParameter
    */
   class DT_GAME_EXPORT MessageParameter : public osg::Referenced
   {
      public:
      
         static const char DEFAULT_DELIMETER;         
               
         MessageParameter(const std::string &name, bool isList = false) : 
            mName(name), mIsList(isList)
         {
            SetParamDelimeter(DEFAULT_DELIMETER);
         }         
        
         const std::string &GetName() const { return mName; }
        
         virtual const dtDAL::DataType &GetDataType() const = 0;
        
         virtual void ToDataStream(DataStream &stream) const = 0;
        
         virtual void FromDataStream(DataStream &stream) = 0;
        
         virtual const std::string ToString() const = 0;
         
         /**
          * @return True if the paramater contains a list of
          *    data, false if this parameter only stores one value.
          */
         bool IsList() const { return mIsList; }
        
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
         static dtCore::RefPtr<MessageParameter> CreateFromType(
         	const dtDAL::DataType& type, 
         	const std::string& name, bool isList=false) throw(dtUtil::Exception);
         
         /**
          * Gets the character used as a delimeter between parameters when serializing
          * a message parameter containing a list of values to a string.
          * @return The delimeter.  By default, this is equal to "|".
          */
         char GetParamDelimeter() const 
         {
            return mParamListDelimeter;
         }
         
         /**
          * Allows one to override the default delimeter used to serialize message
          * parmeters to a string.
          * @param delim The new delimeter.  This should NOT be the space character!
          *    Default value for the delimeter is the pipe character. '|'
          * @see GetParamDelimeter
          */
         void SetParamDelimeter(char delim)
         {
            mParamListDelimeter = delim;
         }
         
      protected:
        
         virtual ~MessageParameter() { }
        
      private:      
         std::string mName;
         
         //This value is used as a delimeter between list data elements
         //when converting to and from a string.
         char mParamListDelimeter;
         
         bool mIsList;  
   };

   template <class ParamType>
   class GenericMessageParameter : public MessageParameter
   {
      public:                  
         GenericMessageParameter(const std::string &name, const ParamType& defaultValue, bool isList=false) 
            : MessageParameter(name,isList)
         {
            mValue = defaultValue;
            mNumberPrecision = 16;
            
            if (IsList()) 
            {
               mValueList = new std::vector<ParamType>();
               mValueList->push_back(defaultValue);
            }
            else
            {
               mValueList = NULL;
            }
         }
         
         /**
          * Sets the precision of floating point number as they are 
          * converted to and from string values.
          * @param precision The amount of numeric percision.
          * @note This value defaults to 16.
          */
         inline void SetNumberPrecision(unsigned int precision)
         {
            mNumberPrecision = precision;
         }
         
         /**
          * Gets the current floating point precision value on this
          * parameter.
          * @return The current number precision.
          */
         inline unsigned int GetNumberPrecision() const
         {
            return mNumberPrecision;
         }
        
         inline virtual void CopyFrom(const MessageParameter &otherParam)
         {
            const GenericMessageParameter<ParamType> *param =
                static_cast<const GenericMessageParameter<ParamType> *>(&otherParam);
         
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
            
         inline virtual void SetValue(const ParamType& value) 
         { 
            if (IsList())
               EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
                  "Cannot call SetValue() on message parameter with a list of values.");
            
            mValue = value; 
         }
               
         inline virtual const ParamType& GetValue() const 
         { 
            if (IsList())
               EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
                  "Cannot call GetValue() on message parameter with a list of values.");
            
            return mValue;
         }
         
         inline const std::vector<ParamType> &GetValueList() const
         {
            if (!IsList())
               EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
                  "Cannot retrieve the parameters value list.  Parameter does not contain a list.");
            return *mValueList;
         }
         
         inline std::vector<ParamType> &GetValueList()
         {
            if (!IsList())
               EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
                  "Cannot retrieve the parameters value list.  Parameter does not contain a list.");
            return *mValueList;
         }
         
         inline void SetValueList(const std::vector<ParamType> &newValues)
         {
            if (!IsList())
               EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION,
                  "Cannot set a list of new values on a parameter that is not a list.");
                  
            *mValueList = newValues;
         }
         
         inline virtual void ToDataStream(DataStream &stream) const 
         { 
            if (IsList())
            {
               stream << mValueList->size();
               for (unsigned int i=0; i<mValueList->size(); i++)
                  stream << (*mValueList)[i];
            }
            else
            {         
               stream << mValue;
            }
         }
        
         inline virtual void FromDataStream(DataStream &stream) 
         { 
            if (IsList())
            {
               mValueList->clear();
               
               unsigned int listSize;
               stream >> listSize;
               for (unsigned int i=0; i<listSize; i++) 
               {
                  ParamType newElement;
                  stream >> newElement;
                  mValueList->push_back(newElement);
               }
            }
            else
            {  
               stream >> mValue;
            } 
         }    

         virtual const std::string ToString() const { return std::string(); }
         virtual bool FromString(const std::string &value) { return false; }
        
      protected:
         ///Keep destructors protected to ensure property smart pointer management.
         virtual ~GenericMessageParameter() { delete mValueList; }
      
      private:
         std::vector<ParamType> *mValueList;
         ParamType mValue;
         unsigned int mNumberPrecision;
   };

   /**
    * @class PODMessageParameter
    * This is a template for the single piece of data types to make reading data to and from a string easier. 
    */
   template <class ParamType>
   class PODMessageParameter: public GenericMessageParameter<ParamType>
   {
      public:
         PODMessageParameter(const std::string &name, const ParamType& defaultValue, bool isList=false)
            : GenericMessageParameter<ParamType>(name, defaultValue, isList) { }
              
         virtual const std::string ToString() const 
         {                     
            std::ostringstream stream;
            stream.precision(GenericMessageParameter<ParamType>::GetNumberPrecision());        
            if (GenericMessageParameter<ParamType>::IsList())
            {
               const std::vector<ParamType> &values = 
                  GenericMessageParameter<ParamType>::GetValueList();
                  
               stream << values[0];
               for (unsigned int i=1; i<values.size(); i++)
                  stream << GenericMessageParameter<ParamType>::GetParamDelimeter() 
                     << values[i];
            }
            else
            {         
              stream << GenericMessageParameter<ParamType>::GetValue();
            }
            
            return stream.str();            
         }
        
         virtual bool FromString(const std::string &value) 
         {  
            std::istringstream stream;               
            stream.precision(GenericMessageParameter<ParamType>::GetNumberPrecision());        
            
            if (GenericMessageParameter<ParamType>::IsList())
            {               
               std::vector<ParamType> &result = 
                  GenericMessageParameter<ParamType>::GetValueList();
               
               ParamType actual;
               result.clear();
               stream.str(value);
               stream >> actual;
               result.push_back(actual);
               while (stream) 
               {
                  char sep;
                  stream >> sep;
                  if (!stream)
                     break;
                     
                  stream >> actual;
                  result.push_back(actual);
               }               
            }
            else
            {
               ParamType newValue;
               stream.str(value);
               stream >> newValue;
               GenericMessageParameter<ParamType>::SetValue(newValue);
            }
            
            return true;
         }
      
      protected:
         virtual ~PODMessageParameter() {}         
   };

   /**
    * @class BooleanMessageParameter 
    */
   class DT_GAME_EXPORT BooleanMessageParameter: public PODMessageParameter<bool>
   {
      public:
         BooleanMessageParameter(const std::string &name, bool defaultValue = false,
            bool isList=false);
            
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::BOOLEAN; }

         ///overridden to return "true" and "false"
         virtual const std::string ToString() const;
        
         ///overridden to accept "true" and other such strings as well as numbers.
         virtual bool FromString(const std::string &value);

      protected:
         virtual ~BooleanMessageParameter();
   };

   /**
     * @class UnsignedCharMessageParameter 
     */
   class DT_GAME_EXPORT UnsignedCharMessageParameter: public PODMessageParameter<unsigned char>
   {
      public:
         UnsignedCharMessageParameter(const std::string &name, 
             unsigned char defaultValue = 0, bool isList=false);
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::UCHAR; }
      protected:
         virtual ~UnsignedCharMessageParameter();
   };

   /**
     * @class ShortIntMessageParameter 
     */
   class DT_GAME_EXPORT ShortIntMessageParameter: public PODMessageParameter<short>
   {
      public:
         ShortIntMessageParameter(const std::string &name, short defaultValue = 0, bool isList=false);
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::SHORTINT; }
      protected:
         virtual ~ShortIntMessageParameter();
   };

   /**
     * @class UnsignedShortIntMessageParameter 
     */
   class DT_GAME_EXPORT UnsignedShortIntMessageParameter: public PODMessageParameter<unsigned short>
   {
      public:
         UnsignedShortIntMessageParameter(const std::string &name,
            unsigned short defaultValue = 0, bool isList=false);
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::USHORTINT; }
      protected:
         virtual ~UnsignedShortIntMessageParameter();
   };

   /**
     * @class UnsignedIntMessageParameter 
     */
   class DT_GAME_EXPORT UnsignedIntMessageParameter: public PODMessageParameter<unsigned int>
   {
      public:
         UnsignedIntMessageParameter(const std::string &name, unsigned int defaultValue=0,	bool isList=false);
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::UINT; }
      protected:
         virtual ~UnsignedIntMessageParameter();
   };

   /**
     * @class IntMessageParameter 
     */
   class DT_GAME_EXPORT IntMessageParameter: public PODMessageParameter<int>
   {
      public:
         IntMessageParameter(const std::string &name, int defaultValue=0,	bool isList=false);
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::INT; }
      protected:
         virtual ~IntMessageParameter();
   };
  
   /**
    * @class UnsignedLongIntMessageParameter 
    */
   class DT_GAME_EXPORT UnsignedLongIntMessageParameter: public PODMessageParameter<unsigned long>
   {
      public:
         UnsignedLongIntMessageParameter(const std::string &name, 
            unsigned long defaultValue = 0, bool isList=false);
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::ULONGINT; }
      protected:
         virtual ~UnsignedLongIntMessageParameter();
   };

   /**
     * @class LongIntMessageParameter 
     */
   class DT_GAME_EXPORT LongIntMessageParameter: public PODMessageParameter<long>
   {
      public:
         LongIntMessageParameter(const std::string &name, long defaultValue=0, 	bool isList=false);
         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::LONGINT; }
      protected:
         virtual ~LongIntMessageParameter();
   };

   /**
     * @class FloatMessageParameter 
     */
   class DT_GAME_EXPORT FloatMessageParameter: public PODMessageParameter<float>
   {
   public:
      FloatMessageParameter(const std::string &name, float defaultValue=0.0f,  bool isList=false);
      virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::FLOAT; }
   protected:
      virtual ~FloatMessageParameter();
   };

   /**
     * @class DoubleMessageParameter 
     */
   class DT_GAME_EXPORT DoubleMessageParameter: public PODMessageParameter<double>
   {
   public:
      DoubleMessageParameter(const std::string &name, double defaultValue=0.0, 	bool isList=false);
      virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::DOUBLE; }
   protected:
      virtual ~DoubleMessageParameter();
   };

   /**
     * @class StringMessageParameter 
     */
   class DT_GAME_EXPORT StringMessageParameter: public GenericMessageParameter<std::string>
   {
      public:
         StringMessageParameter(const std::string& name, const std::string& defaultValue = "",
             bool isList=false);        

         virtual const dtDAL::DataType& GetDataType() const { return dtDAL::DataType::STRING; }         
         virtual const std::string ToString() const;
         virtual bool FromString(const std::string& value);       
         
      protected:
         virtual ~StringMessageParameter();  
   };

   /**
     * @class EnumMessageParameter
     * The EnumMessageParameter works like the string message parameter because there is no way for the message to 
     * efficiently know how to fetch proper enumeration object when the data is populated from a string or stream.
     */
   class DT_GAME_EXPORT EnumMessageParameter: public StringMessageParameter
   {
      public:
         EnumMessageParameter(const std::string& name, const std::string& defaultValue = "", bool isList=false);
         virtual const dtDAL::DataType& GetDataType() const { return dtDAL::DataType::ENUMERATION; }         
      protected:
         virtual ~EnumMessageParameter();
   };

   /**
     * @class ActorMessageParameter
     * The ActorMessageParameter stores a unique id that can be used to retrieve the necessary
     * GameActorProxy from the game manager.
    */
   class DT_GAME_EXPORT ActorMessageParameter: public GenericMessageParameter<dtCore::UniqueId>
   {
      public:
         ActorMessageParameter(const std::string& name, 
          const dtCore::UniqueId& defaultValue = dtCore::UniqueId(""), bool isList=false);               

         virtual const dtDAL::DataType& GetDataType() const { return dtDAL::DataType::ACTOR; }         
         virtual const std::string ToString() const;          
         virtual bool FromString(const std::string& value);
         
      protected:
         virtual ~ActorMessageParameter();
   };

   /**
    * @class VecMessageParameter
    * This is a template for the vector types to make reading data from a string easier. 
    */
   template <class ParamType>
   class VecMessageParameter: public GenericMessageParameter<ParamType>
   {
      public:
         virtual const std::string ToString() const 
         {
            std::ostringstream ss;
            ss.precision(GenericMessageParameter<ParamType>::GetNumberPrecision());        
                        
            if (GenericMessageParameter<ParamType>::IsList())
            {
               const std::vector<ParamType> &values = 
                  GenericMessageParameter<ParamType>::GetValueList();
                  
               ss << values[0];
               for (unsigned int i=1; i<values.size(); i++)
                  ss << GenericMessageParameter<ParamType>::GetParamDelimeter() << 
                     values[i];                  
               return ss.str();
            }
            else
            {
               ss << GenericMessageParameter<ParamType>::GetValue();
               return ss.str();
            }
         }
         
      protected:
         VecMessageParameter(const std::string &name, const ParamType& defaultValue,
            bool isList=false) : GenericMessageParameter<ParamType>(name, defaultValue, isList) {}
         virtual ~VecMessageParameter() {}   
  
         bool InternalFromString(const std::string &value, unsigned size) 
         {  
            if (GenericMessageParameter<ParamType>::IsList())
            {
               std::vector<ParamType> &result = 
                  GenericMessageParameter<ParamType>::GetValueList();
                  
               std::vector<std::string> stringList;
               dtUtil::IsDelimeter delimCheck(GenericMessageParameter<ParamType>::GetParamDelimeter());
               dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(stringList,value,delimCheck);
               
               result.clear();
               for (unsigned int i=0; i<stringList.size(); i++)
               {
                  ParamType v;
                  if (!dtUtil::ParseVec<ParamType>(stringList[i],v,size))                  
                     return false;
                  result.push_back(v);
               }                 
                  
               return true;
            }
            else 
            {
               ParamType newValue;   
               if (dtUtil::ParseVec<ParamType>(value, newValue, size))
               {
                  GenericMessageParameter<ParamType>::SetValue(newValue);
                  return true;
               }
               else 
                  return false;
            }
         }
   };

   /**
    * @class Vec2MessageParameter
    */
   class DT_GAME_EXPORT Vec2MessageParameter: public VecMessageParameter<osg::Vec2>
   {
      public:
         Vec2MessageParameter(const std::string &name, 
             const osg::Vec2& defaultValue = osg::Vec2(0.0,0.0), bool isList=false);

         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC2; }         
         virtual bool FromString(const std::string &value) { return InternalFromString(value, 2); }
         
      protected:
         virtual ~Vec2MessageParameter();
   };

   /**
     * @class Vec2fMessageParameter
     */
   class DT_GAME_EXPORT Vec2fMessageParameter: public VecMessageParameter<osg::Vec2f>
   {
      public:
         Vec2fMessageParameter(const std::string &name, 
             const osg::Vec2f& defaultValue = osg::Vec2f(0.0,0.0), bool isList=false);

         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC2F; }         
         virtual bool FromString(const std::string &value) { return InternalFromString(value, 2); }    

      protected:
         virtual ~Vec2fMessageParameter();
   };

   /**
     * @class Vec2dMessageParameter
     */
   class DT_GAME_EXPORT Vec2dMessageParameter: public VecMessageParameter<osg::Vec2d>
   {
      public:
         Vec2dMessageParameter(const std::string &name, 
         	   const osg::Vec2d& defaultValue = osg::Vec2d(0.0,0.0), bool isList=false);

         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC2D; }
         virtual bool FromString(const std::string &value) { return InternalFromString(value, 2); } 

      protected:
         virtual ~Vec2dMessageParameter();
   };

   /**
     * @class Vec3MessageParameter
     */
   class DT_GAME_EXPORT Vec3MessageParameter: public VecMessageParameter<osg::Vec3>
   {
      public:
         Vec3MessageParameter(const std::string &name, 
         	   const osg::Vec3& defaultValue = osg::Vec3(0.0, 0.0, 0.0), bool isList=false);

         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC3; }
         virtual bool FromString(const std::string &value) { return InternalFromString(value, 3); } 

      protected:
         virtual ~Vec3MessageParameter();
   };

   /**
     * @class Vec3fMessageParameter
     */
   class DT_GAME_EXPORT Vec3fMessageParameter: public VecMessageParameter<osg::Vec3f>
   {
      public:
         Vec3fMessageParameter(const std::string &name, 
         	   const osg::Vec3f& defaultValue = osg::Vec3f(0.0, 0.0, 0.0), bool isList=false);

         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC3F; }                 
         virtual bool FromString(const std::string &value) { return InternalFromString(value, 3); }

      protected:
         virtual ~Vec3fMessageParameter();
   };
   
   /**
     * @class Vec3dMessageParameter
     */
   class DT_GAME_EXPORT Vec3dMessageParameter: public VecMessageParameter<osg::Vec3d>
   {
      public:
         Vec3dMessageParameter(const std::string &name, 
             const osg::Vec3d& defaultValue = osg::Vec3d(0.0, 0.0, 0.0), bool isList=false);

         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC3D; }
         virtual bool FromString(const std::string &value) { return InternalFromString(value, 3); }

      protected:
         virtual ~Vec3dMessageParameter();
   };
 
   /**
     * @class Vec4MessageParameter
     */
   class DT_GAME_EXPORT Vec4MessageParameter: public VecMessageParameter<osg::Vec4>
   {
      public:
         Vec4MessageParameter(const std::string &name, 
             const osg::Vec4& defaultValue = osg::Vec4(0.0, 0.0, 0.0, 0.0), bool isList=false);

         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC4; }
         virtual bool FromString(const std::string &value) { return InternalFromString(value, 4); }

      protected:
         virtual ~Vec4MessageParameter();
   };
   
   /**
    * @class Vec4fMessageParameter
    */
   class DT_GAME_EXPORT Vec4fMessageParameter: public VecMessageParameter<osg::Vec4f>
   {
      public:
         Vec4fMessageParameter(const std::string &name, 	
          const osg::Vec4f& defaultValue = osg::Vec4f(0.0, 0.0, 0.0, 0.0), bool isList=false);

         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC4F; }
         virtual bool FromString(const std::string &value)  {  return InternalFromString(value, 4); }

      protected:
         virtual ~Vec4fMessageParameter();
   };

   /**
    * @class Vec4dMessageParameter
    */
   class DT_GAME_EXPORT Vec4dMessageParameter: public VecMessageParameter<osg::Vec4d>
   {
      public:
         Vec4dMessageParameter(const std::string &name, 
         	const osg::Vec4d& defaultValue = osg::Vec4d(0.0, 0.0, 0.0, 0.0), bool isList=false);

         virtual const dtDAL::DataType &GetDataType() const { return dtDAL::DataType::VEC4D; }
         virtual bool FromString(const std::string &value) { return InternalFromString(value, 4); }

      protected:
         virtual ~Vec4dMessageParameter();
   };

   /**
    * @class ResourceMessageParameter
    */
   class DT_GAME_EXPORT ResourceMessageParameter: public MessageParameter
   {
      public:
         ResourceMessageParameter(const dtDAL::DataType& type, const std::string &name, 
            bool isList=false);
                
         virtual const dtDAL::DataType& GetDataType() const { return *mDataType; }
         
         virtual void ToDataStream(DataStream &stream) const;
        
         virtual void FromDataStream(DataStream &stream);
        
         virtual const std::string ToString() const;
        
         virtual bool FromString(const std::string &value);

         virtual void CopyFrom(const MessageParameter &otherParam);
         
         void SetValue(const dtDAL::ResourceDescriptor* descriptor);
            
         const dtDAL::ResourceDescriptor* GetValue() const; 
         
         const std::vector<dtDAL::ResourceDescriptor> &GetValueList() const;
         
         std::vector<dtDAL::ResourceDescriptor> &GetValueList();
                  
         void SetValueList(const std::vector<dtDAL::ResourceDescriptor> &newValues);         

      protected:
         virtual ~ResourceMessageParameter();
         
      private:
         const dtDAL::DataType* mDataType;
         std::vector<dtDAL::ResourceDescriptor> *mValueList;
         dtDAL::ResourceDescriptor mDescriptor;            
   };


}

#endif
