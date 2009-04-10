/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * Jeffrey Houde
 */
#ifndef DELTA_ARRAY_ACTOR_PROPERTY_BASE
#define DELTA_ARRAY_ACTOR_PROPERTY_BASE

#include <string>
#include <dtDAL/actorproperty.h>
#include <dtDAL/export.h>

#include <dtUtil/log.h>

#include <dtUtil/macros.h>

const char OPEN_CHAR = 1;
const char CLOSE_CHAR = 2;

namespace dtDAL
{
   /**
    * @brief An actor property that contains a structure of NamedParameter Objects.
    *
    * This actor property solves the concept of both structures and lists in actor properties.
    * This property gets and sets a NamedGroupParameter, which can hold any number of other NamedParameters.
    * This is passed to the functors for the actor property which must then handle the data in way that makes
    * sense the actor.  Additionally, a string name can be passed to the property in the constructor
    *
    * @note When loading and saving GroupActorProperties, only the NamedParameter types that have associated actor property types
    *       can be saved.  The rest will be ignored.  There are no plans no solve this issue.
    * @note Also, note that currently Vec3d and Vec3f parameters will be loaded as Vec3. There are plans to fix this.
    */
   class ArrayActorPropertyBase : public ActorProperty
   {
   public:

      /**
      * Creates a new array actor property.
      * @param[in]  name            The name used to access this property.
      * @param[in]  label           A more human readable name to use when displaying the property.
      * @param[in]  desc            A longer description of the property.
      * @param[in]  propertyType    The property type to use in each index.
      * @param[in]  groupName       The property grouping to use when editing properties visually.
      * @param[in]  editorType      A string specifying what type of editor to use for the data in this group.  These
      *                             will specify a UI to use in STAGE.
      * @param[in]  readOnly        True if this property should not be editable.  Defaults to false.
      */
      ArrayActorPropertyBase(const std::string& name,
                              const std::string& label,
                              const std::string& desc,
                              ActorProperty* propertyType,
                              const std::string& groupName,
                              const std::string& editorType = "",
                              bool canReorder = true,
                              bool readOnly = false)
         : ActorProperty(DataType::ARRAY, name, label, desc, groupName, readOnly)
         , mPropertyType(propertyType)
         , mCanReorder(canReorder)
         , mMinSize(-1)
         , mMaxSize(-1)
      {
      }

      /**
       * Sets the value of the property based on a string.
       * @note This method will attempt to use the data to set the value, but it may return false if the data
       * could not be used.
       * @param value the value to set.
       * @return true if the string was usable to set the value, false if not.
       */
      virtual bool FromString(const std::string& value)
      {
         if (!mPropertyType.valid())
         {
            return false;
         }

         std::string data = value;

         // First read the total size of the array.
         std::string token = TakeToken(data);

         int arraySize = atoi(token.c_str());
         for (int index = 0; index < arraySize; index++)
         {
            SetIndex(index);
            token = TakeToken(data);
            mPropertyType->FromString(token);
         }

         return true;
      }

      /**
      * Reads the next token form the given string data.
      * This will also remove the token from the data string
      * and return you the token (with the open and close characters removed).
      * The beginning of the data string must always begin with
      * an opening character or this will cause problems.
      *
      * @param[in]  data  The string data.
      *
      * @return            The first token from the string data.
      */
      std::string TakeToken(std::string& data)
      {
         std::string returnData;

         // If the first character in the data string is not the opening character,
         //  we will just assume the entire data string is the token.
         if (data.c_str()[0] != OPEN_CHAR)
         {
            returnData = data;
            data = "";
         }

         int depth = 0;
         int dataIndex = 0;
         while (data.length() > 0)
         {
            bool appendChar = true;

            // Opening characters increase the depth counter.
            if (data[dataIndex] == OPEN_CHAR)
            {
               depth++;

               if (depth == 1)
               {
                  appendChar = false;
               }
            }
            // Closing characters decrease the depth counter.
            else if (data[dataIndex] == CLOSE_CHAR)
            {
               depth--;

               if (depth == 0)
               {
                  appendChar = false;
               }
            }

            // All other characters are added to the return buffer.
            if (appendChar)
            {
               returnData.append(data.c_str(), 1);
            }

            // Remove the left most character from the data string.
            data = &data[1];

            // We are done once our depth returns to 0.
            if (depth <= 0)
            {
               break;
            }
         }

         return returnData;
      }

      /**
       * @return a string version of the data.  This value can be used when calling SetStringValue.
       * @see #SetStringValue
       */
      virtual const std::string ToString() const
      {
         if (!mPropertyType.valid())
         {
            return "";
         }

         // Iterate through each index in the array and append the strings.
         int arraySize = GetArraySize();

         char buffer[20] = {0,};
         std::string data;
         data += OPEN_CHAR;
         snprintf(buffer, 20, "%d", arraySize);
         data += buffer;
         data += CLOSE_CHAR;

         for (int index = 0; index < arraySize; index++)
         {
            SetIndex(index);
            data += OPEN_CHAR;
            data += mPropertyType->ToString();
            data += CLOSE_CHAR;
         }

         return data;
      }

      /**
       * This is overridden to make handle the fact that the get method returns a refptr.
       * @param otherProp The property to copy from.
       */
      virtual void CopyFrom(const ActorProperty& otherProp)
      {
         if (GetDataType() != otherProp.GetDataType())
         {
            LOG_ERROR("Property types are incompatible. Cannot make copy.");
            return;
         }
      }

      /**
      * This gets the property used in this array.
      */
      virtual ActorProperty* GetArrayProperty()
      {
         return mPropertyType.get();
      }

      virtual const ActorProperty* GetArrayProperty() const
      {
         return mPropertyType.get();
      }

      /**
      * Sets the minimum size of the array.
      */
      virtual void SetMinArraySize(int minSize)
      {
         mMinSize = minSize;

         // TODO ARRAY: Make sure to add indexes if the minimum size is larger than the current size.
      }
      virtual int GetMinArraySize() const {return mMinSize;}

      /**
      * Sets the maximum size of the array.
      */
      virtual void SetMaxArraySize(int maxSize)
      {
         mMaxSize = maxSize;

         // TODO ARRAY: Make sure to remove indexes if the maximum size exceeds the current size.
      }
      virtual int GetMaxArraySize() const {return mMaxSize;}

      /**
      * Gets whether or not this array can be re-ordered.
      */
      virtual bool CanReorder() const {return mCanReorder;}

      /**
      * Gets the total size of the array.
      */
      virtual int GetArraySize() const
      {
         return 0;
      }

      /**
      * Sets the current active index.
      */
      virtual void SetIndex(int index) const
      {
      }

      /**
      * Inserts a new index into the array.
      *
      * @param[in]  index  The index to insert at.
      *
      * @return     True if an element was inserted properly.
      */
      virtual bool Insert(int index)
      {
         return false;
      }

      /**
      * Removes an index from the array.
      *
      * @param[in]  index  The index to remove from.
      *
      * @return     True if an element was removed properly.
      */
      virtual bool Remove(int index)
      {
         return false;
      }

      /**
      * Clears the array.
      */
      virtual void Clear()
      {
      }

      /**
      * Swaps the contents of the current index with the given.
      *
      * @param[in]  first   The first index to swap.
      * @param[in]  second  The second index to swap.
      */
      virtual void Swap(int first, int second)
      {
      }

      /**
      * Copies the contents of the current index with the given.
      *
      * @param[in]  src  The source index to copy from.
      * @param[in]  dst  The destination index to copy to.
      */
      virtual void Copy(int src, int dst)
      {
      }

   protected:

      virtual ~ArrayActorPropertyBase()
      {
      }

      /// The property that each array index uses.
      dtCore::RefPtr<ActorProperty> mPropertyType;

      /// Can this array be re-ordered via shift up and down?
      bool           mCanReorder;

      /// Size limits on the array.
      int            mMinSize;
      int            mMaxSize;
   };
}
#endif
