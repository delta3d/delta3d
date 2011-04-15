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
#include <dtQt/dynamicdoublecontrol.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/datatype.h>
#include <dtDAL/doubleactorproperty.h>

#include <dtQt/dynamicsubwidgets.h>

#include <dtUtil/log.h>

#include <QtGui/QDoubleValidator>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicDoubleControl::DynamicDoubleControl()
      : mTemporaryEditControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicDoubleControl::~DynamicDoubleControl()
   {
   }


   /////////////////////////////////////////////////////////////////////////////////
   void DynamicDoubleControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::DOUBLE)
      {
         mProperty = static_cast<dtDAL::DoubleActorProperty*>(newProperty);
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
   void DynamicDoubleControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         double doubleValue = mProperty->GetValue();
         mTemporaryEditControl->setText(QString::number(doubleValue, 'f', NUM_DECIMAL_DIGITS_DOUBLE));
         mTemporaryEditControl->selectAll();
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicDoubleControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget == mWrapper && mTemporaryEditControl)
      {
         bool   success = false;
         double result  = mTemporaryEditControl->text().toDouble(&success);

         // set our value to our object
         if (success)
         {
            // Save the data if they are different.  Note, we also need to compare the QString value,
            // else we get epsilon differences that cause the map to be marked dirty with no edits :(
            QString proxyValue = QString::number(mProperty->GetValue(), 'f', NUM_DECIMAL_DIGITS_DOUBLE);
            QString newValue = mTemporaryEditControl->text();
            if (result != mProperty->GetValue() && proxyValue != newValue)
            {
               // give undo manager the ability to create undo/redo events

               emit PropertyAboutToChange(*mPropContainer, *mProperty, mProperty->ToString(), QString::number(result).toStdString());

               mProperty->SetValue(result);
               dataChanged = true;
            }
         }
         else
         {
            LOG_ERROR("updateData() failed to convert our value successfully");
         }
      }

      // notify the world (mostly the viewports) that our property changed
      if (dataChanged)
      {
         emit PropertyChanged(*mPropContainer, *mProperty);
      }

      return dataChanged;
   }


   /////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicDoubleControl::createEditor(QWidget* parent,
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
      QDoubleValidator* validator = new QDoubleValidator(mTemporaryEditControl);
      validator->setDecimals(NUM_DECIMAL_DIGITS_DOUBLE);
      mTemporaryEditControl->setValidator(validator);
      mTemporaryEditControl->setToolTip(getDescription());
      mFocusWidget = mTemporaryEditControl;

      mGridLayout->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
      mGridLayout->setColumnMinimumWidth(0, mTemporaryEditControl->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 1);

      updateEditorFromModel(mWrapper);

      return wrapper;
   }

   const QString DynamicDoubleControl::getDisplayName()
   {
      return QString(tr(mProperty->GetLabel().c_str()));
   }

   const QString DynamicDoubleControl::getDescription()
   {
      std::string tooltip = mProperty->GetDescription() + "  [Type: " +
         mProperty->GetDataType().GetName() + "]";
      return QString(tr(tooltip.c_str()));
   }

   const QString DynamicDoubleControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();
      double doubleValue = mProperty->GetValue();
      return QString::number(doubleValue, 'f', NUM_DECIMAL_DIGITS_DOUBLE);
   }

   bool DynamicDoubleControl::isEditable()
   {
      return !mProperty->IsReadOnly();
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   bool DynamicDoubleControl::updateData(QWidget* widget)
   {
      if (!mInitialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return false;
      }

      return updateModelFromEditor(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicDoubleControl::actorPropertyChanged(dtDAL::PropertyContainer& propCon,
            dtDAL::ActorProperty& property)
   {
      DynamicAbstractControl::actorPropertyChanged(propCon, property);

      if (mTemporaryEditControl != NULL && &propCon == mPropContainer && &property == mProperty)
      {
         updateEditorFromModel(mWrapper);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicDoubleControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      // we have to check - sometimes the destructor won't get called before the
      // next widget is created.  Then, when it is called, it sets the NEW editor to NULL!
      if (widget == mWrapper)
      {
         mTemporaryEditControl = NULL;
      }

      DynamicAbstractControl::handleSubEditDestroy(widget, hint);
   }
} // namespace dtQt
