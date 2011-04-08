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

#include <iosfwd>

#include <osg/Referenced>
#include <dtDAL/export.h>
#include <dtDAL/abstractparameter.h>

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
          * @return Returns an enumeration of the data type that this property
          * represents.
          */
         DataType& GetPropertyType() const { return GetDataType(); }

         /**
          * Retrieves a human readable version of the property's value.
          */
         virtual std::string GetValueString() {return ToString();}

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
         const std::string& GetLabel() const { return mLabel; }

         /**
          * Gets the description assigned to this property.
          */
         const std::string& GetDescription() const { return mDescription; }

         /**
          * Gets the read only state of a property
          */
         bool IsReadOnly() const { return mReadOnly; }

         /**
          * Sets the read only state of a property
          */
         void SetReadOnly(bool readOnly) { mReadOnly = readOnly; }

         /**
          * Set the group name
          * @param name The desired group name
          */
         void SetGroupName(const std::string& name) { mGroupName = name; }

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
         void SetNumberPrecision(unsigned int precision);

         /**
          * Gets the current floating point precision value on this
          * parameter.
          * @return The current number precision.
          */
         unsigned int GetNumberPrecision() const;

         /// write the data to a data stream
         virtual void ToDataStream(dtUtil::DataStream& stream) const;

         /// read the data from a data stream.
         virtual bool FromDataStream(dtUtil::DataStream& stream);

      protected:

         /**
          * Constructs the actor property.  This is protected since the real
          * property functionality comes from classes extending this one.
          */
         ActorProperty(dtDAL::DataType& dataType,
                       const dtUtil::RefString& name,
                       const dtUtil::RefString& label,
                       const dtUtil::RefString& desc,
                       const dtUtil::RefString& groupName,
                       bool  readOnly = false);


         /**
          * Protected since actor properties are smart pointers.
          */
         virtual ~ActorProperty();

      private:
         /// The name of the property's group
         dtUtil::RefString mGroupName;

         ///Label for use in UI applications that which to display the property.
         dtUtil::RefString mLabel;

         ///Description of what the property represents.
         dtUtil::RefString mDescription;

         ///the precision used for floating point numbers when doing a GetStringValue and SetStringValue
         unsigned int mNumberPrecision;

         /**
          * Flag the identifies if a property is read only or read/write
          */
         bool mReadOnly;

         /**
          * hidden copy constructor
          */
         ActorProperty(const ActorProperty& toCopy);

         /**
          * hidden operator=
          */
         ActorProperty& operator=(const ActorProperty&);

   };

   DT_DAL_EXPORT std::ostream& operator << (std::ostream& o, const ActorProperty& prop);

   DT_DAL_EXPORT std::istream& operator >> (std::istream& i, ActorProperty& prop);
}

#endif
