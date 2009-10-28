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
#include <prefix/dtstageprefix-src.h>
#include <dtQt/dynamicgroupcontrol.h>
#include <dtQt/propertyeditormodel.h>
#include <QtGui/QWidget>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicGroupControl::DynamicGroupControl(const std::string& newName)
      : mName(newName.c_str())
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicGroupControl::~DynamicGroupControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicGroupControl* DynamicGroupControl::getChildGroupControl(QString name)
   {
      std::vector<DynamicAbstractControl*>::iterator childIter;
      DynamicGroupControl* result = NULL;

      // walk the children to find group controls.
      for (childIter = mChildren.begin(); childIter != mChildren.end(); ++childIter)
      {
         DynamicAbstractControl* control = (*childIter);
         if (control != NULL)
         {
            // for each group control, compare the name
            DynamicGroupControl* group = dynamic_cast<DynamicGroupControl*>(control);
            if (group != NULL && group->mName == QString(name))
            {
               result = group;
               break;
            }
         }
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupControl::addChildControl(DynamicAbstractControl* child, PropertyEditorModel* model)
   {
      // Note - if you change the propertyeditor so that it adds and removes rows instead of destroying
      // the property editor, you need to work with the begin/endinsertrows methods of model.
      if (child != NULL)
      {
         mChildren.push_back(child);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGroupControl::getDisplayName()
   {
      return mName;
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicGroupControl::updateData(QWidget* widget)
   {
      // groups don't have editors, nor do they have any data to update.
      // and they don't have to update their children, because the children
      // get their own events if needed.

      return false;
   }

} // namespace dtQt
