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
#include <dtCore/export.h>
#include <dtCore/abstractparameter.h>

namespace dtCore
{
   class DataType;
   class ObjectType;

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
   class DT_CORE_EXPORT ActorProperty : public AbstractParameter
   {
      public:

         /**
          * Initializes the default value for this property.
          *
          * @param[in]  keyName  The key for this property.
          */
         virtual void InitDefault(ObjectType& type);

         /**
          * @return Returns an enumeration of the data type that this property
          * represents.
          */
         DataType& GetPropertyType() const { return GetDataType(); }

         /**
          * Retrieves a human readable version of the property's value.
          */
         virtual std::string GetValueString() const {return ToString();}

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
          * Copies the metadata otherProp to this property.  This includes all the boolean flags, but not
          * the actual value of the property.  One can override this if a subclass has additional metadata.
          */
         virtual void CopyMetadata(const ActorProperty& otherProp);

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
          * @return true if this property should be send is partial network update messages.
          * @note defaults to false
          */
         bool GetSendInPartialUpdate() const { return mSendInPartialUpdate; }

         /**
          * Sets if this property should be send is partial network update messages.
          */
         void SetSendInPartialUpdate(bool sendInPartialUpdate) { mSendInPartialUpdate = sendInPartialUpdate; }

         /**
          * @return true if this property should be sent in full network update messages.
          * @note defaults to true;
          */
         bool GetSendInFullUpdate() const { return mSendInFullUpdate; }

         /**
          * Sets if this property should be sent in full network update messages.
          */
         void SetSendInFullUpdate(bool sendInFullUpdate) { mSendInFullUpdate = sendInFullUpdate; }

         /**
          * @return true if this property should be ignored when saving even if it is not read-only and value has been set.
          * @note defaults to false
          */
         bool GetIgnoreWhenSaving() const { return mIgnoreWhenSaving; }

         /**
          * Sets if this property should be ignored when saving even if it is not read-only and value has been set.
          */
         void SetIgnoreWhenSaving(bool ignore) { mIgnoreWhenSaving = ignore; }

         /*
          * @return true if this property should be always saved even if it is not read-only and value has been set.
          * @note defaults to false
          */
         bool GetAlwaysSave() const { return mAlwaysSave; }

         /**
          * Sets if this property should be send is partial network update messages.
          */
         void SetAlwaysSave(bool alwaysSave) { mAlwaysSave = alwaysSave; }

         /**
          *	Sets whether this property can be edited when there are multiple
          * containers selected with this property.
          */
         void SetMultipleEdit(bool enabled) { mMultipleEdit = enabled; }

         bool GetMultipleEdit() const { return mMultipleEdit; }

         /**
          * Sets if this property is advanced, and could be hidden as an advanced property.
          * @note defaults to false
          */
         void SetAdvanced(bool enabled) { mAdvanced = enabled; }

         /// @return true if this property is marked as advanced
         bool GetAdvanced() const { return mAdvanced; }

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
          * @param precision The amount of numeric precision.
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
         ActorProperty(dtCore::DataType& dataType,
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
          * hidden copy constructor
          */
         ActorProperty(const ActorProperty& toCopy);

         /**
          * hidden operator=
          */
         ActorProperty& operator=(const ActorProperty&);

         /* **********
          * Flags Section.  Should should remain at the end so that they pack nicely.
          */

         /**
          * Flag the identifies if a property is read only or read/write
          */
         bool mReadOnly : 1;

         /**
          *	Flag that indicates whether this property is read only when there
          * are multiple containers selected.
          */
         bool mMultipleEdit : 1;

         bool mSendInPartialUpdate : 1;
         bool mSendInFullUpdate : 1;
         bool mAdvanced : 1;
         bool mIgnoreWhenSaving : 1;
         bool mAlwaysSave : 1;

   };

   DT_CORE_EXPORT std::ostream& operator << (std::ostream& o, const ActorProperty& prop);

   DT_CORE_EXPORT std::istream& operator >> (std::istream& i, ActorProperty& prop);

   typedef dtCore::RefPtr<dtCore::ActorProperty> PropertyPtr;
}

#endif
