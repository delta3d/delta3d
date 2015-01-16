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

#ifndef DELTA_HIERARCHY_VISITOR_H
#define DELTA_HIERARCHY_VISITOR_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtCore/export.h>
#include <dtCore/hierarchyobject.h>
#include <dtCore/baseactor.h>
#include <dtUtil/objectfactory.h>
#include <queue>
#include <stack>



namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template <class T_HierarchyInterface>
   class HierarchyIterator : public osg::Referenced
   {
   public:
      typedef T_HierarchyInterface HierarchyInterfaceType;
      typedef typename HierarchyInterfaceType::ObjType ObjType;
      typedef typename HierarchyInterfaceType::ObjArray ObjArray;
      typedef typename HierarchyInterfaceType::KeyType ObjKeyType;

      enum Mode
      {
         DEPTH_FIRST,
         BREADTH_FIRST,
         LEFT_FIRST,
         RIGHT_FIRST,
         ANCESTORS
      };

      static const Mode DEFAULT_MODE = DEPTH_FIRST;

      HierarchyIterator(Mode mode = DEFAULT_MODE)
         : mMode(mode)
      {}

      void SetMode(Mode mode)
      {
         mMode = mode;
      }
      
      Mode GetMode() const
      {
         return mMode;
      }

      HierarchyIterator& operator= (ObjType& obj)
      {
         mStartObject = &obj;
         return *this;
      }

      HierarchyIterator& operator++ ()
      {
         Next();
         return *this;
      }

      ObjType* Next()
      {
         bool firstObject = ! mCurrentObject.valid() && mStartObject.valid();
         if (firstObject)
         {
            mCurrentObject = mStartObject.get();

            if (mMode == DEPTH_FIRST && mCurrentObject->GetChildCount() > 0)
            {
               mCurrentObject = DescendToDeepestFirstChild(*mCurrentObject);
            }

            mStartObject = NULL;
         }

         if ( ! firstObject && mCurrentObject.valid())
         {
            switch (mMode)
            {
            case DEPTH_FIRST:
               mCurrentObject = NextDepth(*mCurrentObject);
               break;

            case BREADTH_FIRST:
               mCurrentObject = NextBreadth(*mCurrentObject);
               break;

            case LEFT_FIRST:
               mCurrentObject = NextLeft(*mCurrentObject);
               break;

            case RIGHT_FIRST:
               mCurrentObject = NextRight(*mCurrentObject);
               break;

            case ANCESTORS:
               mCurrentObject = NextAncestor(*mCurrentObject);
               break;

            default: break;
            }
         }

         return mCurrentObject.get();
      }

   protected:
      ObjType* GetFirstChildWithChildren(ObjType& obj) const
      {
         ObjType* child = NULL;

         ObjType* curObj = NULL;
         int limit = obj.GetChildCount();
         for (int i = 0; i < limit; ++i)
         {
            curObj = obj.GetChild(i);
            if (curObj != NULL && curObj->GetChildCount() > 0)
            {
               child = curObj;
               break;
            }
         }

         return child;
      }

      ObjType* NextBreadth(ObjType& current)
      {
         ObjType* obj = NULL;

         ObjType* curObj = NULL;
         int limit = current.GetChildCount();
         for (ObjKeyType i = 0; i < limit; ++i)
         {
            curObj = current.GetChild(i);

            if (curObj != NULL)
            {
               mBreadthQueue.push(curObj);
            }
         }
         
         if ( ! mBreadthQueue.empty())
         {
            obj = mBreadthQueue.front().get();
            mBreadthQueue.pop();
         }

         return obj;
      }

      ObjType* NextDepth(ObjType& current)
      {
         ObjType* obj = NextSibling(current, false);

         if (obj != NULL && obj->GetChildCount() > 0)
         {
            obj = DescendToDeepestFirstChild(*obj);
         }

         if (obj == NULL && current.GetParent() != NULL)
         {
            obj = NextAncestor(current);
         }

         return obj;
      }

      ObjType* NextChild(ObjType& current, bool reverse)
      {
         ObjType* obj = NULL;

         int childCount = current.GetChildCount();
         if (childCount > 0)
         {
            obj = GetChild(current, reverse ? -1 : 0);

            if (obj != NULL)
            {
               // Determine which child to start with, first for
               // the left or last for the right.
               ObjKeyType index = reverse ? childCount - 1 : 0;

               // Track going down one tier.
               Descend(index);
            }
         }
         else // No children for the current object...
         {
            // Get the sibling to the current object.
            obj = NextSibling(current, reverse);

            // If no sibling exists...
            if (obj == NULL)
            {
               // ...go up the ancestors to find the closest one with
               // a valid sibling to traverse.
               ObjType* parent = current.GetParent();
               while (parent != NULL)
               {
                  // Track going up one tier.
                  Ascend();

                  obj = NextSibling(*parent, reverse);
                  if (obj == NULL)
                  {
                     // Current parent has no next sibling
                     // so check for the next ancestor's sibling.
                     parent = parent->GetParent();
                  }
                  else
                  {
                     // Sibling was found so escape traversal of ancestors.
                     break;
                  }
               }
            }
         }

         return obj;
      }

      ObjType* NextLeft(ObjType& current)
      {
         return NextChild(current, false);
      }

      ObjType* NextRight(ObjType& current)
      {
         return NextChild(current, true);
      }
      
      ObjType* NextAncestor(ObjType& current)
      {
         // Track going up one tier.
         Ascend();

         return current.GetParent();
      }

      ObjType* NextSibling(ObjType& current, bool reverse)
      {
         ObjType* obj = NULL;

         ObjType* parent = current.GetParent();
         if (parent != NULL)
         {
            ObjKeyType index = mIndexStack.top();

            // Determine direction of iteration of the current tier.
            if (reverse)
            {
               --index;
            }
            else
            {
               ++index;
            }

            // Update the current index for the current tier.
            if (index >= 0 && index < parent->GetChildCount())
            {
               obj = GetChild(*parent, index);

               mIndexStack.top() = index;
            }
         }

         return obj;
      }

      ObjType* GetChild(ObjType& obj, ObjKeyType index) const
      {
         ObjType* child = NULL;

         if (index < 0)
         {
            index = obj.GetChildCount() - 1;
         }

         if (index >= 0)
         {
            child = obj.GetChild(index);
         }

         return child;
      }

      ObjType* DescendToDeepestFirstChild(ObjType& obj)
      {
         ObjType* deepObj = NULL;

         ObjType* curObj = &obj;
         do
         {
            // Find the left most child of the current tier.
            curObj = GetChild(*curObj, 0);

            if (curObj != NULL)
            {
               deepObj = curObj;

               // Track going down one tier.
               Descend(0);
            }
         } while(curObj != NULL);

         return deepObj;
      }

      ObjKeyType GetCurrentIndex()
      {
         ObjKeyType index = -1;

         if ( ! mIndexStack.empty())
         {
            index = mIndexStack.top();
         }

         return index;
      }

      void Ascend()
      {
         if ( ! mIndexStack.empty())
         {
            mIndexStack.pop();
         }
      }

      void Descend(ObjKeyType index)
      {
         mIndexStack.push(index);
      }

      void Reset()
      {
         mCurrentObject = NULL;
         mIndexStack.clear();
         mBreadthQueue.clear();
      }

      typedef std::queue<dtCore::ObserverPtr<ObjType> > ObjQueue;
      ObjQueue mBreadthQueue;

      dtCore::ObserverPtr<ObjType> mStartObject;
      dtCore::ObserverPtr<ObjType> mCurrentObject;
      Mode mMode;
      std::stack<ObjKeyType> mIndexStack;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template <class T_HierarchyInterface>
   class HierarchyVisitor : public osg::Referenced
   {
   public:
      typedef osg::Referenced BaseClass;

      typedef T_HierarchyInterface HierarchyInterfaceType;
      typedef typename HierarchyInterfaceType::ObjType ObjType;
      typedef typename HierarchyInterfaceType::ObjArray ObjArray;
      typedef dtCore::RefPtr<ObjType> ObjPtr;

      typedef HierarchyIterator<ObjType> IteratorType;
      typedef typename IteratorType::Mode Mode;

      typedef dtUtil::Functor<bool, TYPELIST_1(ObjType&)> HandlerFunc;

      static const Mode DEFAULT_MODE = IteratorType::DEFAULT_MODE;


      HierarchyVisitor(Mode mode = DEFAULT_MODE)
      {
         SetMode(mode);
      }

      void SetMode(Mode mode)
      {
         mIter.SetMode(mode);
      }
      
      Mode GetMode() const
      {
         return mIter.GetMode();
      }

      void Traverse(ObjType& obj)
      {
         mIter = obj;

         ObjType* curObj = NULL;
         do
         {
            curObj = mIter.Next();

            if (curObj != NULL)
            {
               Handle(*curObj);
            }
         }
         while (curObj != NULL);
      }

      void SetHandlerFunc(HandlerFunc func)
      {
         mHandlerFunc = func;
      }

      virtual bool Handle(ObjType& obj)
      {
         if (mHandlerFunc.valid())
         {
            return mHandlerFunc(obj);
         }

         // OVERRIDE: If not using a functor.
         return true;
      }

   protected:
      virtual ~HierarchyVisitor()
      {}

   private:
      IteratorType mIter;
      HandlerFunc mHandlerFunc;
   };

}

#endif
