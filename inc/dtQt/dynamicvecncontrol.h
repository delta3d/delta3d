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
#ifndef DELTA_DYNAMICVEC2CONTROL
#define DELTA_DYNAMICVEC2CONTROL

#include <dtQt/dynamicabstractparentcontrol.h>
#include <dtQt/dynamicvectorelementcontrol.h>
#include <dtCore/refptr.h>
#include <dtUtil/log.h>

#include <QtCore/QString>
#include <QtWidgets/QGridLayout>

namespace dtQt
{


   // this has to be done here because Qt doesn't support template q objects, so the one slot
   // is implemented in a super class.
   class DynamicNoUpdateParentControl: public DynamicAbstractParentControl
   {
      Q_OBJECT;
   public slots:
      virtual bool updateData(QWidget* widget)
      {
         return false;
      }
   };

   /**
    * @class DynamicVecNControl
    * @brief This is the dynamic control for the N dimensional vector data type -
    * used in the property editor
    * @note It adds a group of child elements to the tree, since you can't edit multiple things
    * in one control easily.
    */
   template <typename PropertyType>
   class DynamicVecNControl : public DynamicNoUpdateParentControl
   {
   public:
      typedef typename PropertyType::GetValueType PropertyGetValueType;
      /**
       * Constructor
       */
      DynamicVecNControl();

      /**
       * Destructor
       */
      ~DynamicVecNControl();

      /**
       * @see DynamicAbstractControl#InitializeData
       */
      virtual void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
         dtCore::PropertyContainer* newPC, dtCore::ActorProperty* property);

      /**
       * @see DynamicAbstractControl#updateEditorFromModel
       */
      virtual void updateEditorFromModel(QWidget* widget);

      /**
       * @see DynamicAbstractControl#createEditor
       */
      virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
         const QModelIndex& index);

      /**
       * @see DynamicAbstractControl#getDisplayName
       */
      virtual const QString getDisplayName();

      /**
       * @see DynamicAbstractControl#getDescription
       */
      virtual const QString getDescription();

      /**
       * @see DynamicAbstractControl#getValueAsString
       */
      virtual const QString getValueAsString();

      /**
       * @see DynamicAbstractControl#handleSubEditDestroy
       */
      virtual void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);

   protected:

      DynamicVectorElementControl* CreateElementControl(PropertyType* prop, int index, const std::string& label,
               PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC);

   private:
      DynamicVectorElementControl* xElement;
      DynamicVectorElementControl* yElement;
      DynamicVectorElementControl* zElement;
      DynamicVectorElementControl* wElement;

      // the tool tip type label indicates that the vector is a float or a double
      std::string toolTipTypeLabel;

      SubQLabel* mTemporaryEditControl;

      dtCore::RefPtr<PropertyType> mProperty;
   };

   ///////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   DynamicVecNControl<PropertyType>::DynamicVecNControl()
      : xElement(NULL)
      , yElement(NULL)
      , zElement(NULL)
      , wElement(NULL)
      , mTemporaryEditControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   DynamicVecNControl<PropertyType>::~DynamicVecNControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   DynamicVectorElementControl* DynamicVecNControl<PropertyType>::CreateElementControl(PropertyType* prop, int index, const std::string& label,
            PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC)
   {
      DynamicVectorElementControl* control = new DynamicVectorElementControl(prop, index, label);
      control->InitializeData(this, newModel, newPC, prop);
      mChildren.push_back(control);

      connect(control, SIGNAL(PropertyAboutToChange(dtCore::PropertyContainer&, dtCore::ActorProperty&,
                        const std::string&, const std::string&)),
               this, SLOT(PropertyAboutToChangePassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&,
                        const std::string&, const std::string&)));

      connect(control, SIGNAL(PropertyChanged(dtCore::PropertyContainer&, dtCore::ActorProperty&)),
               this, SLOT(PropertyChangedPassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&)));

      return control;
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   void DynamicVecNControl<PropertyType>::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);

      mProperty = static_cast<PropertyType*>(newProperty);

      // create X
      xElement = CreateElementControl(mProperty.get(), 0, "X", newModel, newPC);
      // create Y
      yElement = CreateElementControl(mProperty.get(), 1, "Y", newModel, newPC);

      // Static if
      if (PropertyGetValueType::num_components > 2)
      {
         zElement = CreateElementControl(mProperty.get(), 2, "Z", newModel, newPC);
      }

      // Static if
      if (PropertyGetValueType::num_components > 3)
      {
         wElement = CreateElementControl(mProperty.get(), 3, "W", newModel, newPC);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   void DynamicVecNControl<PropertyType>::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         mTemporaryEditControl->setText(getValueAsString());
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   QWidget *DynamicVecNControl<PropertyType>::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractControl::createEditor(parent, option, index);

      if (!IsInitialized())
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      // create and init the edit box
      mTemporaryEditControl = new SubQLabel(getValueAsString(), wrapper, this);
      mTemporaryEditControl->setToolTip(getDescription());

      mGridLayout->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
      mGridLayout->setColumnMinimumWidth(0, mTemporaryEditControl->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 1);

      wrapper->setFocusProxy(mTemporaryEditControl);
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   const QString DynamicVecNControl<PropertyType>::getDisplayName()
   {
         return tr(mProperty->GetLabel().c_str());
    }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   const QString DynamicVecNControl<PropertyType>::getDescription()
   {
      std::string tooltip = mProperty->GetDescription() + "  [Type: " +
         mProperty->GetDataType().GetName() + "]";
      return tr(tooltip.c_str());
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   const QString DynamicVecNControl<PropertyType>::getValueAsString()
   {
      typedef typename PropertyGetValueType::value_type VecNValueType;
      DynamicAbstractControl::getValueAsString();

      if (doPropertiesMatch())
      {
         const unsigned int NUM_DECIMAL_DIGITS =
            sizeof(VecNValueType) == sizeof(float)
            ? NUM_DECIMAL_DIGITS_FLOAT : NUM_DECIMAL_DIGITS_DOUBLE;

         PropertyGetValueType vectorValue = mProperty->GetValue();

         QString result = "(X="  + RealToString(vectorValue.x(), NUM_DECIMAL_DIGITS) +
            ", Y=" + RealToString(vectorValue.y(), NUM_DECIMAL_DIGITS) ;

         // Static if
         if (PropertyGetValueType::num_components > 2)
         {
            //Had to use [] access because the code still must parse for vecs without a .z()
            result += ", Z=" + RealToString(vectorValue[2], NUM_DECIMAL_DIGITS) ;
         }

         // Static if
         if (PropertyGetValueType::num_components > 3)
         {
            //Had to use [] access because the code still must parse for vecs without a .w()
            result += ", W=" + RealToString(vectorValue[3], NUM_DECIMAL_DIGITS);
         }

         result += ")";
         return result;
      }
      else
      {
         return "<Multiple Values...>";
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   void DynamicVecNControl<PropertyType>::handleSubEditDestroy(QWidget* widget,
      QAbstractItemDelegate::EndEditHint hint /*= QAbstractItemDelegate::NoHint*/)
   {
      if (widget == mWrapper)
      {
         mTemporaryEditControl = NULL;
      }

      DynamicAbstractParentControl::handleSubEditDestroy(widget, hint);
   }

} // namespace dtEditQt

#endif // DELTA_DYNAMICVEC2CONTROL
