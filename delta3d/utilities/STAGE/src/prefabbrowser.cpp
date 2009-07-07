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

#include <dtEditQt/prefabbrowser.h>

#include <prefix/dtstageprefix-src.h>
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
#include <dtEditQt/resourcelistwidgetitem.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/uiresources.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/stagecamera.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/viewportcontainer.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtCore/globals.h>
#include <dtCore/scene.h>
#include <dtCore/object.h>
#include <dtActors/engineactorregistry.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>
#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/librarymanager.h>
#include <dtUtil/log.h>

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   PrefabBrowser::PrefabBrowser(QWidget* parent)
      : QWidget(parent)
      , mCurrentDir("")
      , mPopupMenu(this)    
   {
      setupGUI();

      connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
         this, SLOT(refreshPrefabs()));
      
      connect(&EditorActions::GetInstance(), SIGNAL(PrefabExported()),
         this, SLOT(refreshPrefabs()));
     
      mPopupMenu.addAction("Add Category", this, SLOT(addCategorySlot()));
      mPopupMenu.addAction("Add Prefab Icon");      
   }

   ///////////////////////////////////////////////////////////////////////////////
   PrefabBrowser::~PrefabBrowser()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::setupGUI()
   {
      // create our new object
      dtCore::RefPtr<dtDAL::ActorProxy> proxy =
         dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtActors", "Prefab");

      mPreviewObject = dynamic_cast<dtActors::PrefabActorProxy*>(proxy.get());
      dtCore::DeltaDrawable* actor = NULL;
      mPreviewObject->GetActor(actor);

      // Setup preview window.
      mPrefabScene = new dtCore::Scene();
      mPrefabScene->AddDrawable(actor);
      mCamera = new StageCamera();
      mCamera->makePerspective(60.0f,1.333f,0.1f,100000.0f);

      QSplitter* splitter = new QSplitter(Qt::Vertical, this);

      splitter->addWidget(previewGroup());
      splitter->addWidget(listGroup());

      splitter->setStretchFactor(0, 1);
      splitter->setStretchFactor(1, 1);

      // Setup the main grid.
      mGrid = new QGridLayout(this);
      mGrid->addWidget(splitter, 0, 0);
      mGrid->addLayout(buttonLayout(), 1, 0, Qt::AlignCenter);

      mGrid->setRowStretch(1, 0);

      refreshPrefabs();

      handleEnableCreateActorBtn();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::SetCameraLookAt()
   {
      // Get the bounding sphere of all the actors that belong to this prefab.
      dtCore::DeltaDrawable* prefab = NULL;
      mPreviewObject->GetActor(prefab);
      if (prefab)
      {
         osg::Vec3 min, max;

         int childCount = prefab->GetNumChildren();
         for (int childIndex = 0; childIndex < childCount; childIndex++)
         {
            dtCore::DeltaDrawable* child = prefab->GetChild(childIndex);
            if (child)
            {
               const osg::BoundingSphere& bs = child->GetOSGNode()->getBound();

               for (int axis = 0; axis < 3; axis++)
               {
                  if (min[axis] > bs.center()[axis] - bs.radius())
                  {
                     min[axis] = bs.center()[axis] - bs.radius();
                  }

                  if (max[axis] < bs.center()[axis] + bs.radius())
                  {
                     max[axis] = bs.center()[axis] + bs.radius();
                  }
               }
            }
         }

         osg::Vec3 center;
         float radius = 0.0f;

         for (int axis = 0; axis < 3; axis++)
         {
            center[axis] = min[axis] + ((max[axis] - min[axis]) * 0.5f);

            float testRadius = (max[axis] - min[axis]) * 0.5f;
            if (testRadius > radius)
            {
               radius = testRadius;
            }
         }

         dtCore::Transformable* tPrefab = dynamic_cast<dtCore::Transformable*>(prefab);
         if (tPrefab)
         {
            float offset = (radius < 1000.0f) ? radius : 0.0f;

            dtCore::Transform xform;
            tPrefab->GetTransform(xform);

            mCamera->resetRotation();
            osg::Vec3 viewDir = mCamera->getViewDir();

            osg::Vec3 translation;
            xform.GetTranslation(translation);
            if (offset > 0.0f)
            {
               mCamera->setPosition(translation + viewDir * offset * -2.0f);
            }
            else
            {
               mCamera->setPosition(translation + center);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::clearPrefabTree()
   {
      mListWidget->clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::displaySelection()
   {
/*
      ResourceTreeWidget* selection = getSelectedPrefabWidget();

      if (selection)
      {
         dtDAL::ResourceActorProperty* resourceProp = NULL;
         resourceProp = dynamic_cast<dtDAL::ResourceActorProperty*>(mPreviewObject->GetProperty("PrefabResource"));
         if (resourceProp)
         {
            dtDAL::ResourceDescriptor descriptor = dtDAL::ResourceDescriptor();
            if (selection->isResource())
            {
               descriptor = selection->getResourceDescriptor();
            }

            resourceProp->SetValue(&descriptor);
            SetCameraLookAt();

            mPerspView->refresh();
            mPerspView->setFocus();
         }
      }
*/
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
            return;
      }

      // disable the button if we got here.
      mCreatePrefabBtn->setEnabled(false);
      mCreateInstanceBtn->setEnabled(false);
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
      catch (dtUtil::Exception e)
      {
         QString reason("Unable to create category.\n This reason was given:\n\n");
         reason += e.ToString().c_str();
   
      	QMessageBox::critical(this, tr("Unable to Create Category"), reason, tr("OK"));

         return;
      }

      refreshPrefabs();      
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
   QGroupBox* PrefabBrowser::previewGroup()
   {
      QGroupBox* groupBox = new QGroupBox(tr("Preview"));

      QGridLayout* grid = new QGridLayout(groupBox);

      // New reference of the viewport manager singleton
      ViewportManager& vpMgr = ViewportManager::GetInstance();

      // Create the perspective viewport for the static mesh preview window
      mPerspView = (PerspectiveViewport*)vpMgr.createViewport("Preview",
         ViewportManager::ViewportType::PERSPECTIVE);

      // Assign the viewport a new scene
      mPerspView->setScene(mPrefabScene.get());

      //By default, perspective viewports have their camera set to the world view
      //camera.  The world view camera is what is used in the main perspective view.
      mPerspView->setCamera(mCamera.get());

      //No need for an overlay for this viewport since we cannot select meshes
      //in the preview window.
      mPerspView->setOverlay(NULL);

      // Disable the interaction modes
      mPerspView->setAutoInteractionMode(false);
      mPerspView->setAutoSceneUpdate(false);
      mPerspView->setEnableKeyBindings(false);

      // Create a viewport container for our static mesh window
      mContainer = new ViewportContainer(mPerspView, groupBox);
      mContainer->setViewport(mPerspView);

      grid->addWidget(mContainer, 0, 0);
      return groupBox;
   }

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* PrefabBrowser::listGroup()
   {
      QGroupBox*   groupBox = new QGroupBox(tr("Prefabs"));
      QGridLayout* grid     = new QGridLayout(groupBox);
      QHBoxLayout* hbox     = new QHBoxLayout();
      
      mListWidget = new ResourceDragListWidget(groupBox);
      mListWidget->setResourceName("Prefab");
      mListWidget->setAutoScroll(true);
      mListWidget->setViewMode(QListWidget::IconMode);
      mListWidget->setIconSize(QSize(40,40));
      mListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

      // connect signals
      connect(mListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(listSelectionChanged()));
      connect(mListWidget, SIGNAL(itemActivated(QListWidgetItem *)), this,
         SLOT(listSelectionDoubleClicked(QListWidgetItem*)));
      connect(mListWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this,
         SLOT(rightClickMenu(const QPoint &)));

      // Checkbox for auto preview
      mPreviewChk = new QCheckBox(tr("Auto Preview"), groupBox);
      connect(mPreviewChk, SIGNAL(stateChanged(int)), this, SLOT(checkBoxSelected()));
      mPreviewChk->setChecked(false);

      // Preview button for a selected mesh
      mPreviewBtn = new QPushButton("Preview", groupBox);
      connect(mPreviewBtn, SIGNAL(clicked()), this, SLOT(displaySelection()));
      mPreviewBtn->setDisabled(true);

      hbox->addWidget(mPreviewChk, 0, Qt::AlignLeft);
      hbox->addWidget(mPreviewBtn, 0, Qt::AlignRight);
      grid->addLayout(hbox, 0, 0);
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
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());

         dtDAL::ResourceDescriptor descriptor = selectedWidget->getResourceDescriptor();
         std::string fullPath = dtDAL::Project::GetInstance().GetResourcePath(descriptor);

         try
         {
            EditorData::GetInstance().getMainWindow()->startWaitCursor();
            EditorEvents::GetInstance().emitBeginChangeTransaction();

            std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > proxyList;
            dtCore::RefPtr<dtDAL::MapParser> parser = new dtDAL::MapParser;
            parser->ParsePrefab(fullPath, proxyList);

            // Auto select all of the proxies.
            ViewportOverlay::ActorProxyList selection = ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
            dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();
            ViewportOverlay* overlay = ViewportManager::GetInstance().getViewportOverlay();

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
               dtDAL::ActorProxy* proxy = const_cast<dtDAL::ActorProxy*>(itor->get());

               // Un-highlight the currently selected proxy.
               if (overlay->isActorSelected(proxy))
               {
                  overlay->removeActorFromCurrentSelection(proxy);
               }
            }

            osg::Vec3 offset = ViewportManager::GetInstance().getWorldViewCamera()->getPosition();

            int groupIndex = currMap->GetGroupCount();

            for (int proxyIndex = 0; proxyIndex < (int)proxyList.size(); proxyIndex++)
            {
               dtDAL::ActorProxy* proxy = proxyList[proxyIndex].get();

               currMap->AddProxy(*proxy);
               currMap->AddActorToGroup(groupIndex, proxy);

               // Notify the creation of the proxies.
               EditorEvents::GetInstance().emitActorProxyCreated(proxy, false);

               dtDAL::TransformableActorProxy* tProxy =
                  dynamic_cast<dtDAL::TransformableActorProxy*>(proxy);

               if (tProxy)
               {
                  if (proxyIndex == 0)
                  {
                     ViewportManager::GetInstance().placeProxyInFrontOfCamera(proxy);
                     
                     offset = tProxy->GetTranslation();
                  }
                  else
                  {
                     tProxy->SetTranslation(tProxy->GetTranslation() + offset);
                  }
               }
            }

            // Finally set the proxies in the prefab to be the current selection.
            ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(true);
            EditorEvents::GetInstance().emitActorsSelected(proxyList);
            EditorEvents::GetInstance().emitEndChangeTransaction();

            EditorData::GetInstance().getMainWindow()->endWaitCursor();
         }
         catch (const dtUtil::Exception& e)
         {
            LOG_ERROR(e.What());

            refreshPrefabs();

            EditorData::GetInstance().getMainWindow()->endWaitCursor();
         }
         fileUtils.PopDirectory();
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
         dtCore::RefPtr<dtDAL::ActorProxy> proxy =
            dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtActors", "Prefab");

         if (proxy.valid())
         {
            // add the new proxy to the map
            dtCore::RefPtr<dtDAL::Map> mapPtr = EditorData::GetInstance().getCurrentMap();
            if (mapPtr.valid())
            {
               mapPtr->AddProxy(*(proxy.get()));
            }

            // Set the prefab resource of the actor to the current prefab.
            dtDAL::ResourceActorProperty* resourceProp = NULL;
            resourceProp = dynamic_cast<dtDAL::ResourceActorProperty*>(proxy->GetProperty("PrefabResource"));
            if (resourceProp)
            {
               dtDAL::ResourceDescriptor descriptor = selectedWidget->getResourceDescriptor();               
               resourceProp->SetValue(&descriptor);
            }

            // let the world know that a new proxy exists
            EditorEvents::GetInstance().emitBeginChangeTransaction();
            EditorEvents::GetInstance().emitActorProxyCreated(proxy.get(), false);
            ViewportManager::GetInstance().placeProxyInFrontOfCamera(proxy.get());
            EditorEvents::GetInstance().emitEndChangeTransaction();

            // Now, let the world that it should select the new actor proxy.
            std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > actors;
            actors.push_back(proxy.get());
            EditorEvents::GetInstance().emitActorsSelected(actors);
         }

         EditorData::GetInstance().getMainWindow()->endWaitCursor();
      }
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

      std::string fullpath = selectedWidget->getCategoryFullName().toStdString();
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      if (fileUtils.DirExists(fullpath))
      {
         mCurrentDir = fullpath;
      }

      refreshPrefabs();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::rightClickMenu(const QPoint& clickPoint)
   {           
      mPopupMenu.exec(QCursor::pos());
   }


   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::checkBoxSelected()
   {
/*
      if (mPreviewChk->isChecked())
      {
         ResourceTreeWidget* selection = NULL;

         if (mTree != NULL)
         {
            QList <QTreeWidgetItem*> list = mTree->selectedItems();

            if (!list.isEmpty())
            {
               selection = dynamic_cast<ResourceTreeWidget*>(list[0]);
            }
            if (selection != NULL)
            {
               if (selection->isResource())
               {
                  EditorData::GetInstance().setCurrentPrefabResource(selection->getResourceDescriptor());
               }
               else
               {
                  EditorData::GetInstance().setCurrentPrefabResource(dtDAL::ResourceDescriptor());
               }
            }
         }

         if(selection != NULL)
         {
            if (selection->isResource())
            {
               displaySelection();
            }
         }
      }
*/
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::refreshPrefabs()
   {
      unsigned int numCategories = 0;

      // resets everything and marks the current expansion
      clearPrefabTree();

      dtDAL::Project& project = dtDAL::Project::GetInstance();

      // We can't do anything if we don't have a valid context.
      if (!project.IsContextValid())
      {
         return;
      }

      EditorData::GetInstance().getMainWindow()->startWaitCursor();

      project.Refresh();
      //project.GetResourcesOfType(dtDAL::DataType::PREFAB, mPrefabList);

      QIcon resourceIcon;
      resourceIcon.addPixmap(QPixmap(UIResources::ICON_ACTOR.c_str()));
      mResourceIcon = resourceIcon;

      QIcon folderIcon;
      folderIcon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()), QIcon::Normal, QIcon::On);
      folderIcon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()), QIcon::Normal, QIcon::Off);
      
      if (mCurrentDir == "")
      {
         mCurrentDir = dtEditQt::EditorData::GetInstance().getCurrentProjectContext()
                           + "/" + dtEditQt::EditorActions::PREFAB_DIRECTORY;
         mTopPrefabDir = dtEditQt::EditorData::GetInstance().getCurrentProjectContext()
            + "/" + dtEditQt::EditorActions::PREFAB_DIRECTORY;       
      }

      std::string contextDir = dtEditQt::EditorData::GetInstance().getCurrentProjectContext();
      std::string iconDir = contextDir + "/" +
                         dtEditQt::EditorActions::PREFAB_DIRECTORY + "/icons";

      dtUtil::DirectoryContents dirFiles = dtUtil::FileUtils::GetInstance().DirGetFiles(mCurrentDir);
      //std::sort(dirFiles.begin(), dirFiles.end());    

      std::string nextFile;
      std::string nextFileFullPath;
      std::string nextIconFullPath;
      bool isFolder = false;
      
      if (! dtUtil::FileUtils::GetInstance().IsSameFile(mCurrentDir, mTopPrefabDir))
      {
         //Show a "Go up a folder" icon
         nextIconFullPath = dtCore::GetDeltaRootPath() + "/utilities/STAGE/icons/upfolder_big.png";
         ResourceListWidgetItem* aWidget = new ResourceListWidgetItem(
                                           dtDAL::ResourceDescriptor(),
                                           QIcon(nextIconFullPath.c_str()),
                                           "Up");         

         std::string prevDir =
             dtUtil::FileUtils::GetInstance().GetAbsolutePath(mCurrentDir + "/..");
         aWidget->setCategoryFullName(prevDir.c_str());

         mListWidget->addItem(aWidget);
      }

      for (size_t i = 0; i < dirFiles.size(); ++i)
      {
         nextFile = dirFiles[i];
         isFolder = false;

         if (dirFiles[i] == "." || dirFiles[i] == "..")
         {
            continue;
         }

         nextFileFullPath = mCurrentDir + "/" + nextFile;

         //determine what the icon is for this file:  ////////////////////////////////////////////         
         if (dtUtil::FileUtils::GetInstance().DirExists(nextFileFullPath))
         {
            //Don't want to see the icons folder
            if(dtUtil::FileUtils::GetInstance().IsSameFile(mCurrentDir, mTopPrefabDir)
                && nextFile == "icons")
            {
               continue;
            }

            //this file is a folder
            nextIconFullPath = dtCore::GetDeltaRootPath() + "/utilities/STAGE/icons/folder.png";
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

            dtCore::RefPtr<dtDAL::MapParser> parser = new dtDAL::MapParser;            
            std::string iconFileName = parser->GetPrefabIconFileName(nextFileFullPath);            
            
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
               nextIconFullPath = dtCore::GetDeltaRootPath() + "/utilities/STAGE/icons/Icon_NoIcon64.png";
            }            
         }
         
         nextFile = nextFile.substr(0, nextFile.rfind(".dtprefab")); //truncate dtprefab extension         
         nextFile = nextFile.substr(0, 16);  //truncate to 16 characters         
         
         //folders go to the front of the list
         if (isFolder)
         { 
            //If this is a folder, DON'T give this a ResourceDescriptor:
            ResourceListWidgetItem* aWidget = new ResourceListWidgetItem(
               dtDAL::ResourceDescriptor(),                              
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
   dtDAL::ResourceDescriptor PrefabBrowser::createResDescriptorFromPath(std::string path)
   {
      QString relPath = path.substr(dtDAL::Project::GetInstance().GetContext().length() + 1).c_str();

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

      relPath = relPath.replace(QRegExp("[\\\\/]"),QString("::"));

      dtDAL::ResourceDescriptor resDesc(relPath.toStdString(), relPath.toStdString());

      return resDesc;
   }


} // namespace dtEditQt
