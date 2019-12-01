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
#include "ui_propertycontrolenum.h"
#include <dtQt/propertycontrolenum.h>
#include <dtCore/abstractenumactorproperty.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyControlEnum::PropertyControlEnum(QWidget* parent)
      : BaseClass(parent, dtCore::DataType::ENUMERATION)
      , mUI(new Ui::PropertyControlEnum)
   {
      mUI->setupUi(this);
   }

   PropertyControlEnum::~PropertyControlEnum()
   {
      delete mUI;
      mUI = NULL;
   }

   QObject* PropertyControlEnum::GetControlUI()
   {
      return mUI->mLayout;
   }

   QLabel* PropertyControlEnum::GetLabel() const
   {
      return mUI->mLabel;
   }

   void PropertyControlEnum::CreateConnections()
   {
      BaseClass::CreateConnections();

      connect(mUI->mValue, SIGNAL(currentIndexChanged(int)),
         this, SLOT(OnUIChanged()));
   }

   void PropertyControlEnum::UpdateUI(const PropertyType& prop)
   {
      const dtCore::AbstractEnumActorProperty* eprop
         = dynamic_cast<const dtCore::AbstractEnumActorProperty*>(&prop);

      if (eprop != NULL)
      {
         typedef std::vector<dtUtil::Enumeration*> EnumList;
         const EnumList& enumList = eprop->GetList();

         dtUtil::Enumeration& value = eprop->GetEnumValue();

         // Determine if the list needs to be populated.
         if ((int)enumList.size() != mUI->mValue->count())
         {
            // The control's item list is about to change.
            // Prevent signals that could change the current
            // property value
            mUI->mValue->blockSignals(true);

            mUI->mValue->clear();

            EnumList::const_iterator curIter = enumList.begin();
            EnumList::const_iterator endIter = enumList.end();
            for (; curIter != endIter; ++curIter)
            {
               QString qstr((*curIter)->GetName().c_str());
               mUI->mValue->addItem(qstr);
            }

            mUI->mValue->blockSignals(false);
         }

         if (mUI->mValue->count() > 0)
         {
            std::string str = value.GetName();
            QString qstr(str.c_str());

            int index = mUI->mValue->findText(qstr);
            if (index >= 0)
            {
               mUI->mValue->setCurrentIndex(index);
            }
         }
      }
   }

   void PropertyControlEnum::UpdateData(PropertyType& propToUpdate)
   {
      dtCore::AbstractEnumActorProperty* prop
         = dynamic_cast<dtCore::AbstractEnumActorProperty*>(&propToUpdate);
      
      if (prop != NULL)
      {
         std::string value = mUI->mValue->currentText().toStdString();

         if (value != prop->GetEnumValue().GetName())
         {
            prop->SetValueFromString(value);
         }
      }
   }

}
