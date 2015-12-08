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

#include <dtCore/actorproxy.h>
#include <dtCore/datatype.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/arrayactorproperty.h>

#include <dtQt/dynamicabstractparentcontrol.h>
#include <dtQt/dynamicpropertycontainercontrol.h>
#include <dtQt/dynamicarraycontrol.h>
#include <dtQt/dynamicboolcontrol.h>
#include <dtQt/dynamiccolorrgbacontrol.h>
#include <dtQt/dynamiccontainercontrol.h>
#include <dtQt/dynamiccontainerselectorcontrol.h>
#include <dtQt/dynamicenumcontrol.h>
#include <dtQt/dynamicnumericcontrol.h>
#include <dtQt/dynamiclabelcontrol.h>
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
#include <QtWidgets/QStyleOptionViewItem>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicControlFactory::DynamicControlFactory()
      : mControlFactory(new dtUtil::ObjectFactory<dtCore::DataType*, DynamicAbstractControl>)
   {
      // register all the data types with the dynamic control factory
      RegisterControlForDataType<DynamicStringControl>(dtCore::DataType::STRING);
      RegisterControlForDataType<DynamicNumericControl<dtCore::FloatActorProperty, QDoubleValidator> >(dtCore::DataType::FLOAT);
      RegisterControlForDataType<DynamicNumericControl<dtCore::DoubleActorProperty, QDoubleValidator> >(dtCore::DataType::DOUBLE);
      RegisterControlForDataType<DynamicNumericControl<dtCore::IntActorProperty, QIntValidator> >(dtCore::DataType::INT);
      RegisterControlForDataType<DynamicNumericControl<dtCore::LongActorProperty, QIntValidator> >(dtCore::DataType::LONGINT);
      RegisterControlForDataType<DynamicBoolControl>(dtCore::DataType::BOOLEAN);
      RegisterControlForDataType<DynamicVecNControl<dtCore::Vec2ActorProperty> >(dtCore::DataType::VEC2);
      RegisterControlForDataType<DynamicVecNControl<dtCore::Vec2fActorProperty> >(dtCore::DataType::VEC2F);
      RegisterControlForDataType<DynamicVecNControl<dtCore::Vec2dActorProperty> >(dtCore::DataType::VEC2D);
      RegisterControlForDataType<DynamicVecNControl<dtCore::Vec3ActorProperty> >(dtCore::DataType::VEC3);
      RegisterControlForDataType<DynamicVecNControl<dtCore::Vec3fActorProperty> >(dtCore::DataType::VEC3F);
      RegisterControlForDataType<DynamicVecNControl<dtCore::Vec3dActorProperty> >(dtCore::DataType::VEC3D);
      RegisterControlForDataType<DynamicVecNControl<dtCore::Vec4ActorProperty> >(dtCore::DataType::VEC4);
      RegisterControlForDataType<DynamicVecNControl<dtCore::Vec4fActorProperty> >(dtCore::DataType::VEC4F);
      RegisterControlForDataType<DynamicVecNControl<dtCore::Vec4dActorProperty> >(dtCore::DataType::VEC4D);
      RegisterControlForDataType<DynamicEnumControl>(dtCore::DataType::ENUMERATION);
      RegisterControlForDataType<DynamicColorRGBAControl>(dtCore::DataType::RGBACOLOR);
      RegisterControlForDataType<DynamicArrayControl>(dtCore::DataType::ARRAY);
      RegisterControlForDataType<DynamicContainerControl>(dtCore::DataType::CONTAINER);
      RegisterControlForDataType<DynamicContainerSelectorControl>(dtCore::DataType::CONTAINER_SELECTOR);
      RegisterControlForDataType<DynamicPropertyContainerControl>(dtCore::DataType::PROPERTY_CONTAINER);
      RegisterControlForDataType<DynamicActorControl>(dtCore::DataType::ACTOR);
      RegisterControlForDataType<DynamicGameEventControl>(dtCore::DataType::GAME_EVENT);
      RegisterControlForDataType<DynamicBitMaskControl>(dtCore::DataType::BIT_MASK);

      size_t datatypeCount = dtCore::DataType::EnumerateType().size();

      for (size_t i = 0; i < datatypeCount; ++i)
      {
         dtCore::DataType* dt = dtCore::DataType::EnumerateType()[i];
         if (dt->IsResource())
         {
            RegisterControlForDataType<DynamicResourceControl>(*dt);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl* DynamicControlFactory::CreateDynamicControl(const dtCore::ActorProperty& prop)
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
      mParent = NULL;
      mModel = NULL;
      mPropertyTree = NULL;
      mDefaultResetButton = NULL;
      mShiftUpButton = NULL;
      mShiftDownButton = NULL;
      mCopyButton = NULL;
      mDeleteButton = NULL;
      mGridLayout = NULL;
      mWrapper = NULL;
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
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
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

      connect(this, SIGNAL(PropertyChanged(dtCore::PropertyContainer&, dtCore::ActorProperty&)),
         this, SLOT(OnPropertyChanged(dtCore::PropertyContainer&, dtCore::ActorProperty&)));

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
         mDefaultResetButton->setIcon(QIcon(":dtQt/icons/resetproperty.png"));

         connect(mDefaultResetButton, SIGNAL(clicked()), this, SLOT(onResetClicked()), Qt::QueuedConnection);

         mDefaultResetButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

         mGridLayout->addWidget(mDefaultResetButton, 0, 14, 1, 1);
         mGridLayout->setColumnStretch(14, 0);
      }

      if (mArrayIndex > -1)
      {
         mShiftUpButton   = new SubQToolButton(wrapper, this);
         mShiftDownButton = new SubQToolButton(wrapper, this);
         mCopyButton      = new SubQToolButton(wrapper, this);
         mDeleteButton    = new SubQToolButton(wrapper, this);

         mShiftUpButton->setText("Up");
         mShiftUpButton->setIcon(QIcon(":dtQt/icons/blackUpArrow.png"));
         mShiftUpButton->setToolTip(tr("Move the element up a position"));

         mShiftDownButton->setText("Down");
         mShiftDownButton->setIcon(QIcon(":dtQt/icons/blackDownArrow.png"));
         mShiftDownButton->setToolTip(tr("Move the element down a position"));

         mCopyButton->setText("Copy");
         mCopyButton->setIcon(QIcon(":dtQt/icons/duplicate.png"));
         mCopyButton->setToolTip(tr("Duplicate the element"));

         mDeleteButton->setText("Delete");
         mDeleteButton->setIcon(QIcon(":dtQt/icons/subtract.png"));
         mDeleteButton->setToolTip(tr("Delete the element"));

         UpdateButtonStates();

         connect(mShiftUpButton,   SIGNAL(clicked()), this, SLOT(onShiftUpClicked()), Qt::QueuedConnection);
         connect(mShiftDownButton, SIGNAL(clicked()), this, SLOT(onShiftDownClicked()), Qt::QueuedConnection);
         connect(mCopyButton,      SIGNAL(clicked()), this, SLOT(onCopyClicked()), Qt::QueuedConnection);
         connect(mDeleteButton,    SIGNAL(clicked()), this, SLOT(onDeleteClicked()), Qt::QueuedConnection);

         mGridLayout->addWidget(mShiftUpButton,   0, 10, 1, 1);
         mGridLayout->addWidget(mShiftDownButton, 0, 11, 1, 1);
         mGridLayout->addWidget(mCopyButton,      0, 12, 1, 1);
         mGridLayout->addWidget(mDeleteButton,    0, 13, 1, 1);
         mGridLayout->setColumnMinimumWidth(10, mShiftUpButton->sizeHint().width() / 2);
         mGridLayout->setColumnMinimumWidth(11, mShiftDownButton->sizeHint().width() / 2);
         mGridLayout->setColumnMinimumWidth(12, mCopyButton->sizeHint().width() / 2);
         mGridLayout->setColumnMinimumWidth(13, mDeleteButton->sizeHint().width() / 2);
         mGridLayout->setColumnStretch(10, 0);
         mGridLayout->setColumnStretch(11, 0);
         mGridLayout->setColumnStretch(12, 0);
         mGridLayout->setColumnStretch(13, 0);
      }

      mWrapper = wrapper;
      return wrapper;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::AddLinkedProperty(dtCore::RefPtr<dtCore::PropertyContainer>& propCon, dtCore::ActorProperty* property)
   {
      LinkedPropertyData data;
      data.propCon = propCon;
      data.property = property;
      mLinkedProperties.push_back(data);
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<DynamicAbstractControl::LinkedPropertyData>& DynamicAbstractControl::GetLinkedProperties()
   {
      return mLinkedProperties;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::vector<DynamicAbstractControl::LinkedPropertyData>& DynamicAbstractControl::GetLinkedProperties() const
   {
      return mLinkedProperties;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::CopyBaseValueToLinkedProperties(bool resetAlwaysSave)
   {
      std::string baseValue = mBaseProperty->ToString();

      int count = (int)mLinkedProperties.size();
      for (int index = 0; index < count; ++index)
      {
         LinkedPropertyData& data = mLinkedProperties[index];

         dtCore::ActorProperty* linkedProp = data.property;
         if (linkedProp)
         {
            std::string oldValue = linkedProp->ToString();

            if (oldValue != baseValue)
            {
               linkedProp->FromString(baseValue);
               emit PropertyAboutToChange(*data.propCon, *linkedProp, oldValue, baseValue);
               emit PropertyChanged(*data.propCon, *linkedProp);
            }

            if (resetAlwaysSave)
               linkedProp->SetAlwaysSave(false);

         }
      }
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
   void DynamicAbstractControl::PropertyAboutToChangePassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop,
            std::string oldValue, std::string newValue)
   {
      emit PropertyAboutToChange(pc, prop, oldValue, newValue);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::PropertyChangedPassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop)
   {
      emit PropertyChanged(pc, prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::OnPropertyChanged(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& prop)
   {
      actorPropertyChanged(propCon, prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::enterPressed()
   {
      updateModelFromEditor(mWrapper);
      CopyBaseValueToLinkedProperties();
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
   QString DynamicAbstractControl::RealToString( qreal num, int digits )
   {
       QString str = QString::number( num, 'f', digits );

       str.remove( QRegExp("0+$") ); // Remove any number of trailing 0's
       str.remove( QRegExp("\\.$") ); // If the last character is just a '.' then remove it

       return str;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::isEditable()
   {
      if (!mBaseProperty->GetMultipleEdit() && !mLinkedProperties.empty())
      {
         return false;
      }

      return !mBaseProperty->IsReadOnly();
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::isControlDoesCustomPainting(int column)
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::doPropertiesMatch()
   {
      NotifyParentOfPreUpdate();

      // If there are no linked properties to compare with then
      // there is no need to perform the test.
      if (mLinkedProperties.empty())
      {
         return true;
      }

      // Retrieve the value of our base property.
      std::string baseValue = mBaseProperty->ToString();

      // Iterate through our linked properties and compare values.
      int count = (int)mLinkedProperties.size();
      for (int index = 0; index < count; ++index)
      {
         const LinkedPropertyData& data = mLinkedProperties[index];

         dtCore::ActorProperty* linkedProp = data.property;
         if (linkedProp)
         {
            std::string linkedValue = linkedProp->ToString();

            // If at any time, one of the linked values do not match
            // the base, then we contain multiple values.
            if (baseValue != linkedValue)
            {
               return false;
            }
         }
      }

      return true;
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
      // emitting property changed probably flags always save to true, which is the opposite of what we want here.
      mBaseProperty->SetAlwaysSave(false);

      CopyBaseValueToLinkedProperties(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::onShiftUpClicked()
   {
      emit SignalShiftUpClicked(GetArrayIndex());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::onShiftDownClicked()
   {
      emit SignalShiftDownClicked(GetArrayIndex());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::onCopyClicked()
   {
      emit SignalCopyClicked(GetArrayIndex());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::onDeleteClicked()
   {
      emit SignalDeleteClicked(GetArrayIndex());
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

      dtCore::ArrayActorPropertyBase* arrayProp = parent->GetProperty();
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

         dtCore::ArrayActorPropertyBase* arrayProp = parent->GetProperty();
         if (arrayProp)
         {
            arrayProp->SetIndex(mArrayIndex);
         }

         // Update the index on all linked properties as well.
         std::vector<LinkedPropertyData>& linkedProperties = parent->GetLinkedProperties();
         int linkCount = (int)linkedProperties.size();
         for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
         {
            LinkedPropertyData& data = linkedProperties[linkIndex];
            dtCore::ArrayActorPropertyBase* linkedArrayProp =
               dynamic_cast<dtCore::ArrayActorPropertyBase*>(data.property);
            if (linkedArrayProp)
            {
               linkedArrayProp->SetIndex(mArrayIndex);
            }
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
            const dtCore::ArrayActorPropertyBase* arrayProp = parent->GetProperty();
            if (arrayProp)
            {
               arrayProp->SetIndex(mArrayIndex);
            }
         }
      }

      if (!mPropContainer->DoesDefaultExist(*mBaseProperty) ||
          (mPropContainer->IsPropertyDefault(*mBaseProperty) && !mBaseProperty->GetAlwaysSave() ) )
      {
         return true;
      }
      else
      {
         return false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::actorPropertyChanged(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& property)
   {
      NotifyParentOfPreUpdate();
      updateEditorFromModel(mWrapper);
      property.SetAlwaysSave(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::updateData(QWidget* widget)
   {
      if (!mInitialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return false;
      }

      return updateModelFromEditor(widget);
   }

} // namespace dtEditQt
