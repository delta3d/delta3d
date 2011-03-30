/* -*-c++-*-
 * Delta3D
 * Copyright 2009, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 * William E. Johnson II
 */
#include <prefix/dtqtprefix.h>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QListWidgetItem>
#include <QtGui/QListWidget>
#include <QtCore/QStringList>
#include <QtGui/QMainWindow>
#include <QtGui/QGroupBox>

#include <dtQt/baselibrarylisteditor.h>
#include <dtUtil/log.h>
#include <dtUtil/librarysharingmanager.h>

#include <osgDB/FileNameUtils>

namespace dtQt
{

   ////////////////////////////////////////////////////////
   BaseLibraryListEditor::BaseLibraryListEditor(QWidget* parent)
      : QDialog(parent)
   {
      setWindowTitle(tr("Library Editor"));

      QGroupBox*   groupBox   = new QGroupBox(tr("Loaded Libraries"), this);
      QGridLayout* gridLayout = new QGridLayout(groupBox);

      // add the lib names to the grid
      mLibView = new QListWidget(groupBox);
      mLibView->setSelectionMode(QAbstractItemView::SingleSelection);
      gridLayout->addWidget(mLibView, 0, 0);

      // Create the arrow buttons for changing the library order.
      QVBoxLayout* arrowLayout = new QVBoxLayout;
      mUpLib   = new QPushButton(tr("^"), groupBox);
      mDownLib = new QPushButton(tr("v"), groupBox);
      arrowLayout->addStretch(1);
      arrowLayout->addWidget(mUpLib);
      arrowLayout->addWidget(mDownLib);
      arrowLayout->addStretch(1);
      gridLayout->addLayout(arrowLayout, 0, 1);

      // create the buttons, default delete to disabled
      QHBoxLayout* buttonLayout = new QHBoxLayout;
      QPushButton* importLib    = new QPushButton(tr("Import Library"), this);
      QPushButton* close        = new QPushButton(tr("Close"), this);
      mDeleteLib = new QPushButton(tr("Remove Library"), this);

      mDeleteLib->setDisabled(true);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(mDeleteLib);
      buttonLayout->addWidget(importLib);
      buttonLayout->addWidget(close);
      buttonLayout->addStretch(1);

      // make the connections
      connect(mDeleteLib, SIGNAL(clicked()),         this, SLOT(SpawnDeleteConfirmation()));
      connect(importLib, SIGNAL(clicked()),         this, SLOT(SpawnFileBrowser()));
      connect(mUpLib,     SIGNAL(clicked()),         this, SLOT(ShiftLibraryUp()));
      connect(mDownLib,   SIGNAL(clicked()),         this, SLOT(ShiftLibraryDown()));
      connect(close,     SIGNAL(clicked()),         this, SLOT(close()));
      connect(mLibView,   SIGNAL(currentRowChanged(int)), this, SLOT(EnableButtons(int)));

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);
   }

   ////////////////////////////////////////////////////////
   BaseLibraryListEditor::~BaseLibraryListEditor()
   {
   }

   ////////////////////////////////////////////////////////
   void BaseLibraryListEditor::showEvent(QShowEvent*)
   {
      RefreshLibraries();
   }
   ////////////////////////////////////////////////////////
   void BaseLibraryListEditor::RefreshLibraries()
   {
      mLibView->clear();

      std::vector<QListWidgetItem*> libs;
      GetLibraryNames(libs);
      for (unsigned int i = 0; i < libs.size(); ++i)
      {
         mLibView->addItem(libs[i]);
      }

      if (mLibView->currentItem() != NULL)
      {
         //is this necessary.
         mLibView->setItemSelected(mLibView->currentItem(), true);
      }
   }

   ////////////////////////////////////////////////////////
   std::pair<std::string, std::string> BaseLibraryListEditor::SelectLibraryToOpen(const std::string& startingDir)
   {
      QString file;
      QString hack = startingDir.c_str();
      hack.replace('\\', '/');

      std::string libs = "Libraries(" + dtUtil::LibrarySharingManager::GetPlatformSpecificLibraryName("*") + ")";

      file = QFileDialog::getOpenFileName(this, tr("Select a library"), "", tr(libs.c_str()));

      // did they hit cancel?
      if (file.isEmpty())
      {
         return std::make_pair(std::string(), std::string());
      }

      std::string libName = dtUtil::LibrarySharingManager::GetPlatformIndependentLibraryName(file.toStdString());
      return std::make_pair(libName, file.toStdString());
   }

   ////////////////////////////////////////////////////////
   void BaseLibraryListEditor::ShiftLibraryUp()
   {
      if (mLibView->currentItem() != NULL)
      {
         int row = mLibView->currentRow();
         RefreshLibraries();
         QListWidgetItem* item = mLibView->item(row + 1);
         if (item != NULL)
         {
            mLibView->setCurrentItem(item);
         }
      }
   }

   ////////////////////////////////////////////////////////
   void BaseLibraryListEditor::ShiftLibraryDown()
   {
      // gotta love my QA
      if (mLibView->currentItem() != NULL)
      {
         int row = mLibView->currentRow();
         RefreshLibraries();
         QListWidgetItem* item = mLibView->item(row + 1);
         if (item != NULL)
         {
            mLibView->setCurrentItem(item);
         }
      }
   }

   ////////////////////////////////////////////////////////
   void BaseLibraryListEditor::HandleFailure(const int errorCode, const std::string& errorMsg)
   {
      if (errorCode == ERROR_LIB_NOT_LOADED)
      {
         QMessageBox::critical(this, tr("Failed to delete library"),
            errorMsg.empty() ? tr("Library is not currently loaded") : QString(errorMsg.c_str()),
            tr("&OK"));
      }
      else if (errorCode == ERROR_OBJECTS_IN_LIB_EXIST)
      {
         QString str = errorMsg.c_str();
         if (str.isEmpty())
         {
            str = "Failed to remove the library, objects exist in the system that depend on it.";
         }

         QMessageBox::critical(this, tr("Failed to delete library"),
            str,
            tr("&OK"));
      }
      else if (errorCode == ERROR_INVALID_LIB)
      {
         QString message(tr("Error Message: "));
         message.append(errorMsg.c_str());
         message.append("\n\nPlease ensure that the name is correct, the library is in the path (or the working directory), ");
         message.append("the library can load correctly, and dependent libraries are available.");

         QMessageBox::critical(this, tr("Failed to import library"),
            message,
            tr("&OK"));
      }
      else
      {
         QMessageBox::critical(this, tr("Failed to delete library"),
            tr("Failed to remove the library, unknown error"),
            tr("&OK"));
      }
   }

   ////////////////////////////////////////////////////////
   void BaseLibraryListEditor::EnableButtons(int row)
   {
      if (row < 0 || mLibView->currentItem() == NULL)
      {
         mDeleteLib->setEnabled(false);
         mUpLib->setEnabled(false);
         mDownLib->setEnabled(false);
      }
      else
      {
         int itemCount = mLibView->count();
         mDeleteLib->setEnabled(row >= 0);
         mUpLib->setEnabled(row > 0);
         mDownLib->setEnabled(row >= 0 && row < (itemCount - 1));
      }
   }

} // namespace dtQt
