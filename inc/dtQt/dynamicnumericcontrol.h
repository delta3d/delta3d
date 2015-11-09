/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamicnumericcontrol.h - Using 'The MIT License'
 * Copyright (C) 2015, Caper Holdings LLC.
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
 * David Guthrie
 * Jeff Houde
 * Curtiss Murphy
 */
#ifndef DELTA_DYNAMICNUMERICCONTROL
#define DELTA_DYNAMICNUMERICCONTROL

#include <dtQt/export.h>
#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/dynamicsubwidgets.h>
#include <dtCore/typetoactorproperty.h>
#include <QtGui/QGridLayout>

class QWidget;

namespace dtCore
{
   class FloatActorProperty;
}

namespace dtQt
{

   // this has to be done here because Qt doesn't support template q objects, so the one slot
   // is implemented in a super class.
   class DynamicNumericBaseControl: public DynamicAbstractControl
   {
      Q_OBJECT;
   public slots:
      virtual bool updateData(QWidget* widget)
      {
         if (!mInitialized || widget == NULL)
         {
            LOG_ERROR("Tried to updateData before being initialized");
            return false;
         }

         return updateModelFromEditor(widget);
      }
   };


   /**
    * @class DynamicNumericControl
    * @brief This is the dynamic control for the float data type - used in the property editor
    */
   template <typename PropertyType, typename ValidatorType>
   class DT_QT_EXPORT DynamicNumericControl : public DynamicNumericBaseControl
   {
   public:
      static const int NUM_DECIMAL_DIGITS = sizeof(typename PropertyType::GetValueType) * 2;

      /**
       * Constructor
       */
      DynamicNumericControl();

      /**
       * Destructor
       */
      virtual ~DynamicNumericControl();

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
       * @see DynamicAbstractControl#updateModelFromEditor
       */
      virtual bool updateModelFromEditor(QWidget* widget);

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
      void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);

   protected:

   private:
      ValidatorType* CreateValidator();

      PropertyType* mProperty;
      // This pointer is not really in our control.  It is constructed in the createEditor()
      // method and destroyed whenever QT feels like it (mostly when the control looses focus).
      // We work around this by trapping the destruction of this object, it should
      // call our handleSubEditDestroy() method so we know to not hold this anymore
      SubQLineEdit* mTemporaryEditControl;
   };

   ///////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType, typename ValidatorType>
   inline DynamicNumericControl<PropertyType, ValidatorType>::DynamicNumericControl()
   : mTemporaryEditControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType, typename ValidatorType>
   inline DynamicNumericControl<PropertyType, ValidatorType>::~DynamicNumericControl()
   {
   }

   template <typename PropertyType, typename ValidatorType>
   inline void DynamicNumericControl<PropertyType, ValidatorType>::InitializeData(DynamicAbstractControl* newParent,
         PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      mProperty = dynamic_cast<PropertyType*>(newProperty);
      if (mProperty != NULL)
      {
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
   template <typename PropertyType, typename ValidatorType>
   inline void DynamicNumericControl<PropertyType, ValidatorType>::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         typename PropertyType::GetValueType floatValue = mProperty->GetValue();
         mTemporaryEditControl->setText(RealToString(floatValue, NUM_DECIMAL_DIGITS));
         mTemporaryEditControl->selectAll();
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType, typename ValidatorType>
   inline bool DynamicNumericControl<PropertyType, ValidatorType>::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget == mWrapper && mTemporaryEditControl)
      {
         bool success = false;
         // This is just to test success;
         mTemporaryEditControl->text().toDouble(&success);

         typename PropertyType::GetValueType result = dtUtil::ToType<typename PropertyType::GetValueType>(mTemporaryEditControl->text().toStdString());

         // set our value to our object
         if (success)
         {
            // Save the data if they are different.  Note, we also need to compare the QString value,
            // else we get epsilon differences that cause the map to be marked dirty with no edits :(
            QString proxyValue = RealToString(mProperty->GetValue(), NUM_DECIMAL_DIGITS);
            QString newValue = mTemporaryEditControl->text();
            if (result != mProperty->GetValue() && proxyValue != newValue)
            {
               // give undo manager the ability to create undo/redo events
               emit PropertyAboutToChange(*mPropContainer, *mProperty,
                     mProperty->ToString(), RealToString(result, NUM_DECIMAL_DIGITS).toStdString());

               mProperty->SetValue(result);
               dataChanged = true;
            }
         }
         else
         {
            LOG_ERROR("updateData() failed to convert our value successfully");
         }

         // reselect all the text when we commit.
         // Gives the user visual feedback that something happened.
         mTemporaryEditControl->selectAll();
      }

      // notify the world (mostly the viewports) that our property changed
      if (dataChanged)
      {
         emit PropertyChanged(*mPropContainer, *mProperty);
      }

      return dataChanged;
   }

   template <>
   inline QDoubleValidator* DynamicNumericControl<dtCore::DoubleActorProperty, QDoubleValidator>::CreateValidator()
   {
      QDoubleValidator* validator = new QDoubleValidator(mTemporaryEditControl);
      validator->setDecimals(NUM_DECIMAL_DIGITS_DOUBLE);
      return validator;
   }
   template <>
   inline QDoubleValidator* DynamicNumericControl<dtCore::FloatActorProperty, QDoubleValidator>::CreateValidator()
   {
      QDoubleValidator* validator = new QDoubleValidator(mTemporaryEditControl);
      validator->setDecimals(NUM_DECIMAL_DIGITS_FLOAT);
      return validator;
   }
   template <>
   inline QIntValidator* DynamicNumericControl<dtCore::IntActorProperty, QIntValidator>::CreateValidator()
   {
      QIntValidator* validator = new QIntValidator(mTemporaryEditControl);
      return validator;
   }
   template <>
   inline QIntValidator* DynamicNumericControl<dtCore::LongActorProperty, QIntValidator>::CreateValidator()
   {
      QIntValidator* validator = new QIntValidator(mTemporaryEditControl);
      return validator;
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType, typename ValidatorType>
   inline QWidget* DynamicNumericControl<PropertyType, ValidatorType>::createEditor(QWidget* parent,
         const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractControl::createEditor(parent, option, index);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      // create and init the edit box
      mTemporaryEditControl = new SubQLineEdit(wrapper, this);
      ValidatorType* validator = CreateValidator();
      mTemporaryEditControl->setValidator(validator);
      mTemporaryEditControl->setToolTip(getDescription());

      connect(mTemporaryEditControl, SIGNAL(returnPressed()), this, SLOT(enterPressed()));

      mGridLayout->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
      mGridLayout->setColumnMinimumWidth(0, mTemporaryEditControl->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 1);

      updateEditorFromModel(mWrapper);

      wrapper->setFocusProxy(mTemporaryEditControl);
      return wrapper;
   }

   template <typename PropertyType, typename ValidatorType>
   inline const QString DynamicNumericControl<PropertyType, ValidatorType>::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }
      return QString(tr(mProperty->GetLabel().c_str()));
   }

   template <typename PropertyType, typename ValidatorType>
   inline const QString DynamicNumericControl<PropertyType, ValidatorType>::getDescription()
   {
      std::string tooltip = mProperty->GetDescription() + "  [Type: " +
            mProperty->GetDataType().GetName() + "]";
      return QString(tr(tooltip.c_str()));
   }

   template <typename PropertyType, typename ValidatorType>
   inline const QString DynamicNumericControl<PropertyType, ValidatorType>::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();
      if (doPropertiesMatch())
      {
         float floatValue = mProperty->GetValue();
         return RealToString(floatValue, NUM_DECIMAL_DIGITS);
      }
      else
      {
         return "<Multiple Values...>";
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType, typename ValidatorType>
   inline void DynamicNumericControl<PropertyType, ValidatorType>::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
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

#endif // DELTA_DYNAMICFLOATCONTROL
