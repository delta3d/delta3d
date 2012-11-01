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

#include <dtCore/actorproxy.h>
#include <dtCore/datatype.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/stringselectoractorproperty.h>

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
      , mSelectorProperty(NULL)
      , mTemporaryEditControl(NULL)
      , mTemporaryComboControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicStringControl::~DynamicStringControl()
   {
   }


   /////////////////////////////////////////////////////////////////////////////////
   void DynamicStringControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType() == dtCore::DataType::STRING)
      {
         mProperty = static_cast<dtCore::StringActorProperty*>(newProperty);
         mSelectorProperty = dynamic_cast<dtCore::StringSelectorActorProperty*>(newProperty);
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
      if (widget == mWrapper)
      {
         if (mTemporaryEditControl)
         {
            // set the current value from our property
            mTemporaryEditControl->setText(tr(mProperty->GetValue().c_str()));
            mTemporaryEditControl->selectAll();
         }

         if (mTemporaryComboControl)
         {
            mTemporaryComboControl->setCurrentIndex(mTemporaryComboControl->findText(tr(mProperty->GetValue().c_str())));
            mTemporaryComboControl->setEditText(tr(mProperty->GetValue().c_str()));
         }
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicStringControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget == mWrapper)
      {
         std::string result;

         // get the data from our control
         if (mTemporaryEditControl)
         {
            result = mTemporaryEditControl->text().toStdString();

            // reselect all the text when we commit.
            // Gives the user visual feedback that something happened.
            mTemporaryEditControl->selectAll();
         }
         else if (mTemporaryComboControl)
         {
            result = mTemporaryComboControl->currentText().toStdString();
         }

         // set our value to our object
         if (result != mProperty->GetValue())
         {
            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mPropContainer, *mProperty,
               mProperty->ToString(), result);

            mProperty->SetValue(result);
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
   QWidget *DynamicStringControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractControl::createEditor(parent, option, index);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      if (!mSelectorProperty)
      {
         // create and init the edit box
         mTemporaryEditControl = new SubQLineEdit(wrapper, this);
         mTemporaryEditControl->setToolTip(getDescription());
         if (mProperty->GetMaxLength() > 0)
         {
            mTemporaryEditControl->setMaxLength(mProperty->GetMaxLength());
         }

         connect(mTemporaryEditControl, SIGNAL(returnPressed()), this, SLOT(enterPressed()));

         updateEditorFromModel(mWrapper);

         mGridLayout->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
         mGridLayout->setColumnMinimumWidth(0, mTemporaryEditControl->sizeHint().width() / 2);
         mGridLayout->setColumnStretch(0, 1);

         wrapper->setFocusProxy(mTemporaryEditControl);
      }
      else
      {
         mTemporaryComboControl = new SubQComboBox(wrapper, this);
         mTemporaryComboControl->setToolTip(getDescription());
         std::vector<std::string> stringList = mSelectorProperty->GetList();
         int count = (int)stringList.size();
         for (int index = 0; index < count; ++index)
         {
            mTemporaryComboControl->addItem(stringList[index].c_str());
         }

         mTemporaryComboControl->setEditable(mSelectorProperty->IsEditable());

         updateEditorFromModel(mWrapper);

         mGridLayout->addWidget(mTemporaryComboControl, 0, 0, 1, 1);
         mGridLayout->setColumnMinimumWidth(0, mTemporaryComboControl->sizeHint().width() / 2);
         mGridLayout->setColumnStretch(0, 1);

         connect(mTemporaryComboControl, SIGNAL(activated(int)), this, SLOT(itemSelected(int)));

         wrapper->setFocusProxy(mTemporaryComboControl);
      }

      return wrapper;
   }

   const QString DynamicStringControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }
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
      if (doPropertiesMatch())
      {
         return QString(tr(mProperty->GetValue().c_str()));
      }
      else
      {
         return "<Multiple Values...>";
      }
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
   void DynamicStringControl::itemSelected(int index)
   {
      if (mTemporaryComboControl != NULL)
      {
         updateModelFromEditor(mWrapper);
         CopyBaseValueToLinkedProperties();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicStringControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mTemporaryEditControl = NULL;
         mTemporaryComboControl = NULL;
      }
      DynamicAbstractControl::handleSubEditDestroy(widget, hint);
   }
} // namespace dtQt
