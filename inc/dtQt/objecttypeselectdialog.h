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

#ifndef DELTA_OBJECT_TYPE_SELECT_DIALOG_H
#define DELTA_OBJECT_TYPE_SELECT_DIALOG_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA
#include <dtQt/export.h>
#include <dtCore/objecttype.h>
// QT
#include <QtGui/QDialog>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class BaseDialog;
}

namespace dtQt
{
   class ObjectTypeListPanel;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT ObjectTypeSelectDialog : public QDialog
   {
      Q_OBJECT
   public:
      typedef QDialog BaseClass;

      typedef std::vector<const dtCore::ObjectType*> ObjectTypeList;

      ObjectTypeSelectDialog(QWidget* parent = NULL);

      virtual ~ObjectTypeSelectDialog();

      ObjectTypeListPanel& GetObjectTypeListPanel();

      int GetSelection(ObjectTypeList& outList) const;

      const dtCore::ObjectType* GetSelectedType() const;

      int GetItemCount() const;

      void SetSingleSelectMode(bool singleSelect);

      bool IsSingleSelectMode() const;

      virtual void UpdateUI();

   protected:
      virtual void CreateConnections();

      Ui::BaseDialog* mUI;

      dtQt::ObjectTypeListPanel* mPanel;
   };

}

#endif
