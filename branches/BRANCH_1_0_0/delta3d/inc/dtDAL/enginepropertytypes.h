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
* @author Matthew W. Campbell
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

namespace dtDAL
{

    ////////////////////////////////////////////////////////////////////////////
    /**
     * This actor property represents an actor
     */
    ////////////////////////////////////////////////////////////////////////////
    typedef dtCore::DeltaDrawable Actor;
    class DT_EXPORT ActorActorProperty : public GenericActorProperty<Actor*,Actor*>
    {
    public:
        ActorActorProperty(const std::string &name, const std::string &label,
            Functor1<Actor*> set, Functor0Ret<Actor*> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<Actor*,Actor*>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::ACTOR; }

        /**
         * Sets the value of the property based on a string.
         * The string should be the actor id.
         * @param value the value to set.
         * @return false if an actor could not be found with that id.
         */
        virtual bool SetStringValue(const std::string& value);

        /**
         * @return a string version of the data.  This value can be used when calling SetStringValue.
         * @see #SetStringValue
         */
        virtual const std::string GetStringValue() const;

    protected:
        virtual ~ActorActorProperty() { }
    };

    ////////////////////////////////////////////////////////////////////////////
    /**
     * This actor property represents a resource
     */
    ////////////////////////////////////////////////////////////////////////////
    class DT_EXPORT ResourceActorProperty : public ActorProperty
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
    class DT_EXPORT FloatActorProperty : public GenericActorProperty<float,float>
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
    class DT_EXPORT DoubleActorProperty : public GenericActorProperty<double,double>
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
    class DT_EXPORT IntActorProperty : public GenericActorProperty<int,int>
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
    class DT_EXPORT LongActorProperty : public GenericActorProperty<long,long>
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
    class DT_EXPORT StringActorProperty : public GenericActorProperty<const std::string &,
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
    class DT_EXPORT BooleanActorProperty : public GenericActorProperty<bool,bool>
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
    class DT_EXPORT AbstractEnumActorProperty
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
    class DT_EXPORT Vec2ActorProperty :
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
    * This actor property represents a vector data member with 3 values.
    */
    ////////////////////////////////////////////////////////////////////////////
    class DT_EXPORT Vec3ActorProperty :
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
    * This actor property represents a vector data member with 4 values.
    */
    ////////////////////////////////////////////////////////////////////////////
    class DT_EXPORT Vec4ActorProperty :
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
     * This actor property represents a vector data member with 4 floats.
     */
    ////////////////////////////////////////////////////////////////////////////
    class DT_EXPORT ColorRgbaActorProperty :
        public GenericActorProperty<const osg::Vec4&,osg::Vec4>
    {
    public:
        ColorRgbaActorProperty(const std::string &name, const std::string &label,
            Functor1<const osg::Vec4&> set, Functor0Ret<osg::Vec4> get,
            const std::string &desc = "", const std::string &groupName = "") :
        GenericActorProperty<const osg::Vec4&,osg::Vec4>(name,label,set,get,desc,groupName) { }

        DataType &GetPropertyType() const { return DataType::RGBACOLOR; }

        /**
         * Sets the value of the property based on a string.
         * The string should be four floating point values as a strings separated by spaces.
         * This will return false if the values can't be set.
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
        virtual ~ColorRgbaActorProperty() { }
    };


}

#endif //DELTA_ENGINE_PROPERTY_TYPES
