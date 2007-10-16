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
 */
#ifndef DELTA_ACTORPROPERTY
#define DELTA_ACTORPROPERTY

#include <iostream>

#include <osg/Referenced>
#include <dtDAL/functor.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/export.h>
#include <dtDAL/abstractparameter.h>
#include <dtUtil/log.h>

#include <dtUtil/breakoverride.h>

namespace dtDAL
{

   class DataType;

   /**
    * The actor property class provides a get/set mechanism for
    * accessing properties of a class.  The get/set portions of the
    * property are functor objects which point to a getter and
    * setter method on a particular class.  Therefore, when a property
    * is created, it knows how to set and get the data it represents.
    * @note
    *      There are, by default, a set of classes that map to the
    *      data types listed in the Delta3D datatypes enumeration.  The
    *      most common/preferred method signatures for each property
    *      type is supported.  If, however, the need arises for completely
    *      custom properties, this is possible through the GenericActorProperty
    *      class.
    *  @see DataType
    *  @see GenericActorProperty
    */
   class DT_DAL_EXPORT ActorProperty : public AbstractParameter
   {
      public:

         /**
          * Gets the unique name assigned to this property.
          */
         virtual const std::string &GetName() const { return mName; }

         ///@return the type of this property.
         virtual const DataType &GetDataType() const { return mDataType; };

         /**
         * This is an alias to GetStringValue.
         * @see #GetStringValue
         * @return the value of the property as a string.
         */
         //virtual const std::string ToString() const { return GetStringValue(); };

         /**
         * This is an alias to SetStringValue.
         * @see #SetStringValue
         * @return true if the property could be set via the string.
         */
         //virtual bool FromString(const std::string &value) { return SetStringValue(value); }

         /**
         * @return Returns an enumeration of the data type that this property
         * represents.
         */
         DataType &GetPropertyType() const { return mDataType; };

         /**
          * Assigns the value of this property the value contained in the
          * specified property.
          * @param otherProp The property containing the value to copy.
          * @note
          *  This method exists to provide subclasses the opportunity to assign
          *  values to themselves from other properties in a generic manor.
          *  For example, if there is a list of generic actor properties of
          *  varying types, using this method, you can loop through each list
          *  and merely call this method to copy one property value to another.
          * @note
          *  This is not like a copy constructor or assignment operator.  The
          *  is merely a method to set the "value" of a property from another property.
          * @see GenericActorProperty#copyFrom
          */
         virtual void CopyFrom(const ActorProperty& otherProp) = 0;


         /**
          * Gets the label assigned to this property.
          */
         const std::string &GetLabel() const { return mLabel; }

         /**
          * Gets the description assigned to this property.
          */
         const std::string &GetDescription() const { return mDescription; }

         /**
          * Gets the read only state of a property
          */
         bool IsReadOnly() const { return mReadOnly; }

         /**
          * Sets the read only state of a property
          */
         void SetReadOnly(bool readOnly) { mReadOnly = readOnly; }

         /**
          * Sets the value of the property based on a string.
          * @note This method will attempt to use the data to set the value, but it may return false if the data
          * could not be used.
          * @param value the value to set.
          * @return true if the string was usable to set the value, false if not.
          */
         //virtual bool SetStringValue(const std::string& value) = 0;

         /**
          * @return a string version of the data.  This value can be used when calling SetStringValue.
          * @see #SetStringValue
          */
         //virtual const std::string GetStringValue() const = 0;

         /**
          * Set the group name
          * @param name The desired group name
          */
         void SetGroupName(const std::string &name) { mGroupName = name; }

         /**
          * Get the group name
          * @return The current group name
          */
         const std::string& GetGroupName() const { return mGroupName; }

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

      protected:
         /**
          * Constructs the actor property.  This is protected since the real
          * property functionality comes from classes extending this one.
          */
         ActorProperty(dtDAL::DataType& dataType, 
                     const std::string &name,
                     const std::string &label,
                     const std::string &desc,
                     const std::string &groupName,
                     bool  readOnly = false) :
            mDataType(dataType),
            mName(name),
            mLabel(label),
            mDescription(desc),
            mNumberPrecision(16),
            mReadOnly(readOnly)
            {
               groupName.empty() ? SetGroupName("Base") : SetGroupName(groupName);
            }

         /// The name of the property's group
         std::string mGroupName;

         /**
          * Protected since actor properties are smart pointers.
          */
         virtual ~ActorProperty() { }

      private:
         ///The datatype this property accesses
         DataType& mDataType;
      
         /**
          * Name of the property.  This is the key when searching for properties,
          * therefore, no two properties should have the same name.
          */
         std::string mName;

         ///Label for use in UI applications that which to display the property.
         std::string mLabel;

         ///Description of what the property represents.
         std::string mDescription;
           
         ///the precision used for floating point numbers when doing a GetStringValue and SetStringValue
         unsigned int mNumberPrecision;

         /**
          * Flag the identifies if a property is read only or read/write
          */
         bool mReadOnly;

         /**
          * hidden copy constructor
          */
         ActorProperty(const ActorProperty& toCopy): mDataType(toCopy.mDataType) { }

         /**
          * hidden operator=
          */
         ActorProperty& operator=(const ActorProperty&) { return *this; }


         ///deprecated 06/20/07  Use ToString()
         BREAK_OVERRIDE(GetStringValue())
         ///deprecated 06/20/07 Use FromString()
         BREAK_OVERRIDE(SetStringValue(const std::string&))
   };

   inline std::ostream& operator << (std::ostream& o, const ActorProperty& prop)
   {
      o << prop.ToString();
      return o;
   }

   inline std::istream& operator >> (std::istream& i, ActorProperty& prop)
   {
      std::string value;
      i >> value;
      prop.FromString(value);      
      return i;
   }
}

#endif
