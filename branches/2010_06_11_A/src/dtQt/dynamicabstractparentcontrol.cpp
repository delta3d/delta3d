/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Curtiss Murphy
 */
#include <prefix/dtqtprefix.h>
#include <dtQt/dynamicabstractparentcontrol.h>
#include <dtQt/propertyeditormodel.h>
#include <QtCore/QModelIndex>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicAbstractParentControl::DynamicAbstractParentControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractParentControl::~DynamicAbstractParentControl()
   {
      removeAllChildren(NULL);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractParentControl::removeAllChildren(PropertyEditorModel* model)
   {
      //// Note that we no longer use hte model.  it remains for future potential issues.
      // Note 2 - If you change the property editor so that it can add and remove items,
      // you need to change this to send the begin and end remove events.

      std::vector<DynamicAbstractControl*>::iterator childIter;

      // remove all the controls from the inner layout
      for (childIter = mChildren.begin(); childIter != mChildren.end(); ++childIter)
      {
         DynamicAbstractControl* control = (*childIter);

         if (control != NULL)
         {
            // Groups delete their children in the destructor.
            delete control;
         }
      }
      mChildren.clear();
   }

   /////////////////////////////////////////////////////////////////////////////////
   int DynamicAbstractParentControl::getChildIndex(DynamicAbstractControl* child)
   {
      std::vector <DynamicAbstractControl*>::const_iterator iter;
      int index = 0;
      bool found = false;

      // walk through our list to find the child.
      for (iter = mChildren.begin(); iter != mChildren.end(); ++iter, ++index)
      {
         if ((*iter) == child)
         {
            found = true;
            break;
         }
      }

      // found it, so return result
      if (found)
      {
         return index;
      }
      else
      {
         return 0;
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl* DynamicAbstractParentControl::getChild(int index)
   {
      if (mChildren.size() > (unsigned) index)
      {
         return mChildren[index];
      }
      else
      {
         return NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   int DynamicAbstractParentControl::getChildCount()
   {
      return mChildren.size();
   }

} // namespace dtQt
