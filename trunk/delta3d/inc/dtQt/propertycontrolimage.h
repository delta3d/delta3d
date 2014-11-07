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

#ifndef DELTA_PROPERTY_CONTROL_IMAGE_H
#define DELTA_PROPERTY_CONTROL_IMAGE_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtQt/basepropertycontrol.h>
#include <Qt/QPixmap.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class PropertyControlImage;
}



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT PropertyControlImage : public BasePropertyControl
   {
      Q_OBJECT
   public:
      typedef BasePropertyControl BaseClass;

      PropertyControlImage(QWidget* parent = NULL);

      virtual ~PropertyControlImage();

      /*virtual*/ void Init();

      /*virtual*/ QObject* GetControlUI();

      /*virtual*/ QLabel* GetLabel() const;

      /**
       * Update the UI values from the referenced data.
       */
      /*virtual*/ void UpdateUI(const PropertyType& prop);

      const QPixmap* GetUIImage() const;
      void SetUIImage(QPixmap qimage);

      /**
       * Update the referenced data with values from the UI.
       */
      /*virtual*/ void UpdateData(PropertyType& propToUpdate);

      void GetImagePick();

   protected:
      Ui::PropertyControlImage* mUI;

      bool eventFilter(QObject *obj, QEvent *qevent);
   };
}

#endif
