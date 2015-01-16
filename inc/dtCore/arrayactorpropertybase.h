/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, MOVES Institute
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

#include <dtCore/actorproperty.h>
#include <dtCore/export.h>
#include <dtCore/refptr.h>
#include <string>


namespace dtCore
{
   class ObjectType;

   /**
    * @brief An ActorProperty that acts like an array of values.
    *
    * This ActorProperty provides an interface to an array of values.  The array
    * is of the type handled by the ActorProperty returned by GetArrayProperty().
    * The ArrayActorProperty has a current size, minimum size, and 
    * maximum size.  Use GetArraySize(), GetMinArraySize(), GetMaxArraySize() to
    * query.  
    * 
    * To operate on a value stored in the array, first set the
    * index using SetIndex(), then use the ActorProperty returned from GetArrayProperty()
    * to get/set the value of that particular index.
    *
    * @note The ActorProperty returned from GetArrayProperty() and the number of 
    * elements contained in the array, are not counted as ActorProperties in the
    * PropertyContainer which holds this array ArrayActorPropertyBase.
    * 
    * For example, to sum up the values of an ArrayActorProperty which operates
    * on IntActorProperty:
    * @code
    *  const IntActorProperty *accessorProperty = 
    *              static_cast<IntActorProperty*>(myArrayProperty->GetArrayProperty());
    *  int sum = 0;
    *  for (int i=0; i<myArrayProperty->GetArraySize(); i++)
    *  {
    *      myArrayProperty->SetIndex(i);
    *      sum += accessorProperty->GetValue();
    *  }
    * @endcode
    */
   class DT_CORE_EXPORT ArrayActorPropertyBase : public ActorProperty
   {
   public:
      typedef ActorProperty BaseClass;

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
                              bool readOnly = false);

      /**
       * Initializes the default value for this property.
       *
       * @param[in]  keyName  The key for this property.
       */
      virtual void InitDefault(ObjectType& keyName);

      /**
       * Sets the value of the property based on a string.
       * @note This method will attempt to use the data to set the value, but it may return false if the data
       * could not be used.
       * @param value the value to set.
       * @return true if the string was usable to set the value, false if not.
       */
      virtual bool FromString(const std::string& value);

      /**
       * @return a string version of the data.  This value can be used when calling SetStringValue.
       * @see #SetStringValue
       */
      virtual const std::string ToString() const;

      /**
       * This is overridden to make handle the fact that the get method returns a refptr.
       * @param otherProp The property to copy from.
       */
      virtual void CopyFrom(const ActorProperty& otherProp);

      /**
       * This gets the ActorProperty used to access the values in this array,
       * indexed by the last call to SetIndex().
       */
      virtual ActorProperty* GetArrayProperty();

      /**
       * This gets the const ActorProperty used to access the values in this array,
       * indexed by the last call to SetIndex().
       */
      virtual const ActorProperty* GetArrayProperty() const;

      /// Set the array property after creation
      void SetArrayProperty(ActorProperty& property);

      /**
       * Sets the minimum size of the array.
       */
      virtual void SetMinArraySize(int minSize);
      virtual int GetMinArraySize() const;

      /**
       * Sets the maximum size of the array.
       */
      virtual void SetMaxArraySize(int maxSize);
      virtual int GetMaxArraySize() const;

      /// Attempts to resize the array.  Returns the new size respecting min and max size.
      virtual int Resize(unsigned newSize);

      /**
       * Gets whether or not this array can be re-ordered.
       */
      virtual bool CanReorder() const;

      /**
       * Gets the total number of ActorProperty stored in the array.
       */
      virtual int GetArraySize() const = 0;


      /**
       * Sets the current active index, indicating which of the values
       * in the array will be accessed. [0..GetArraySize()-1]
       * @see GetArrayProperty()
       */
      virtual void SetIndex(int index) const = 0;

      /**
       * Inserts a new index into the array.
       *
       * @param[in]  index  The index to insert at.
       *
       * @return     True if an element was inserted properly.
       */
      virtual bool Insert(int index) = 0;

      /**
       * Adds a new item to the end of the array.
       *
       * @return     True if an element was inserted properly.
       */
      virtual bool PushBack() { return Insert(GetArraySize()); }

      /**
       * Removes the last item from the array
       *
       * @return     True if an element removed properly.
       */
      virtual bool PopBack() { return Remove(GetArraySize() - 1); }

      /**
       * Removes an index from the array.
       *
       * @param[in]  index  The index to remove from.
       *
       * @return     True if an element was removed properly.
       */
      virtual bool Remove(int index) = 0;

      /**
       * Removes all the stored ActorProperty stored in the array.
       */
      virtual void Clear() = 0;

      /**
       * Swaps the contents of the current index with the given.
       *
       * @param[in]  first   The first index to swap.
       * @param[in]  second  The second index to swap.
       */
      virtual void Swap(int first, int second) = 0;

      /**
       * Copies the contents of the current index with the given.
       *
       * @param[in]  src  The source index to copy from.
       * @param[in]  dst  The destination index to copy to.
       */
      virtual void Copy(int src, int dst) = 0;

   protected:

      virtual ~ArrayActorPropertyBase();

   private:
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
