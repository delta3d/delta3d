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

#ifndef DELTA_PROPERTY_CONTROL_COLOR_H
#define DELTA_PROPERTY_CONTROL_COLOR_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtQt/basepropertycontrol.h>
#include <dtQt/clickablelabelcontrol.h>

#include <QtWidgets/QLabel>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class PropertyControlColor;
}



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class ClickableLabelColorPicker : public ClickableLabelControl
   {
      Q_OBJECT
   public:
      typedef ClickableLabelControl BaseClass;

      ClickableLabelColorPicker(QLabel& label);

      virtual ~ClickableLabelColorPicker();

      QColor GetUIColor() const;
      void SetUIColor(QColor qcolor);

      void GetColorPick();
      
      /*virtual*/ void OnClick();
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT PropertyControlColor : public BasePropertyControl
   {
      Q_OBJECT
   public:
      typedef BasePropertyControl BaseClass;

      PropertyControlColor(QWidget* parent = NULL);

      virtual ~PropertyControlColor();

      /*virtual*/ void Init();

      /*virtual*/ QObject* GetControlUI();

      /*virtual*/ QLabel* GetLabel() const;

      /**
       * Update the UI values from the referenced data.
       */
      /*virtual*/ void UpdateUI(const PropertyType& prop);

      /**
       * Update the referenced data with values from the UI.
       */
      /*virtual*/ void UpdateData(PropertyType& propToUpdate);

      void UpdateColor();
      void UpdateSliders();

   public slots:
      void OnColorChanged();
      void OnSliderChanged(int value);

   protected:
      Ui::PropertyControlColor* mUI;

      ClickableLabelColorPicker* mPicker;
   };
}

#endif
