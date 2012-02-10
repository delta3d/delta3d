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
#include <dtQt/dynamicboolcontrol.h>

#include <dtCore/actorproxy.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/datatype.h>

#include <dtQt/dynamicsubwidgets.h>

#include <dtUtil/log.h>

#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

namespace dtQt
{

   const QString DynamicBoolControl::TRUE_LABEL("True");
   const QString DynamicBoolControl::FALSE_LABEL("False");

   ///////////////////////////////////////////////////////////////////////////////
   DynamicBoolControl::DynamicBoolControl()
      : mTemporaryEditControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicBoolControl::~DynamicBoolControl()
   {
   }


   /////////////////////////////////////////////////////////////////////////////////
   void DynamicBoolControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType() == dtCore::DataType::BOOLEAN)
      {
         mProperty = static_cast<dtCore::BooleanActorProperty*>(newProperty);
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
   void DynamicBoolControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         bool value = mProperty->GetValue();
         mTemporaryEditControl->setCurrentIndex(mTemporaryEditControl->findText((value) ? TRUE_LABEL : FALSE_LABEL));
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicBoolControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         QString selection = mTemporaryEditControl->currentText();
         bool    result    = (selection == TRUE_LABEL);

         // set our value to our object
         if (result != mProperty->GetValue())
         {
            std::string oldValue = mProperty->ToString();
            mProperty->SetValue(result);
            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mPropContainer, *mProperty,
               oldValue, mProperty->ToString());
            dataChanged = true;
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
   QWidget* DynamicBoolControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractControl::createEditor(parent, option, index);

      // create and init the combo box
      mTemporaryEditControl = new SubQComboBox(wrapper, this);
      mTemporaryEditControl->addItem(TRUE_LABEL);
      mTemporaryEditControl->addItem(FALSE_LABEL);
      mTemporaryEditControl->setToolTip(getDescription());

      mGridLayout->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
      mGridLayout->setColumnMinimumWidth(0, mTemporaryEditControl->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 1);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      updateEditorFromModel(mWrapper);

      connect(mTemporaryEditControl, SIGNAL(activated(int)), this, SLOT(itemSelected(int)));

      wrapper->setFocusProxy(mTemporaryEditControl);
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicBoolControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }
      return QString(tr(mProperty->GetLabel().c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicBoolControl::getDescription()
   {
      std::string tooltip = mProperty->GetDescription() + "  [Type: " +
         mProperty->GetPropertyType().GetDisplayName() + "]";
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicBoolControl::getValueAsString()
   {
      if (doPropertiesMatch())
      {
         DynamicAbstractControl::getValueAsString();
         bool value = mProperty->GetValue();
         return (value) ? TRUE_LABEL : FALSE_LABEL;
      }
      else
      {
         return "<Multiple Values...>";
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicBoolControl::itemSelected(int index)
   {
      if (mTemporaryEditControl != NULL)
      {
         updateModelFromEditor(mWrapper);
         CopyBaseValueToLinkedProperties();
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicBoolControl::updateData(QWidget* widget)
   {
      if (!mInitialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return false;
      }

      return updateModelFromEditor(widget);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicBoolControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
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
