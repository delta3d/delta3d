/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 *
 * Author: Jeff P. Houde
 */

#include <prefix/dtdirectorqtprefix.h>
#include <dtDirectorQt/importeditor.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undoaddimportevent.h>
#include <dtDirectorQt/undoremoveimportevent.h>

#include <dtCore/project.h>

#include <dtDirector/director.h>
#include <dtDirector/directorgraph.h>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QMainWindow>
#include <QtGui/QGroupBox>

#include <osgDB/FileNameUtils>

#include <sstream>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   ImportEditor::ImportEditor(DirectorEditor* editor)
      : QDialog(editor)
      , mEditor(editor)
   {
      setWindowTitle(tr("Import Editor"));

      QGroupBox*   groupBox   = new QGroupBox(tr("Imported Scripts"), this);
      QGridLayout* gridLayout = new QGridLayout(groupBox);

      // add the lib names to the grid
      mImportView = new QListWidget(groupBox);
      mImportView->setSelectionMode(QAbstractItemView::SingleSelection);
      gridLayout->addWidget(mImportView, 0, 0);

      // create the buttons, default delete to disabled
      QHBoxLayout* buttonLayout = new QHBoxLayout;
      QPushButton* importButton = new QPushButton(tr("Import Script"), this);
      QPushButton* closeButton  = new QPushButton(tr("Close"), this);
      mDeleteButton = new QPushButton(tr("Remove Script"), this);

      mDeleteButton->setDisabled(true);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(mDeleteButton);
      buttonLayout->addWidget(importButton);
      buttonLayout->addWidget(closeButton);
      buttonLayout->addStretch(1);

      // make the connections
      connect(mDeleteButton, SIGNAL(clicked()),              this, SLOT(SpawnDeleteConfirmation()));
      connect(importButton,  SIGNAL(clicked()),              this, SLOT(SpawnFileBrowser()));
      connect(closeButton,   SIGNAL(clicked()),              this, SLOT(close()));
      connect(mImportView,   SIGNAL(currentRowChanged(int)), this, SLOT(EnableButtons(int)));

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      mCurrentDir = osgDB::convertFileNameToNativeStyle(dtCore::Project::GetInstance().GetContext()+"/directors/").c_str();
      mCurrentDir = osgDB::getRealPath(mCurrentDir.toStdString()).c_str();
      mCurrentDir.replace('\\', '/');

      Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   ImportEditor::~ImportEditor()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ImportEditor::SpawnFileBrowser()
   {
      QFileDialog dialog;
      QFileInfo filePath = dialog.getOpenFileName(
         this, tr("Load a Director Script File"), mCurrentDir, tr("Director Scripts (*.dtdir *.dtdirb)"));

      if(!filePath.isFile())
      {
         return;
      }

      QString relPath = dtUtil::FileUtils::GetInstance().RelativePath(mCurrentDir.toStdString(), filePath.absoluteFilePath().toStdString()).c_str();
      relPath.replace('\\', ':');
      relPath.replace('/', ':');
      relPath.prepend("Directors:");

      // Attempt to import the script.
      dtDirector::Director* imported = mEditor->GetDirector()->ImportScript(relPath.toStdString());

      if (imported)
      {
         // Create an undo event.
         dtCore::RefPtr<UndoAddImportEvent> event = new UndoAddImportEvent(mEditor, relPath.toStdString());
         event->SetDescription("Importing script \'" + relPath.toStdString() + "\'.");
         mEditor->GetUndoManager()->AddEvent(event);

         Refresh();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ImportEditor::SpawnDeleteConfirmation()
   {
      if (QMessageBox::question(this, tr("Confirm deletion"),
         tr("Are you sure you want to remove this script?"),
         tr("&Yes"), tr("&No"), QString::null, 1) == 0)
      {
         std::string toRemove = mImportView->currentItem()->text().toStdString();

         if (mEditor->GetDirector()->RemoveImportedScript(toRemove))
         {
            // Create an undo event.
            dtCore::RefPtr<UndoRemoveImportEvent> event = new UndoRemoveImportEvent(mEditor, toRemove);
            event->SetDescription("Removal of imported script \'" + toRemove + "\'.");
            mEditor->GetUndoManager()->AddEvent(event);
            Refresh();
         }
      }
   }

   ////////////////////////////////////////////////////////
   void ImportEditor::EnableButtons(int row)
   {
      if (row < 0 || mImportView->currentItem() == NULL)
      {
         mDeleteButton->setEnabled(false);
      }
      else
      {
         mDeleteButton->setEnabled(row >= 0);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ImportEditor::GetScriptNames(std::vector<QListWidgetItem*>& items) const
   {
      items.clear();

      int count = (int)mEditor->GetDirector()->GetImportedScriptList().size();
      for (int index = 0; index < count; ++index)
      {
         dtDirector::Director* script = mEditor->GetDirector()->GetImportedScriptList()[index];
         if (script)
         {
            std::string scriptId = script->GetResource().GetResourceIdentifier();
            QListWidgetItem* item = new QListWidgetItem;
            item->setText(tr(scriptId.c_str()));
            items.push_back(item);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ImportEditor::Refresh()
   {
      mImportView->clear();

      std::vector<QListWidgetItem*> scripts;
      GetScriptNames(scripts);
      for (unsigned int i = 0; i < scripts.size(); ++i)
      {
         mImportView->addItem(scripts[i]);
      }

      if (mImportView->count() > 0)
      {
         mImportView->setCurrentItem(mImportView->item(mImportView->count() - 1));
      }

      mEditor->RefreshGraphs();
   }

} // namespace dtDirector
