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
#include <dtQt/dynamicstringcontrol.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/datatype.h>
#include <dtDAL/stringactorproperty.h>

#include <dtQt/dynamicsubwidgets.h>

#include <dtUtil/log.h>

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicStringControl::DynamicStringControl()
      : mProperty(NULL)
      , mTemporaryEditControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicStringControl::~DynamicStringControl()
   {
   }


   /////////////////////////////////////////////////////////////////////////////////
   void DynamicStringControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::STRING)
      {
         mProperty = static_cast<dtDAL::StringActorProperty*>(newProperty);
         DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property [" +
            propertyName + "] is not the correct type.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicStringControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         mTemporaryEditControl->setText(tr(mProperty->GetValue().c_str()));
         mTemporaryEditControl->selectAll();
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicStringControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget == mWrapper && mTemporaryEditControl)
      {
         // get the data from our control
         std::string result = mTemporaryEditControl->text().toStdString();

         // set our value to our object
         if (result != mProperty->GetValue())
         {
            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mPropContainer, *mProperty,
               mProperty->ToString(), result);

            mProperty->SetValue(result);
            dataChanged = true;
         }

         // reselect all the text when we commit.
         // Gives the user visual feedback that something happened.
         mTemporaryEditControl->selectAll();
      }

      // notify the world (mostly the viewports) that our property changed
      if (dataChanged)
      {
         emit PropertyChanged(*mPropContainer, *mProperty);
      }

      return dataChanged;
   }


   /////////////////////////////////////////////////////////////////////////////////
   QWidget *DynamicStringControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractControl::createEditor(parent, option, index);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      // create and init the edit box
      mTemporaryEditControl = new SubQLineEdit(wrapper, this);
      mTemporaryEditControl->setToolTip(getDescription());
      mFocusWidget = mTemporaryEditControl;
      if (mProperty->GetMaxLength() > 0)
      {
         mTemporaryEditControl->setMaxLength(mProperty->GetMaxLength());
      }

      updateEditorFromModel(mWrapper);

      mGridLayout->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
      mGridLayout->setColumnMinimumWidth(0, mTemporaryEditControl->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 1);

      return wrapper;
   }

   const QString DynamicStringControl::getDisplayName()
   {
      return QString(tr(mProperty->GetLabel().c_str()));
   }

   const QString DynamicStringControl::getDescription()
   {
      std::string tooltip = mProperty->GetDescription() + "  [Type: " +
         mProperty->GetDataType().GetName() + "]";
      return QString(tr(tooltip.c_str()));
   }

   const QString DynamicStringControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();
      return QString(tr(mProperty->GetValue().c_str()));
   }

   bool DynamicStringControl::isEditable()
   {
      return !mProperty->IsReadOnly();
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   bool DynamicStringControl::updateData(QWidget* widget)
   {
      // returns true if we successfully change data
      bool dataChanged = false;

      if (!mInitialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return dataChanged;
      }

      return updateModelFromEditor(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicStringControl::actorPropertyChanged(dtDAL::PropertyContainer& propCon,
            dtDAL::ActorProperty& property)
   {
      DynamicAbstractControl::actorPropertyChanged(propCon, property);

      if (mTemporaryEditControl != NULL && &propCon == mPropContainer && &property == mProperty)
      {
         updateEditorFromModel(mWrapper);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicStringControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mTemporaryEditControl = NULL;
      }
      DynamicAbstractControl::handleSubEditDestroy(widget, hint);
   }
} // namespace dtQt
