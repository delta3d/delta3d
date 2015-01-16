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
#include "ui_basedialog.h"
#include <dtQt/objecttypeselectdialog.h>
#include <dtQt/objecttypelistpanel.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   ObjectTypeSelectDialog::ObjectTypeSelectDialog(QWidget* parent)
      : BaseClass()
      , mUI(new Ui::BaseDialog)
      , mPanel(NULL)
   {
      mUI->setupUi(this);

      mPanel = new ObjectTypeListPanel(mUI->mContentArea);

      setWindowTitle("Enum Select");
   }

   ObjectTypeSelectDialog::~ObjectTypeSelectDialog()
   {
      delete mUI;
      mUI = NULL;
   }

   void ObjectTypeSelectDialog::CreateConnections()
   {
      // TODO:
   }

   ObjectTypeListPanel& ObjectTypeSelectDialog::GetObjectTypeListPanel()
   {
      return *mPanel;
   }

   int ObjectTypeSelectDialog::GetSelection(ObjectTypeList& outList) const
   {
      return mPanel->GetSelection(outList);
   }

   const dtCore::ObjectType* ObjectTypeSelectDialog::GetSelectedType() const
   {
      const dtCore::ObjectType* result = NULL;

      ObjectTypeList typeList;
      GetSelection(typeList);

      if ( ! typeList.empty())
      {
         result = typeList.front();
      }
      
      return result;
   }

   int ObjectTypeSelectDialog::GetItemCount() const
   {
      return mPanel->GetItemCount();
   }

   void ObjectTypeSelectDialog::SetSingleSelectMode(bool singleSelect)
   {
      mPanel->SetSingleSelectMode(singleSelect);
   }

   bool ObjectTypeSelectDialog::IsSingleSelectMode() const
   {
      return mPanel->IsSingleSelectMode();
   }

   void ObjectTypeSelectDialog::UpdateUI()
   {
      mPanel->UpdateUI();
   }

}
