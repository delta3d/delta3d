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
#ifndef DELTA_ARRAY_ACTOR_PROPERTY
#define DELTA_ARRAY_ACTOR_PROPERTY

#include <string>
#include <dtDAL/actorproperty.h>
#include <dtDAL/export.h>
#include <dtDAL/arrayactorpropertybase.h>

namespace dtDAL
{
   /**
    * @brief An actor property that handles an array of data.
    */
   template <class T>
   class ArrayActorProperty : public ArrayActorPropertyBase
   {
   public:
      typedef dtUtil::Functor<void, TYPELIST_1(int)> SetIndexFuncType;
      typedef dtUtil::Functor<T, TYPELIST_0()> GetDefaultFuncType;
      typedef dtUtil::Functor<std::vector<T>, TYPELIST_0()> GetArrayFuncType;
      typedef dtUtil::Functor<void, TYPELIST_1(const std::vector<T>&)> SetArrayFuncType;

      /**
       * Creates a new array actor property.
       * @param[in]  name            The name used to access this property.
       * @param[in]  label           A more human readable name to use when displaying the property.
       * @param[in]  desc            A longer description of the property.
       * @param[in]  setIndexFunc    Function callback to notify owner that the index has changed.
       * @param[in]  getDefaultFunc  Function callback to retrieve the default value of the array data.
       * @param[in]  getArrayFunc    Function callback that retrieves the array.
       * @param[in]  propertyType    The property type to use in each index.
       * @param[in]  groupName       The property grouping to use when editing properties visually.
       * @param[in]  editorType      A string specifying what type of editor to use for the data in this group.  These
       *                             will specify a UI to use in STAGE.
       * @param[in]  readOnly        True if this property should not be editable.  Defaults to false.
       */
      ArrayActorProperty(const std::string& name,
                         const std::string& label,
                         const std::string& desc,
                         SetIndexFuncType setIndexFunc,
                         GetDefaultFuncType getDefaultFunc,
                         GetArrayFuncType getArrayFunc,
                         SetArrayFuncType setArrayFunc,
                         ActorProperty* propertyType,
                         const std::string& groupName,
                         const std::string& editorType = "",
                         bool canReorder = true,
                         bool readOnly = false)
          : ArrayActorPropertyBase(name, label, desc, propertyType, groupName, editorType, canReorder, readOnly)
          , mSetIndexFunc(setIndexFunc)
          , mGetDefaultFunc(getDefaultFunc)
          , mGetArrayFunc(getArrayFunc)
          , mSetArrayFunc(setArrayFunc)
      {
      }

      /**
      * Gets the array.
      */
      virtual std::vector<T> GetValue()
      {
         return mGetArrayFunc();
      }

      /**
      * Sets the array.
      */
      virtual void SetValue(const std::vector<T>& value)
      {
         mSetArrayFunc(value);
      }

      /**
      * Gets the default value of an array element.
      */
      virtual T GetDefault()
      {
         return mGetDefaultFunc();
      }

      /**
      * Gets the total size of the array.
      */
      virtual int GetArraySize() const
      {
         return (int)mGetArrayFunc().size();
      }

      /**
      * Sets the current active index.
      */
      virtual void SetIndex(int index) const
      {
         mSetIndexFunc(index);
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
         // Check if we are at our size limit.
         if (mMaxSize > -1 && GetArraySize() >= mMaxSize)
         {
            LOG_WARNING("ArrayActorProperty is attempting to insert a new entry when the array is of max size");
            return false;
         }

         SetIndex(index);
         std::vector<T> value = GetValue();
         if (index < GetArraySize())
         {
            value.insert(value.begin() + index, GetDefault());
         }
         else
         {
            value.push_back(GetDefault());
         }
         SetValue(value);
         return true;
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
         // Check if we are at our size limit.
         if (mMinSize > -1 && GetArraySize() <= mMinSize)
         {
            LOG_WARNING("ArrayActorProperty is attempting to remove an entry when the array is already of minimum size");
            return false;
         }

         SetIndex(index);
         std::vector<T> value = GetValue();
         if (index < GetArraySize())
         {
            value.erase(value.begin() + index);
         }
         SetValue(value);
         return true;
      }

      /**
      * Clears the array.
      */
      virtual void Clear()
      {
         int arrayIndex = 0;
         if (mMinSize > -1)
         {
            arrayIndex = mMinSize;
         }

         while(arrayIndex < GetArraySize())
         {
            if (!Remove(arrayIndex))
            {
               break;
            }
         }
      }

      /**
      * Swaps the contents of the current index with the given.
      *
      * @param[in]  first   The first index to swap.
      * @param[in]  second  The second index to swap.
      */
      virtual void Swap(int first, int second)
      {
         std::vector<T> value = GetValue();
         if (first < GetArraySize() && second < GetArraySize())
         {
            T data = value[first];
            value[first] = value[second];
            value[second] = data;
         }
         SetValue(value);
      }

      /**
      * Copies the contents of the current index with the given.
      *
      * @param[in]  src  The source index to copy from.
      * @param[in]  dst  The destination index to copy to.
      */
      virtual void Copy(int src, int dst)
      {
         std::vector<T> value = GetValue();
         if (src < GetArraySize() && dst < GetArraySize())
         {
            value[dst] = value[src];
         }
         SetValue(value);
      }

   protected:

      /**
      * Functor to notify the owner than the current array index has changed.
      *
      * @param[in]  index  The new current index.
      */
      SetIndexFuncType mSetIndexFunc;

      /**
      * Functor to retrieve the default value of the data when inserting a new
      * element into the array.
      *
      * @return  The default value of the data in the array.
      */
      GetDefaultFuncType mGetDefaultFunc;

      /**
      * Functor to get the contents of the array.
      *
      * @return  Should return a reference to the array data.
      */
      GetArrayFuncType mGetArrayFunc;

      /**
      * Functor to set the contents of the array.
      *
      * @param[in]  value  The array to set with.
      */
      SetArrayFuncType mSetArrayFunc;
   };
}
#endif
