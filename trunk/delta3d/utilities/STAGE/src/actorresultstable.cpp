/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 *
 * Curtiss Murphy
 */
#include <prefix/stageprefix.h>
#include <QtCore/QStringList>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <dtEditQt/actorresultstable.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/uiresources.h>
#include <dtCore/map.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   ActorResultsTable::ActorResultsTable(bool showActions, bool showGoto, QWidget* parent)
      : QWidget(parent)
      , mShowActions(showActions)
      , mShowGoto(showGoto)
   {
      // if we have a parent box, then we will use it later.
      mParentBox = dynamic_cast<QGroupBox*>(parent);
      if (mParentBox != NULL)
      {
         mParentBaseTitle = mParentBox->title();
      }

      QVBoxLayout* boxLayout = new QVBoxLayout(this);

      // build our tree/table
      mResultsTree = new QTreeWidget(this);
      mResultsTree->setAlternatingRowColors(true);
      mResultsTree->setRootIsDecorated(false);
      mResultsTree->setSortingEnabled(true);
      mResultsTree->sortItems(0, Qt::AscendingOrder);
      mResultsTree->setSelectionMode(QTreeView::ExtendedSelection);
      mResultsTree->setSelectionBehavior(QTreeView::SelectRows);
      connect(mResultsTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(gotoPressed()));
      // set the headers
      QStringList headerLabels;
      headerLabels << "Name" << "Category" << "Type";
      mResultsTree->setHeaderLabels(headerLabels);
      mResultsTree->header()->setClickable(true);

      // Select button - this button was removed and now selection happens everytime an
      // object is clicked or the selection changes.  Why else would the user select an object anyway?
      //selectBtn = new QPushButton(tr("Select Actor(s)"), this);
      //selectBtn->setToolTip(tr("Select the marked actors in the PropertyEditor and Viewports"));
      //connect(selectBtn, SIGNAL(clicked()), this, SLOT(onSelectionChanged()));

      // goto button
      if (showGoto)
      {
         mGotoBtn = new QPushButton(/*tr("Goto")*/"", this);
         mGotoBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_GOTO.c_str()));
         mGotoBtn->setToolTip(tr("Goto the selected actor in all viewports."));
         connect(mGotoBtn, SIGNAL(clicked()), this, SLOT(gotoPressed()));
      }
      else
      {
         mGotoBtn = NULL;
      }

      // duplicate button
      mDupBtn = new QPushButton(""/*tr("Duplicate")*/, this);
      mDupBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_DUPLICATE.c_str()));
      mDupBtn->setToolTip(tr("Duplicate the selected actor(s)."));
      connect(mDupBtn, SIGNAL(clicked()), this, SLOT(duplicatePressed()));

      // delete button
      mDeleteBtn = new QPushButton(""/*tr("Delete")*/, this);
      mDeleteBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_DELETE.c_str()));
      mDeleteBtn->setToolTip(tr("Delete the marked actor(s)"));
      connect(mDeleteBtn, SIGNAL(clicked()), this, SLOT(deletePressed()));

      // build up the button layout
      QHBoxLayout* hBox = new QHBoxLayout();
      hBox->addStretch(1);
      //hBox->addWidget(selectBtn);
      if (showGoto)
      {
         hBox->addWidget(mGotoBtn);
         hBox->addSpacing(2);
      }
      hBox->addWidget(mDupBtn);
      hBox->addSpacing(2);
      hBox->addWidget(mDeleteBtn);
      hBox->addStretch(1);


      // add the controls  to the main layout
      boxLayout->addWidget(mResultsTree, 1, 0);
      // only add the buttons if we're supposed to.
      if (showActions)
      {
         boxLayout->addLayout(hBox);
      }

      // connect all our signals
      connect(mResultsTree, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));

      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryImported()),
         this, SLOT(clearAll()));
      // Remove search items that are being destroyed
      connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyAboutToBeDestroyed(dtCore::ActorPtr)),
         this, SLOT(actorProxyAboutToBeDestroyed(dtCore::ActorPtr)));
      connect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorRefPtrVector &)),
         this, SLOT(selectedActors(ActorRefPtrVector &)));

      QAction* copySelect = new QAction(tr("&Copy Selection"), this);
      copySelect->setStatusTip(tr("Copy the names of the selected actors."));
      connect(copySelect, SIGNAL(triggered()), this, SLOT(ClipboardCopySelectedItems()));
      QMenu menu(this);
      mContextMenu.addAction(copySelect);

      // make sure buttons and count are correct on start up
      updateResultsCount();
      doEnableButtons();
   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorResultsTable::~ActorResultsTable()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::addProxies(std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > foundProxies)
   {
      std::vector< dtCore::RefPtr<dtCore::BaseActorObject > >::const_iterator iter;
      int row = 0;

      // do something with the results
      for (iter = foundProxies.begin(); iter != foundProxies.end(); ++iter)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> myProxy = (*iter);

         addProxy(myProxy, false);

         //selectedActors.push_back(myProxy);
         row ++;
      }

      updateResultsCount();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::addProxy(dtCore::RefPtr<dtCore::BaseActorObject> myProxy, bool updateCount)
   {
      QString name(myProxy->GetName().c_str());
      QString type(myProxy->GetActorType().GetName().c_str());
      QString category(myProxy->GetActorType().GetCategory().c_str());

      // create the tree entry
      ActorResultsTreeItem* item = new ActorResultsTreeItem(mResultsTree, myProxy);
      item->setText(0, name);
      item->setText(1, category);
      item->setText(2, type);

      if (updateCount)
      {
         updateResultsCount();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::HandleProxyUpdated(dtCore::RefPtr<dtCore::BaseActorObject> proxy)
   {
      if (mResultsTree != NULL && proxy.valid())
      {
         QTreeWidgetItem* item;
         int index = 0;

         // Iterate through the items in our list and find a match. If we find the
         // matching proxy, then update it's 3 fields
         while (NULL != (item = mResultsTree->topLevelItem(index)))
         {
            ActorResultsTreeItem* treeItem = static_cast<ActorResultsTreeItem*>(item);
            if (proxy == treeItem->GetActor())
            {
               QString name(proxy->GetName().c_str());
               QString type(proxy->GetActorType().GetName().c_str());
               QString category(proxy->GetActorType().GetCategory().c_str());

               treeItem->setText(0, name);
               treeItem->setText(1, category);
               treeItem->setText(2, type);
            }

            index ++;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::doEnableButtons()
   {
      QList<QTreeWidgetItem*> list = mResultsTree->selectedItems();

      // goto Button only works with one.
      if (mShowGoto)
      {
         mGotoBtn->setDisabled(list.size() != 1);
      }

      mDeleteBtn->setDisabled(list.size() == 0);
      mDupBtn->setDisabled(list.size()    == 0);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::UnselectAllItemsManually(QTreeWidgetItem* keepSelectedItem)
   {
      QTreeWidgetItem* item;
      int index = 0;

      // clear any selections - Yes, there is a clearSelection() method, but that method also
      // resets the current item, which causes weird keyboard focus issues that will resend
      // a selection event sometimes or cause the selection to flicker...  it's sloppy.  So,
      // the easiest thing to do was just unselect items one at a time.
      while (NULL != (item = mResultsTree->topLevelItem(index)))
      {
         if (item != keepSelectedItem && mResultsTree->isItemSelected(item))
         {
            mResultsTree->setItemSelected(item, false);
         }
         ++index;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::UniqueId> ActorResultsTable::getSelectedItems()
   {
      std::vector<dtCore::UniqueId> resultList;
      QList<QTreeWidgetItem*> itemList = mResultsTree->selectedItems();

      int count = itemList.count();
      for (int index = 0; index < count; ++index)
      {
         ActorResultsTreeItem* item = dynamic_cast<ActorResultsTreeItem*>(itemList[index]);
         if (item)
         {
            dtCore::RefPtr<dtCore::BaseActorObject> proxy = item->GetActor();
            if (proxy.valid())
            {
               resultList.push_back(proxy->GetId());
            }
         }
      }

      return resultList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::ClipboardCopySelectedItems()
   {
       QList<QTreeWidgetItem*> itemList = mResultsTree->selectedItems();

       QClipboard* clipboard = QApplication::clipboard();
       QString textData;

       int count = itemList.count();
       for (int index = 0; index < count; ++index)
       {
          ActorResultsTreeItem* item = dynamic_cast<ActorResultsTreeItem*>(itemList[index]);
          if (item)
          {
             dtCore::RefPtr<dtCore::BaseActorObject> actor = item->GetActor();
             if (actor.valid())
             {
                if (index > 0)
                   textData.append(QString("\n"));
                textData.append(QString(actor->GetName().c_str()));
             }
          }
       }
       clipboard->setText(textData);
    }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::setSelectedItems(const std::vector<dtCore::UniqueId>& items)
   {
      // Also, protect from recursive issues.
      UnselectAllItemsManually(NULL);

      // Now select the same items as our selection.
      int selectionCount = (int)items.size();
      for (int selectionIndex = 0; selectionIndex < selectionCount; ++selectionIndex)
      {
         dtCore::UniqueId proxyId = items[selectionIndex];

         int itemCount = (int)mResultsTree->topLevelItemCount();
         for (int itemIndex = 0; itemIndex < itemCount; ++itemIndex)
         {
            ActorResultsTreeItem* item = dynamic_cast<ActorResultsTreeItem*>(mResultsTree->topLevelItem(itemIndex));
            if (item)
            {
               dtCore::RefPtr<dtCore::BaseActorObject> proxy = item->GetActor();
               if (proxy.valid() && proxy->GetId() == proxyId)
               {
                  mResultsTree->setItemSelected(item, true);
                  break;
               }
            }
         }
      }

      doEnableButtons();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::contextMenuEvent(QContextMenuEvent* event)
   {
      if (mDeleteBtn->isEnabled())
         mContextMenu.exec(event->globalPos());
   }
   ///////////////////////////////////////////////////////////////////////////////
   // SLOTS
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::clearAll()
   {
      mResultsTree->clear();
      doEnableButtons();
      updateResultsCount();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::sendSelection()
   {
      dtCore::Map* map = EditorData::GetInstance().getCurrentMap();
      QList<QTreeWidgetItem*> list = mResultsTree->selectedItems();
      QListIterator<QTreeWidgetItem*> iter(list);
      dtCore::ActorRefPtrVector actors;

      // move the objects to a vector for the message
      while (iter.hasNext())
      {
         ActorResultsTreeItem* item = static_cast<ActorResultsTreeItem*>(iter.next());
         dtCore::ActorPtr actor = item->GetActor();
         actors.push_back(actor);

         // Also select all other proxies that belong to its group.
         if (map)
         {
            int groupIndex = map->FindGroupForActor(*actor);
            if (groupIndex > -1)
            {
               int actorCount = map->GetGroupActorCount(groupIndex);
               for (int actorIndex = 0; actorIndex < actorCount; actorIndex++)
               {
                  dtCore::ActorPtr curActor = map->GetActorFromGroup(groupIndex, actorIndex);
                  if (curActor != actor)
                  {
                     actors.push_back(curActor);
                  }
               }
            }
         }
      }

      // tell the world to select these items - handle several recursive cases
      mResultsTree->blockSignals(true);
      EditorEvents::GetInstance().emitActorsSelected(actors);
      mResultsTree->blockSignals(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::gotoPressed()
   {
      ActorResultsTreeItem* selection = getSelectedResultTreeWidget();

      if (selection != NULL)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> proxyPtr = selection->GetActor();

         // Make sure we are in sync so that we goto the right object.
         sendSelection();

         // now tell the viewports to goto that actor
         EditorEvents::GetInstance().emitGotoActor(proxyPtr);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::duplicatePressed()
   {
      // absolutely, positively, guarantee that the our selection is the same as the
      // rest of the system. If out of sync, the user duplicates the WRONG objects!!!
      sendSelection();

      mResultsTree->blockSignals(true);

      // Go ahead and unselect all items now. That prevents a wierd recursive event effect.
      UnselectAllItemsManually(NULL);

      // duplicate the currently selected actors
      EditorActions::GetInstance().slotEditDuplicateActors();

      mResultsTree->blockSignals(false);

      updateResultsCount();
      doEnableButtons();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::deletePressed()
   {
      // absolutely, positively, guarantee that the our selection is the same as the
      // rest of the system. If out of sync, the user deletes the WRONG objects!!!
      sendSelection();

      // Protect from recursive issues.
      mResultsTree->blockSignals(true);

      // delete the currently selected actors
      EditorActions::GetInstance().slotEditDeleteActors();

      mResultsTree->blockSignals(false);

      updateResultsCount();
      doEnableButtons();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::actorProxyAboutToBeDestroyed(dtCore::RefPtr<dtCore::BaseActorObject> proxy)
   {
      QTreeWidgetItem* item;
      int index = 0;

      // iterate through our top level items until we have no more.
      while (NULL != (item = mResultsTree->topLevelItem(index)))
      {
         ActorResultsTreeItem* treeItem = static_cast<ActorResultsTreeItem*>(item);

         if (proxy == treeItem->GetActor())
         {
            mResultsTree->takeTopLevelItem(index);
            updateResultsCount();
            doEnableButtons();
            break;  // we're done
         }

         ++index;
      }

      // NOTE - it is very likely that a delete operation also sends a selection event.  It's
      // supposed to.  So we should not have to handle our selection separately.  We'll get
      // an event.
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::onSelectionChanged()
   {
      // always change the real selection whenever our list selection changes
      sendSelection();
      doEnableButtons();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::selectedActors(std::vector< dtCore::RefPtr<dtCore::BaseActorObject> >& actors)
   {
      // Also, protect from recursive issues.
      mResultsTree->blockSignals(true);
      UnselectAllItemsManually(NULL);

      // Now select the same items as our selection.
      int selectionCount = (int)actors.size();
      for (int selectionIndex = 0; selectionIndex < selectionCount; ++selectionIndex)
      {
         dtCore::BaseActorObject* actor = actors[selectionIndex].get();

         int listCount = mResultsTree->topLevelItemCount();
         for (int listIndex = 0; listIndex < listCount; ++listIndex)
         {
            ActorResultsTreeItem* item = static_cast<ActorResultsTreeItem*>(mResultsTree->topLevelItem(listIndex));
            dtCore::BaseActorObject* listProxy = item->GetActor().get();
            if (listProxy == actor)
            {
               mResultsTree->setItemSelected(item, true);
            }
         }
      }
      mResultsTree->blockSignals(false);

      doEnableButtons();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::keyPressEvent(QKeyEvent* event)
   {
      switch(event->key())
      {
      case Qt::Key_Delete:
         {
            deletePressed();
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorResultsTreeItem* ActorResultsTable::getSelectedResultTreeWidget()
   {
      ActorResultsTreeItem* returnVal = NULL;

      if (mResultsTree != NULL)
      {
         QList<QTreeWidgetItem*> list = mResultsTree->selectedItems();

         if (!list.isEmpty())
         {
            returnVal = dynamic_cast<ActorResultsTreeItem*>(list[0]);
         }
      }

      return returnVal;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorResultsTable::updateResultsCount()
   {
      QString newTitle;

      if (mParentBox != NULL)
      {
         int count = mResultsTree->topLevelItemCount();

         // add the count if it's greater than 0 and account for the 's'.
         if (count > 0)
         {
            newTitle = mParentBaseTitle + " (" + QString::number(count) + " Actor" +
               ((count > 1) ? "s" : "") + ")";
         }
         else
         {
            newTitle = mParentBaseTitle;
         }

         mParentBox->setTitle(newTitle);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   //
   // ActorResultsTreeItem
   //
   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   ActorResultsTreeItem::ActorResultsTreeItem(QTreeWidget* parent,
      dtCore::RefPtr<dtCore::BaseActorObject> actor)
      : QTreeWidgetItem(parent)
      , mActor(actor)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorResultsTreeItem::~ActorResultsTreeItem()
   {
   }

} // namespace dtEditQt
