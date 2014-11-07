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
#include "ui_propertycontrolbool.h"
#include <dtQt/propertycontrolbool.h>
#include <dtUtil/stringutils.h>


namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyControlBool::PropertyControlBool(QWidget* parent)
      : BaseClass(parent, dtCore::DataType::BOOLEAN)
      , mUI(new Ui::PropertyControlBool)
   {
      mUI->setupUi(this);
   }

   PropertyControlBool::~PropertyControlBool()
   {
      delete mUI;
      mUI = NULL;
   }

   QObject* PropertyControlBool::GetControlUI()
   {
      return mUI->mLayout;
   }

   QLabel* PropertyControlBool::GetLabel() const
   {
      return mUI->mLabel;
   }

   void PropertyControlBool::CreateConnections()
   {
      BaseClass::CreateConnections();

      connect(mUI->mValue, SIGNAL(stateChanged(int)),
         this, SLOT(OnUIChanged()));
   }

   void PropertyControlBool::UpdateUI(const PropertyType& prop)
   {
      bool value = dtUtil::ToType<bool>(prop.ToString().c_str());
      mUI->mValue->setCheckState(value ? Qt::Checked : Qt::Unchecked);
   }

   void PropertyControlBool::UpdateData(PropertyType& propToUpdate)
   {
      std::string value(mUI->mValue->checkState() == Qt::Checked ? "True" : "False");
      propToUpdate.FromString(value);
   }

}
