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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_propertycontrolarray.h"
#include <dtQt/propertycontrolarray.h>
#include <QtGui/QLabel>
#include <QtGui/QListWidgetItem>
#include <QtGui/QSizePolicy>
#include <sstream>

// TEMP:
#include <dtQt/propertycontrolcolor.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyControlArray::PropertyControlArray(QWidget* parent)
      : BaseClass(parent, dtCore::DataType::ARRAY)
      , mUI(new Ui::PropertyControlArray)
   {
      mUI->setupUi(this);
   }

   PropertyControlArray::~PropertyControlArray()
   {
      delete mUI;
      mUI = NULL;
   }

   QObject* PropertyControlArray::GetControlUI()
   {
      return mUI->mLayout;
   }

   QLabel* PropertyControlArray::GetLabel() const
   {
      return mUI->mLabel;
   }

   void PropertyControlArray::CreateConnections()
   {
      BaseClass::CreateConnections();

      connect(mUI->mArraySize, SIGNAL(valueChanged(int)),
         this, SLOT(OnArraySizeChanged(int)));

      connect(mUI->mButtonAdd, SIGNAL(clicked(bool)),
         this, SLOT(OnAdd()));
      connect(mUI->mButtonRemove, SIGNAL(clicked(bool)),
         this, SLOT(OnRemove()));
      connect(mUI->mButtonMoveUp, SIGNAL(clicked(bool)),
         this, SLOT(OnMoveUp()));
      connect(mUI->mButtonMoveDown, SIGNAL(clicked(bool)),
         this, SLOT(OnMoveDown()));

      connect(mUI->mValues, SIGNAL(currentCellChanged(int, int, int, int)),
         this, SLOT(OnCurrentIndexChanged(int, int, int, int)));
   }

   void PropertyControlArray::UpdateUI(const PropertyType& prop)
   {
      const ArrayProperty* aprop = 
         dynamic_cast<const ArrayProperty*>(&prop);

      if (aprop != NULL)
      {
         dtCore::DataType& dataType = aprop->GetDataType();

         int numItems = aprop->GetArraySize();

         // Reset the list.
         mUI->mValues->clear();
         mUI->mValues->setRowCount(0);

         QListWidgetItem* item = NULL;
         for (int i = 0; i < numItems; ++i)
         {
            // TODO: Change the property to return an object
            // rather than setting a "sticky" index.
            aprop->SetIndex(i);

            // HACK:
            QLabel* label = new QLabel;
            label->setFrameShape(QFrame::Box);
            label->setLineWidth(1);
            label->setBaseSize(60,60);
            label->setAutoFillBackground(true);
            ClickableLabelColorPicker* picker = new ClickableLabelColorPicker(*label);
            QColor c(255,255,255);
            picker->SetUIColor(c);

            // Create the actual item.
            QWidget* widget = picker;//new QSpinBox;

            // Add the item and its associated widget.
            mUI->mValues->insertRow(i);
            mUI->mValues->setCellWidget(i,0,widget);
         }

         mUI->mArraySize->blockSignals(true);
         mUI->mArraySize->setValue(numItems);
         mUI->mArraySize->blockSignals(false);

         UpdateButtons();
      }
   }

   void PropertyControlArray::UpdateData(PropertyType& propToUpdate)
   {
      // TODO:
   }

   void PropertyControlArray::UpdateButtons()
   {
      ArrayProperty* prop = GetArrayProperty();
      bool canReorder = prop != NULL && prop->CanReorder();
      int currentIndex = GetCurrentIndex();
      int numItems = GetItemCount();

      // Set enable states.
      mUI->mButtonRemove->setEnabled(numItems > 0);
      mUI->mButtonMoveUp->setEnabled(currentIndex > 0);
      mUI->mButtonMoveDown->setEnabled(numItems > 1 && currentIndex + 1 < numItems);

      // Set visible states.
      mUI->mButtonMoveUp->setVisible(canReorder);
      mUI->mButtonMoveDown->setVisible(canReorder);
   }

   void PropertyControlArray::InsertItem(int index)
   {
      ArrayProperty* prop = GetArrayProperty();

      // Add element to array property.
      if (prop != NULL && prop->Insert(index))
      {
         // Signal to self to update the UI.
         emit SignalDataChanged();
      }
   }

   void PropertyControlArray::RemoveItem(int index)
   {
      ArrayProperty* prop = GetArrayProperty();

      // Remove element from array property.
      if (prop != NULL && prop->Remove(index))
      {
         // Signal to self to update the UI.
         emit SignalDataChanged();
      }
   }

   void PropertyControlArray::SetCurrentIndex(int index)
   {
      mUI->mValues->setCurrentCell(index, 0);
   }

   int PropertyControlArray::GetCurrentIndex() const
   {
      return mUI->mValues->currentRow();
   }

   int PropertyControlArray::GetItemCount() const
   {
      return mUI->mValues->rowCount();
   }

   PropertyControlArray::ArrayProperty* PropertyControlArray::GetArrayProperty() const
   {
      return dynamic_cast<PropertyControlArray::ArrayProperty*>(GetLinkedProperty());
   }

   void PropertyControlArray::OnAdd()
   {
      int index = GetCurrentIndex();

      if (index < 0)
      {
         index = GetItemCount();
      }

      InsertItem(index);

      // Set focus
      SetCurrentIndex(index);
   }

   void PropertyControlArray::OnRemove()
   {
      int index = GetCurrentIndex();

      if (index < 0)
      {
         index = GetItemCount();
      }

      if (index >= 0)
      {
         RemoveItem(index);

         // Set focus
         SetCurrentIndex(index);
      }
   }

   void PropertyControlArray::OnMoveUp()
   {
      ArrayProperty* prop = GetArrayProperty();

      if (prop != NULL)
      {
         int index = GetCurrentIndex();
         int itemCount = GetItemCount();

         if (itemCount > 1 && index > 0)
         {
            prop->Swap(index, index - 1);

            // Signal to self to update the UI.
            emit SignalDataChanged();

            // Set focus
            SetCurrentIndex(index - 1);
         }
      }
   }

   void PropertyControlArray::OnMoveDown()
   {
      ArrayProperty* prop = GetArrayProperty();

      if (prop != NULL)
      {
         int index = GetCurrentIndex();
         int itemCount = GetItemCount();

         if (itemCount > 1 && index >= 0 && index + 1 < itemCount)
         {
            prop->Swap(index, index + 1);

            // Signal to self to update the UI.
            emit SignalDataChanged();

            // Set focus
            SetCurrentIndex(index + 1);
         }
      }
   }

   void PropertyControlArray::OnArraySizeChanged(int newSize)
   {
      ArrayProperty* prop = GetArrayProperty();

      if (prop != NULL)
      {
         int currentSize = prop->GetArraySize();
         int diff = newSize - currentSize;
         int limit = abs(diff);

         if (diff != 0)
         {
            for (int i = 0; i < limit; ++i)
            {
               if (diff < 0)
               {
                  prop->PopBack();
               }
               else
               {
                  prop->PushBack();
               }
            }

            // Signal to self to update the UI.
            emit SignalDataChanged();
         }
      }
   }

   void PropertyControlArray::OnCurrentIndexChanged(int curRow, int curColumn, int prevRow, int prevColumn)
   {
      UpdateButtons();

      // Signal to any external objects that the current index changed.
      emit SignalCurrentIndexChanged(curRow);
   }

}
