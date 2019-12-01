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

   void DynamicGroupControl::ConnectNewChildSlots(DynamicAbstractControl* child)
   {
      connect(child, SIGNAL(PropertyAboutToChange(dtCore::PropertyContainer&, dtCore::ActorProperty&,
         const std::string&, const std::string&)),
         this, SLOT(PropertyAboutToChangePassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&,
         const std::string&, const std::string&)));

      connect(child, SIGNAL(PropertyChanged(dtCore::PropertyContainer&, dtCore::ActorProperty&)),
         this, SLOT(PropertyChangedPassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&)));
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupControl::addChildControl(DynamicAbstractControl* child, PropertyEditorModel* model)
   {
      // Note - if you change the propertyeditor so that it adds and removes rows instead of destroying
      // the property editor, you need to work with the begin/endinsertrows methods of model.
      if (child != NULL)
      {
         mChildren.push_back(child);
         ConnectNewChildSlots(child);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupControl::addChildControlSorted(DynamicAbstractControl* child, PropertyEditorModel* model)
   {
      // Note - if you change the propertyeditor so that it adds and removes rows instead of destroying
      // the property editor, you need to work with the begin/endinsertrows methods of model.
      if (child != NULL)
      {
         QString newChildName = child->getDisplayName();

         // Sort the new control.
         bool inserted = false;
         int count = (int)mChildren.size();
         for (int index = 0; index < count; ++index)
         {
            QString name = mChildren[index]->getDisplayName();
            if (name > newChildName)
            {
               inserted = true;
               mChildren.insert(mChildren.begin() + index, 1, child);
               break;
            }
         }

         if (!inserted)
         {
            mChildren.push_back(child);
         }
         ConnectNewChildSlots(child);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGroupControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }
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

   //////////////////////////////////////////////////////////////////////////
   bool DynamicGroupControl::IsPropertyDefault()
   {
      std::vector<DynamicAbstractControl*>::const_iterator itr = mChildren.begin();

      while (itr != mChildren.end())
      {
         if ((*itr)->IsPropertyDefault() == false)
         {
            return false;
         }
         ++itr;
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupControl::actorPropertyChanged(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& property)
   {
      DynamicAbstractParentControl::actorPropertyChanged(propCon, property);

      std::vector<DynamicAbstractControl*>::const_iterator itr = mChildren.begin();

      while (itr != mChildren.end())
      {
         (*itr)->actorPropertyChanged(propCon, property);
         ++itr;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupControl::PropertyAboutToChangePassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop,
            std::string oldValue, std::string newValue)
   {
      // Subcontrols can have their own property containers, but that doesn't work with the
      // undo system.  It expects the parent or for now, we switch the container back to the one on the group.
      if (!mPropContainer.valid())
         BaseClass::PropertyAboutToChangePassThrough(pc, prop, oldValue, newValue);
      else
         BaseClass::PropertyAboutToChangePassThrough(*mPropContainer, prop, oldValue, newValue);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupControl::PropertyChangedPassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop)
   {
      // Subcontrols can have their own property containers, but that doesn't work with the
      // undo system.  It expects the parent or for now, we switch the container back to the one on the group.
      if (!mPropContainer.valid())
         BaseClass::PropertyChangedPassThrough(pc, prop);
      else
         BaseClass::PropertyChangedPassThrough(*mPropContainer, prop);
   }



   ////////////////////////////////////////////////////////////////////////////////
   bool DynamicGroupControl::isEditable()
   {
      return GetArrayIndex() >= 0;
   }
} // namespace dtQt
