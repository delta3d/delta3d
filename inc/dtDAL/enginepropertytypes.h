/* -*-c++-*-
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
 * Matthew W. Campbell
 * David Guthrie
 * William E. Johnson II
 */
#ifndef DELTA_ENGINE_PROPERTY_TYPES
#define DELTA_ENGINE_PROPERTY_TYPES

#include <osg/Vec3>
#include <osg/Vec3d>
#include <osg/Vec4>
#include <osg/Vec4d>
#include <osg/Vec2>
#include <osg/Vec2d>
#include <dtCore/deltadrawable.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/genericactorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/export.h>
#include <dtUtil/log.h>

namespace dtDAL
{

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents an actor
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT ActorActorProperty : public ActorProperty
   {
      public:
         typedef dtUtil::Functor<void, TYPELIST_1(ActorProxy*)> SetFuncType;
         typedef dtUtil::Functor<dtCore::DeltaDrawable*, TYPELIST_0()> GetFuncType;
         ActorActorProperty(ActorProxy &actorProxy,
                           const dtUtil::RefString& name,
                           const dtUtil::RefString& label,
                           SetFuncType Set,
                           GetFuncType Get,
                           const dtUtil::RefString& desiredActorClass = "",
                           const dtUtil::RefString& desc = "",
                           const dtUtil::RefString& groupName = "") :
         ActorProperty(DataType::ACTOR, name, label, desc, groupName),
            mProxy(&actorProxy),
            SetPropFunctor(Set),
            GetActorFunctor(Get),
            mDesiredActorClass(desiredActorClass)
            {

            }

         /**
          * Copies an ActorActorProperty value to this one from the property
          * specified. This method fails if otherProp is not an ActorActorProperty.
          * @param otherProp The property to copy the value from.
          */
         virtual void CopyFrom(const ActorProperty& otherProp)
         {
            if (GetDataType() != otherProp.GetDataType())
            {
               LOG_ERROR("Property types are incompatible. Cannot make copy.");
               return;
            }
            
            const ActorActorProperty& prop =
               static_cast<const ActorActorProperty& >(otherProp);

            SetValue(prop.GetValue());
         }

         /**
          * Sets the value of this property by calling the set functor
          * assigned to this property.
          * Hack for the resource class
          * @param value the value to set or NULL to clear it.  The passed in pointer is
          * not stored.  The values are extracted and stored in a separate object.
          */
         void SetValue(ActorProxy* value);

         /**
          * Gets the value proxy assiged to this property.
          * Hack for the resource class
          * @return the currently set ActorProxy for this property.
          */
         ActorProxy* GetValue() const;

         /**
          * Gets the drawable that this property is representing
          * @return The actor
          */
         dtCore::DeltaDrawable* GetRealActor();

         /**
          * Gets the drawable that this property is representing
          * @return The actor
          */
         const dtCore::DeltaDrawable* GetRealActor() const;

         /**
          * Sets the value of the property based on a string.
          * The string should be the both the unique id and the display string separated by a comma.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return false if the value does not reference an existing proxy or the proxy is of the wrong class.
          */
         virtual bool FromString(const std::string& value);

         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

         /**
          * @return the class of proxy this expects so that the UI can filter the list.
          */
         const std::string& GetDesiredActorClass() const { return mDesiredActorClass; }

      private:
         ActorProxy *mProxy;
         SetFuncType SetPropFunctor;
         GetFuncType GetActorFunctor;
         dtUtil::RefString mDesiredActorClass;
      protected:
         virtual ~ActorActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a game event property.
    *
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT GameEventActorProperty
      : public GenericActorProperty<GameEvent *,GameEvent *>
   {
      public:
         typedef GenericActorProperty<GameEvent *,GameEvent *> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         GameEventActorProperty(ActorProxy &actorProxy,
                                const dtUtil::RefString& name, const dtUtil::RefString& label,
                                SetFuncType set, GetFuncType get,
                                const dtUtil::RefString& desc = "",
                                const dtUtil::RefString& groupName = "") :
            BaseClass(DataType::GAME_EVENT,name,label,set,get,desc,groupName), mProxy(&actorProxy)
         {
         }

         /**
          * Sets the value of this property using the given string.
          * @param value The string representing the game event.  This string contains
          *   unique id of the game event.  This is used to look up the actual event
          *   which then gets referenced by this property.
          * @return True if the value could be parsed and the specified event was found, false otherwise.
          */
         virtual bool FromString(const std::string& value);

         /**
          * Gets a string version of the game event data.
          * @return A string containing the unique id of the game event.
          * @see #FromString
         */
         virtual const std::string ToString() const;

      protected:
         virtual ~GameEventActorProperty() { }

      private:

         ActorProxy *mProxy;
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a resource
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT ResourceActorProperty : public ActorProperty
   {
      public:
         typedef dtUtil::Functor<void, TYPELIST_1(const std::string&)> SetFuncType;

         ResourceActorProperty(ActorProxy &actorProxy,
                              DataType &type,
                              const dtUtil::RefString& name,
                              const dtUtil::RefString& label,
                              SetFuncType Set,
                              const dtUtil::RefString& desc = "",
                              const dtUtil::RefString& groupName = "") :
         ActorProperty(type, name, label, desc, groupName),
            mProxy(&actorProxy),
            SetPropFunctor(Set)
            {

            }

         /**
          * Copies a ResourceActorProperty value to this one from the property
          * specified. This method fails if otherProp is not a ResourceActorProperty.
          * @param otherProp The property to copy the value from.
          */
         virtual void CopyFrom(const ActorProperty& otherProp)
         {
            if (GetDataType() != otherProp.GetDataType())
               LOG_ERROR("Property types are incompatible. Cannot make copy.");
            
            const ResourceActorProperty& prop =
               static_cast<const ResourceActorProperty& >(otherProp);

            SetValue(prop.GetValue());
         }

         /**
          * Sets the value of this property by calling the set functor
          * assigned to this property.
          * Hack for the resource class
          * @param value the value to set or NULL to clear it.  The passed in pointer is
          * not stored.  The values are extracted and stored in a separate object.
          */
         void SetValue(ResourceDescriptor *value);

         /**
          * Gets the value of this property be calling the get functor
          * assigned to this property.
          * Hack for the resource class
          * @return the currently set ResourceDescriptor for this property.
          */
         ResourceDescriptor* GetValue() const;

         /**
          * Sets the value of the property based on a string.
          * The string should be the both the unique id and the display string separated by a comma.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return false of the value is not valid.
          */
         virtual bool FromString(const std::string& value);

         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      private:
         ActorProxy *mProxy;
         SetFuncType SetPropFunctor;
      protected:
         virtual ~ResourceActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a floating point data member.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT FloatActorProperty : public GenericActorProperty<float,float>
   {
      public:
         typedef GenericActorProperty<float,float> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;
         
         FloatActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::FLOAT, name,label,set,get,desc,groupName)
         {
            SetNumberPrecision(8);
         }

         /**
          * Sets the value of the property based on a string.
          * The string should be a float value as a string.  This will set the value to 0 if it's not valid.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);

         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~FloatActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a 64-bit precision decimal data member.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT DoubleActorProperty : public GenericActorProperty<double,double>
   {
      public:
         typedef GenericActorProperty<double,double> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         DoubleActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::DOUBLE, name,label,set,get,desc,groupName) { }

         /**
          * Sets the value of the property based on a string.
          * The string should be a double value as a string.  This will set the value to 0 if it's not valid.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);

         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const ;

      protected:
         virtual ~DoubleActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a signed integer data member.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT IntActorProperty : public GenericActorProperty<int,int>
   {
      public:
         typedef GenericActorProperty<int, int> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;
         
         IntActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                        SetFuncType set, GetFuncType get,
                        const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::INT, name,label,set,get,desc,groupName) { }

         /**
          * Sets the value of the property based on a string.
          * The string should be an integer value as a string.  This will set the value to 0 if it's not valid.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);

         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;
      protected:
         virtual ~IntActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a signed long integer data member.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT LongActorProperty : public GenericActorProperty<long,long>
   {
      public:
         typedef GenericActorProperty<long, long> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         LongActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::LONGINT, name,label,set,get,desc,groupName) { }

         /**
          * Sets the value of the property based on a string.
          * The string should be a long value as a string.  This will set the value to 0 if it's not valid.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);

         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;
      protected:
         virtual ~LongActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a string data member.  Where as the
    * property has a maximum string length, by default, the string has no
    * maximum length.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT StringActorProperty : public GenericActorProperty<const std::string&,
                                                                         std::string>
   {
      public:
         typedef GenericActorProperty<const std::string&, std::string> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         StringActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "",
                           const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::STRING, name,label,set,get,desc,groupName)
         {
            mMaxLength = 0;
         }

         /**
          * Sets the maximum length of strings stored in this property.
          * @note Strings longer than this value are truncated.
          * @param length Max length.
          */
         void SetMaxLength(unsigned int length) { mMaxLength = length; }

         /**
          * Gets the maximum length of strings stored in this property.
          * @return The maximum length of strings stored in this property.
          */
         unsigned int GetMaxLength() const { return mMaxLength; }

         /**
          * Overloaded set value method so that the new string value can be
          * truncated if it is longer than the maximum length allowed.
          * @param str The new string value to set on this property.
          */
         void SetValue(const std::string &str)
         {
            std::string newStr = str;
            if (mMaxLength != 0 && newStr.size() > mMaxLength)
               newStr = newStr.erase(mMaxLength);

            GenericActorProperty<const std::string&,std::string>::SetValue(newStr);
         }

         /**
          * Does the same thing as SetValue.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value) { SetValue(value); return true; }

         /**
          * @return the same as GetValue.
          */
         virtual const std::string ToString() const { return GenericActorProperty<const std::string&,std::string>::GetValue(); }

      protected:
         virtual ~StringActorProperty() { }

         ///Maximum length the string for this string property can be.
         unsigned int mMaxLength;
      };

      ////////////////////////////////////////////////////////////////////////////
      /**
       * This actor property represents a boolean data member.
       */
      ////////////////////////////////////////////////////////////////////////////
      class DT_DAL_EXPORT BooleanActorProperty : public GenericActorProperty<bool,bool>
      {
      public:
         typedef GenericActorProperty<bool, bool> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         BooleanActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                              SetFuncType set, GetFuncType get,
                              const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::BOOLEAN, name,label,set,get,desc,groupName) { }

         /**
          * Sets the value of the property based on a string.
          * The string should be a boolean value (true or false) as a string.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true if the value could be parsed.
          */
         virtual bool FromString(const std::string& value);

         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~BooleanActorProperty() { }
      };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents an abstract enumeration data member.
    * @note This abstract class exists so that all enumerations can be cast to this and worked with generically.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT AbstractEnumActorProperty
   {
      public:

         /**
          * @return the datatype for this enum actor property.
          */
         virtual DataType& GetPropertyType() const = 0;

         /**
          * @return the value as a generic enumeration
          */
         virtual dtUtil::Enumeration& GetEnumValue() const = 0;

         /**
          * Sets the value as a generic enumeration, but it must be the actual type stored
          * in the implementation of this pure virtual class.
          * @param value
          */
         virtual void SetEnumValue(dtUtil::Enumeration& value) = 0;

         /**
          * @return A list of the possibilities for this enumeration property
          */
         virtual const std::vector<dtUtil::Enumeration*>& GetList() const = 0;

         /**
          * Sets the value of the property based on the string name of the enumeration, i.e. getName()
          * on Enumeration.  If it fails, the value will remain unchanged.
          * @param name The name of the value.
          * @return true if it was successful in matching an name to a value.
          * @see Enumeration#getName
          */
         virtual bool SetValueFromString(const std::string &name) = 0;

         /**
          * This is a convenience method and enforces that only actor properties can implement this interface.
          * @return the property as an actor property.
          */
         virtual ActorProperty* AsActorProperty() { return dynamic_cast<ActorProperty*>(this); }

         /**
          * This is a convenience method and enforces that only actor properties can implement this interface.
          * @return the property as an actor property.
          */
         virtual const ActorProperty* AsActorProperty() const { return dynamic_cast<const ActorProperty*>(this); }

      protected:
         virtual ~AbstractEnumActorProperty() { }
   };


   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents an enumeration data member.
    */
   ////////////////////////////////////////////////////////////////////////////
   template <typename T>
   class EnumActorProperty : public GenericActorProperty<T&,T&>, public AbstractEnumActorProperty
   {
      public:
         typedef GenericActorProperty<T&, T&> BaseClass;
         typedef typename BaseClass::SetFuncType SetFuncType;
         typedef typename BaseClass::GetFuncType GetFuncType;

         EnumActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::ENUMERATION, name,label,set,get,desc,groupName) { }

         ///@note this method exists to reconcile the two same-named methods on the superclases.
         DataType& GetPropertyType() const { return GenericActorProperty<T&,T&>::GetPropertyType(); }

         virtual const std::vector<dtUtil::Enumeration*>& GetList() const 
         { 
            return T::Enumerate(); 
         }

         virtual const std::vector<T*>& GetListOfType() const 
         { 
            return T::EnumerateType(); 
         }

         virtual bool SetValueFromString(const std::string &name)
         {
            dtUtil::Enumeration* e = T::GetValueForName(name);
            if (e != NULL)
            {
               SetValue(*static_cast<T*>(e));
               return true;
            }

            return false;
         }

         virtual dtUtil::Enumeration& GetEnumValue() const
         {
            return static_cast<dtUtil::Enumeration&>(GenericActorProperty<T&,T&>::GetValue());
         }

         virtual void SetEnumValue(dtUtil::Enumeration& value)
         {
            T* val = dynamic_cast<T*>(&value);
            if (val != NULL)
            {
               SetValue(*val);
            }
         }

         /**
          * the same as SetValueFromString
          * @see #SetValueFromString
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true if the value could be parsed.
          */
         virtual bool FromString(const std::string& value) { return SetValueFromString(value); }

         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const { return GetEnumValue().GetName(); };

      protected:
         virtual ~EnumActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 2 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT Vec2ActorProperty :
      public GenericActorProperty<const osg::Vec2&,osg::Vec2>
   {
      public:
         typedef GenericActorProperty<const osg::Vec2&, osg::Vec2> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec2ActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::VEC2, name,label,set,get,desc,groupName) 
         { 
            SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
         }

         /**
          * Sets the value of the property based on a string.
          * The string should be two floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~Vec2ActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 2 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT Vec2fActorProperty :
      public GenericActorProperty<const osg::Vec2f&,osg::Vec2f>
   {
      public:
         typedef GenericActorProperty<const osg::Vec2f&, osg::Vec2f> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec2fActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::VEC2F, name,label,set,get,desc,groupName) 
         { 
            SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
         }

         /**
          * Sets the value of the property based on a string.
          * The string should be two floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~Vec2fActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 2 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT Vec2dActorProperty :
      public GenericActorProperty<const osg::Vec2d&,osg::Vec2d>
   {
      public:
         typedef GenericActorProperty<const osg::Vec2d&, osg::Vec2d> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec2dActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::VEC2D, name,label,set,get,desc,groupName) 
         { 
            SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
         }

         /**
          * Sets the value of the property based on a string.
          * The string should be two floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~Vec2dActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 3 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT Vec3ActorProperty :
      public GenericActorProperty<const osg::Vec3&,osg::Vec3>
   {
      public:
         typedef GenericActorProperty<const osg::Vec3&, osg::Vec3> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec3ActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::VEC3, name,label,set,get,desc,groupName) 
         { 
            SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
         }

         /**
          * Sets the value of the property based on a string.
          * The string should be three floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;
      protected:
         virtual ~Vec3ActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 3 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT Vec3fActorProperty :
      public GenericActorProperty<const osg::Vec3f&,osg::Vec3f>
   {
      public:
         typedef GenericActorProperty<const osg::Vec3f&, osg::Vec3f> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec3fActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::VEC3F, name,label,set,get,desc,groupName) 
         {
            SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
         }

         /**
          * Sets the value of the property based on a string.
          * The string should be three floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;
      protected:
         virtual ~Vec3fActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 3 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT Vec3dActorProperty :
      public GenericActorProperty<const osg::Vec3d&,osg::Vec3d>
   {
      public:
         typedef GenericActorProperty<const osg::Vec3d&, osg::Vec3d> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec3dActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::VEC3D, name,label,set,get,desc,groupName) 
         { 
            SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
         }

         /**
          * Sets the value of the property based on a string.
          * The string should be three floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;
      protected:
         virtual ~Vec3dActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 4 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT Vec4ActorProperty :
      public GenericActorProperty<const osg::Vec4&,osg::Vec4>
   {
      public:
         typedef GenericActorProperty<const osg::Vec4&, osg::Vec4> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec4ActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "", 
                           dtDAL::DataType& type = DataType::VEC4) :
         BaseClass(type, name,label,set,get,desc,groupName) 
         { 
            SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
         }

         /**
          * Sets the value of the property based on a string.
          * The string should be four floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:

         virtual ~Vec4ActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 4 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT Vec4fActorProperty :
      public GenericActorProperty<const osg::Vec4f&,osg::Vec4f>
   {
      public:
         typedef GenericActorProperty<const osg::Vec4f&, osg::Vec4f> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec4fActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::VEC4F, name,label,set,get,desc,groupName) 
         { 
            SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
         }

         /**
          * Sets the value of the property based on a string.
          * The string should be four floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~Vec4fActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a vector data member with 4 values.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT Vec4dActorProperty :
      public GenericActorProperty<const osg::Vec4d&,osg::Vec4d>
   {
      public:
         typedef GenericActorProperty<const osg::Vec4d&, osg::Vec4d> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         Vec4dActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(DataType::VEC4D, name,label,set,get,desc,groupName) 
         { 
            SetNumberPrecision(2 * sizeof(GetValue()[0]) + 1);
         }

         /**
          * Sets the value of the property based on a string.
          * The string should be four floating point values as a strings separated by spaces.
          * This will return false if the values can't be set.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);


         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const;

      protected:
         virtual ~Vec4dActorProperty() { }
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a color data member with 4 floats.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_DAL_EXPORT ColorRgbaActorProperty : public Vec4ActorProperty
   {
      public:
         typedef Vec4ActorProperty BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;

         ColorRgbaActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                              SetFuncType set, GetFuncType get,
                              const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "") :
         BaseClass(name,label,set,get,desc,groupName, DataType::RGBACOLOR) { }

         // This is a work around a bug in Visual Studio where the Unit Tests would fail at runtime because
         // it couldn't find these functions in this class, even though they are inherited.
         virtual bool FromString(const std::string& value) { return Vec4ActorProperty::FromString(value); }
         virtual const std::string ToString() const      { return Vec4ActorProperty::ToString(); }

      protected:
         virtual ~ColorRgbaActorProperty() { }
   };

}

#endif //DELTA_ENGINE_PROPERTY_TYPES
