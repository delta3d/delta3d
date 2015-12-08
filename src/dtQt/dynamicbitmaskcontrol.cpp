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
#include <dtQt/dynamicbitmaskcontrol.h>
#include <dtQt/dynamicbyteelementcontrol.h>

#include <dtCore/actorproxy.h>
#include <dtCore/datatype.h>
#include <dtCore/bitmaskactorproperty.h>

#include <dtQt/dynamicsubwidgets.h>
#include <dtQt/propertyeditormodel.h>

#include <dtUtil/log.h>

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtCore/QVariant>

#include <assert.h>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicBitMaskControl::DynamicBitMaskControl()
      : mTemporaryEditControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicBitMaskControl::~DynamicBitMaskControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicBitMaskControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType() == dtCore::DataType::BIT_MASK)
      {
         mProperty = static_cast<dtCore::BitMaskActorProperty*>(newProperty);
         DynamicAbstractParentControl::InitializeData(newParent, newModel, newPC, newProperty);

         for (int byteIndex = 0; byteIndex < 4; byteIndex++)
         {
            DynamicByteElementControl* element = new DynamicByteElementControl(byteIndex);
            element->SetTreeView(mPropertyTree);
            element->SetDynamicControlFactory(GetDynamicControlFactory());

            int linkCount = (int)mLinkedProperties.size();
            for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
            {
               LinkedPropertyData& data = mLinkedProperties[linkIndex];
               element->AddLinkedProperty(data.propCon, data.property);
            }

            element->InitializeData(this, GetModel(), mPropContainer, mProperty);
            connect(element, SIGNAL(PropertyAboutToChange(dtCore::PropertyContainer&, dtCore::ActorProperty&,
               const std::string&, const std::string&)),
               this, SLOT(PropertyAboutToChangePassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&,
               const std::string&, const std::string&)));

            connect(element, SIGNAL(PropertyChanged(dtCore::PropertyContainer&, dtCore::ActorProperty&)),
               this, SLOT(PropertyChangedPassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&)));

            mChildren.push_back(element);
         }

         newModel->insertRows(0, 4, newModel->IndexOf(this));
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property \"" +
            propertyName + "\" is not the correct type.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicBitMaskControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         mTemporaryEditControl->setText(getValueAsString());

         unsigned int value = mProperty->GetValue();

         QMenu* menu = mTemporaryEditControl->menu();
         if (menu)
         {
            QList<QAction*> actions = menu->actions();
            int count = actions.count();
            for (int index = 0; index < count; ++index)
            {
               QAction* action = actions.at(index);
               if (action)
               {
                  unsigned int bitMask = action->data().toUInt();
                  if ((value & bitMask) == bitMask)
                  {
                     action->setChecked(true);
                  }
                  else
                  {
                     action->setChecked(false);
                  }
               }
            }
         }
      }

      DynamicAbstractParentControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicBitMaskControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractParentControl::updateModelFromEditor(widget);

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicBitMaskControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractParentControl::createEditor(parent, option, index);

      if (!IsInitialized())
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      // create and init the edit box
      mTemporaryEditControl = new SubQToolButton(wrapper, this);
      mTemporaryEditControl->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
      mTemporaryEditControl->setIcon(QIcon(QPixmap(1, 1)));
      mTemporaryEditControl->setToolTip(getDescription());
      mTemporaryEditControl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      mTemporaryEditControl->setPopupMode(QToolButton::InstantPopup);

      QMenu* menu = new QMenu("Resources");
      connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(itemSelected(QAction*)));
      mTemporaryEditControl->setMenu(menu);

      std::vector<std::string>  names;
      std::vector<unsigned int> values;
      mProperty->GetMaskList(names, values);
      assert(names.size() == values.size());
      int count = (int)names.size();
      for (int index = 0; index < count; ++index)
      {
         unsigned int value = values[index];
         std::string  name = "(" + getHexString(value).toStdString() + ") " + names[index];

         QAction* entry = menu->addAction(name.c_str());
         if (entry)
         {
            entry->setCheckable(true);
            entry->setData(QVariant(value));
         }
      }

      mGridLayout->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
      mGridLayout->setColumnMinimumWidth(0, mTemporaryEditControl->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 1);

      updateEditorFromModel(wrapper);

      wrapper->setFocusProxy(mTemporaryEditControl);
      return wrapper;
   }

   //////////////////////////////////////////////////////////////////////////
   const QString DynamicBitMaskControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }

      return tr(mProperty->GetLabel().c_str());
   }

   //////////////////////////////////////////////////////////////////////////
   const QString DynamicBitMaskControl::getDescription()
   {
      std::string tooltip = mProperty->GetDescription() + "  [Type: " +
         mProperty->GetDataType().GetName() + "]";
      return tr(tooltip.c_str());
   }

   //////////////////////////////////////////////////////////////////////////
   const QString DynamicBitMaskControl::getValueAsString()
   {
      DynamicAbstractParentControl::getValueAsString();

      if (doPropertiesMatch())
      {
         unsigned int propVal = mProperty->GetValue();
         unsigned int maskVal = propVal;

         QString result;

         std::vector<std::string>  names;
         std::vector<unsigned int> values;
         mProperty->GetMaskList(names, values);

         assert(names.size() == values.size());

         unsigned int highVal = 0xFFFFFFFF;
         int count = (int)names.size();
         for (int index = 0; index < count; ++index)
         {
            std::string  name  = "";
            unsigned int value = 0;
            for (int maskIndex = 0; maskIndex < count; ++maskIndex)
            {
               unsigned int testVal = values[maskIndex];
               if (testVal <= highVal &&
                  testVal >= value)
               {
                  value = testVal;
                  name = names[maskIndex];
               }
            }

            if (value > 0)
            {
               highVal = value - 1;
            }

            if ((maskVal & value) == value)
            {
               if (!result.isEmpty())
               {
                  result = QString(name.c_str()) + "|" + result;
               }
               else
               {
                  result = name.c_str();
               }

               maskVal &= (~value);
            }
         }

         result = "[" + getHexString(propVal) + "] " + result;

         return result;
      }
      else
      {
         return "<Multiple Values...>";
      }
   }

   //////////////////////////////////////////////////////////////////////////
   const QString DynamicBitMaskControl::getHexString(unsigned int value)
   {
      QString result = QString::number(value, 16);
      while (result.length() < 8)
      {
         result = "0" + result;
      }

      result = result.toUpper();

      result = "0x" + result;

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   void DynamicBitMaskControl::itemSelected(QAction* action)
   {
      if (!action)
      {
         return;
      }

      NotifyParentOfPreUpdate();

      unsigned int bitMask = action->data().toUInt();

      unsigned int result = mProperty->GetValue();

      if (action->isChecked())
      {
         // Set the bit(s).
         result |= bitMask;
      }
      else
      {
         // Un-set the bit(s).
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
      }

      CopyBaseValueToLinkedProperties();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicBitMaskControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      // we have to check - sometimes the destructor won't get called before the
      // next widget is created.  Then, when it is called, it sets the NEW editor to NULL!
      if (widget == mWrapper)
      {
         mTemporaryEditControl = NULL;
      }

      DynamicAbstractParentControl::handleSubEditDestroy(widget, hint);
   }
} // namespace dtQt
