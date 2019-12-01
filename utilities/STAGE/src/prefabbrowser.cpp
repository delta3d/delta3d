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
 * Jeffrey P. Houde
 */

#include <prefix/stageprefix.h>
#include <dtEditQt/prefabbrowser.h>

#include <QtGui/QDirModel>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QInputDialog>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtCore/QStringList>
#include <QtGui/QSplitter>
#include <QtGui/QCheckBox>
#include <QtGui/QToolButton>

#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/actortypetreewidget.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/prefabsaveasdialog.h>
#include <dtEditQt/resourcelistwidgetitem.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/uiresources.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/stagecamera.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/viewportcontainer.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtCore/scene.h>
#include <dtCore/object.h>
#include <dtActors/engineactorregistry.h>
#include <dtCore/actorfactory.h>
#include <dtCore/map.h>
#include <dtCore/datatype.h>
#include <dtCore/project.h>
#include <dtCore/mapxml.h>
#include <dtCore/actorproperty.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/actorfactory.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/log.h>
#include <algorithm>

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   PrefabBrowser::PrefabBrowser(QWidget* parent)
   : QWidget(parent)
   , mCurrentDir("")
   , mPopupMenu(this)
   , mExportNewPrefabAction(new QAction("Export New Prefab", this))
   , mDeleteShortcut(QKeySequence(Qt::Key_Delete), this, SLOT(deleteKeyPushedSlot()))
   {
      setupGUI();

      connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
            this, SLOT(refreshPrefabs()));

      connect(&EditorActions::GetInstance(), SIGNAL(PrefabExported()),
            this, SLOT(refreshPrefabs()));

      mPopupMenu.addAction("Add Category", this, SLOT(addCategorySlot()));
      mPopupMenu.addAction(mExportNewPrefabAction);
      connect(mExportNewPrefabAction, SIGNAL(triggered()), this, SLOT(exportNewPrefabSlot()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   PrefabBrowser::~PrefabBrowser()
   {      
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::setupGUI()
   {
      // Setup the main grid.
      mGrid = new QGridLayout(this);
      mGrid->addWidget(listGroup(), 0, 0);
      mGrid->addLayout(buttonLayout(), 1, 0, Qt::AlignCenter);

      mGrid->setRowStretch(1, 0);

      refreshPrefabs();

      handleEnableCreateActorBtn();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::clearPrefabTree()
   {
      mListWidget->clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::handleEnableCreateActorBtn()
   {
      ResourceListWidgetItem* selectedWidget = getSelectedPrefabWidget();

      if(selectedWidget != NULL && selectedWidget->isResource())
      {
         //enable the button
         mCreatePrefabBtn->setEnabled(true);
         mCreateInstanceBtn->setEnabled(true);

         // Set the current resource.
         EditorData::GetInstance().setCurrentResource(dtCore::DataType::PREFAB, selectedWidget->getResourceDescriptor());
         return;
      }

      // disable the button if we got here.
      mCreatePrefabBtn->setEnabled(false);
      mCreateInstanceBtn->setEnabled(false);

      // Clear the current resource.
      EditorData::GetInstance().setCurrentResource(dtCore::DataType::PREFAB, dtCore::ResourceDescriptor());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::addCategorySlot()
   {
      bool ok;
      bool validCategory = false;
      QString newCategoryName;
      std::string newCatFullPath;
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      while (!validCategory)
      {
         newCategoryName = QInputDialog::getText(this, tr("Category Name"),
               tr("Enter New Category Name:"), QLineEdit::Normal,
               tr(""), &ok);

         //Pushed OK button?
         if (!ok || newCategoryName == "")
         {
            //pushed Cancel button, we're done here
            return;
         }

         if (newCategoryName.contains(QRegExp("[\\\\/:\\*\\?\"<>|]")))
         {
            QMessageBox::critical(this, tr("Invalid Characters"),
                  tr("category names cannot contain any of these characters\n \\/\"*?<>|"),
                  tr("OK"));

            continue; //category name isn't valid
         }

         newCatFullPath = mCurrentDir + "/" + newCategoryName.toStdString();

         if (fileUtils.DirExists(newCatFullPath))
         {
            QMessageBox::critical(this, tr("Category Already Exists"), 
                  tr("Category Already Exists"), tr("OK"));
            continue; //category isn't valid
         }     

         validCategory = true;
      }

      try
      {
         fileUtils.MakeDirectory(newCatFullPath);
      }
      catch (const dtUtil::Exception& e)
      {
         QString reason("Unable to create category.\n This reason was given:\n\n");
         reason += e.ToString().c_str();

         QMessageBox::critical(this, tr("Unable to Create Category"), reason, tr("OK"));

         return;
      }

      refreshPrefabs();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::exportNewPrefabSlot()
   {
      PrefabSaveDialog dlg(this);
      std::string contextDir = dtCore::Project::GetInstance().GetContext();
      std::string contextDirPlusPrefab = contextDir + "/Prefabs";
      std::string categoryRelPath = mCurrentDir.substr(contextDirPlusPrefab.size());

      //chop off any preceeding slashes the category name
      if(categoryRelPath[0] == '/' || categoryRelPath[0] == '\\')
      {
         categoryRelPath = categoryRelPath.substr(1);
      }

      dlg.setPrefabCategory(categoryRelPath);

      if (dlg.exec() == QDialog::Rejected)
      {
         return;
      }

      EditorActions::GetInstance().SaveNewPrefab(dlg.getPrefabCategory(),
            dlg.getPrefabFileName(),
            dlg.GetPrefabIconFileName(),
            dlg.getPrefabDescription());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ResourceListWidgetItem* PrefabBrowser::getSelectedPrefabWidget()
   {   
      if (mListWidget != NULL)
      {
         QList<QListWidgetItem*> list = mListWidget->selectedItems();

         if (! list.isEmpty())  
         {            
            return dynamic_cast<ResourceListWidgetItem*>(list[0]);
         }
      }

      //nothing is selected
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* PrefabBrowser::listGroup()
   {
      QGroupBox*   groupBox = new QGroupBox(tr("Prefabs"));
      QGridLayout* grid     = new QGridLayout(groupBox);

      mListWidget = new ResourceDragListWidget(groupBox);
      mListWidget->setResourceName("Prefab");
      mListWidget->setAutoScroll(true);
      //mListWidget->setViewMode(QListWidget::IconMode);
      mListWidget->setIconSize(QSize(50,50));
      mListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

      // connect signals
      connect(mListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(listSelectionChanged()));
      connect(mListWidget, SIGNAL(itemActivated(QListWidgetItem *)), this,
            SLOT(listSelectionDoubleClicked(QListWidgetItem*)));
      connect(mListWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(rightClickMenu(const QPoint &)));

      // Checkbox for auto preview
      grid->addWidget(mListWidget, 1, 0);

      return groupBox;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QBoxLayout* PrefabBrowser::buttonLayout()
   {
      mCreatePrefabBtn = new QPushButton(tr("Create Actor(s)"), this);
      connect(mCreatePrefabBtn, SIGNAL(clicked()), this, SLOT(createPrefabPressed()));

      mCreateInstanceBtn = new QPushButton(tr("Create Prefab"), this);
      connect(mCreateInstanceBtn, SIGNAL(clicked()), this, SLOT(createPrefabInstancePressed()));

      mRefreshPrefabBtn = new QPushButton(tr("Refresh"), this);
      connect(mRefreshPrefabBtn, SIGNAL(clicked()), this, SLOT(refreshPrefabs()));

      QHBoxLayout* btnLayout = new QHBoxLayout();
      btnLayout->addStretch(1);
      btnLayout->addWidget(mCreatePrefabBtn);
      btnLayout->addWidget(mCreateInstanceBtn);
      btnLayout->addWidget(mRefreshPrefabBtn);
      btnLayout->addStretch(1);

      return btnLayout;
   }

   ///////////////////////////////////////////////////////////////////////////////
   // SLOTS
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::createPrefabPressed()
   {
      ResourceListWidgetItem* selectedWidget = getSelectedPrefabWidget();

      if (selectedWidget && selectedWidget->isResource())
      {
         dtCore::ResourceDescriptor descriptor = selectedWidget->getResourceDescriptor();
         try
         {
            EditorData::GetInstance().getMainWindow()->startWaitCursor();
            EditorEvents::GetInstance().emitBeginChangeTransaction();

            dtCore::ActorRefPtrVector actors;
            dtCore::Project::GetInstance().LoadPrefab(descriptor, actors);

            // Auto select all of the proxies.
            ViewportOverlay::ActorProxyList selection = ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
            ViewportOverlay* overlay = ViewportManager::GetInstance().getViewportOverlay();


            dtCore::RefPtr<dtCore::Map> currMap = EditorData::GetInstance().getCurrentMap();
            // Make sure we have valid data.
            if (!currMap.valid())
            {
               LOG_ERROR("Current map is not valid.");
               return;
            }

            // Once we have a reference to the current selection and the scene,
            // clone each proxy, add it to the scene, make the newly cloned
            // proxy(s) the current selection.
            ViewportOverlay::ActorProxyList::iterator itor, itorEnd;
            itor    = selection.begin();
            itorEnd = selection.end();

            // Un-select all proxies currently selected.
            for (; itor != itorEnd; ++itor)
            {
               dtCore::BaseActorObject* proxy = const_cast<dtCore::BaseActorObject*>(itor->get());

               // Un-highlight the currently selected proxy.
               if (overlay->isActorSelected(proxy))
               {
                  overlay->removeActorFromCurrentSelection(proxy);
               }
            }

            osg::Vec3 offset = ViewportManager::GetInstance().getWorldViewCamera()->getPosition();

            int groupIndex = currMap->GetGroupCount();

            for (unsigned proxyIndex = 0; proxyIndex < actors.size(); proxyIndex++)
            {
               dtCore::BaseActorObject* actor = actors[proxyIndex].get();
               if (actor->IsActorComponent()) continue;
               if (actor->IsSystemComponent()) continue;

               EditorActions::GetInstance().AddActorToMap(*actor, *currMap, true);
               currMap->AddActorToGroup(groupIndex, *actor);

               currMap->CorrectLibraryList(false);

               // Notify the creation of the proxies.
               EditorEvents::GetInstance().emitActorProxyCreated(actor, false);

               dtCore::TransformableActorProxy* tProxy =
                     dynamic_cast<dtCore::TransformableActorProxy*>(actor);

               if (tProxy)
               {
                  auto gap = dynamic_cast<dtGame::GameActorProxy*>(tProxy);
                  if (gap == nullptr || gap->GetParentActor() == nullptr)
                  {
                     if (proxyIndex == 0)
                     {
                        ViewportManager::GetInstance().placeProxyInFrontOfCamera(actor);

                        offset = tProxy->GetTranslation();
                     }
                     else
                     {
                        tProxy->SetTranslation(tProxy->GetTranslation() + offset);
                     }
                  }
               }
            }


            // Finally set the proxies in the prefab to be the current selection.
            ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(true);
            EditorEvents::GetInstance().emitActorsSelected(actors);
            EditorEvents::GetInstance().emitEndChangeTransaction();

            EditorData::GetInstance().getMainWindow()->endWaitCursor();
         }
         catch (const dtUtil::Exception& e)
         {
            LOG_ERROR(e.What());

            refreshPrefabs();

            EditorData::GetInstance().getMainWindow()->endWaitCursor();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::createPrefabInstancePressed()
   {
      ResourceListWidgetItem* selectedWidget = getSelectedPrefabWidget();

      // if we have an actor type, then create the proxy and emit the signal
      if (selectedWidget && selectedWidget->isResource())
      {
         EditorData::GetInstance().getMainWindow()->startWaitCursor();

         // create our new object
         dtCore::RefPtr<dtCore::BaseActorObject> proxy =
               dtCore::ActorFactory::GetInstance().CreateActor("dtActors", "Prefab");

         if (proxy.valid())
         {
            // add the new proxy to the map
            dtCore::RefPtr<dtCore::Map> mapPtr = EditorData::GetInstance().getCurrentMap();
            if (mapPtr.valid())
            {
               EditorActions::GetInstance().AddActorToMap(*proxy, *mapPtr, true);
               mapPtr->CorrectLibraryList(false);
            }

            // Set the prefab resource of the actor to the current prefab.
            dtCore::ResourceActorProperty* resourceProp = NULL;
            resourceProp = dynamic_cast<dtCore::ResourceActorProperty*>(proxy->GetProperty("PrefabResource"));
            if (resourceProp)
            {
               resourceProp->SetValue(selectedWidget->getResourceDescriptor());
            }

            // let the world know that a new proxy exists
            EditorEvents::GetInstance().emitBeginChangeTransaction();
            EditorEvents::GetInstance().emitActorProxyCreated(proxy.get(), false);
            ViewportManager::GetInstance().placeProxyInFrontOfCamera(proxy.get());
            EditorEvents::GetInstance().emitEndChangeTransaction();

            // Now, let the world that it should select the new actor proxy.
            dtCore::ActorRefPtrVector actors;
            actors.push_back(proxy.get());
            EditorEvents::GetInstance().emitActorsSelected(actors);
         }

         EditorData::GetInstance().getMainWindow()->endWaitCursor();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::deleteKeyPushedSlot()
   {
      if (! isActiveWindow() || ! underMouse())
      {
         return;
      }

      ResourceListWidgetItem* selItem = getSelectedPrefabWidget();

      if (selItem == NULL)
      {
         return;
      }

      //If not a Resource, assume this is a prefab category (folder)
      if (! selItem->isResource())
      {
         QMessageBox msgBox(this);
         std::string informative = "Deleting this Category will delete all Prefabs in the Category.\n";
         informative += "Ensure that none of your maps use these Prefabs before deleting this Category.\n";
         informative += "\nAre you sure you want to delete this Category?";

         msgBox.setWindowTitle("Delete All Prefabs in this Category?");
         msgBox.setText(informative.c_str());
         msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
         msgBox.setDefaultButton(QMessageBox::Cancel);
         int ret = msgBox.exec();

         if (ret == QMessageBox::Cancel)
         {
            return;
         }

         //Deleting the category is a go!
         try 
         {
            dtUtil::FileUtils::GetInstance().DirDelete(selItem->getCategoryFullName().toStdString(), true);
         }
         catch (const dtUtil::Exception& e)
         {
            QString reason("Error deleting category.\n This reason was given:\n\n");
            reason += e.ToString().c_str();

            QMessageBox::critical(this, tr("Unable to delete Category"), reason, tr("OK"));
            return;
         }                  
      }
      else  //if selItem IS a Resource, assume it is a Prefab
      {
         QMessageBox msgBox(this);
         std::string informative = "You should ensure that none of your maps use this Prefab";
         informative += " before deleting it.\n\n";
         informative += "Are you sure you want to delete this Prefab?";

         msgBox.setWindowTitle("Delete Prefab?");
         msgBox.setText(informative.c_str());
         msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
         msgBox.setDefaultButton(QMessageBox::Cancel);
         int ret = msgBox.exec();

         if (ret == QMessageBox::Cancel)
         {
            return;
         }

         //Deleting the prefab is a go!
         std::string prefabFullPath = dtEditQt::EditorData::GetInstance().getCurrentProjectContext();        
         dtCore::ResourceDescriptor& resDes = selItem->getResourceDescriptor();
         QString prefabName = resDes.GetResourceIdentifier().c_str();
         prefabName = prefabName.replace(QRegExp(":"), QString("/"));
         prefabFullPath += "/" + prefabName.toStdString();

         try
         {
            dtUtil::FileUtils::GetInstance().FileDelete(prefabFullPath);
         }
         catch (const dtUtil::Exception& e)
         {
            QString reason("Error deleting Prefab.\n This reason was given:\n\n");
            reason += e.ToString().c_str();

            QMessageBox::critical(this, tr("Unable to Delete Prefab"), reason, tr("OK"));
            return;
         }                          
      }

      refreshPrefabs();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::listSelectionChanged()
   {
      handleEnableCreateActorBtn();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::listSelectionDoubleClicked(QListWidgetItem* activatedItem)
   {
      ResourceListWidgetItem* selectedWidget = getSelectedPrefabWidget();      

      if (selectedWidget == NULL)
      {
         return;
      }

      QString fullpath = selectedWidget->getCategoryFullName();
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      if (fileUtils.DirExists(fullpath.toStdString()))
      {
         mCurrentDir = fullpath.replace(QString("\\"), QString("/")).toStdString();
      }

      refreshPrefabs();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::rightClickMenu(const QPoint& clickPoint)
   {
      //disable "Export Prefab" menu Action if there are no actors selected:
      dtEditQt::ViewportOverlay::ActorProxyList& selectionList = 
            dtEditQt::ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      if(selectionList.size() < 1)
      {
         mExportNewPrefabAction->setEnabled(false);
      }
      else
      {
         mExportNewPrefabAction->setEnabled(true);
      }

      mPopupMenu.exec(QCursor::pos());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::refreshPrefabs()
   {
      unsigned int numCategories = 0;      

      // resets everything and marks the current expansion
      clearPrefabTree();

      dtCore::Project& project = dtCore::Project::GetInstance();

      // We can't do anything if we don't have a valid context.
      if (!project.IsContextValid())
      {
         return;
      }

      EditorData::GetInstance().getMainWindow()->startWaitCursor();

      project.Refresh();
      //project.GetResourcesOfType(dtCore::DataType::PREFAB, mPrefabList);

      QIcon resourceIcon;
      resourceIcon.addPixmap(QPixmap(UIResources::ICON_ACTOR.c_str()));
      mResourceIcon = resourceIcon;

      QIcon folderIcon;
      folderIcon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()), QIcon::Normal, QIcon::On);
      folderIcon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()), QIcon::Normal, QIcon::Off);

      std::string contextDir = dtCore::Project::GetInstance().GetContext(0);
      std::string iconDir = contextDir + "/" +
            dtEditQt::EditorActions::PREFAB_DIRECTORY + "/icons";

      std::string prefabDir = contextDir + "/" + dtEditQt::EditorActions::PREFAB_DIRECTORY;

      // Test if we have changed our context directory.
      if (mTopPrefabDir != prefabDir)
      {
         mCurrentDir = prefabDir;
         mTopPrefabDir = prefabDir; 
      }

      dtUtil::DirectoryContents dirFiles;
      if (dtUtil::FileUtils::GetInstance().DirExists(mCurrentDir))
      {
         dirFiles = dtUtil::FileUtils::GetInstance().DirGetFiles(mCurrentDir);
         //std::sort(dirFiles.begin(), dirFiles.end());
      }
      else
      {
         //Prefabs dir hasn't been created yet ... nothing to show
         EditorData::GetInstance().getMainWindow()->endWaitCursor();
         return;
      }

      std::string nextFile;
      std::string nextFileFullPath;
      std::string nextIconFullPath;
      bool isFolder = false;

      if (!dtUtil::FileUtils::GetInstance().IsSameFile(mCurrentDir, mTopPrefabDir))      
      {
         //Show a "Go up a folder" icon
         nextIconFullPath = UIResources::ICON_UP_FOLDER;
         ResourceListWidgetItem* aWidget = new ResourceListWidgetItem(
               dtCore::ResourceDescriptor(),
               QIcon(nextIconFullPath.c_str()),
               "Up");

         std::string prevDir =
               dtUtil::FileUtils::GetInstance().GetAbsolutePath(mCurrentDir + "/..");
         aWidget->setCategoryFullName(prevDir.c_str());

         mListWidget->addItem(aWidget);
      }

      dtCore::RefPtr<dtCore::MapParser> parser = new dtCore::MapParser;

      for (size_t i = 0; i < dirFiles.size(); ++i)
      {
         nextFile = dirFiles[i];
         isFolder = false;

         //don't show any hidden directories
         if (dirFiles[i][0] == '.')
         {
            continue;
         }

         nextFileFullPath = mCurrentDir + "/" + nextFile;

         //determine what the icon is for this file:  ////////////////////////////////////////////         
         if (dtUtil::FileUtils::GetInstance().DirExists(nextFileFullPath))
         {
            //Don't want to see the icons folder
            if(dtUtil::FileUtils::GetInstance().IsSameFile(mCurrentDir, mTopPrefabDir) &&
                  nextFile == "icons")
            {
               continue;
            }

            //this file is a folder
            nextIconFullPath = UIResources::ICON_FOLDER;
            isFolder = true;
         }
         else //this should be a regular file
         {
            std::string ext = nextFile.substr(nextFile.rfind("."));
            std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
            if(ext != ".dtprefab")
            {
               //only doing files with .dtprefab extension
               continue;
            }

            try
            {
               std::string iconFileName = parser->ParseMapHeaderData(nextFileFullPath, true)->GetIconFile();

               nextIconFullPath = "";
               if (iconFileName != "")
               {
                  nextIconFullPath = iconDir + "/" + iconFileName;

                  if (! dtUtil::FileUtils::GetInstance().FileExists(nextIconFullPath))
                  {
                     nextIconFullPath = "";
                  }
               }

               if(nextIconFullPath == "")
               {
                  nextIconFullPath = UIResources::ICON_NO_ICON.c_str();
               }
            }
            catch (const dtUtil::Exception& ex)
            {
               nextIconFullPath = UIResources::ICON_NO_ICON.c_str();
            }
         }

         nextFile = nextFile.substr(0, nextFile.rfind(".dtprefab")); //truncate dtprefab extension
         //nextFile = nextFile.substr(0, 16);  //truncate to 16 characters

         //folders go to the front of the list
         if (isFolder)
         { 
            //If this is a folder, DON'T give this a ResourceDescriptor:
            ResourceListWidgetItem* aWidget = new ResourceListWidgetItem(
                  dtCore::ResourceDescriptor(),
                  QIcon(nextIconFullPath.c_str()), nextFile.c_str());
            aWidget->setCategoryFullName(nextFileFullPath.c_str());

            //want "Up a folder" to be first on all but the top level
            if(dtUtil::FileUtils::GetInstance().IsSameFile(mCurrentDir, mTopPrefabDir))
            {
               mListWidget->insertItem(0 + numCategories, aWidget);
            }
            else
            {
               mListWidget->insertItem(1 + numCategories, aWidget);
            }
            ++numCategories;
         }
         else 
         {
            ResourceListWidgetItem* aWidget = new ResourceListWidgetItem(
                  createResDescriptorFromPath(nextFileFullPath),
                  QIcon(nextIconFullPath.c_str()), nextFile.c_str());
            mListWidget->addItem(aWidget);
         }
      } // end for

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor PrefabBrowser::createResDescriptorFromPath(std::string path)
   {
      QString relPath = path.substr(dtCore::Project::GetInstance().GetContext().length() + 1).c_str();

      //remove redundant slashes first:
      QRegExp re("\\\\\\\\");
      while(relPath.contains(re))
      {
         relPath.replace(QRegExp("\\\\\\\\"), QString("/"));
      }
      re.setPattern("//");
      while(relPath.contains(re))
      {
         relPath.replace(QRegExp("//"), QString("/"));
      }

      relPath = relPath.replace(QRegExp("[\\\\/]"),QString(":"));

      dtCore::ResourceDescriptor resDesc(relPath.toStdString(), relPath.toStdString());

      return resDesc;
   }


} // namespace dtEditQt
