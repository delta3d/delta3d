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

#ifndef DELTA_HIERARCHY_OBJECT_H
#define DELTA_HIERARCHY_OBJECT_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtCore/actortype.h>
#include <dtCore/hierarchyinterface.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <vector>



namespace dtCore
{

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template<class T_Object, class T_Key>
   class BaseHierarchyObject : public HierarchyInterface<T_Object, T_Key>
   {
   public:
      typedef HierarchyInterface BaseClass;
      typedef HierarchyInterface InterfaceType;
      typedef typename BaseClass::ObjType ObjType;
      typedef typename BaseClass::KeyType KeyType;
      typedef typename BaseClass::ObjArray ObjArray;

      /*virtual*/ bool HasChild(const ObjType& obj) const { return false; }

      /*virtual*/ bool AddChild(ObjType& obj) { return false; }

      /*virtual*/ bool RemoveChild(ObjType& obj) { return false; }

      /*virtual*/ ObjType* GetChild(ObjectFilterFunc func) const { return NULL; }

      /*virtual*/ ObjType* GetChild(KeyType key) const { return NULL; }

      /*virtual*/ KeyType GetChildKey(const ObjType& obj) const { return KeyType(); }

      /*virtual*/ int GetChildCount() const { return 0; };

      /*virtual*/ int GetChildren(ObjArray& outObjects) const { return 0; }

      /*virtual*/ int GetChildrenFiltered(ObjArray& outObjects, ObjectFilterFunc func) const { return 0; }

      /*virtual*/ void ClearChildren() {}

      /*virtual*/ bool IsChildAllowed(const ObjType& obj) const { return false; }

      /*virtual*/ void SetChildFilter(ObjectFilterFunc func) {}
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template<class T_Object, class T_Key>
   class BaseParentOnlyHierarchyObject : public BaseHierarchyObject<T_Object, T_Key>
   {
   public:
      typedef BaseHierarchyObject BaseClass;

      BaseParentOnlyHierarchyObject(ObjType* parent = NULL)
         : mParent(parent)
      {}

      /*virtual*/ void SetParent(ObjType* parent)
      {
         // Avoid extra processing.
         if (mParent == parent || (parent != NULL && HasReference(*parent)))
         {
            return;
         }

         // Keep track of the old parent and remove the reference
         // to it before changing references on other objects.
         dtCore::RefPtr<ObjType> previousParent = mParent.get();

         mParent = parent;

         ObjType* owner = dynamic_cast<ObjType*>(this);

         // Remove the old parent's reference to the owner.
         if (previousParent.valid() && parent != this)
         {
            if (owner != NULL)
            {
               previousParent->RemoveChild(*owner);
            }
         }

         // Add a reference to the owner for the new parent.
         if (mParent.valid() && owner != NULL)
         {
            mParent->AddChild(*owner);
         }

         // Notify the owner that the parent has changed.
         // --- Notify that the old parent was removed.
         if (previousParent.valid())
         {
            OnParentRemoved(*previousParent);
         }
         
         // --- Notify that the new one has been added.
         if (mParent.valid())
         {
            OnParentAdded(*mParent);
         }
      }

      /*virtual*/ ObjType* GetParent() const
      {
         return mParent.get();
      }

      /*virtual*/ bool HasReference(const ObjType& obj) const
      {
         return mParent.get() == &obj || this == &obj || HasChild(obj);
      }

      /*virtual*/ ObjectFilterFunc GetChildFilter() const
      {
         return ObjectFilterFunc(this, &BaseParentOnlyHierarchyObject::FilterOutAll);
      }

   protected:
      virtual ~BaseParentOnlyHierarchyObject()
      {
         ClearChildren();
      }

      dtCore::ObserverPtr<ObjType> mParent;

   private:
      bool FilterOutAll(const ObjType&) const
      {
         return false;
      }
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template<class T_Object>
   class HierarchyObject : public BaseParentOnlyHierarchyObject<T_Object, int>
   {
   public:
      typedef BaseParentOnlyHierarchyObject BaseClass;
      
      HierarchyObject(ObjType* parent = NULL)
         : BaseClass(NULL)
      {
         // Ensure the dual linkage happens here instead of the base constructor.
         // The virtual function table will not work at this point so the method
         // needs to be called directly.
         SetParent(parent);
      }
      
      /*virtual*/ bool HasChild(const ObjType& obj) const
      {
         return mChildren.end() != std::find(mChildren.begin(), mChildren.end(), &obj);
      }

      /*virtual*/ int GetChildCount() const
      {
         return (int)mChildren.size();
      }

      /*virtual*/ ObjType* GetChild(ObjectFilterFunc func) const
      {
         ObjType* result = NULL;

         if ( ! mChildren.empty() && func.valid())
         {
            ObjType* curObj = NULL;
            ObjArray::const_iterator curIter = mChildren.begin();
            ObjArray::const_iterator endIter = mChildren.end();
            for (; curIter != endIter; ++curIter)
            {
               curObj = *curIter;
               if (curObj != NULL && func(*curObj))
               {
                  result = curObj;
                  break;
               }
            }
         }

         return result;
      }

      /*virtual*/ int GetChildren(ObjArray& outObjects) const
      {
         int results = (int)mChildren.size();

         outObjects.insert(outObjects.end(), mChildren.begin(), mChildren.end());

         return results;
      }

      /*virtual*/ int GetChildrenFiltered(ObjArray& outObjects, ObjectFilterFunc func) const
      {
         int results = 0;

         if ( ! mChildren.empty() && func.valid())
         {
            ObjType* curObj = NULL;
            ObjArray::const_iterator curIter = mChildren.begin();
            ObjArray::const_iterator endIter = mChildren.end();
            for (; curIter != endIter; ++curIter)
            {
               curObj = *curIter;
               if (curObj != NULL && func(*curObj))
               {
                  outObjects.push_back(curObj);
                  ++results;
               }
            }
         }

         return results;
      }

      /*virtual*/ ObjType* GetChild(KeyType key) const
      {
         ObjType* obj = NULL;
         
         if (key >= 0 && size_t(key) < mChildren.size())
         {
            obj = mChildren[key];
         }

         return obj;
      }

      /*virtual*/ KeyType GetChildKey(const ObjType& obj) const
      {
         KeyType key = -1;

         ObjType* curObj = NULL;
         ObjArray::const_iterator curIter = mChildren.begin();
         ObjArray::const_iterator endIter = mChildren.end();
         for (KeyType k = 0; curIter != endIter; ++curIter, ++k)
         {
            curObj = *curIter;

            if (curObj != NULL)
            {
               key = k;
               break;
            }
         }

         return key;
      }

      /*virtual*/ bool AddChild(ObjType& obj)
      {
         bool success = false;
         
         if (IsChildAllowed(obj))
         {
            mChildren.push_back(&obj);
            success = true;
         }

         // Setup the refence to the owner for the new child object.
         if (success)
         {
            ObjType* owner = dynamic_cast<ObjType*>(this);
            if (owner != NULL)
            {
               obj.SetParent(owner);
            }

            // Notify the owner that an object has been added.
            if (success)
            {
               OnChildAdded(obj);
            }
         }

         return success;
      }

      /*virtual*/ bool RemoveChild(ObjType& obj)
      {
         // Maintain a temporary pointer to keep the object alive
         // in case the linkage is the only thing keeping it around.
         // This will prevent the object from being deleted after
         // removal so that it can be passed as a notification to
         // the owner, so that it knows what is exactly removed and will
         // have a chance to act on the object before the object goes away.
         dtCore::RefPtr<HierarchyObject::ObjType> objPtr = &obj;

         bool success = false;
         ObjArray::iterator foundIter = std::find(mChildren.begin(), mChildren.end(), &obj);
         if (foundIter != mChildren.end())
         {
            mChildren.erase(foundIter);
            success = true;
         }

         if (success)
         {
            // If the owner is the parent...
            if (obj.GetParent() == this)
            {
               obj.SetParent(NULL);
            }
            // If the owner is a child to obj...
            else if (GetParent() == &obj)
            {
               obj.RemoveChild(*GetParent());
            }

            // Notify the owner that an object has been removed.
            if (success)
            {
               OnChildRemoved(*objPtr);
            }
         }

         return success;
      }

      /*virtual*/ void ClearChildren()
      {
         // Remove each object individually to ensure that
         // each object's two-way references are resolved.
         ObjType* curObj = NULL;
         while ( ! mChildren.empty())
         {
            curObj = mChildren.back();

            RemoveChild(*curObj);
         }
      }

      /*virtual*/ bool IsChildAllowed(const ObjType& obj) const
      {
         return ! HasReference(obj) && ( ! mFilter.valid() || mFilter(obj));
      }

      /*virtual*/ void SetChildFilter(ObjectFilterFunc func)
      {
         mFilter = func;
      }

      /*virtual*/ ObjectFilterFunc GetChildFilter() const
      {
         return mFilter;
      }

   protected:
      virtual ~HierarchyObject()
      {
         ClearChildren();
      }

      ObjArray mChildren;
      ObjectFilterFunc mFilter;
   };

}

#endif
