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

#include <prefix/dtstageprefix-src.h>
#include <QtGui/QDirModel>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>
#include <QtCore/QStringList>
#include <QtGui/QTreeView>
#include <QtGui/QTreeWidget>

#include <dtEditQt/prefabbrowser.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/actortypetreewidget.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/resourcetreewidget.h>
#include <dtEditQt/uiresources.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/stagecamera.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>
#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
#include <dtDAL/mapxml.h>
#include <dtUtil/log.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   PrefabBrowser::PrefabBrowser(QWidget* parent)
      : QWidget(parent)
      , mRootPrefabTree(NULL)
      , mRootNodeWasExpanded(false)
   {
      setupGUI();
      connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
         this, SLOT(refreshPrefabs()));
      
      connect(&EditorActions::GetInstance(), SIGNAL(PrefabExported()),
         this, SLOT(refreshPrefabs()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   PrefabBrowser::~PrefabBrowser()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::setupGUI()
   {
      QGroupBox*   groupBox = new QGroupBox(tr("Prefabs"), this);
      QVBoxLayout* vBox     = new QVBoxLayout(groupBox);
      //vBox->setSpacing(2);
      //vBox->setMargin(3);

      // create root
      mTree = new ResourceTree(groupBox);
      mTree->setColumnCount(1);
      mTree->header()->hide();
      connect(mTree, SIGNAL(itemSelectionChanged()), this, SLOT(treeSelectionChanged()));
      vBox->addWidget(mTree);

      mCreatePrefabBtn = new QPushButton(tr("Create Prefab"), this);
      connect(mCreatePrefabBtn, SIGNAL(clicked()), this, SLOT(createPrefabPressed()));

      mRefreshPrefabBtn = new QPushButton(tr("Refresh"), this);
      connect(mRefreshPrefabBtn, SIGNAL(clicked()), this, SLOT(refreshPrefabs()));

      QHBoxLayout* btnLayout = new QHBoxLayout();
      btnLayout->addStretch(1);
      btnLayout->addWidget(mCreatePrefabBtn);
      btnLayout->addWidget(mRefreshPrefabBtn);
      btnLayout->addStretch(1);

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox, 1);
      mainLayout->addLayout(btnLayout);

      refreshPrefabs();

      handleEnableCreateActor();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::clearPrefabTree()
   {
      markCurrentExpansion();

      mTree->clear();
      mPrefabList.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::handleEnableCreateActor()
   {
      ResourceTreeWidget* selectedWidget = getSelectedPrefabWidget();

      // if we have a leaf, then enable the button
      if (selectedWidget != NULL && selectedWidget->isResource())
      {
         mCreatePrefabBtn->setEnabled(true);
         return;
      }

      // disable the button if we got here.
      if (mCreatePrefabBtn != NULL)
      {
         mCreatePrefabBtn->setEnabled(false);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   ResourceTreeWidget* PrefabBrowser::getSelectedPrefabWidget()
   {
      ResourceTreeWidget* returnVal = NULL;

      if (mTree != NULL)
      {
         QList<QTreeWidgetItem*> list = mTree->selectedItems();

         if (!list.isEmpty())
         {
            returnVal = dynamic_cast<ResourceTreeWidget*>(list[0]);
         }
      }

      return returnVal;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::markCurrentExpansion()
   {
      if (mTree != NULL && mRootPrefabTree != NULL)
      {
         // we trap the root node separately to make the tree walking easier.
         mRootNodeWasExpanded = mTree->isItemExpanded(mRootPrefabTree);

         // clear out previous marks
         mExpandedActorTypeNames.clear();

         // start recursion
         recurseMarkCurrentExpansion(mRootPrefabTree, mExpandedActorTypeNames);

         // also store the last location of the scroll bar... so that they go back
         // to where they were next time.
         mLastScrollBarLocation = mTree->verticalScrollBar()->sliderPosition();
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::recurseMarkCurrentExpansion(ResourceTreeWidget* parent,
      dtUtil::tree<QString>& currentTree)
   {
      for (int i = 0; i < parent->childCount(); ++i)
      {
         ResourceTreeWidget* child = dynamic_cast<ResourceTreeWidget*>(parent->child(i));

         // if we have children, then we could potentially be expanded...
         if (child != NULL && child->childCount() > 0)
         {
            if (mTree->isItemExpanded(child))
            {
               // add it to our list
               dtUtil::tree<QString>& insertedItem = currentTree.
                  insert(child->getCategoryName()).tree_ref();

               // recurse on the child with the new tree
               recurseMarkCurrentExpansion(child, insertedItem);
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::restorePreviousExpansion()
   {
      if (mTree != NULL && mRootPrefabTree != NULL)
      {
         // re-expand the root node separately to make the tree walking easier.
         if (mRootNodeWasExpanded)
         {
            mTree->expandItem(mRootPrefabTree);
         }

         recurseRestorePreviousExpansion(mRootPrefabTree, mExpandedActorTypeNames);

         // Put the scroll bar back where it was last time
         mTree->verticalScrollBar()->setSliderPosition(mLastScrollBarLocation);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::recurseRestorePreviousExpansion(ResourceTreeWidget* parent,
      dtUtil::tree<QString>& currentTree)
   {
      // walk through the children...
      for (dtUtil::tree<QString>::const_iterator iter = currentTree.in(); iter != currentTree.end(); ++iter)
      {
         QString name = (*iter);

         // Try to find a control with this name in our model
         for (int i = 0; i < parent->childCount(); ++i)
         {
            ResourceTreeWidget* child = dynamic_cast<ResourceTreeWidget*>(parent->child(i));
            // found a match!  expand it
            if (child->getCategoryName() == name)
            {
               mTree->expandItem(child);

               // recurse over the children of this object
               recurseRestorePreviousExpansion(child, iter.tree_ref());
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   // SLOTS
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::createPrefabPressed()
   {
      //LOG_INFO("User Created an Actor - Slot");
      ResourceTreeWidget* selectedWidget = getSelectedPrefabWidget();

      if (selectedWidget && selectedWidget->isResource())
      {
         dtDAL::ResourceDescriptor descriptor = selectedWidget->getResourceDescriptor();
         std::string fullPath = dtDAL::Project::GetInstance().GetResourcePath(descriptor);

         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());
         try
         {
            EditorData::GetInstance().getMainWindow()->startWaitCursor();
            EditorEvents::GetInstance().emitBeginChangeTransaction();

            std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > proxyList;
            dtDAL::Map* map = EditorData::GetInstance().getCurrentMap();
            dtCore::RefPtr<dtDAL::MapParser> parser = new dtDAL::MapParser;
            parser->ParsePrefab(map, fullPath, proxyList);

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

            for (int proxyIndex = 0; proxyIndex < (int)proxyList.size(); proxyIndex++)
            {
               dtDAL::ActorProxy* proxy = proxyList[proxyIndex].get();

               // Notify the creation of the proxies.
               EditorEvents::GetInstance().emitActorProxyCreated(proxy, false);

               // Offset the position of all new proxies in the prefab.
               dtDAL::TransformableActorProxy* tProxy =
                  dynamic_cast<dtDAL::TransformableActorProxy*>(proxy);

               if (tProxy)
               {
                  osg::Vec3 pos = tProxy->GetTranslation();
                  pos += offset;
                  tProxy->SetTranslation(pos);
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
            //QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
            //   tr("Error"), QString(e.What().c_str()), tr("OK"));

            //slotRestartAutosave();
            //return;
         }
         fileUtils.PopDirectory();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::treeSelectionChanged()
   {
      handleEnableCreateActor();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabBrowser::refreshPrefabs()
   {
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
      project.GetResourcesOfType(dtDAL::DataType::PREFAB, mPrefabList);

      QIcon resourceIcon;
      resourceIcon.addPixmap(QPixmap(UIResources::ICON_ACTOR.c_str()));
      mResourceIcon = resourceIcon;

      QIcon folderIcon;
      folderIcon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()), QIcon::Normal, QIcon::On);
      folderIcon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()), QIcon::Normal, QIcon::Off);

      // recreate our root.
      mRootPrefabTree = new ResourceTreeWidget(mTree);
      mRootPrefabTree->setText(0, "Prefabs");
      mRootPrefabTree->recursivelyCreateResourceTree(mPrefabList, mResourceIcon);
      mRootPrefabTree->setIsResource(false);
      mRootPrefabTree->setIcon(0, folderIcon);

      // connect tree signals
      connect(mTree, SIGNAL(itemSelectionChanged()), this, SLOT(treeSelectionChanged()));

      // Now, go back and try to re-expand items and restore our scroll position
      restorePreviousExpansion();

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

} // namespace dtEditQt
