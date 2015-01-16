/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef DELTA_HIERARCHY_INTERFACE_H
#define DELTA_HIERARCHY_INTERFACE_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtCore/refptr.h>
#include <dtUtil/functor.h>
#include <osg/Referenced>
#include <vector>



namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template<class T_Object, class T_Key>
   class HierarchyInterface
   {
   public:
      typedef T_Object ObjType;
      typedef T_Key KeyType;
      typedef dtCore::RefPtr<ObjType> ObjPtr;
      typedef std::vector<ObjPtr> ObjArray;

      /**
       * Functor for determining valid objects for various operations,
       * such as finding specific child objects.
       */
      typedef dtUtil::Functor<bool, TYPELIST_1(const ObjType&)> ObjectFilterFunc;

      /**
       * Set the reference to the object that is the parent to the owner object.
       */
      virtual void SetParent(ObjType* parent) = 0;
      virtual ObjType* GetParent() const = 0;

      /**
       * Determines if the specified object is referenced as either the owner, parent or child.
       */
      virtual bool HasReference(const ObjType& obj) const = 0;

      /**
       * Determines if the specified object is referenced as a child to the owner.
       */
      virtual bool HasChild(const ObjType& obj) const = 0;

      /**
       * Adds the specified object as a child reference to the owner.
       */
      virtual bool AddChild(ObjType& obj) = 0;

      /**
       * Removes the specified object from being a child reference to the owner.
       */
      virtual bool RemoveChild(ObjType& obj) = 0;

      /**
       * Returns the first child reference that passes the specified functor.
       * @param func Functor to iterate over child objects and that returns TRUE when an object passes its conditions.
       * @return First child object to pass the conditions of the functor; NULL if none pass or exist.
       */
      virtual ObjType* GetChild(ObjectFilterFunc func) const = 0;

      /**
       * Returns a child reference by a direct key.
       * @param key Key that directly refers to a child reference without having to perform a find operation.
       * @return Child reference associated with the specified key; NULL if no reference is found.
       */
      virtual ObjType* GetChild(KeyType key) const = 0;

      /**
       * Get the key associated with the specified child reference.
       */
      virtual KeyType GetChildKey(const ObjType& obj) const = 0;

      /**
       * Returns the number of child references associated with the owner.
       */
      virtual int GetChildCount() const = 0;

      /**
       * Acquires all child object references in the specified array.
       * @param outObject Collection to capture all child references.
       * @return Number of references captured.
       */
      virtual int GetChildren(ObjArray& outObjects) const = 0;

      /**
       * Returns child references that pass the specified functor.
       * @param outObjects Collection to capture all child references that pass the functor's conditions.
       * @param func Functor to iterate over child objects and that returns TRUE for each object that passes its conditions.
       * @return Number of child references that were captured; 0 if none pass or exist.
       */
      virtual int GetChildrenFiltered(ObjArray& outObjects, ObjectFilterFunc func) const = 0;

      /**
       * Returns the number of child references associated with the owner.
       */
      virtual void ClearChildren() = 0;

      /**
       * Sets a functor that determines if an object is a valid child to add.
       */
      virtual void SetChildFilter(ObjectFilterFunc func) = 0;

      /**
       * Returns the functor that determines if an object is a valid child to add.
       */
      virtual ObjectFilterFunc GetChildFilter() const = 0;

      /**
       * Method to be called for the owner when a child reference is added.
       * @param obj Child object that was added.
       */
      virtual void OnChildAdded(ObjType& obj) {}
      
      /**
       * Method to be called for the owner when a child reference is removed.
       * @param obj Child object that was removed.
       */
      virtual void OnChildRemoved(ObjType& obj) {}

      /**
       * Method to be called for the owner when a parent reference is added.
       * @param obj Parent object that was added.
       */
      virtual void OnParentAdded(ObjType& obj) {}

      /**
       * Method to be called for the owner when a parent reference is removed.
       * @param obj Parent object that was removed.
       */
      virtual void OnParentRemoved(ObjType& obj) {}

      virtual ~HierarchyInterface()
      {}
   };

}

#endif
