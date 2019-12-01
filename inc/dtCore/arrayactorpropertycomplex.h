/* -*-c++-*-
 * Delta3D
 * Copyright 2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * 
 * David Guthrie
 */

#ifndef DELTA_ARRAY_ACTOR_PROPERTY_COMPLEX_H_
#define DELTA_ARRAY_ACTOR_PROPERTY_COMPLEX_H_
#include <dtUtil/typetraits.h>
#include <dtCore/arrayactorpropertybase.h>

namespace dtCore
{
   class ActorProperty;


   /**
    * @brief An actor property that handles an array of data.
    */
   template <class T>
   class ArrayActorPropertyComplex : public ArrayActorPropertyBase
   {
   public:
      typedef typename dtUtil::TypeTraits<T>::param_type SetType;
      typedef typename dtUtil::TypeTraits<T>::return_type GetType;
      typedef typename dtUtil::TypeTraits<T>::value_type value_type;

      typedef dtUtil::Functor<void, TYPELIST_2(int, SetType)> SetFuncType;
      typedef dtUtil::Functor<GetType, TYPELIST_1(int)> GetFuncType;
      typedef dtUtil::Functor<void, TYPELIST_1(int)> RemoveFuncType;
      typedef RemoveFuncType InsertFuncType;
      typedef dtUtil::Functor<size_t, TYPELIST_0()> GetSizeFuncType;

//      static dtUtil::RefPtr<ActorProperty> CreateInternalProperty()
//      {
//
//      }

      /**
       * Creates a new array actor property.  You must call SetArrayProperty after creation.
       * It is done this way because the property needs to have its get/set functors point
       * to GetCurrentValue/SetCurrentValue.
       * @param[in]  name            The name used to access this property.
       * @param[in]  label           A more human readable name to use when displaying the property.
       * @param[in]  set             Set value at an index functor
       * @param[in]  get             Get value at an index functor
       * @param[in]  insert          Insert a new item at an index, or at the end if the index == size
       * @param[in]  size            functor to return the current size.
       * @param[in]  remove          functor to remove an item at an index
       * @param[in]  desc            A longer description of the property.
       * @param[in]  groupName       The property grouping to use when editing properties visually.
       * @param[in]  editorType      A string specifying what type of editor to use for the data in this group.  These
       *                             will specify a UI to use in STAGE.
       * @param[in]  readOnly        True if this property should not be editable.  Defaults to false.
       */
      ArrayActorPropertyComplex(const std::string& name,
                         const std::string& label,
                         SetFuncType set,
                         GetFuncType get,
                         GetSizeFuncType size,
                         InsertFuncType insert,
                         RemoveFuncType remove,
                         const std::string& desc,
                         const std::string& groupName,
                         const std::string& editorType = "",
                         bool canReorder = true,
                         bool readOnly = false)
      : ArrayActorPropertyBase(name, label, desc, NULL, groupName, editorType, canReorder, readOnly)
      , mIndex(0)
      , mSetFunc(set)
      , mGetFunc(get)
      , mGetSizeFunc(size)
      , mInsertFunc(insert)
      , mRemoveFunc(remove)
      {
      }

      /**
       * @return a vector holding all the values of the array.
       */
      std::vector<value_type> GetValue() const
      {
         std::vector<value_type> result;
         int size = GetArraySize();
         result.reserve(size);
         for (int i = 0; i < size; ++i)
         {
            result.push_back(mGetFunc(i));
         }
         return result;
      }

      /**
       * Sets the array to match the data in the vector, within the max and min sizes.
       */
      void SetValue(std::vector<value_type>& newVector)
      {
         int size = GetArraySize();
         if (size > int(newVector.size()))
         {
            while (size > newVector.size() && size > GetMinArraySize())
            {
               mRemoveFunc(size - 1);
               size = GetArraySize();
            }
         }

         if (size < int(newVector.size()))
         {
            while (size < int(newVector.size()) && size < GetMaxArraySize())
            {
               mInsertFunc(size);
               size = GetArraySize();
            }
         }

         for (int i = 0; i < size && i < int(newVector.size()); ++i)
         {
            mSetFunc(newVector[i]);
         }
      }

      /// @return the value at the point of the sticky index
      GetType GetCurrentValue() const
      {
         return mGetFunc(mIndex);
      }

      /// Sets the value at the point of the sticky index
      void SetCurrentValue(SetType newValue) const
      {
         return mSetFunc(mIndex, newValue);
      }

      /**
       * Gets the total size of the array.
       */
      virtual int GetArraySize() const
      {
         return (int)mGetSizeFunc();
      }

      /**
       * Sets the current sticky index.
       */
      virtual void SetIndex(int index) const
      {
         if (index >= 0 && index < GetArraySize())
         {
            mIndex = index;
         }
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
         if (GetMaxArraySize() >= 0 && GetArraySize() >= GetMaxArraySize())
         {
            LOG_WARNING("ArrayActorProperty is attempting to insert a new entry when the array is of max size");
            return false;
         }

         mInsertFunc(index);
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
         if (GetMinArraySize() >= 0 && GetArraySize() <= GetMinArraySize())
         {
            LOG_WARNING("ArrayActorProperty is attempting to remove an entry when the array is already of minimum size");
            return false;
         }

         mRemoveFunc(index);
         return true;
      }

      /**
       * Clears the array.
       */
      virtual void Clear()
      {
         size_t size = mGetSizeFunc();
         while (size > 0)
         {
            mRemoveFunc(size - 1);
            size = mGetSizeFunc();
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
         int size = GetArraySize();
         if (first < size && second < size)
         {
            value_type item1 = mGetFunc(first);
            value_type item2 = mGetFunc(second);
            mSetFunc(first, item2);
            mSetFunc(second, item1);
         }
      }

      /**
       * Copies the contents of the current index with the given.
       *
       * @param[in]  src  The source index to copy from.
       * @param[in]  dst  The destination index to copy to.
       */
      virtual void Copy(int src, int dst)
      {
         int size = GetArraySize();
         if (src < size && dst < size)
         {
            value_type item = mGetFunc(src);
            mSetFunc(dst, item);
         }
      }

   protected:

      /// Current Array Index.  Had to make this mutable because you have to be able to read
      /// values with a const array actor property, and that requires changing the index.
      mutable int mIndex;

      SetFuncType mSetFunc;
      GetFuncType mGetFunc;
      GetSizeFuncType mGetSizeFunc;
      InsertFuncType mInsertFunc;
      RemoveFuncType mRemoveFunc;
   };
}


#endif /* ARRAYACTORPROPERTYCOMPLEX_H_ */
