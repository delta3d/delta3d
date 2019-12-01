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
#include <QtGui/QGroupBox>

#include <dtQt/librarypathseditor.h>
#include <dtUtil/librarysharingmanager.h>

#include <cassert>

enum
{
   ERROR_LIB_NOT_LOADED = 0,
   ERROR_ACTORS_IN_LIB,
   ERROR_INVALID_LIB,
   ERROR_UNKNOWN
};

namespace dtQt
{

   LibraryPathsEditor::LibraryPathsEditor(QWidget* parent, const std::string& fileBrowserStartDir)
      : QDialog(parent)
      , mFileBrowserDir(fileBrowserStartDir)
   {
      bool okay = true;
      setWindowTitle(tr("Library Editor"));

      QGroupBox*   groupBox = new QGroupBox(tr("Library Search Path Order"), this);
      QGridLayout* gridLayout = new QGridLayout(groupBox);

      // add the lib names to the grid
      pathView = new QListWidget(groupBox);
      pathView->setSelectionMode(QAbstractItemView::SingleSelection);
      gridLayout->addWidget(pathView, 0, 0);

      // Create the arrow buttons for changing the library order.
      QVBoxLayout* arrowLayout = new QVBoxLayout;
      upPath = new QPushButton(tr("^"), groupBox);
      downPath = new QPushButton(tr("v"), groupBox);
      arrowLayout->addStretch(1);
      arrowLayout->addWidget(upPath);
      arrowLayout->addWidget(downPath);
      arrowLayout->addStretch(1);
      gridLayout->addLayout(arrowLayout, 0, 1);

      // create the buttons, default delete to disabled
      QHBoxLayout* buttonLayout = new QHBoxLayout;
      QPushButton* addPath      = new QPushButton(tr("Add Path"), this);
      QPushButton* close        = new QPushButton(tr("Close"), this);
      deletePath = new QPushButton(tr("Remove Path"), this);

      buttonLayout->addStretch(1);
      buttonLayout->addWidget(addPath);
      buttonLayout->addWidget(deletePath);
      buttonLayout->addWidget(close);
      buttonLayout->addStretch(1);

      // Hide functionality that does not yet exist
      upPath->hide();
      downPath->hide();

      // make the connections
      okay = okay && connect(deletePath, SIGNAL(clicked()),              this, SLOT(SpawnDeleteConfirmation()));
      okay = okay && connect(addPath,    SIGNAL(clicked()),              this, SLOT(SpawnFileBrowser()));
      okay = okay && connect(upPath,     SIGNAL(clicked()),              this, SLOT(ShiftPathUp()));
      okay = okay && connect(downPath,   SIGNAL(clicked()),              this, SLOT(ShiftPathDown()));
      okay = okay && connect(close,      SIGNAL(clicked()),              this, SLOT(close()));
      okay = okay && connect(pathView,   SIGNAL(itemSelectionChanged()), this, SLOT(RefreshButtons()));

      // make sure all connections were successfully made
      assert(okay);

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      RefreshPaths();
      RefreshButtons();
   }

   LibraryPathsEditor::~LibraryPathsEditor()
   {
   }

   bool LibraryPathsEditor::AnyItemsSelected() const
   {
      //return pathView->currentItem() != NULL;  // note: this test returns false positives
      return !pathView->selectedItems().empty(); // this one is better
   }

   void LibraryPathsEditor::GetPathNames(std::vector<QListWidgetItem*>& items) const
   {
      items.clear();

      std::vector<std::string> pathList;
      dtUtil::LibrarySharingManager::GetInstance().GetSearchPath(pathList);
      if (pathList.empty())
      {
         return;
      }

      for (std::vector<std::string>::const_iterator iter = pathList.begin();
         iter != pathList.end();
         ++iter)
      {
         items.push_back(new QListWidgetItem(tr((*iter).c_str())));
      }
   }

   ///////////////////////// Slots /////////////////////////
   void LibraryPathsEditor::SpawnFileBrowser()
   {
      QString file;
      QString hack = mFileBrowserDir.c_str();
      hack.replace('\\', '/');

      file = QFileDialog::getExistingDirectory(this, tr("Select a directory to add to the library path"));

      // did they hit cancel?
      if (file.isEmpty())
      {
         return;
      }

      dtUtil::LibrarySharingManager::GetInstance().AddToSearchPath(file.toStdString());

      RefreshPaths();
   }

   void LibraryPathsEditor::SpawnDeleteConfirmation()
   {
      if (QMessageBox::question(this, tr("Confirm deletion"),
         tr("Are you sure you want to remove this path?"),
         tr("&Yes"), tr("&No"), QString::null, 1) == 0)
      {
         std::string pathToRemove = pathView->currentItem()->text().toStdString();

         dtUtil::LibrarySharingManager::GetInstance().RemoveFromSearchPath(pathToRemove);

         RefreshPaths();
      }
   }

   void LibraryPathsEditor::ShiftPathUp()
   {
      QListWidgetItem* item = pathView->item(pathView->count() - 1);
      if (item != NULL)
      {
         pathView->setCurrentItem(item);
         if (item == pathView->item(0))
         {
            upPath->setDisabled(true);
         }

         if (item == pathView->item(pathView->count() - 1))
         {
            downPath->setDisabled(true);
         }
      }

      dtUtil::LibrarySharingManager::GetInstance().ClearSearchPath();

      for (int i = 0; i < pathView->count(); ++i)
      {
         dtUtil::LibrarySharingManager::GetInstance().AddToSearchPath(pathView->item(i)->text().toStdString());
      }

      RefreshPaths();
   }

   void LibraryPathsEditor::ShiftPathDown()
   {
      // ensure the current item is selected
      QListWidgetItem* item = pathView->item(pathView->count() + 1);
      if (item != NULL)
      {
         pathView->setCurrentItem(item);
         if (item == pathView->item(0))
         {
            upPath->setDisabled(true);
         }

         if (item == pathView->item(pathView->count() - 1))
         {
            downPath->setDisabled(true);
         }
      }

      dtUtil::LibrarySharingManager::GetInstance().ClearSearchPath();

      for (int i = 0; i < pathView->count(); ++i)
      {
         dtUtil::LibrarySharingManager::GetInstance().AddToSearchPath(pathView->item(i)->text().toStdString());
      }

      RefreshPaths();
   }

   void LibraryPathsEditor::RefreshButtons()
   {
      bool pathIsSelected = AnyItemsSelected();

      deletePath->setEnabled(pathIsSelected);
      upPath->setEnabled(pathIsSelected);
      downPath->setEnabled(pathIsSelected);
   }

   void LibraryPathsEditor::RefreshPaths()
   {
      pathView->clear();

      std::vector<QListWidgetItem*> paths;
      GetPathNames(paths);

      for (size_t i = 0; i < paths.size(); ++i)
      {
         pathView->addItem(paths[i]);
      }

      if (AnyItemsSelected())
      {
         pathView->setItemSelected(pathView->currentItem(), true);
      }
   }

} // namespace dtEditQt
