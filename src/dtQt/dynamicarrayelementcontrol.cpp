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
 * Jeffrey Houde
 */

#include <prefix/dtstageprefix-src.h>

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include <dtQt/dynamicarrayelementcontrol.h>
#include <dtQt/dynamicarraycontrol.h>
#include <dtQt/basepropertyeditor.h>
#include <dtQt/propertyeditormodel.h>
#include <dtQt/propertyeditortreeview.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/arrayactorpropertybase.h>
#include <dtDAL/actorproperty.h>

#include <dtUtil/log.h>

//#include <dtEditQt/editordata.h>
//#include <dtEditQt/editorevents.h>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicArrayElementControl::DynamicArrayElementControl(int index)
      : mPropertyControl(NULL)
      , mShiftUpButton(NULL)
      , mShiftDownButton(NULL)
      , mCopyButton(NULL)
      , mDeleteButton(NULL)
   {
      SetIndex(index);
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicArrayElementControl::~DynamicArrayElementControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayElementControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::ARRAY)
      {
         mProperty = static_cast<dtDAL::ArrayActorPropertyBase*>(newProperty);
         DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);

         dtDAL::ActorProperty* propType = mProperty->GetArrayProperty();
         if (propType != NULL)
         {
            // Create property data for this array index.
            mPropertyControl = GetDynamicControlFactory()->CreateDynamicControl(*propType);
            if (mPropertyControl != NULL)
            {
               mPropertyControl->SetTreeView(mPropertyTree);
               mPropertyControl->SetDynamicControlFactory(GetDynamicControlFactory());
               mPropertyControl->InitializeData(this, newModel, newPC, propType);
               connect(mPropertyControl, SIGNAL(PropertyAboutToChange(dtDAL::PropertyContainer&, dtDAL::ActorProperty&,
                                 const std::string&, const std::string&)),
                        this, SLOT(PropertyAboutToChangePassThrough(dtDAL::PropertyContainer&, dtDAL::ActorProperty&,
                                 const std::string&, const std::string&)));

               connect(mPropertyControl, SIGNAL(PropertyChanged(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)),
                        this, SLOT(PropertyChangedPassThrough(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)));
               mChildren.push_back(mPropertyControl);
            }
         }
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property [" +
            propertyName + "] is not the correct type.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicArrayElementControl::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = new QWidget(parent);
      wrapper->setFocusPolicy(Qt::StrongFocus);
      // set the background color to white so that it sort of blends in with the rest of the controls
      SetBackgroundColor(wrapper, PropertyEditorTreeView::ROW_COLOR_ODD);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      QGridLayout* grid = new QGridLayout(wrapper);
      grid->setMargin(0);
      grid->setSpacing(1);

      // label
      mTextLabel = new SubQLabel(getValueAsString(), wrapper, this);
      SetBackgroundColor(mTextLabel, PropertyEditorTreeView::ROW_COLOR_ODD);

      mShiftUpButton   = new SubQPushButton(tr("Up"),     wrapper, this);
      mShiftDownButton = new SubQPushButton(tr("Down"),   wrapper, this);
      mCopyButton      = new SubQPushButton(tr("Copy"),   wrapper, this);
      mDeleteButton    = new SubQPushButton(tr("Delete"), wrapper, this);

      UpdateButtonStates();

      connect(mShiftUpButton,   SIGNAL(clicked()), this, SLOT(onShiftUpClicked()));
      connect(mShiftDownButton, SIGNAL(clicked()), this, SLOT(onShiftDownClicked()));
      connect(mCopyButton,      SIGNAL(clicked()), this, SLOT(onCopyClicked()));
      connect(mDeleteButton,    SIGNAL(clicked()), this, SLOT(onDeleteClicked()));

      mTextLabel->setToolTip(getDescription());

      grid->addWidget(mTextLabel,       0, 0, 1, 1);
      grid->addWidget(mShiftUpButton,   0, 1, 1, 1);
      grid->addWidget(mShiftDownButton, 0, 2, 1, 1);
      grid->addWidget(mCopyButton,      0, 3, 1, 1);
      grid->addWidget(mDeleteButton,    0, 4, 1, 1);
      grid->setColumnMinimumWidth(1, mShiftUpButton->sizeHint().width() / 2);
      grid->setColumnMinimumWidth(2, mShiftDownButton->sizeHint().width() / 2);
      grid->setColumnMinimumWidth(3, mCopyButton->sizeHint().width() / 2);
      grid->setColumnMinimumWidth(4, mDeleteButton->sizeHint().width() / 2);
      grid->setColumnStretch(0, 2);
      grid->setColumnStretch(1, 0);
      grid->setColumnStretch(2, 0);
      grid->setColumnStretch(3, 0);
      grid->setColumnStretch(4, 0);

      mWrapper = wrapper;
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayElementControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mWrapper         = NULL;
         mTextLabel       = NULL;
         mShiftUpButton   = NULL;
         mShiftDownButton = NULL;
         mCopyButton      = NULL;
         mDeleteButton    = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayElementControl::OnChildPreUpdate(DynamicAbstractControl* child)
   {
      DynamicAbstractControl::OnChildPreUpdate(this);

      // If the child is being edited, set this as the active index.
      SetActive();
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicArrayElementControl::getDisplayName()
   {
      return QString("[") + QString::number(mIndex) + QString("]");
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicArrayElementControl::getDescription()
   {
      if (mProperty.valid())
      {
         return QString(tr(mProperty->GetDescription().c_str())) + QString("  [Type: ") +
            QString(tr(mProperty->GetDataType().GetName().c_str())) + QString(" Index: ") +
            QString::number(mIndex) + QString("]");
      }
      else
      {
         LOG_ERROR("Dynamic array control has an invalid property type");
         return tr("");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicArrayElementControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();

      if (mPropertyControl)
      {
         const dtDAL::ActorProperty* prop = mProperty->GetArrayProperty();

         if (!prop)
         {
            return QString("[Unknown Type] = {") + mPropertyControl->getValueAsString() + QString("}");
         }

         // TODO ARRAY: Show the value of the property only when this control is not expanded.
         return QString("[") + QString(prop->GetDataType().GetDisplayName().c_str()) + QString("] = {") + mPropertyControl->getValueAsString() + QString("}");
      }

      return QString("");
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DynamicArrayElementControl::isEditable()
   {
      return !mProperty->IsReadOnly();
   }

   ////////////////////////////////////////////////////////////////////////////////
   int DynamicArrayElementControl::GetIndex()
   {
      return mIndex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayElementControl::SetIndex(int index)
   {
      mIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayElementControl::SetActive()
   {
      if (mProperty.valid())
      {
         mProperty->SetIndex(mIndex);
      }
      else
      {
         LOG_ERROR("Dynamic array control has an invalid property type");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   bool DynamicArrayElementControl::updateData(QWidget* widget)
   {
      // this guy doesn't have any editors.  All the data is edited in child controls
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayElementControl::onShiftUpClicked()
   {
      if (!mProperty->CanReorder() || mIndex <= 0)
      {
         return;
      }

      // Get our parent.
      DynamicArrayControl* parent = static_cast<DynamicArrayControl*>(getParent());
      if (!parent)
      {
         return;
      }

      NotifyParentOfPreUpdate();
      mProperty->Swap(mIndex, mIndex - 1);
      parent->resizeChildren(true, true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayElementControl::onShiftDownClicked()
   {
      if (!mProperty->CanReorder() || mIndex + 1 >= mProperty->GetArraySize())
      {
         return;
      }

      // Get our parent.
      DynamicArrayControl* parent = static_cast<DynamicArrayControl*>(getParent());
      if (!parent)
      {
         return;
      }

      NotifyParentOfPreUpdate();
      mProperty->Swap(mIndex, mIndex + 1);
      parent->resizeChildren(true, true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayElementControl::onCopyClicked()
   {
      // Check if we are at our size limit.
      int curSize = mProperty->GetArraySize();
      int maxSize = mProperty->GetMaxArraySize();
      if (maxSize > -1 && curSize >= maxSize)
      {
         return;
      }

      // Get our parent.
      DynamicArrayControl* parent = static_cast<DynamicArrayControl*>(getParent());
      if (!parent)
      {
         return;
      }

      NotifyParentOfPreUpdate();
      mProperty->Insert(mIndex);
      mProperty->Copy(mIndex + 1, mIndex);
      parent->resizeChildren(false, true);
      UpdateButtonStates();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayElementControl::onDeleteClicked()
   {
      // Check if we are at our min array size.
      int curSize = mProperty->GetArraySize();
      int minSize = mProperty->GetMinArraySize();
      if (minSize > -1 && curSize <= minSize)
      {
         return;
      }

      // Get our parent.
      DynamicArrayControl* parent = static_cast<DynamicArrayControl*>(getParent());
      if (!parent)
      {
         return;
      }

      NotifyParentOfPreUpdate();
      mProperty->Remove(mIndex);
      UpdateButtonStates();

      // Must be done last, there is a possibility that this element will be deleted here.
      parent->resizeChildren(false, true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayElementControl::UpdateButtonStates()
   {
      // Don't do anything if we have no buttons.
      if (!mShiftUpButton || !mShiftDownButton ||
         !mCopyButton || !mDeleteButton)
      {
         return;
      }

      bool canShiftUp   = true;
      bool canShiftDown = true;
      bool canCopy      = true;
      bool canDelete    = true;

      // Check if this element can be shifted up.
      if (!mProperty->CanReorder() || mIndex <= 0)
      {
         canShiftUp = false;
      }

      // Check if this element can be shifted down.
      if (!mProperty->CanReorder() || mIndex + 1 >= mProperty->GetArraySize())
      {
         canShiftDown = false;
      }

      // Check if we are at our max array size.
      int curSize = mProperty->GetArraySize();
      int maxSize = mProperty->GetMaxArraySize();
      if (maxSize > -1 && curSize >= maxSize)
      {
         canCopy = false;
      }

      // Check if we are at our min array size.
      int minSize = mProperty->GetMinArraySize();
      if (minSize > -1 && curSize <= minSize)
      {
         canDelete = false;
      }

      mShiftUpButton->setDisabled(!canShiftUp);
      mShiftDownButton->setDisabled(!canShiftDown);
      mCopyButton->setDisabled(!canCopy);
      mDeleteButton->setDisabled(!canDelete);
   }

} // namespace dtQt
