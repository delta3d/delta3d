/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_osgwriteroptionsdialog.h"
#include <dtQt/osgwriteroptionsdialog.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgWriterOptionsDialog::OsgWriterOptionsDialog(QWidget* parent)
      : BaseClass(parent)
      , mUI(new Ui::OsgWriterOptionsDialog)
   {
      mUI->setupUi(this);

      // Ensure that the optimizer checkboxes are in sync.
      mUI->mOptimizerOptions->setChecked(mUI->mOsgWriterOptionsPanel->IsOptimizeChecked());

      CreateConnections();
   }

   OsgWriterOptionsDialog::~OsgWriterOptionsDialog()
   {
      delete mUI;
      mUI = nullptr;
   }

   void OsgWriterOptionsDialog::CreateConnections()
   {
      // Setup bi-directional connection for keeping the optimize check boxes in sync.

      // Notify this of changes to the Optimize option in the Writer options.
      connect(mUI->mOsgWriterOptionsPanel, SIGNAL(SignalOptimizeChecked(bool)),
         this, SLOT(OnWriterOptionsOptimizeChecked(bool)));

      // Notify the Writer options of changes to checked state of the Optimizer options group.
      connect(mUI->mOptimizerOptions, SIGNAL(clicked(bool)),
         mUI->mOsgWriterOptionsPanel, SLOT(SetOptimizeChecked(bool)));
   }

   dtQt::OsgWriterOptionsPanel& OsgWriterOptionsDialog::GetWriterOptionsPanel()
   {
      return *mUI->mOsgWriterOptionsPanel;
   }

   dtQt::OsgOptimizerPanel& OsgWriterOptionsDialog::GetOptimizerOptionsPanel()
   {
      return *mUI->mOsgOptimizerPanel;
   }

   void OsgWriterOptionsDialog::SetOptimizerChecked(bool checked)
   {
      mUI->mOptimizerOptions->setChecked(checked);
   }

   bool OsgWriterOptionsDialog::IsOptimizerChecked() const
   {
      return mUI->mOptimizerOptions->isChecked();
   }

   void OsgWriterOptionsDialog::OnWriterOptionsOptimizeChecked(bool checked)
   {
      SetOptimizerChecked(checked);
   }

}
