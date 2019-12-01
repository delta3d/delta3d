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
#include "ui_propertycontrolcolor.h"
#include <dtQt/propertycontrolcolor.h>
#include <dtCore/colorrgbaactorproperty.h>
#include <QtGui/QColorDialog.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // HELPER FUNCTIONS
   /////////////////////////////////////////////////////////////////////////////
   osg::Vec4 ConvertToOsgColor(const QColor& qcolor)
   {
      qreal r = 0.0f;
      qreal g = 0.0f;
      qreal b = 0.0f;
      qreal a = 0.0f;
      qcolor.getRgbF(&r, &g, &b, &a);

      osg::Vec4 color(r, g, b, a);

      return color;
   }

   QColor ConvertToQColor(const osg::Vec4& color)
   {
      QColor qcolor;
      qcolor.setRgbF(color.x(), color.y(), color.z(), color.w());
      return qcolor;
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyControlColor::PropertyControlColor(QWidget* parent)
      : BaseClass(parent, dtCore::DataType::RGBACOLOR)
      , mUI(new Ui::PropertyControlColor)
   {
      mUI->setupUi(this);
   }

   PropertyControlColor::~PropertyControlColor()
   {
      delete mUI;
      mUI = NULL;
   }

   void PropertyControlColor::Init()
   {
      BaseClass::Init();

      //mUI->mValue->installEventFilter(this);

      mPicker = new ClickableLabelColorPicker(*mUI->mValue);
      mUI->mValueArea->layout()->addWidget(mPicker);

      connect(mPicker, SIGNAL(SignalUIChanged()),
         this, SLOT(OnColorChanged()));
      connect(mPicker, SIGNAL(SignalUIChanged()),
         this, SLOT(OnUIChanged()));

      connect(mUI->mSliderR, SIGNAL(valueChanged(int)),
         this, SLOT(OnSliderChanged(int)));
      connect(mUI->mSliderG, SIGNAL(valueChanged(int)),
         this, SLOT(OnSliderChanged(int)));
      connect(mUI->mSliderB, SIGNAL(valueChanged(int)),
         this, SLOT(OnSliderChanged(int)));
      connect(mUI->mSliderA, SIGNAL(valueChanged(int)),
         this, SLOT(OnSliderChanged(int)));

      mUI->mExtendedContentArea->setVisible(false);
   }

   QObject* PropertyControlColor::GetControlUI()
   {
      return mUI->mContentArea;
   }

   QLabel* PropertyControlColor::GetLabel() const
   {
      return mUI->mLabel;
   }

   void PropertyControlColor::UpdateUI(const PropertyType& prop)
   {
      const dtCore::ColorRgbaActorProperty* cprop
         = dynamic_cast<const dtCore::ColorRgbaActorProperty*>(&prop);

      if (cprop != NULL)
      {
         QColor qcolor = ConvertToQColor(cprop->GetValue());

         mPicker->SetUIColor(qcolor);
      }

      UpdateSliders();
   }

   void PropertyControlColor::UpdateData(PropertyType& propToUpdate)
   {
      osg::Vec4 color = ConvertToOsgColor(mPicker->GetUIColor());

      dtCore::ColorRgbaActorProperty* prop
         = dynamic_cast<dtCore::ColorRgbaActorProperty*>(&propToUpdate);
      if (prop != NULL)
      {
         prop->SetValue(color);
      }
   }

   void PropertyControlColor::UpdateColor()
   {
      int r = mUI->mSliderR->value();
      int g = mUI->mSliderG->value();
      int b = mUI->mSliderB->value();
      int a = mUI->mSliderA->value();
      QColor color(r,g,b,a);
      
      mPicker->SetUIColor(color);
   }

   void PropertyControlColor::UpdateSliders()
   {
      QColor color = mPicker->GetUIColor();
      mUI->mSliderR->setValue(color.red());
      mUI->mSliderG->setValue(color.green());
      mUI->mSliderB->setValue(color.blue());
      mUI->mSliderA->setValue(color.alpha());
   }

   void PropertyControlColor::OnColorChanged()
   {
      UpdateSliders();
   }

   void PropertyControlColor::OnSliderChanged(int value)
   {
      UpdateColor();
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   ClickableLabelColorPicker::ClickableLabelColorPicker(QLabel& label)
      : BaseClass(label)
   {}

   ClickableLabelColorPicker::~ClickableLabelColorPicker()
   {}

   QColor ClickableLabelColorPicker::GetUIColor() const
   {
      return mLabel->palette().color(QPalette::Window);
   }

   void ClickableLabelColorPicker::SetUIColor(QColor qcolor)
   {
      mLabel->setPalette(QPalette(qcolor));

      // Signal the change. Property will be set in UpdateData.
      SignalUIChanged();
   }
   
   void ClickableLabelColorPicker::GetColorPick()
   {
      QColor oldColor = GetUIColor();

      QColor color = QColorDialog::getColor(oldColor, this);

      if (color.isValid())
      {
         SetUIColor(color);
      }
   }
   
   void ClickableLabelColorPicker::OnClick()
   {
      GetColorPick();
   }

}
