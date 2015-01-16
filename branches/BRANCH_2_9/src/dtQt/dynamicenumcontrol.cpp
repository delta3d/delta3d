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
#include <dtQt/dynamicenumcontrol.h>

#include <dtCore/abstractenumactorproperty.h>
#include <dtCore/actorproxy.h>
#include <dtCore/datatype.h>

#include <dtUtil/enumeration.h>
#include <dtUtil/log.h>

#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

namespace dtQt
{

   //const QString DynamicBoolControl::TRUE_LABEL("True");
   //const QString DynamicBoolControl::FALSE_LABEL("False");

   ///////////////////////////////////////////////////////////////////////////////
   DynamicEnumControl::DynamicEnumControl()
      : mTemporaryEditControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicEnumControl::~DynamicEnumControl()
   {
   }


   /////////////////////////////////////////////////////////////////////////////////
   void DynamicEnumControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // Note - Unlike the other properties, we can't static or reinterpret cast this object.
      // We need to dynamic cast it...
      if (newProperty != NULL && newProperty->GetDataType() == dtCore::DataType::ENUMERATION)
      {
         mProperty = dynamic_cast<dtCore::AbstractEnumActorProperty*>(newProperty);
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
   void DynamicEnumControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         dtUtil::Enumeration& value = mProperty->GetEnumValue();
         mTemporaryEditControl->setCurrentIndex(mTemporaryEditControl->findText(QString(value.GetName().c_str())));
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicEnumControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget == mWrapper && mTemporaryEditControl)
      {
         // Get the current selected string and the previously set string value
         QString selection = mTemporaryEditControl->currentText();
         std::string selectionString = selection.toStdString();
         dtUtil::Enumeration& previousValue = mProperty->GetEnumValue();
         std::string previousString = previousValue.GetName();

         // set our value to our object
         if (previousString != selectionString)
         {
            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mPropContainer,
               *mProperty->AsActorProperty(), previousString, selectionString);

            mProperty->SetValueFromString(selectionString);
            dataChanged = true;
         }
      }

      // notify the world (mostly the viewports) that our property changed
      if (dataChanged)
      {
         emit PropertyChanged(*mPropContainer, *mProperty->AsActorProperty());
      }

      return dataChanged;
   }


   /////////////////////////////////////////////////////////////////////////////////
   QWidget *DynamicEnumControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractControl::createEditor(parent, option, index);

      // create and init the combo box
      mTemporaryEditControl = new SubQComboBox(wrapper, this);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      const std::vector<dtUtil::Enumeration*>& options = mProperty->GetList();
      std::vector<dtUtil::Enumeration*>::const_iterator iter;

      for (iter = options.begin(); iter != options.end(); ++iter)
      {
         dtUtil::Enumeration* enumValue = (*iter);
         mTemporaryEditControl->addItem(QString(enumValue->GetName().c_str()));
      }
      mTemporaryEditControl->setToolTip(getDescription());

      mGridLayout->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
      mGridLayout->setColumnMinimumWidth(0, mTemporaryEditControl->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 1);

      connect(mTemporaryEditControl, SIGNAL(activated (int)), this, SLOT(itemSelected(int)));

      updateEditorFromModel(mWrapper);

      wrapper->setFocusProxy(mTemporaryEditControl);
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicEnumControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }
      return QString(tr(mProperty->AsActorProperty()->GetLabel().c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicEnumControl::getDescription()
   {
      std::string tooltip = mProperty->AsActorProperty()->GetDescription() + "  [Type: " +
         mProperty->AsActorProperty()->GetDataType().GetName() + "]";
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicEnumControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();
      if (doPropertiesMatch())
      {
         dtUtil::Enumeration &value = mProperty->GetEnumValue();
         return QString(value.GetName().c_str());
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
   void DynamicEnumControl::itemSelected(int index)
   {
      updateModelFromEditor(mWrapper);
      CopyBaseValueToLinkedProperties();
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicEnumControl::updateData(QWidget* widget)
   {
      if (!mInitialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return false;
      }

      return updateModelFromEditor(widget);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicEnumControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
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
