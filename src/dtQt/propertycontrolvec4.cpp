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
#include "ui_propertycontrolvec4.h"
#include <dtQt/propertycontrolvec4.h>
#include <dtCore/vectoractorproperties.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyControlVec4::PropertyControlVec4(QWidget* parent)
      : BaseClass(parent, dtCore::DataType::VEC4)
      , mUI(new Ui::PropertyControlVec4)
   {
      mUI->setupUi(this);
   }

   PropertyControlVec4::~PropertyControlVec4()
   {
      delete mUI;
      mUI = NULL;
   }

   QObject* PropertyControlVec4::GetControlUI()
   {
      return mUI->mLayout;
   }

   QLabel* PropertyControlVec4::GetLabel() const
   {
      return mUI->mLabel;
   }

   void PropertyControlVec4::CreateConnections()
   {
      BaseClass::CreateConnections();

      connect(mUI->mX, SIGNAL(valueChanged(double)),
         this, SLOT(OnUIChanged()));
      connect(mUI->mY, SIGNAL(valueChanged(double)),
         this, SLOT(OnUIChanged()));
      connect(mUI->mZ, SIGNAL(valueChanged(double)),
         this, SLOT(OnUIChanged()));
      connect(mUI->mW, SIGNAL(valueChanged(double)),
         this, SLOT(OnUIChanged()));
   }

   void PropertyControlVec4::UpdateUI(const PropertyType& prop)
   {
      const dtCore::Vec4ActorProperty* vprop
         = dynamic_cast<const dtCore::Vec4ActorProperty*>(&prop);

      if (vprop == NULL)
      {
         LOG_ERROR("Cannot access valid Vec4ActorProperty.");
      }
      else
      {
         osg::Vec4 value = vprop->GetValue();

         mUI->mX->setValue(value.x());
         mUI->mY->setValue(value.y());
         mUI->mZ->setValue(value.z());
         mUI->mW->setValue(value.w());
      }
   }

   void PropertyControlVec4::UpdateData(PropertyType& propToUpdate)
   {
      dtCore::Vec4ActorProperty* prop = dynamic_cast<dtCore::Vec4ActorProperty*>(&propToUpdate);

      if (prop == NULL)
      {
         LOG_ERROR("Cannot convert property to Vec4ActorProperty: " + propToUpdate.GetName());
      }
      else
      {
         osg::Vec4 value;
         value.x() = mUI->mX->value();
         value.y() = mUI->mY->value();
         value.z() = mUI->mZ->value();
         value.w() = mUI->mW->value();

         prop->SetValue(value);
      }
   }

}
