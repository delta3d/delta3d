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
/*#include "ui_propertycontrolrange.h"
#include <dtQt/propertycontrolrange.h>
#include <dtCore/rangeactorproperty.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyControlRange::PropertyControlRange(QWidget* parent)
      : BaseClass(parent, dtCore::DataType::RANGE)
      , mUI(new Ui::PropertyControlRange)
   {
      mUI->setupUi(this);
   }

   PropertyControlRange::~PropertyControlRange()
   {
      delete mUI;
      mUI = NULL;
   }

   QObject* PropertyControlRange::GetControlUI()
   {
      return mUI->mContentArea;
   }

   QLabel* PropertyControlRange::GetLabel() const
   {
      return mUI->mLabel;
   }

   void PropertyControlRange::CreateConnections()
   {
      BaseClass::CreateConnections();

      connect(mUI->mValue, SIGNAL(valueChanged(double)),
         this, SLOT(OnValueChanged(double)));
      connect(mUI->mValueMin, SIGNAL(valueChanged(double)),
         this, SLOT(OnMinChanged(double)));
      connect(mUI->mValueMax, SIGNAL(valueChanged(double)),
         this, SLOT(OnMaxChanged(double)));
      connect(mUI->mValueSlider, SIGNAL(valueChanged(int)),
         this, SLOT(OnSliderChanged(int)));
   }

   void PropertyControlRange::UpdateUI(const PropertyType& prop)
   {
      const dtCore::RangeActorProperty* rprop
         = dynamic_cast<const dtCore::RangeActorProperty*>(&prop);

      if (rprop != NULL)
      {
         dtCore::Range range = rprop->GetValue();

         mUI->mValue->blockSignals(true);
         mUI->mValue->setValue(range.GetValue());
         mUI->mValue->blockSignals(false);

         mUI->mValueMin->blockSignals(true);
         mUI->mValueMin->setValue(range.GetMin());
         mUI->mValueMin->blockSignals(false);

         mUI->mValueMax->blockSignals(true);
         mUI->mValueMax->setValue(range.GetMax());
         mUI->mValueMax->blockSignals(false);

         UpdateControlLimits();
      }
   }

   void PropertyControlRange::UpdateData(PropertyType& propToUpdate)
   {
      propToUpdate.FromString(mUI->mValue->text().toStdString());
   }

   void PropertyControlRange::UpdateControlLimits()
   {
      // Block signals for this method since more than
      // one control value may change, which could cause
      // extra unwanted and redundant signals to update data.
      mUI->mValue->blockSignals(true);
      mUI->mValueMin->blockSignals(true);
      mUI->mValueMax->blockSignals(true);

      double minimum = mUI->mValueMin->value();
      double maximum = mUI->mValueMax->value();

      mUI->mValueMin->setMaximum(maximum);
      minimum = mUI->mValueMin->value();

      mUI->mValueMax->setMinimum(minimum);
      maximum = mUI->mValueMax->value();

      mUI->mValue->setMinimum(minimum);
      mUI->mValue->setMaximum(maximum);

      int minVal = (int)(minimum * 1000);
      int maxVal = (int)(maximum * 1000);
      int val = (int)(mUI->mValue->value() * 1000);

      // Update the limits of the slider.
      // Block signals to prevent a bad update loop.
      QSlider* slider = mUI->mValueSlider;
      slider->blockSignals(true);
      slider->setMinimum(minVal);
      slider->setMaximum(maxVal);
      slider->setValue(val);
      slider->blockSignals(false);

      // Re-enable signals since control values
      // should have been finalized.
      mUI->mValue->blockSignals(false);
      mUI->mValueMin->blockSignals(false);
      mUI->mValueMax->blockSignals(false);
   }

   void PropertyControlRange::OnSliderChanged(int value)
   {
      double val = (double)(value)/1000.0;

      if (mUI->mValue->value() != val)
      {
         mUI->mValue->setValue(val);
      }
   }

   void PropertyControlRange::OnValueChanged(double value)
   {
      UpdateControlLimits();

      emit SignalUIChanged();
   }

   void PropertyControlRange::OnMinChanged(double value)
   {
      UpdateControlLimits();

      emit SignalUIChanged();
   }

   void PropertyControlRange::OnMaxChanged(double value)
   {
      UpdateControlLimits();

      emit SignalUIChanged();
   }

}*/
