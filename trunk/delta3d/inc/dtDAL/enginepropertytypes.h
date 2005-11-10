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
* @author Matthew W. Campbell, David Guthrie, William E. Johnson
*/
#ifndef DELTA_ENGINE_PROPERTY_TYPES
#define DELTA_ENGINE_PROPERTY_TYPES

#include <sstream>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Vec2>
#include <dtCore/deltadrawable.h>
#include "dtDAL/resourcedescriptor.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/export.h"

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
        ActorActorProperty(ActorProxy &actorProxy,
                         const std::string& name,
                         const std::string& label,
                         Functor1<ActorProxy*> Set,
                         Functor0Ret<dtCore::DeltaDrawable*> Get, 
                         const std::string& desiredActorClass = "",
                         const std::string& desc = "",
                         const std::string& groupName = "") :
                         ActorProperty(name, label, desc, groupName),
                         mProxy(&actorProxy),
                         SetPropFunctor(Set),
                         GetActorFunctor(Get), 
                         mDesiredActorClass(desiredActorClass)
        {

        }

        DataType& GetPropertyType() const { return DataType::ACTOR; }

        /**
         * Copies a ResourceActorProperty value to this one from the property
         * specified. This method fails if otherProp is not a ResourceActorProperty.
         * @param otherProp The property to copy the value from.
         */
        virtual void CopyFrom(ActorProperty* otherProp)
        {
            ActorActorProperty *prop = dynamic_cast<ActorActorProperty *>(otherProp);
            if (prop != NULL)
                SetValue(prop->GetValue());
            else
                LOG_ERROR("Property types are incompatible. Cannot make a copy.");
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
        ActorProxy* GetValue();

        /**
        * Gets the value proxy assiged to this property.
        * Hack for the resource class
        * @return the currently set ActorProxy for this property.
        */
        const ActorProxy* GetValue() const;

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
        virtual bool SetStringValue(const std::string& value);

        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const;

        /**
         * @return the class of proxy this expects so that the UI can filter the list.
         */
        const std::string& GetDesiredActorClass() const { return mDesiredActorClass; }
    private:
        ActorProxy *mProxy;
        Functor1<ActorProxy*> SetPropFunctor;
        Functor0Ret<dtCore::DeltaDrawable*> GetActorFunctor;
        std::string mDesiredActorClass;
    protected:
        virtual ~ActorActorProperty() { }
    };

    ////////////////////////////////////////////////////////////////////////////
    /**
     * This actor property represents a resource
     */
    ////////////////////////////////////////////////////////////////////////////
    class DT_DAL_EXPORT ResourceActorProperty : public ActorProperty
    {
    public:
        ResourceActorProperty(ActorProxy &actorProxy,
                         DataType &type,
                         const std::string &name,
                         const std::string &label,
                         Functor1<const std::string&> Set,
                         const std::string &desc = "",
                         const std::string &groupName = "") :
                         ActorProperty(name, label, desc, groupName),
                         mDataType(&type),
                         mProxy(&actorProxy),
                         SetPropFunctor(Set)
        {

        }

        DataType& GetPropertyType() const { return *mDataType; }

        /**
         * Copies a ResourceActorProperty value to this one from the property
         * specified. This method fails if otherProp is not a ResourceActorProperty.
         * @param otherProp The property to copy the value from.
         */
        virtual void CopyFrom(ActorProperty *otherProp)
        {
            ResourceActorProperty *prop = dynamic_cast<ResourceActorProperty *>(otherProp);
            if (prop != NULL)
                SetValue(prop->GetValue());
            else
                LOG_ERROR("Property types are incompatible. Cannot make copy.");
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
        virtual bool SetStringValue(const std::string& value);

        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const;

    private:
        DataType *mDataType;
        ActorProxy *mProxy;
        Functor1<const std::string&> SetPropFunctor;
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
        FloatActorProperty(const std::string &name, const std::string &label,
            Functor1<float> set, Functor0Ret<float> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<float,float>(name,label,set,get,desc,groupName) { }

        DataType& GetPropertyType() const { return DataType::FLOAT; }

        /**
         * Sets the value of the property based on a string.
         * The string should be a float value as a string.  This will set the value to 0 if it's not valid.
         * @note Returns false it the property is read only
         * @param value the value to set.
         * @return true
         */
        virtual bool SetStringValue(const std::string& value);

        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const;
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
        DoubleActorProperty(const std::string &name, const std::string &label,
            Functor1<double> set, Functor0Ret<double> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<double,double>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::DOUBLE; }

        /**
         * Sets the value of the property based on a string.
         * The string should be a double value as a string.  This will set the value to 0 if it's not valid.
         * @note Returns false it the property is read only
         * @param value the value to set.
         * @return true
         */
        virtual bool SetStringValue(const std::string& value);

        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const ;
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
        IntActorProperty(const std::string &name, const std::string &label,
            Functor1<int> set, Functor0Ret<int> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<int,int>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::INT; }

        /**
         * Sets the value of the property based on a string.
         * The string should be an integer value as a string.  This will set the value to 0 if it's not valid.
         * @note Returns false it the property is read only
         * @param value the value to set.
         * @return true
         */
        virtual bool SetStringValue(const std::string& value);

        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const;
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
        LongActorProperty(const std::string &name, const std::string &label,
            Functor1<long> set, Functor0Ret<long> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<long,long>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::LONGINT; }

        /**
         * Sets the value of the property based on a string.
         * The string should be a long value as a string.  This will set the value to 0 if it's not valid.
         * @note Returns false it the property is read only
         * @param value the value to set.
         * @return true
         */
        virtual bool SetStringValue(const std::string& value);

        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const;
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
    class DT_DAL_EXPORT StringActorProperty : public GenericActorProperty<const std::string &,
        std::string>
    {
    public:
        StringActorProperty(const std::string &name, const std::string &label,
            Functor1<const std::string &> set,
            Functor0Ret<std::string> get,
            const std::string &desc = "",
            const std::string &groupName = "") :
        GenericActorProperty<const std::string &,std::string>
            (name,label,set,get,desc,groupName)
        {
            mMaxLength = 0;
        }

        DataType &GetPropertyType() const { return DataType::STRING; }

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
        virtual bool SetStringValue(const std::string& value) { SetValue(value); return true; }

        /**
         * @return the same as GetValue.
         */
        virtual const std::string GetStringValue() const { return GenericActorProperty<const std::string&,std::string>::GetValue(); }

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
        BooleanActorProperty(const std::string &name, const std::string &label,
            Functor1<bool> set, Functor0Ret<bool> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<bool,bool>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::BOOLEAN; }

        /**
         * Sets the value of the property based on a string.
         * The string should be a boolean value (true or false) as a string.
         * @note Returns false it the property is read only
         * @param value the value to set.
         * @return true if the value could be parsed.
         */
        virtual bool SetStringValue(const std::string& value);

        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const;

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
         * @return DataType::ENUMERATION
         */
        virtual DataType& GetPropertyType() const { return DataType::ENUMERATION; }

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
        EnumActorProperty(const std::string &name, const std::string &label,
            Functor1<T&> set, Functor0Ret<T&> get,
            const std::string &desc = "", const std::string &groupName = "") :
            GenericActorProperty<T&,T&>(name,label,set,get,desc,groupName) { }

        virtual const std::vector<dtUtil::Enumeration*>& GetList() const { return T::Enumerate(); }

        virtual DataType& GetPropertyType() const { return AbstractEnumActorProperty::GetPropertyType(); }

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
        virtual bool SetStringValue(const std::string& value) { return SetValueFromString(value); }

        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const { return GetEnumValue().GetName(); };

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
        Vec2ActorProperty(const std::string &name, const std::string &label,
            Functor1<const osg::Vec2&> set, Functor0Ret<osg::Vec2> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<const osg::Vec2&,osg::Vec2>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::VEC2; }

        /**
         * Sets the value of the property based on a string.
         * The string should be two floating point values as a strings separated by spaces.
         * This will return false if the values can't be set.
         * @note Returns false it the property is read only
         * @param value the value to set.
         * @return true
         */
        virtual bool SetStringValue(const std::string& value);


        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const;

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
        Vec2fActorProperty(const std::string &name, const std::string &label,
            Functor1<const osg::Vec2f&> set, Functor0Ret<osg::Vec2f> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<const osg::Vec2f&,osg::Vec2f>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::VEC2F; }

        /**
        * Sets the value of the property based on a string.
        * The string should be two floating point values as a strings separated by spaces.
        * This will return false if the values can't be set.
        * @note Returns false it the property is read only
        * @param value the value to set.
        * @return true
        */
        virtual bool SetStringValue(const std::string& value);


        /**
        * @return a string version of the data.  This value can be used when calling SetStringValue.
        * @see #SetStringValue
        */
        virtual const std::string GetStringValue() const;

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
        Vec2dActorProperty(const std::string &name, const std::string &label,
            Functor1<const osg::Vec2d&> set, Functor0Ret<osg::Vec2d> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<const osg::Vec2d&,osg::Vec2d>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::VEC2D; }

        /**
        * Sets the value of the property based on a string.
        * The string should be two floating point values as a strings separated by spaces.
        * This will return false if the values can't be set.
        * @note Returns false it the property is read only
        * @param value the value to set.
        * @return true
        */
        virtual bool SetStringValue(const std::string& value);


        /**
        * @return a string version of the data.  This value can be used when calling SetStringValue.
        * @see #SetStringValue
        */
        virtual const std::string GetStringValue() const;

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
        Vec3ActorProperty(const std::string &name, const std::string &label,
            Functor1<const osg::Vec3&> set, Functor0Ret<osg::Vec3> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<const osg::Vec3&,osg::Vec3>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::VEC3; }

        /**
         * Sets the value of the property based on a string.
         * The string should be three floating point values as a strings separated by spaces.
         * This will return false if the values can't be set.
         * @note Returns false it the property is read only
         * @param value the value to set.
         * @return true
         */
        virtual bool SetStringValue(const std::string& value);


        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const;
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
        Vec3fActorProperty(const std::string &name, const std::string &label,
            Functor1<const osg::Vec3f&> set, Functor0Ret<osg::Vec3f> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<const osg::Vec3f&,osg::Vec3f>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::VEC3F; }

        /**
        * Sets the value of the property based on a string.
        * The string should be three floating point values as a strings separated by spaces.
        * This will return false if the values can't be set.
        * @note Returns false it the property is read only
        * @param value the value to set.
        * @return true
        */
        virtual bool SetStringValue(const std::string& value);


        /**
        * @return a string version of the data.  This value can be used when calling SetStringValue.
        * @see #SetStringValue
        */
        virtual const std::string GetStringValue() const;
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
        Vec3dActorProperty(const std::string &name, const std::string &label,
            Functor1<const osg::Vec3d&> set, Functor0Ret<osg::Vec3d> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<const osg::Vec3d&,osg::Vec3d>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::VEC3D; }

        /**
        * Sets the value of the property based on a string.
        * The string should be three floating point values as a strings separated by spaces.
        * This will return false if the values can't be set.
        * @note Returns false it the property is read only
        * @param value the value to set.
        * @return true
        */
        virtual bool SetStringValue(const std::string& value);


        /**
        * @return a string version of the data.  This value can be used when calling SetStringValue.
        * @see #SetStringValue
        */
        virtual const std::string GetStringValue() const;
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
        Vec4ActorProperty(const std::string &name, const std::string &label,
            Functor1<const osg::Vec4&> set, Functor0Ret<osg::Vec4> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<const osg::Vec4&,osg::Vec4>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::VEC4; }

        /**
         * Sets the value of the property based on a string.
         * The string should be four floating point values as a strings separated by spaces.
         * This will return false if the values can't be set.
         * @note Returns false it the property is read only
         * @param value the value to set.
         * @return true
         */
        virtual bool SetStringValue(const std::string& value);


        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const;

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
        Vec4fActorProperty(const std::string &name, const std::string &label,
            Functor1<const osg::Vec4f&> set, Functor0Ret<osg::Vec4f> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<const osg::Vec4f&,osg::Vec4f>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::VEC4F; }

        /**
        * Sets the value of the property based on a string.
        * The string should be four floating point values as a strings separated by spaces.
        * This will return false if the values can't be set.
        * @note Returns false it the property is read only
        * @param value the value to set.
        * @return true
        */
        virtual bool SetStringValue(const std::string& value);


        /**
        * @return a string version of the data.  This value can be used when calling SetStringValue.
        * @see #SetStringValue
        */
        virtual const std::string GetStringValue() const;

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
        Vec4dActorProperty(const std::string &name, const std::string &label,
            Functor1<const osg::Vec4d&> set, Functor0Ret<osg::Vec4d> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<const osg::Vec4d&,osg::Vec4d>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::VEC4D; }

        /**
        * Sets the value of the property based on a string.
        * The string should be four floating point values as a strings separated by spaces.
        * This will return false if the values can't be set.
        * @note Returns false it the property is read only
        * @param value the value to set.
        * @return true
        */
        virtual bool SetStringValue(const std::string& value);


        /**
        * @return a string version of the data.  This value can be used when calling SetStringValue.
        * @see #SetStringValue
        */
        virtual const std::string GetStringValue() const;

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
        ColorRgbaActorProperty(const std::string &name, const std::string &label,
            Functor1<const osg::Vec4&> set, Functor0Ret<osg::Vec4> get,
            const std::string &desc = "", const std::string &groupName = "") :
        Vec4ActorProperty(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::RGBACOLOR; }
        
        // This is a work around a bug in Visual Studio where the Unit Tests would fail at runtime because
        // it couldn't find these functions in this class, even though they are inherited. 
        virtual bool SetStringValue(const std::string& value) { return Vec4ActorProperty::SetStringValue(value); }
        virtual const std::string GetStringValue() const      { return Vec4ActorProperty::GetStringValue(); } 

    protected:
        virtual ~ColorRgbaActorProperty() { }
    };


}

#endif //DELTA_ENGINE_PROPERTY_TYPES
