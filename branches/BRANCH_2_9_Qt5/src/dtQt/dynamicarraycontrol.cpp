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

#include <prefix/dtqtprefix.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <dtQt/dynamicarraycontrol.h>
#include <dtQt/propertyeditormodel.h>
#include <dtQt/propertyeditortreeview.h>

#include <dtCore/actorproxy.h>
#include <dtCore/actorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/arrayactorpropertybase.h>
#include <dtCore/namedarrayparameter.h>
#include <dtUtil/log.h>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // HELPER
   /////////////////////////////////////////////////////////////////////////////
   struct ForEachPropertyPred
   {
      enum Mode
      {
         ADD,
         SWAP,
         COPY,
         DELETE,
         CLEAR
      };

      DynamicArrayControl* mControl;
      Mode mMode;
      int mIndex;
      int mTargetIndex;

      ForEachPropertyPred(DynamicArrayControl& control, Mode mode)
         : mControl(&control)
         , mMode(mode)
         , mIndex(0)
         , mTargetIndex(0)
      {}

      void HandleProperty(dtCore::PropertyContainer& container, dtCore::ArrayActorPropertyBase& prop)
      {
         switch (mMode)
         {
         case ADD:
            mControl->ItemAdd(container, prop);
            break;
         case SWAP:
            mControl->ItemSwap(container, prop, mIndex, mTargetIndex);
            break;
         case COPY:
            mControl->ItemCopy(container, prop, mIndex);
            break;
         case DELETE:
            mControl->ItemDelete(container, prop, mIndex);
            break;
         case CLEAR:
            mControl->ItemClear(container, prop);
            break;
         default:
            break;
         }
      }

      void operator() (DynamicAbstractControl::LinkedPropertyData& data)
      {
         // Perform an operation on each of the linked properties.
         dtCore::ArrayActorPropertyBase* linkedProp =
            dynamic_cast<dtCore::ArrayActorPropertyBase*>(data.property);

         if (linkedProp != NULL)
         {
            HandleProperty(*data.propCon, *linkedProp);
         }
      }
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DynamicArrayControl::DynamicArrayControl()
      : mTextLabel(NULL)
      , mAddButton(NULL)
      , mClearButton(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   DynamicArrayControl::~DynamicArrayControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::updateEditorFromModel(QWidget* widget)
   {
      DynamicAbstractParentControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType() == dtCore::DataType::ARRAY)
      {
         mProperty = static_cast<dtCore::ArrayActorPropertyBase*>(newProperty);
         DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);

         // Create each element.
         resizeChildren(false, false);
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property [" +
            propertyName + "] is not the correct type.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicArrayControl::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractParentControl::createEditor(parent, option, index);

      if (!IsInitialized())
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      // label
      mTextLabel = new SubQLabel(getValueAsString(), wrapper, this);
      SetBackgroundColor(mTextLabel, PropertyEditorTreeView::ROW_COLOR_ODD);

      mAddButton   = new SubQToolButton(wrapper, this);
      mAddButton->setIcon(QIcon(":dtQt/icons/add.png"));
      mAddButton->setToolTip(tr("Add a new element"));
      mAddButton->setText("Add");

      mClearButton = new SubQToolButton(wrapper, this);
      mClearButton->setIcon(QIcon(":dtQt/icons/trashcan.png"));
      mClearButton->setText("Clear");
      mClearButton->setToolTip(tr("Remove all elements"));

      UpdateButtonStates();

      connect(mAddButton,   SIGNAL(clicked()), this, SLOT(onAddClicked()));
      connect(mClearButton, SIGNAL(clicked()), this, SLOT(onClearClicked()));

      mTextLabel->setToolTip(getDescription());

      mGridLayout->addWidget(mTextLabel,   0, 0, 1, 1);
      mGridLayout->addWidget(mAddButton,   0, 1, 1, 1);
      mGridLayout->addWidget(mClearButton, 0, 2, 1, 1);
      mGridLayout->setColumnMinimumWidth(1, mAddButton->sizeHint().width() / 2);
      mGridLayout->setColumnMinimumWidth(2, mClearButton->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 2);
      mGridLayout->setColumnStretch(1, 0);
      mGridLayout->setColumnStretch(2, 0);

      wrapper->setFocusProxy(mAddButton);
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mTextLabel   = NULL;
         mAddButton   = NULL;
         mClearButton = NULL;
      }

      DynamicAbstractParentControl::handleSubEditDestroy(widget, hint);
   }

   /////////////////////////////////////////////////////////////////////////////
   const QString DynamicArrayControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }

      if (mProperty.valid())
      {
         return QString(tr(mProperty->GetLabel().c_str()));
      }
      else
      {
         LOG_ERROR("Dynamic array control has an invalid property type");
         return tr("");
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const QString DynamicArrayControl::getDescription()
   {
      if(mProperty.valid())
      {
         return QString(tr(mProperty->GetDescription().c_str())) + QString("  [Type: ") +
            QString(tr(mProperty->GetDataType().GetName().c_str())) + QString("]");
      }
      else
      {
         LOG_ERROR("Dynamic array control has an invalid property type");
         return tr("");
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const QString DynamicArrayControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();

      if (doPropertiesMatchInArraySize())
      {
         if (mProperty.valid())
         {
            int arrayCount = mProperty->GetArraySize();
            int minCount   = mProperty->GetMinArraySize();
            int maxCount   = mProperty->GetMaxArraySize();

            QString minText("0");
            if (minCount >  0) { minText = QString::number(minCount); }

            QString maxText("*");
            if (maxCount > -1) { maxText = QString::number(maxCount); }

            return QString::number(arrayCount) + " Elements (" + minText + "-" + maxText + ")";
         }
         else
         {
            LOG_ERROR("Dynamic array control has an invalid property type");
            return tr("");
         }
      }
      else
      {
         return "<Multiple Sizes (Disabled)>";
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::resizeChildren(bool forceRefresh, bool isChild)
   {
      if (!doPropertiesMatchInArraySize())
      {
         return;
      }

      int childCount = getChildCount();
      int size = mProperty->GetArraySize();

      PropertyEditorModel* model = GetModel();
      if (model != NULL)
      {
         // If we are removing, clear all the children and then add the proper amount back.
         // This is done so all items will be refreshed.
         if (size < childCount || forceRefresh)
         {
            model->removeRows(0, childCount, model->IndexOf(this));

            removeAllChildren(model);

            childCount = 0;
         }

         // Add new children...
         if (size > childCount)
         {
            int addCount = size - childCount;

            for (int childIndex = 0; childIndex < addCount; childIndex++)
            {
               mProperty->SetIndex(childCount + childIndex);

               dtCore::ActorProperty* propType = mProperty->GetArrayProperty();
               if (propType)
               {
                  DynamicAbstractControl* element = GetDynamicControlFactory()->CreateDynamicControl(*propType);
                  element->SetTreeView(mPropertyTree);
                  element->SetDynamicControlFactory(GetDynamicControlFactory());
                  element->SetArrayIndex(childCount + childIndex);

                  int linkCount = (int)mLinkedProperties.size();
                  for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
                  {
                     LinkedPropertyData& data = mLinkedProperties[linkIndex];
                     dtCore::ArrayActorPropertyBase* linkedArrayProp =
                        dynamic_cast<dtCore::ArrayActorPropertyBase*>(data.property);
                     if (linkedArrayProp)
                     {
                        linkedArrayProp->SetIndex(childCount + childIndex);
                        dtCore::ActorProperty* linkedPropType = linkedArrayProp->GetArrayProperty();
                        element->AddLinkedProperty(data.propCon, linkedPropType);
                     }
                  }

                  element->InitializeData(this, GetModel(), mPropContainer.get(), propType);
                  connect(element, SIGNAL(PropertyAboutToChange(dtCore::PropertyContainer&, dtCore::ActorProperty&,
                     const std::string&, const std::string&)),
                     this, SLOT(PropertyAboutToChangePassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&,
                     const std::string&, const std::string&)));

                  connect(element, SIGNAL(PropertyChanged(dtCore::PropertyContainer&, dtCore::ActorProperty&)),
                     this, SLOT(PropertyChangedPassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&)));

                  // Connect the buttons of the new control.
                  connect(element, SIGNAL(SignalShiftUpClicked(int)),
                     this, SLOT(onItemShiftUpClicked(int)));

                  connect(element, SIGNAL(SignalShiftDownClicked(int)),
                     this, SLOT(onItemShiftDownClicked(int)));

                  connect(element, SIGNAL(SignalCopyClicked(int)),
                     this, SLOT(onItemCopyClicked(int)));

                  connect(element, SIGNAL(SignalDeleteClicked(int)),
                     this, SLOT(onItemDeleteClicked(int)));

                  mChildren.push_back(element);
               }
            }

            model->insertRows(childCount, addCount, model->IndexOf(this));

            if (!isChild)
            {
               UpdateButtonStates();
            }
         }
      }

      // update our label
      if (mTextLabel !=  NULL)
      {
         mTextLabel->setText(getValueAsString());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::SetIndexFocus(int index)
   {
      if (index >= 0 && index < (int)mChildren.size())
      {
         mPropertyTree->setCurrentIndex(mModel->IndexOf(mChildren[index]));
      }
      else
      {
         mPropertyTree->setCurrentIndex(mModel->IndexOf(this));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DynamicArrayControl::doPropertiesMatchInArraySize()
   {
      // If there are no linked properties to compare with then
      // there is no need to perform the test.
      if (mLinkedProperties.empty())
      {
         return true;
      }

      // Retrieve the array size of the base property.
      int baseSize = mProperty->GetArraySize();

      // Iterate through our linked properties and compare values.
      int count = (int)mLinkedProperties.size();
      for (int index = 0; index < count; ++index)
      {
         const LinkedPropertyData& data = mLinkedProperties[index];

         dtCore::ArrayActorPropertyBase* linkedProp =
            dynamic_cast<dtCore::ArrayActorPropertyBase*>(data.property);
         if (linkedProp)
         {
            int linkedSize = linkedProp->GetArraySize();

            // If at any time, one of the linked values does not match
            // with the base, then we do not match.
            if (baseSize != linkedSize)
            {
               return false;
            }
         }
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   bool DynamicArrayControl::updateData(QWidget* widget)
   {
      // this guy doesn't have any editors.  All the data is edited in child controls
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::onAddClicked()
   {
      NotifyParentOfPreUpdate();

      ForEachPropertyPred pred(*this, ForEachPropertyPred::ADD);
      pred.HandleProperty(*mPropContainer, *mProperty);

      // Now perform the add operation on each of the linked properties.
      std::vector<LinkedPropertyData>& props = GetLinkedProperties();
      std::for_each(props.begin(), props.end(), pred);

      resizeChildren();
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::onClearClicked()
   {
      NotifyParentOfPreUpdate();

      ForEachPropertyPred pred(*this, ForEachPropertyPred::CLEAR);
      pred.HandleProperty(*mPropContainer, *mProperty);

      // Now perform the clear operation on each of the linked properties.
      std::vector<LinkedPropertyData>& props = GetLinkedProperties();
      std::for_each(props.begin(), props.end(), pred);

      resizeChildren();
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::onResetClicked()
   {
      std::string oldValue = mProperty->ToString();
      DynamicAbstractParentControl::onResetClicked();

      //PropertyAboutToChange(*mPropContainer, *mProperty,
      //   oldValue, mProperty->ToString());
      //PropertyChanged(*mPropContainer, *mProperty);

      resizeChildren();
   }

   /////////////////////////////////////////////////////////////////////////////////
   // PRIVATE
   /////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::UpdateButtonStates()
   {
      DynamicAbstractParentControl::UpdateButtonStates();

      // Don't do anything if we have no buttons.
      if (!mAddButton || !mClearButton)
      {
         return;
      }

      // If we are multiple selecting and the array size does not
      // match on all selected, we cannot allow this property to
      // be edited.
      if (!doPropertiesMatchInArraySize())
      {
         mAddButton->setEnabled(false);
         mClearButton->setEnabled(true);
         if (mDefaultResetButton != NULL) mDefaultResetButton->setEnabled(false);
      }
      else
      {
         // Check if we are at our max array size.
         int curSize = mProperty->GetArraySize();
         int maxSize = mProperty->GetMaxArraySize();
         if (maxSize > -1 && curSize >= maxSize)
         {
            mAddButton->setDisabled(true);
         }
         else
         {
            mAddButton->setDisabled(false);
         }

         // Check if we are at our min array size.
         int minSize = mProperty->GetMinArraySize();
         if (minSize > -1 && curSize <= minSize)
         {
            mClearButton->setDisabled(true);
         }
         else
         {
            mClearButton->setDisabled(false);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::onItemShiftUpClicked(int itemIndex)
   {
      if (itemIndex <= 0)
      {
         return;
      }

      if (mProperty.valid())
      {
         if (!mProperty->CanReorder())
         {
            return;
         }

         NotifyParentOfPreUpdate();

         ForEachPropertyPred pred(*this, ForEachPropertyPred::SWAP);
         pred.mIndex = itemIndex;
         pred.mTargetIndex = itemIndex - 1;
         pred.HandleProperty(*mPropContainer, *mProperty);

         // Now perform the shift operation on each of the linked properties.
         std::vector<LinkedPropertyData>& props = GetLinkedProperties();
         std::for_each(props.begin(), props.end(), pred);

         int nextIndex = itemIndex - 1;
         mPropertyTree->closeEditor(mWrapper, QAbstractItemDelegate::NoHint);
         resizeChildren(true, true);
         SetIndexFocus(nextIndex);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::onItemShiftDownClicked(int itemIndex)
   {
      if (mProperty.valid())
      {
         if (!mProperty->CanReorder() || itemIndex + 1 >= mProperty->GetArraySize())
         {
            return;
         }

         NotifyParentOfPreUpdate();

         ForEachPropertyPred pred(*this, ForEachPropertyPred::SWAP);
         pred.mIndex = itemIndex;
         pred.mTargetIndex = itemIndex + 1;
         pred.HandleProperty(*mPropContainer, *mProperty);

         // Now perform the shift operation on each of the linked properties.
         std::vector<LinkedPropertyData>& props = GetLinkedProperties();
         std::for_each(props.begin(), props.end(), pred);

         int nextIndex = itemIndex + 1;
         mPropertyTree->closeEditor(mWrapper, QAbstractItemDelegate::NoHint);
         resizeChildren(true, true);
         SetIndexFocus(nextIndex);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::onItemCopyClicked(int itemIndex)
   {
      if (mProperty.valid())
      {
         int curSize = mProperty->GetArraySize();
         int maxSize = mProperty->GetMaxArraySize();
         if (maxSize > -1 && curSize >= maxSize)
         {
            return;
         }

         NotifyParentOfPreUpdate();

         ForEachPropertyPred pred(*this, ForEachPropertyPred::COPY);
         pred.mIndex = itemIndex;
         pred.HandleProperty(*mPropContainer, *mProperty);

         // Now perform the copy operation on each of the linked properties.
         std::vector<LinkedPropertyData>& props = GetLinkedProperties();
         std::for_each(props.begin(), props.end(), pred);

         int nextIndex = itemIndex;
         mPropertyTree->closeEditor(mWrapper, QAbstractItemDelegate::NoHint);
         resizeChildren(false, true);
         SetIndexFocus(nextIndex);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::onItemDeleteClicked(int itemIndex)
   {
      if (mProperty.valid())
      {
         int curSize = mProperty->GetArraySize();
         int minSize = mProperty->GetMinArraySize();

         if (minSize > -1 && curSize <= minSize)
         {
            return;
         }

         // Collapse the control if this is the last item.
         // It seems a crash will happen if the control is removed
         // while in the middle of processing the current signal
         // sent from the control.
         if (curSize == 1)
         {
            QModelIndex myIndex = this->mModel->IndexOf(this);
            if (myIndex.isValid())
            {
               mPropertyTree->collapse(myIndex);
            }
         }

         NotifyParentOfPreUpdate();

         ForEachPropertyPred pred(*this, ForEachPropertyPred::DELETE);
         pred.mIndex = itemIndex;
         pred.HandleProperty(*mPropContainer, *mProperty);

         // Now perform the delete operation on each of the linked properties.
         std::vector<LinkedPropertyData>& props = GetLinkedProperties();
         std::for_each(props.begin(), props.end(), pred);

         int nextIndex = itemIndex;
         if (mProperty->GetArraySize() <= nextIndex)
         {
            nextIndex = mProperty->GetArraySize() - 1;
         }
         mPropertyTree->closeEditor(mWrapper, QAbstractItemDelegate::NoHint);
         resizeChildren(false, true);

         if (nextIndex >= 0)
         {
            SetIndexFocus(nextIndex);
         }
      }
   }

   void DynamicArrayControl::ItemAdd(dtCore::PropertyContainer& container, dtCore::ArrayActorPropertyBase& prop)
   {
      std::string oldValue = prop.ToString();
      prop.Insert(prop.GetArraySize());

      NotifyPropertyChange(container, prop, oldValue);
   }

   void DynamicArrayControl::ItemSwap(dtCore::PropertyContainer& container, dtCore::ArrayActorPropertyBase& prop, int itemIndex, int targetIndex)
   {
      std::string oldValue = prop.ToString();
      prop.Swap(itemIndex, targetIndex);

      NotifyPropertyChange(container, prop, oldValue);
   }
   
   void DynamicArrayControl::ItemCopy(dtCore::PropertyContainer& container, dtCore::ArrayActorPropertyBase& prop, int itemIndex)
   {
      std::string oldValue = prop.ToString();
      prop.Insert(itemIndex);
      prop.Copy(itemIndex + 1, itemIndex);

      NotifyPropertyChange(container, prop, oldValue);
   }
   
   void DynamicArrayControl::ItemDelete(dtCore::PropertyContainer& container, dtCore::ArrayActorPropertyBase& prop, int itemIndex)
   {
      std::string oldValue = prop.ToString();
      prop.Remove(itemIndex);

      NotifyPropertyChange(container, prop, oldValue);
   }

   void DynamicArrayControl::ItemClear(dtCore::PropertyContainer& container, dtCore::ArrayActorPropertyBase& prop)
   {
      std::string oldValue = prop.ToString();
      prop.Clear();

      NotifyPropertyChange(container, prop, oldValue);
   }

   void DynamicArrayControl::NotifyPropertyChange(dtCore::PropertyContainer& container,
      dtCore::ArrayActorPropertyBase& prop, const std::string& oldValue)
   {
      emit PropertyAboutToChange(container, prop,
         oldValue, prop.ToString());
      emit PropertyChanged(container, prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::PropertyAboutToChangePassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop,
            std::string oldValue, std::string newValue)
   {
      dtCore::RefPtr<dtCore::NamedArrayParameter> val = new dtCore::NamedArrayParameter(mProperty->GetName());

      dtCore::NamedParameter* subParam =  val->AddParameter(prop.GetName(), prop.GetDataType());
      subParam->FromString(oldValue);
      // Workaround.  Because the undo/redo system doesn't (yet) support the values of nested properties, I have to change it to
      // using the containing property and adding the changed property as a value.
      oldValue = val->ToString();
      subParam->FromString(newValue);
      newValue = val->ToString();

      BaseClass::PropertyAboutToChangePassThrough(*mPropContainer, *mProperty, oldValue, newValue);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicArrayControl::PropertyChangedPassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop)
   {
      BaseClass::PropertyChangedPassThrough(*mPropContainer, *mProperty);
   }


} // namespace dtEditQt
