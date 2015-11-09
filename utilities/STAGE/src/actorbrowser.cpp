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
 * Curtiss Murphy
 */

#include <prefix/stageprefix.h>
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

#include <dtEditQt/actorbrowser.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/actortypetreewidget.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/mainwindow.h>
#include <dtCore/actorfactory.h>
#include <dtCore/map.h>
#include <dtUtil/log.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   ActorBrowser::ActorBrowser(QWidget* parent)
      : QWidget(parent)
      , mRootActorType(NULL)
      , mRootNodeWasExpanded(false)
   {
      setupGUI();
      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryImported()),
         this, SLOT(refreshActorTypes()));
      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryRemoved()),
         this, SLOT(refreshActorTypes()));
      connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
         this, SLOT(refreshActorTypes()));
      connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
         this, SLOT(refreshActorTypes()));
      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryAboutToBeRemoved()),
         this, SLOT(clearActorTypesTree()));
      connect(&EditorEvents::GetInstance(), SIGNAL(createActor()),
         this, SLOT(createActorPressed()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorBrowser::~ActorBrowser()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::setupGUI()
   {
      QGroupBox*   groupBox = new QGroupBox(tr("Actor Type"), this);
      QVBoxLayout* vBox     = new QVBoxLayout(groupBox);
      //vBox->setSpacing(2);
      //vBox->setMargin(3);

      // create root
      mTree = new ActorDragTree(groupBox);
      mTree->setColumnCount(1);
      connect(mTree, SIGNAL(itemSelectionChanged()), this, SLOT(treeSelectionChanged()));
      mTree->header()->hide();
      vBox->addWidget(mTree);

      mCreateActorBtn = new QPushButton(tr("Create Actor"), this);
      connect(mCreateActorBtn, SIGNAL(clicked()), this, SLOT(createActorPressed()));
      mSwitchActorTypeBtn = new QPushButton(tr("Switch Actor Type"), this);

      QHBoxLayout* btnLayout = new QHBoxLayout();
      btnLayout->addStretch(1);
      btnLayout->addWidget(mCreateActorBtn);
      btnLayout->addWidget(mSwitchActorTypeBtn);
      btnLayout->addStretch(1);

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox, 1);
      mainLayout->addLayout(btnLayout);


      reloadActors();

      handleEnableCreateActor();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::clearActorTypesTree()
   {
      // get the currently open tree branches and current caret position so we
      // can scroll back to it as best as we can later
      markCurrentExpansion();

      mRootActorType = NULL;
      mTree->clear();
      mActorTypes.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::reloadActors()
   {
      EditorData::GetInstance().getMainWindow()->startWaitCursor();

      // resets everything and marks the current expansion
      clearActorTypesTree();

      dtCore::ActorFactory::GetInstance().GetActorTypes(mActorTypes);

      // recreate our root.
      mRootActorType = new ActorTypeTreeWidget(mTree, tr("Actor Types"));

      // iterate through the actor types and create all the internal nodes.
      for (unsigned int i = 0; i < mActorTypes.size(); ++i)
      {
         if (mActorTypes[i] != NULL)
         {
            QString fullCategory(tr(mActorTypes[i]->GetCategory().c_str()));

            if (!fullCategory.isNull())
            {
               QStringList subCategories = fullCategory.split(tr(ActorTypeTreeWidget::CATEGORY_SEPARATOR.c_str()),
                  QString::SkipEmptyParts);
               QMutableStringListIterator* listIterator = new QMutableStringListIterator(subCategories);
               mRootActorType->recursivelyAddCategoryAndActorTypeAsChildren(listIterator, mActorTypes[i]);
               delete listIterator;
            }
         }
      }

      // Now, go back and try to re-expand items and restore our scroll position
      restorePreviousExpansion();

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::handleEnableCreateActor()
   {
      ActorTypeTreeWidget* selectedWidget = getSelectedActorTreeWidget();

      // if we have a leaf, then enable the button
      if (selectedWidget != NULL && selectedWidget->isLeafNode())
      {
         mCreateActorBtn->setEnabled(true);
      }
      // disable the button if we got here.
      else
      {
         mCreateActorBtn->setEnabled(false);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::HandleEnableSwitchActorType()
   {
      ActorTypeTreeWidget* selectedWidget = getSelectedActorTreeWidget();

      // if we have a leaf, then enable the button
      if (selectedWidget != NULL && selectedWidget->isLeafNode())
      {
         dtCore::ActorPtrVector toFill;
         EditorData::GetInstance().GetSelectedActors(toFill);

         mSwitchActorTypeBtn->setEnabled(!toFill.empty());
      }
      // disable the button if we got here.
      else
      {
         mSwitchActorTypeBtn->setEnabled(false);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::SwitchActorTypes()
   {
      LOG_INFO("User Created an Actor - Slot");
      dtEditQt::ActorTypeTreeWidget* selectedWidget = getSelectedActorTreeWidget();

      // if we have an actor type, then create the proxy and emit the signal
      if (selectedWidget != NULL && selectedWidget->getActorType() != NULL)
      {
         EditorData::GetInstance().getMainWindow()->startWaitCursor();

         dtCore::ActorPtrVector toFill;
         EditorData::GetInstance().GetSelectedActors(toFill);

         // add the new proxy to the map
         dtCore::RefPtr<dtCore::Map> mapPtr = EditorData::GetInstance().getCurrentMap();

         if (mapPtr.valid())
         {
            dtCore::ActorPtrVector::iterator i, iend;
            i = toFill.begin();
            iend = toFill.end();
            for (; i != iend; ++i)
            {
               dtCore::RefPtr<dtCore::BaseActorObject> oldActor = *i;

               // create our new object
               dtCore::RefPtr<dtCore::BaseActorObject> newActor =
                  dtCore::ActorFactory::GetInstance().CreateActor(*selectedWidget->getActorType()).get();

               if (newActor.valid())
               {
                  // let the world know that a new proxy exists
                  EditorEvents::GetInstance().emitBeginChangeTransaction();
                  EditorEvents::GetInstance().emitActorProxyAboutToBeDestroyed(oldActor);
                  EditorActions::GetInstance().RemoveActorFromMap(*oldActor, *mapPtr);
                  EditorEvents::GetInstance().emitActorProxyDestroyed(oldActor);

                  newActor->SetId(oldActor->GetId());
                  newActor->CopyPropertiesFrom(*oldActor);

                  EditorActions::GetInstance().AddActorToMap(*newActor, *mapPtr, true);
                  EditorEvents::GetInstance().emitActorProxyCreated(newActor, false);
                  ViewportManager::GetInstance().placeProxyInFrontOfCamera(newActor);
                  EditorEvents::GetInstance().emitEndChangeTransaction();

                  // Now, let the world that it should select the new actor proxy.
                  std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > actors;
                  actors.push_back(newActor.get());
                  EditorEvents::GetInstance().emitActorsSelected(actors);
               }
            }
         }

         EditorData::GetInstance().getMainWindow()->endWaitCursor();
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorTypeTreeWidget* ActorBrowser::getSelectedActorTreeWidget()
   {
      ActorTypeTreeWidget* returnVal = NULL;

      if (mTree != NULL)
      {
         QList<QTreeWidgetItem*> list = mTree->selectedItems();

         if (!list.isEmpty())
         {
            returnVal = dynamic_cast<ActorTypeTreeWidget*>(list[0]);
         }
      }

      return returnVal;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::markCurrentExpansion()
   {
      if (mTree != NULL && mRootActorType != NULL)
      {
         // we trap the root node separately to make the tree walking easier.
         mRootNodeWasExpanded = mTree->isItemExpanded(mRootActorType);

         // clear out previous marks
         mExpandedActorTypeNames.clear();

         // start recursion
         recurseMarkCurrentExpansion(mRootActorType, mExpandedActorTypeNames);

         // also store the last location of the scroll bar... so that they go back
         // to where they were next time.
         mLastScrollBarLocation = mTree->verticalScrollBar()->sliderPosition();
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::recurseMarkCurrentExpansion(ActorTypeTreeWidget* parent,
      dtUtil::tree<QString>& currentTree)
   {
      for (int i = 0; i < parent->childCount(); ++i)
      {
         ActorTypeTreeWidget* child = dynamic_cast<ActorTypeTreeWidget*>(parent->child(i));

         // if we have children, then we could potentially be expanded...
         if (child != NULL && child->childCount() > 0)
         {
            if (mTree->isItemExpanded(child))
            {
               // add it to our list
               dtUtil::tree<QString>& insertedItem = currentTree.
                  insert(child->getCategoryOrName()).tree_ref();

               // recurse on the child with the new tree
               recurseMarkCurrentExpansion(child, insertedItem);
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::restorePreviousExpansion()
   {
      if (mTree != NULL && mRootActorType != NULL)
      {
         // re-expand the root node separately to make the tree walking easier.
         if (mRootNodeWasExpanded)
         {
            mTree->expandItem(mRootActorType);
         }

         recurseRestorePreviousExpansion(mRootActorType, mExpandedActorTypeNames);

         // Put the scroll bar back where it was last time
         mTree->verticalScrollBar()->setSliderPosition(mLastScrollBarLocation);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::recurseRestorePreviousExpansion(ActorTypeTreeWidget* parent,
      dtUtil::tree<QString>& currentTree)
   {
      // walk through the children...
      for (dtUtil::tree<QString>::const_iterator iter = currentTree.in(); iter != currentTree.end(); ++iter)
      {
         QString name = (*iter);

         // Try to find a control with this name in our model
         for (int i = 0; i < parent->childCount(); ++i)
         {
            ActorTypeTreeWidget* child = dynamic_cast<ActorTypeTreeWidget*>(parent->child(i));
            // found a match!  expand it
            if (child->getCategoryOrName() == name)
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
   void ActorBrowser::createActorPressed()
   {
      LOGN_INFO("actorbrowser.cpp", "User Created an Actor - Slot");
      dtEditQt::ActorTypeTreeWidget* selectedWidget = getSelectedActorTreeWidget();

      // if we have an actor type, then create the proxy and emit the signal
      if (selectedWidget != NULL && selectedWidget->getActorType() != NULL)
      {
         EditorData::GetInstance().getMainWindow()->startWaitCursor();

         // create our new object
         dtCore::RefPtr<dtCore::BaseActorObject> proxy =
            dtCore::ActorFactory::GetInstance().CreateActor(*selectedWidget->getActorType()).get();

         if (proxy.valid())
         {
            // add the new proxy to the map
            dtCore::RefPtr<dtCore::Map> mapPtr = EditorData::GetInstance().getCurrentMap();
            if (mapPtr.valid())
            {
               EditorActions::GetInstance().AddActorToMap(*proxy, *mapPtr, true);
            }

            // let the world know that a new proxy exists
            EditorEvents::GetInstance().emitBeginChangeTransaction();
            EditorEvents::GetInstance().emitActorProxyCreated(proxy.get(), false);
            ViewportManager::GetInstance().placeProxyInFrontOfCamera(proxy.get());
            EditorEvents::GetInstance().emitEndChangeTransaction();

            // Now, let the world that it should select the new actor proxy.
            std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > actors;
            actors.push_back(proxy.get());
            EditorEvents::GetInstance().emitActorsSelected(actors);
         }

         EditorData::GetInstance().getMainWindow()->endWaitCursor();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::treeSelectionChanged()
   {
      handleEnableCreateActor();
      HandleEnableSwitchActorType();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorBrowser::refreshActorTypes()
   {
      reloadActors();
   }

} // namespace dtEditQt
