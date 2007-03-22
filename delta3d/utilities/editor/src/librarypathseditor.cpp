/*
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * William E. Johnson II
 */
#include <prefix/dtstageprefix-src.h>
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

#include <dtEditQt/LibraryPathsEditor.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editoractions.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/actorpluginregistry.h>
#include <dtDAL/map.h>
#include <dtUtil/log.h>

#include <osgDB/FileNameUtils>
#include <osgDB/Registry>

using dtDAL::ActorProxy;
using dtDAL::ActorType;
using dtDAL::Map;
using dtDAL::ActorPluginRegistry;
using dtDAL::LibraryManager;
/// @cond DOXYGEN_SHOULD_SKIP_THIS
using std::vector;
using std::string;
/// @endcond

enum { ERROR_LIB_NOT_LOADED = 0, ERROR_ACTORS_IN_LIB, ERROR_INVALID_LIB, ERROR_UNKNOWN };

namespace dtEditQt
{
   LibraryPathsEditor::LibraryPathsEditor(QWidget *parent) : QDialog(parent), numActorsInScene(0)
   {
      setWindowTitle(tr("Library Editor"));
      
      QGroupBox *groupBox = new QGroupBox(tr("Library Search Path Order"),this);
      QGridLayout *gridLayout = new QGridLayout(groupBox);
      
      // add the lib names to the grid
      pathView = new QListWidget(groupBox);
      pathView->setSelectionMode(QAbstractItemView::SingleSelection);
      gridLayout->addWidget(pathView,0,0);
            
      //Create the arrow buttons for changing the library order.
      QVBoxLayout *arrowLayout = new QVBoxLayout;
      upPath = new QPushButton(tr("^"),groupBox);
      downPath = new QPushButton(tr("v"),groupBox);
      arrowLayout->addStretch(1);
      arrowLayout->addWidget(upPath);
      arrowLayout->addWidget(downPath);
      arrowLayout->addStretch(1);
      gridLayout->addLayout(arrowLayout,0,1);
      
      // create the buttons, default delete to disabled
      QHBoxLayout *buttonLayout = new QHBoxLayout;
      QPushButton *addPath = new QPushButton(tr("Add Path"),this);
      QPushButton *close = new QPushButton(tr("Close"),this);
      deletePath = new QPushButton(tr("Remove Path"),this);
      
      deletePath->setDisabled(true);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(addPath);
      buttonLayout->addWidget(deletePath);
      buttonLayout->addWidget(close);
      buttonLayout->addStretch(1);
      
      // make the connections
      connect(deletePath, SIGNAL(clicked()),   this, SLOT(spawnDeleteConfirmation()));
      connect(addPath, SIGNAL(clicked()),   this, SLOT(spawnFileBrowser()));
      connect(upPath,     SIGNAL(clicked()),   this, SLOT(shiftPathUp()));
      connect(downPath,   SIGNAL(clicked()),   this, SLOT(shiftPathDown()));
      connect(close,     SIGNAL(clicked()),   this, SLOT(close()));
      connect(this, SIGNAL(noPathsSelected()), this, SLOT(disableButtons()));
      connect(this, SIGNAL(pathSelected()),this, SLOT(enableButtons()));
      
      QVBoxLayout *mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      refreshPaths();
   }

   LibraryPathsEditor::~LibraryPathsEditor()
   {
      
   }

   void LibraryPathsEditor::getPathNames(vector<QListWidgetItem*>& items) const
   {
      items.clear();
      
      std::vector<std::string> pathList;
      dtUtil::LibrarySharingManager::GetInstance().GetSearchPath(pathList);
      if(pathList.empty())
         return;

      for(std::vector<std::string>::const_iterator iter = pathList.begin(); 
          iter != pathList.end(); 
          ++iter)
      {
         items.push_back(new QListWidgetItem(tr((*iter).c_str())));
      }
   }

   ///////////////////////// Slots /////////////////////////
   void LibraryPathsEditor::spawnFileBrowser()
   {
      QString file;
      string dir = EditorData::GetInstance().getCurrentLibraryDirectory();
      QString hack = dir.c_str();
      hack.replace('\\', '/');
      
      file = QFileDialog::getExistingDirectory(this, tr("Select a directory to add to the library path"));
      
      // did they hit cancel?
      if(file.isEmpty())
         return;

      dtUtil::LibrarySharingManager::GetInstance().AddToSearchPath(file.toStdString());

      refreshPaths();
   }

   void LibraryPathsEditor::spawnDeleteConfirmation()
   {
      if(QMessageBox::question(this, tr("Confirm deletion"),
                              tr("Are you sure you want to remove this path?"),
                              tr("&Yes"), tr("&No"), QString::null, 1) == 0)
      {
         std::string pathToRemove = pathView->currentItem()->text().toStdString();
         
         dtUtil::LibrarySharingManager::GetInstance().RemoveFromSearchPath(pathToRemove);
          
         refreshPaths();
      }
   }

   void LibraryPathsEditor::shiftPathUp()
   {
      std::vector<std::string> pathList;
      dtUtil::LibrarySharingManager::GetInstance().GetSearchPath(pathList);

      std::string itemText = pathView->currentItem()->text().toStdString();
      std::vector<std::string>::iterator iter;
      int i = 0;
      bool found = false;

      for(iter = pathList.begin(); iter != pathList.end(); ++iter, ++i)
      {
         std::string text = *iter;

         if(text == itemText && iter != pathList.begin())
         {
            found = true;
            break;
         }
      }

      if(!found)
      {
         return;
      }
      else
      {
         std::vector<std::string>::iterator a = iter;
         std::vector<std::string>::iterator b = --iter;
           
         std::string tmp = *a;
         *a = *b;
         *b = tmp;
      }

      refreshPaths();

      // ensure the current item is selected
      QListWidgetItem *item = pathView->item(i - 1);
      if(item)
      {
         pathView->setCurrentItem(item);
         if(item == pathView->item(0))
         {
            upPath->setDisabled(true);
         }

         if(item == pathView->item(pathView->count()-1))
         {
            downPath->setDisabled(true);
         }
      }
   }

   void LibraryPathsEditor::shiftPathDown()
   {
      std::vector<std::string> pathList;
      dtUtil::LibrarySharingManager::GetInstance().GetSearchPath(pathList);

      std::string itemText = pathView->currentItem()->text().toStdString();
      std::vector<std::string>::iterator iter;
      int i = 0;
      bool found = false;

      if(pathList.size() <= 1)
      {
         return;
      }

      // Need to get the actual end of the list so that we don't accidentally
      // attempt to swap positions with the last item and the list's end position
      iter = pathList.end();
      iter--;

      // Store the actual last item position away for later use.
      std::vector<std::string>::iterator lastItem = iter;

      for(iter = pathList.begin(); iter != pathList.end(); ++iter, ++i)
      {
         std::string text = *iter;

         // Cannot test against pathList->end() 
         if(text == itemText && iter != lastItem)
         {
            found = true;
            break;
         }
      }

      if(!found)
      {
         return;
      }
      else
      {
         std::vector<std::string>::iterator a = iter;
         std::vector<std::string>::iterator b = ++iter;
           
         std::string tmp = *a;
         *a = *b;
         *b = tmp;
      }

      refreshPaths();

      // ensure the current item is selected
      QListWidgetItem *item = pathView->item(i + 1);
      if(item)
      {
         pathView->setCurrentItem(item);
         if(item == pathView->item(0))
         {
            upPath->setDisabled(true);
         }
           
         if(item == pathView->item(pathView->count()-1))
         {
            downPath->setDisabled(true);
         }
      }
   }

   void LibraryPathsEditor::enableButtons()
   {
      deletePath->setDisabled(false);
      upPath->setDisabled(false);
      downPath->setDisabled(false);
   }

   void LibraryPathsEditor::disableButtons()
   {
      deletePath->setDisabled(true);
      upPath->setDisabled(true);
      downPath->setDisabled(true);
   }

   void LibraryPathsEditor::refreshPaths()
   {
      std::string customPath;
      pathView->clear();

      std::vector<QListWidgetItem*> paths;
      getPathNames(paths);
       
      for(size_t i = 0; i < paths.size(); i++)
      {   
         pathView->addItem(paths[i]);
         customPath += paths[i]->text().toStdString();
         customPath += ";";
      }
       
      connect(pathView, SIGNAL(itemSelectionChanged()), this, SLOT(enableButtons()));
      if(pathView->currentItem() == NULL)
         emit noPathsSelected();
      else
         pathView->setItemSelected(pathView->currentItem(), true);
   }
}


