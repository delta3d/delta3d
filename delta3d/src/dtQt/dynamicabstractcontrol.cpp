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
#include <dtQt/dynamicabstractcontrol.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/datatype.h>
#include <dtDAL/vectoractorproperties.h>
#include <dtDAL/arrayactorproperty.h>

#include <dtQt/dynamicabstractparentcontrol.h>
#include <dtQt/dynamicpropertycontainercontrol.h>
#include <dtQt/dynamicarraycontrol.h>
#include <dtQt/dynamicboolcontrol.h>
#include <dtQt/dynamiccolorrgbacontrol.h>
#include <dtQt/dynamiccontainercontrol.h>
#include <dtQt/dynamicdoublecontrol.h>
#include <dtQt/dynamicenumcontrol.h>
#include <dtQt/dynamicfloatcontrol.h>
#include <dtQt/dynamicintcontrol.h>
#include <dtQt/dynamiclabelcontrol.h>
#include <dtQt/dynamiclongcontrol.h>
#include <dtQt/dynamicstringcontrol.h>
#include <dtQt/dynamicvecncontrol.h>
#include <dtQt/propertyeditortreeview.h>
#include <dtQt/dynamicsubwidgets.h>
#include <dtQt/dynamicactorcontrol.h>
#include <dtQt/dynamicresourcecontrol.h>
#include <dtQt/dynamicgameeventcontrol.h>
#include <dtQt/dynamicbitmaskcontrol.h>

#include <QtCore/QSize>

#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QStyleOptionViewItem>
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicControlFactory::DynamicControlFactory()
      : mControlFactory(new dtUtil::ObjectFactory<dtDAL::DataType*, DynamicAbstractControl>)
   {
      // register all the data types with the dynamic control factory
      RegisterControlForDataType<DynamicStringControl>(dtDAL::DataType::STRING);
      RegisterControlForDataType<DynamicFloatControl>(dtDAL::DataType::FLOAT);
      RegisterControlForDataType<DynamicDoubleControl>(dtDAL::DataType::DOUBLE);
      RegisterControlForDataType<DynamicIntControl>(dtDAL::DataType::INT);
      RegisterControlForDataType<DynamicLongControl>(dtDAL::DataType::LONGINT);
      RegisterControlForDataType<DynamicBoolControl>(dtDAL::DataType::BOOLEAN);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec2ActorProperty> >(dtDAL::DataType::VEC2);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec2fActorProperty> >(dtDAL::DataType::VEC2F);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec2dActorProperty> >(dtDAL::DataType::VEC2D);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec3ActorProperty> >(dtDAL::DataType::VEC3);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec3fActorProperty> >(dtDAL::DataType::VEC3F);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec3dActorProperty> >(dtDAL::DataType::VEC3D);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec4ActorProperty> >(dtDAL::DataType::VEC4);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec4fActorProperty> >(dtDAL::DataType::VEC4F);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec4dActorProperty> >(dtDAL::DataType::VEC4D);
      RegisterControlForDataType<DynamicEnumControl>(dtDAL::DataType::ENUMERATION);
      RegisterControlForDataType<DynamicColorRGBAControl>(dtDAL::DataType::RGBACOLOR);
      RegisterControlForDataType<DynamicArrayControl>(dtDAL::DataType::ARRAY);
      RegisterControlForDataType<DynamicContainerControl>(dtDAL::DataType::CONTAINER);
      RegisterControlForDataType<DynamicPropertyContainerControl>(dtDAL::DataType::PROPERTY_CONTAINER);
      RegisterControlForDataType<DynamicActorControl>(dtDAL::DataType::ACTOR);
      RegisterControlForDataType<DynamicGameEventControl>(dtDAL::DataType::GAME_EVENT);
      RegisterControlForDataType<DynamicBitMaskControl>(dtDAL::DataType::BIT_MASK);

      size_t datatypeCount = dtDAL::DataType::EnumerateType().size();

      for (size_t i = 0; i < datatypeCount; ++i)
      {
         dtDAL::DataType* dt = dtDAL::DataType::EnumerateType()[i];
         if (dt->IsResource())
         {
            RegisterControlForDataType<DynamicResourceControl>(*dt);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl* DynamicControlFactory::CreateDynamicControl(const dtDAL::ActorProperty& prop)
   {
      return mControlFactory->CreateObject(&prop.GetPropertyType());
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicControlFactory::~DynamicControlFactory()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl::DynamicAbstractControl()
      : mInitialized(false)
      , mArrayIndex(-1)
      , mParent(NULL)
      , mModel(NULL)
      , mPropertyTree(NULL)
      , mDefaultResetButton(NULL)
      , mShiftUpButton(NULL)
      , mShiftDownButton(NULL)
      , mCopyButton(NULL)
      , mDeleteButton(NULL)
      , mGridLayout(NULL)
      , mWrapper(NULL)
      //, mNewCommitEmitter(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl::~DynamicAbstractControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicControlFactory* DynamicAbstractControl::GetDynamicControlFactory()
   {
      return mControlFactory.get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::SetDynamicControlFactory(DynamicControlFactory* factory)
   {
      mControlFactory = factory;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper)
      {
         UpdateButtonStates();
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::updateModelFromEditor(QWidget* widget)
   {
      if (widget == mWrapper)
      {
         // Notify the parent that the control is about to be updated.
         NotifyParentOfPreUpdate();
      }
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* newProperty)
   {
      mParent = newParent;
      mPropContainer = newPC;
      mBaseProperty = newProperty;
      mModel  = newModel;

      // Set the tooltip description on the control.
      if (mModel)
      {
         mModel->setDescription(this);
      }

      connect(this, SIGNAL(PropertyChanged(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)),
         this, SLOT(OnPropertyChanged(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)));

      mInitialized = true;

   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicAbstractControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = new QWidget(parent);
      wrapper->setFocusPolicy(Qt::StrongFocus);
      SetBackgroundColor(wrapper, PropertyEditorTreeView::ROW_COLOR_ODD);

      mGridLayout = new QGridLayout(wrapper);
      mGridLayout->setMargin(0);
      mGridLayout->setSpacing(1);

      if (mPropContainer->DoesDefaultExist(*mBaseProperty))
      {
         mDefaultResetButton = new SubQToolButton(parent, this);
         mDefaultResetButton->setText("Reset");
         mDefaultResetButton->setToolTip("Reset the property to its default value.");
         mDefaultResetButton->setIcon(QIcon(":icons/resetproperty.png"));

         connect(mDefaultResetButton, SIGNAL(clicked()), this, SLOT(onResetClicked()));

         mDefaultResetButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

         mGridLayout->addWidget(mDefaultResetButton, 0, 10, 1, 1);
         mGridLayout->setColumnStretch(10, 0);
      }

      if (mArrayIndex > -1)
      {
         mShiftUpButton   = new SubQToolButton(wrapper, this);
         mShiftDownButton = new SubQToolButton(wrapper, this);
         mCopyButton      = new SubQToolButton(wrapper, this);
         mDeleteButton    = new SubQToolButton(wrapper, this);

         mShiftUpButton->setText("Up");
         mShiftUpButton->setIcon(QIcon(":icons/blackUpArrow.png"));
         mShiftUpButton->setToolTip(tr("Move the element up a position"));

         mShiftDownButton->setText("Down");
         mShiftDownButton->setIcon(QIcon(":icons/blackDownArrow.png"));
         mShiftDownButton->setToolTip(tr("Move the element down a position"));

         mCopyButton->setText("Copy");
         mCopyButton->setIcon(QIcon(":icons/duplicate_small.png"));
         mCopyButton->setToolTip(tr("Duplicate the element"));

         mDeleteButton->setText("Delete");
         mDeleteButton->setIcon(QIcon(":icons/trash_small.png"));
         mDeleteButton->setToolTip(tr("Delete the element"));

         UpdateButtonStates();

         connect(mShiftUpButton,   SIGNAL(clicked()), this, SLOT(onShiftUpClicked()));
         connect(mShiftDownButton, SIGNAL(clicked()), this, SLOT(onShiftDownClicked()));
         connect(mCopyButton,      SIGNAL(clicked()), this, SLOT(onCopyClicked()));
         connect(mDeleteButton,    SIGNAL(clicked()), this, SLOT(onDeleteClicked()));

         mGridLayout->addWidget(mShiftUpButton,   0, 11, 1, 1);
         mGridLayout->addWidget(mShiftDownButton, 0, 12, 1, 1);
         mGridLayout->addWidget(mCopyButton,      0, 13, 1, 1);
         mGridLayout->addWidget(mDeleteButton,    0, 14, 1, 1);
         mGridLayout->setColumnMinimumWidth(11, mShiftUpButton->sizeHint().width() / 2);
         mGridLayout->setColumnMinimumWidth(12, mShiftDownButton->sizeHint().width() / 2);
         mGridLayout->setColumnMinimumWidth(13, mCopyButton->sizeHint().width() / 2);
         mGridLayout->setColumnMinimumWidth(14, mDeleteButton->sizeHint().width() / 2);
         mGridLayout->setColumnStretch(11, 0);
         mGridLayout->setColumnStretch(12, 0);
         mGridLayout->setColumnStretch(13, 0);
         mGridLayout->setColumnStretch(14, 0);
      }

      mWrapper = wrapper;
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl* DynamicAbstractControl::getParent()
   {
      return mParent;
   }

   /////////////////////////////////////////////////////////////////////////////////
   int DynamicAbstractControl::getChildIndex(DynamicAbstractControl* child)
   {
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl* DynamicAbstractControl::getChild(int index)
   {
      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////////
   int DynamicAbstractControl::getChildCount()
   {
      return 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::PropertyAboutToChangePassThrough(dtDAL::PropertyContainer& proxy, dtDAL::ActorProperty& prop,
            std::string oldValue, std::string newValue)
   {
      emit PropertyAboutToChange(proxy, prop, oldValue, newValue);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::PropertyChangedPassThrough(dtDAL::PropertyContainer& proxy, dtDAL::ActorProperty& prop)
   {
      emit PropertyChanged(proxy, prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::OnPropertyChanged(dtDAL::PropertyContainer& propCon, dtDAL::ActorProperty& prop)
   {
      actorPropertyChanged(propCon, prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::OnChildPreUpdate(DynamicAbstractControl* child)
   {
      NotifyParentOfPreUpdate();
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicAbstractControl::getDisplayName()
   {
      if (mArrayIndex > -1)
      {
         return QString("[") + QString::number(mArrayIndex) + QString("]");
      }

      return QString("");
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicAbstractControl::getDescription()
   {
      return QString("");
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicAbstractControl::getValueAsString()
   {
      NotifyParentOfPreUpdate();
      return QString("");
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::isEditable()
   {
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::isControlDoesCustomPainting(int column)
   {
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::paintColumn(int column, QPainter* painter, const QStyleOptionViewItem& opt)
   {
      // do nothing
   }

   /////////////////////////////////////////////////////////////////////////////////
   QSize DynamicAbstractControl::sizeHint(int column, const QStyleOptionViewItem& opt)
   {
      // do nothing
      return QSize();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::SetTreeView(PropertyEditorTreeView* newPropertyTree)
   {
      mPropertyTree = newPropertyTree;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mWrapper = NULL;
         mDefaultResetButton = NULL;
         mShiftUpButton      = NULL;
         mShiftDownButton    = NULL;
         mCopyButton         = NULL;
         mDeleteButton       = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::onResetClicked()
   {
      if (mWrapper)
      {
         mWrapper->setFocus();
      }

      NotifyParentOfPreUpdate();
      std::string oldValue = mBaseProperty->ToString();
      mPropContainer->ResetProperty(*mBaseProperty);
      updateEditorFromModel(mWrapper);

      // give undo manager the ability to create undo/redo events
      emit PropertyAboutToChange(*mPropContainer, *mBaseProperty,
         oldValue, mBaseProperty->ToString());
      emit PropertyChanged(*mPropContainer, *mBaseProperty);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::onShiftUpClicked()
   {
      if (mArrayIndex <= 0)
      {
         return;
      }

      DynamicArrayControl* parent = static_cast<DynamicArrayControl*>(getParent());
      if (!parent)
      {
         return;
      }

      dtDAL::ArrayActorPropertyBase* arrayProp = parent->GetProperty();
      if (arrayProp)
      {
         if (!arrayProp->CanReorder())
         {
            return;
         }

         NotifyParentOfPreUpdate();

         std::string oldValue = arrayProp->ToString();
         arrayProp->Swap(mArrayIndex, mArrayIndex - 1);

         emit PropertyAboutToChange(*mPropContainer, *arrayProp,
            oldValue, arrayProp->ToString());
         emit PropertyChanged(*mPropContainer, *arrayProp);

         int nextIndex = mArrayIndex - 1;
         mPropertyTree->closeEditor(mWrapper, QAbstractItemDelegate::NoHint);
         parent->resizeChildren(true, true);
         parent->SetIndexFocus(nextIndex);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::onShiftDownClicked()
   {
      // Get our parent.
      DynamicArrayControl* parent = static_cast<DynamicArrayControl*>(getParent());
      if (!parent)
      {
         return;
      }

      dtDAL::ArrayActorPropertyBase* arrayProp = parent->GetProperty();
      if (arrayProp)
      {
         if (!arrayProp->CanReorder() || mArrayIndex + 1 >= arrayProp->GetArraySize())
         {
            return;
         }

         NotifyParentOfPreUpdate();

         std::string oldValue = arrayProp->ToString();
         arrayProp->Swap(mArrayIndex, mArrayIndex + 1);

         emit PropertyAboutToChange(*mPropContainer, *arrayProp,
            oldValue, arrayProp->ToString());
         emit PropertyChanged(*mPropContainer, *arrayProp);

         int nextIndex = mArrayIndex + 1;
         mPropertyTree->closeEditor(mWrapper, QAbstractItemDelegate::NoHint);
         parent->resizeChildren(true, true);
         parent->SetIndexFocus(nextIndex);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::onCopyClicked()
   {
      // Get our parent.
      DynamicArrayControl* parent = static_cast<DynamicArrayControl*>(getParent());
      if (!parent)
      {
         return;
      }

      dtDAL::ArrayActorPropertyBase* arrayProp = parent->GetProperty();
      if (arrayProp)
      {
         int curSize = arrayProp->GetArraySize();
         int maxSize = arrayProp->GetMaxArraySize();
         if (maxSize > -1 && curSize >= maxSize)
         {
            return;
         }

         NotifyParentOfPreUpdate();

         std::string oldValue = arrayProp->ToString();
         arrayProp->Insert(mArrayIndex);
         arrayProp->Copy(mArrayIndex + 1, mArrayIndex);

         emit PropertyAboutToChange(*mPropContainer, *arrayProp,
            oldValue, arrayProp->ToString());
         emit PropertyChanged(*mPropContainer, *arrayProp);

         int nextIndex = mArrayIndex;
         mPropertyTree->closeEditor(mWrapper, QAbstractItemDelegate::NoHint);
         parent->resizeChildren(false, true);
         parent->SetIndexFocus(nextIndex);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::onDeleteClicked()
   {
      // Get our parent.
      DynamicArrayControl* parent = static_cast<DynamicArrayControl*>(getParent());
      if (!parent)
      {
         return;
      }

      dtDAL::ArrayActorPropertyBase* arrayProp = parent->GetProperty();
      if (arrayProp)
      {
         int curSize = arrayProp->GetArraySize();
         int minSize = arrayProp->GetMinArraySize();
         if (minSize > -1 && curSize <= minSize)
         {
            return;
         }

         NotifyParentOfPreUpdate();

         std::string oldValue = arrayProp->ToString();
         arrayProp->Remove(mArrayIndex);

         emit PropertyAboutToChange(*mPropContainer, *arrayProp,
            oldValue, arrayProp->ToString());
         emit PropertyChanged(*mPropContainer, *arrayProp);

         int nextIndex = mArrayIndex;
         mPropertyTree->closeEditor(mWrapper, QAbstractItemDelegate::NoHint);
         parent->resizeChildren(false, true);
         parent->SetIndexFocus(nextIndex);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::UpdateButtonStates()
   {
      if (mDefaultResetButton)
      {
         if (IsPropertyDefault())
         {
            mDefaultResetButton->setEnabled(false);
         }
         else
         {
            mDefaultResetButton->setEnabled(true);
         }
      }

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

      // Get our parent.
      DynamicArrayControl* parent = static_cast<DynamicArrayControl*>(getParent());
      if (!parent)
      {
         return;
      }

      dtDAL::ArrayActorPropertyBase* arrayProp = parent->GetProperty();
      if (arrayProp)
      {
         // Check if this element can be shifted up.
         if (!arrayProp->CanReorder() || mArrayIndex <= 0)
         {
            canShiftUp = false;
         }

         // Check if this element can be shifted down.
         if (!arrayProp->CanReorder() || mArrayIndex + 1 >= arrayProp->GetArraySize())
         {
            canShiftDown = false;
         }

         // Check if we are at our max array size.
         int curSize = arrayProp->GetArraySize();
         int maxSize = arrayProp->GetMaxArraySize();
         if (maxSize > -1 && curSize >= maxSize)
         {
            canCopy = false;
         }

         // Check if we are at our min array size.
         int minSize = arrayProp->GetMinArraySize();
         if (minSize > -1 && curSize <= minSize)
         {
            canDelete = false;
         }
      }

      mShiftUpButton->setDisabled(!canShiftUp);
      mShiftDownButton->setDisabled(!canShiftDown);
      mCopyButton->setDisabled(!canCopy);
      mDeleteButton->setDisabled(!canDelete);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::NeedsPersistentEditor()
   {
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::SetBackgroundColor(QWidget* widget, QColor color)
   {
      QPalette palette(widget->palette());
      palette.setColor(QPalette::Active,   QPalette::Background, color);
      palette.setColor(QPalette::Inactive, QPalette::Background, color);
      palette.setColor(QPalette::Disabled, QPalette::Background, color) ;
      widget->setPalette(palette);
      //4.1 only
      widget->setAutoFillBackground(true);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::InstallEventFilterOnControl(QObject* filterObj)
   {
      QObject::installEventFilter(filterObj);
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::NotifyParentOfPreUpdate()
   {
      // Notify the parent that a change is about to occur.
      DynamicAbstractControl* parent = getParent();
      if (parent)
      {
         parent->OnChildPreUpdate(this);
      }

      if (mArrayIndex > -1)
      {
         // Get our parent.
         DynamicArrayControl* parent = static_cast<DynamicArrayControl*>(getParent());
         if (!parent)
         {
            return;
         }

         dtDAL::ArrayActorPropertyBase* arrayProp = parent->GetProperty();
         if (arrayProp)
         {
            arrayProp->SetIndex(mArrayIndex);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::IsPropertyDefault()
   {
      if (!mInitialized || !mPropContainer.valid() || mBaseProperty == NULL)
      {
         return true;
      }

      if (mArrayIndex > -1)
      {
         // Get our parent.
         const DynamicArrayControl* parent = static_cast<DynamicArrayControl*>(getParent());
         if (parent)
         {
            const dtDAL::ArrayActorPropertyBase* arrayProp = parent->GetProperty();
            if (arrayProp)
            {
               arrayProp->SetIndex(mArrayIndex);
            }
         }
      }

      if (!mPropContainer->DoesDefaultExist(*mBaseProperty) ||
          mPropContainer->IsPropertyDefault(*mBaseProperty))
      {
         return true;
      }
      else
      {
         return false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::actorPropertyChanged(dtDAL::PropertyContainer& propCon, dtDAL::ActorProperty& property)
   {
      NotifyParentOfPreUpdate();
      updateEditorFromModel(mWrapper);
   }
} // namespace dtEditQt
