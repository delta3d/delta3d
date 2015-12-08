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
 * Jeff P. Houde
 */

#include <prefix/dtqtprefix.h>

#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

#include <dtQt/dynamicbyteelementcontrol.h>
#include <dtQt/dynamicbitmaskcontrol.h>
#include <dtQt/basepropertyeditor.h>
#include <dtQt/propertyeditormodel.h>
#include <dtQt/propertyeditortreeview.h>

#include <dtCore/actorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/arrayactorpropertybase.h>
#include <dtCore/actorproperty.h>
#include <dtCore/bitmaskactorproperty.h>

#include <dtUtil/log.h>

//#include <dtEditQt/editordata.h>
//#include <dtEditQt/editorevents.h>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicByteElementControl::DynamicByteElementControl(int index)
      : mProperty(NULL)
      , mTextLabel(NULL)
   {
      SetByteIndex(index);
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicByteElementControl::~DynamicByteElementControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicByteElementControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType() == dtCore::DataType::BIT_MASK)
      {
         mProperty = static_cast<dtCore::BitMaskActorProperty*>(newProperty);
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
   void DynamicByteElementControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTextLabel)
      {
         if (doPropertiesMatch())
         {
            // set the current value from our property
            mTextLabel->setText(getHexString(getByteValue()));

            int count = (int)mBitButtons.size();
            for (int index = 0; index < count; ++index)
            {
               SubQToolButton* bitButton = mBitButtons[index];
               if (bitButton)
               {
                  bool toggled = getBitValue(index);
                  if (toggled)
                  {
                     bitButton->setText("1");
                     bitButton->setChecked(true);
                  }
                  else
                  {
                     bitButton->setText("0");
                     bitButton->setChecked(false);
                  }
               }
            }
         }
         else
         {
            int count = (int)mBitButtons.size();
            for (int index = 0; index < count; ++index)
            {
               SubQToolButton* bitButton = mBitButtons[index];
               if (bitButton)
               {
                  bitButton->setText("0");
                  bitButton->setChecked(false);
               }
            }
         }
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicByteElementControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicByteElementControl::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = new QWidget(parent);
      wrapper->setFocusPolicy(Qt::StrongFocus);
      // set the background color to white so that it sort of blends in with the rest of the controls
      SetBackgroundColor(wrapper, PropertyEditorTreeView::ROW_COLOR_ODD);

      if (!IsInitialized())
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      mGridLayout = new QGridLayout(wrapper);
      mGridLayout->setMargin(0);
      mGridLayout->setSpacing(1);

      // label
      mTextLabel = new SubQLabel(getHexString(getByteValue()), wrapper, this);
      mTextLabel->setToolTip(getDescription());
      SetBackgroundColor(mTextLabel, PropertyEditorTreeView::ROW_COLOR_ODD);

      mGridLayout->addWidget(mTextLabel, 0, 0, 1, 1);

      SubQToolButton* bitButton = NULL;
      int cols = 8;
      for (int col = 1; col <= cols; ++col)
      {
         bitButton = new SubQToolButton(wrapper, this);
         if (bitButton)
         {
            mBitButtons.push_back(bitButton);

            bitButton->setCheckable(true);
            bitButton->setMaximumWidth(30);
            mGridLayout->addWidget(bitButton, 0, col, 1, 1);

            connect(bitButton, SIGNAL(toggled(bool)), this, SLOT(onBitToggled(bool)));

            mGridLayout->setColumnMinimumWidth(col, bitButton->sizeHint().width() / 2);
            mGridLayout->setColumnStretch(col, 0);
         }
      }

      mGridLayout->setColumnStretch(9, 1);

      updateModelFromEditor(wrapper);

      mWrapper = wrapper;
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicByteElementControl::getDisplayName()
   {
      return QString("[") + QString::number(mIndex) + QString("]");
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicByteElementControl::getDescription()
   {
      if (mProperty)
      {
         return QString(tr(mProperty->GetDescription().c_str())) + QString("  [Type: ") +
            QString(tr(mProperty->GetDataType().GetName().c_str())) + QString(" Index: ") +
            QString::number(mIndex) + QString("]");
      }
      else
      {
         LOG_ERROR("Dynamic byte element control has an invalid property type");
         return tr("");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicByteElementControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();

      if (doPropertiesMatch())
      {
         QString result = getHexString(getByteValue());
         result += " (";

         int count = 8;
         for (int index = 0; index < count; ++index)
         {
            if (index > 0)
            {
               result += ",";
            }

            if (getBitValue(index))
            {
               result += "1";
            }
            else
            {
               result += "0";
            }
         }
         result += ")";

         return result;
      }
      else
      {
         return "<Multiple Values...>";
      }
   }

   //////////////////////////////////////////////////////////////////////////
   const QString DynamicByteElementControl::getHexString(unsigned int value)
   {
      QString result = QString::number(value, 16);

      while (result.length() < 2)
      {
         result = "0" + result;
      }

      while (result.length() < 2 * (4 - mIndex))
      {
         result += "-";
      }

      while (result.length() < 8)
      {
         result = "-" + result;
      }

      result = result.toUpper();

      result = "0x" + result + "  ";

      return result;
   }

   //////////////////////////////////////////////////////////////////////////
   unsigned int DynamicByteElementControl::getByteValue()
   {
      unsigned int value = mProperty->GetValue();

      int shift = 8 * (3 - mIndex);
      value = value >> shift;
      return (value & 0xFF);
   }

   //////////////////////////////////////////////////////////////////////////
   bool DynamicByteElementControl::getBitValue(int bitIndex)
   {
      unsigned int value = getByteValue();
      int shift = 7 - bitIndex;
      value = value >> shift;
      return (value & 1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int DynamicByteElementControl::GetByteIndex()
   {
      return mIndex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicByteElementControl::SetByteIndex(int index)
   {
      mIndex = index;
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   void DynamicByteElementControl::onBitToggled(bool checked)
   {
      if (!sender())
      {
         return;
      }

      NotifyParentOfPreUpdate();
      SubQToolButton* bitButton = NULL;
      int count = (int)mBitButtons.size();
      int index = 0;
      for (index = 0; index < count; ++index)
      {
         bitButton = mBitButtons[index];
         if (bitButton == sender())
         {
            break;
         }
         bitButton = NULL;
      }

      if (bitButton && index < 8)
      {
         unsigned int bitMask = 1 << (7 - index);
         bitMask = bitMask << (8 * (3 - mIndex));

         unsigned int result = mProperty->GetValue();

         if (checked)
         {
            result |= bitMask;
         }
         else
         {
            result &= (~bitMask);
         }

         if (result != mProperty->GetValue())
         {
            std::string oldValue = mProperty->ToString();
            mProperty->SetValue(result);

            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mPropContainer, *mProperty,
               oldValue, mProperty->ToString());

            emit PropertyChanged(*mPropContainer, *mProperty);

            CopyBaseValueToLinkedProperties();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicByteElementControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mTextLabel  = NULL;
         mGridLayout = NULL;
         mBitButtons.clear();
      }

      DynamicAbstractControl::handleSubEditDestroy(widget, hint);
   }
} // namespace dtQt
