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
#include "ui_propertycontrolbitmask.h"
#include <dtQt/propertycontrolbitmask.h>
#include <dtCore/bitmaskactorproperty.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyControlBitMask::PropertyControlBitMask(QWidget* parent)
      : BaseClass(parent, dtCore::DataType::BIT_MASK)
      , mUI(new Ui::PropertyControlBitMask)
      , mBits(new QCheckBox*[32])
   {
      mUI->setupUi(this);

      mBits[0] = mUI->m1_1;
      mBits[1] = mUI->m1_2;
      mBits[2] = mUI->m1_3;
      mBits[3] = mUI->m1_4;
      mBits[4] = mUI->m1_5;
      mBits[5] = mUI->m1_6;
      mBits[6] = mUI->m1_7;
      mBits[7] = mUI->m1_8;

      mBits[8] = mUI->m2_1;
      mBits[9] = mUI->m2_2;
      mBits[10] = mUI->m2_3;
      mBits[11] = mUI->m2_4;
      mBits[12] = mUI->m2_5;
      mBits[13] = mUI->m2_6;
      mBits[14] = mUI->m2_7;
      mBits[15] = mUI->m2_8;

      mBits[16] = mUI->m3_1;
      mBits[17] = mUI->m3_2;
      mBits[18] = mUI->m3_3;
      mBits[19] = mUI->m3_4;
      mBits[20] = mUI->m3_5;
      mBits[21] = mUI->m3_6;
      mBits[22] = mUI->m3_7;
      mBits[23] = mUI->m3_8;

      mBits[24] = mUI->m4_1;
      mBits[25] = mUI->m4_2;
      mBits[26] = mUI->m4_3;
      mBits[27] = mUI->m4_4;
      mBits[28] = mUI->m4_5;
      mBits[29] = mUI->m4_6;
      mBits[30] = mUI->m4_7;
      mBits[31] = mUI->m4_8;
   }

   PropertyControlBitMask::~PropertyControlBitMask()
   {
      delete [] mBits;
      mBits = NULL;

      delete mUI;
      mUI = NULL;
   }

   QObject* PropertyControlBitMask::GetControlUI()
   {
      return mUI->mContentArea;
   }

   QLabel* PropertyControlBitMask::GetLabel() const
   {
      return mUI->mLabel;
   }

   void PropertyControlBitMask::CreateConnections()
   {
      BaseClass::CreateConnections();

      for (int i = 0; i < 32; ++i)
      {
         connect(mBits[i], SIGNAL(stateChanged(int)),
            this, SLOT(OnCheckboxChanged(int)));
      }
   }

   void PropertyControlBitMask::UpdateUI(const PropertyType& prop)
   {
      typedef dtCore::BitMaskActorProperty::GetValueType BitMaskType;
      const dtCore::BitMaskActorProperty* bprop
         = dynamic_cast<const dtCore::BitMaskActorProperty*>(&prop);

      if (bprop != NULL)
      {
         BitMaskType value = bprop->GetValue();

         for (int i = 0; i < 32; ++i)
         {
            if (0 != (value & (1 << i)))
            {
               mBits[i]->setCheckState(Qt::Checked);
            }
            else
            {
               mBits[i]->setCheckState(Qt::Unchecked);
            }
         }
      }
   }

   void PropertyControlBitMask::UpdateData(PropertyType& propToUpdate)
   {
      typedef dtCore::BitMaskActorProperty::GetValueType BitMaskType;
      dtCore::BitMaskActorProperty* bprop
         = dynamic_cast<dtCore::BitMaskActorProperty*>(&propToUpdate);

      if (bprop != NULL)
      {
         BitMaskType value = 0;

         for (int i = 0; i < 32; ++i)
         {
            if (mBits[i]->checkState() == Qt::Checked)
            {
               value |= (1 << i);
            }
         }
         
         bprop->SetValue(value);
      }
   }

   void PropertyControlBitMask::OnCheckboxChanged(int checkState)
   {
      emit SignalUIChanged();
   }

}
