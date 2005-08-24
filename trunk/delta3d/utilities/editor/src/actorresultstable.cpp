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
* @author Curtiss Murphy
*/

#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QSize>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTreeView>
#include <QTreeWidgetItem>
#include "dtDAL/actorproxy.h"
#include "dtDAL/actortype.h"
#include "dtDAL/librarymanager.h"
#include <dtUtil/log.h>
#include "dtDAL/map.h"
#include "dtEditQt/actorresultstable.h"
#include "dtEditQt/editoractions.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/uiresources.h"

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    ActorResultsTable::ActorResultsTable(bool showActions, bool showGoto, QWidget *parent)
        :QWidget(parent), showActions(showActions), showGoto(showGoto),
        recurseProtectSendingSelection(false), recurseProtectEmitSelectionChanged(false)
    {
        // if we have a parent box, then we will use it later.
        parentBox = dynamic_cast<QGroupBox *>(parent);
        if (parentBox != NULL) 
        {
            parentBaseTitle = parentBox->title();
        }

        QVBoxLayout *boxLayout = new QVBoxLayout(this);

        // build our tree/table
        table = new ActorResultsSubTree(this, this);
        table->setAlternatingRowColors(true);
        //table->setEvenRowColor(QColor(237, 243, 254));
        //table->setOddRowColor(QColor(255, 255, 255));
        table->setRootIsDecorated(false);
        table->setSortingEnabled(true);
        table->setSelectionMode(QTreeView::MultiSelection);
        table->setSelectionBehavior(QTreeView::SelectRows);
        // set the headers
        QStringList headerLabels;
        headerLabels << "Name" << "Category" << "Type";
        table->setHeaderLabels(headerLabels);
        table->header()->setClickable(true);

        // Select button - this button was removed and now selection happens everytime an
        // object is clicked or the selection changes.  Why else would the user select an object anyway?
        //selectBtn = new QPushButton(tr("Select Actor(s)"), this);
        //selectBtn->setToolTip(tr("Select the marked actors in the PropertyEditor and Viewports"));
        //connect(selectBtn, SIGNAL(clicked()), this, SLOT(onSelectionChanged()));

        // goto button
        if (showGoto) 
        {
            gotoBtn = new QPushButton(/*tr("Goto")*/"", this);
            gotoBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_GOTO.c_str()));
            gotoBtn->setToolTip(tr("Goto the selected actor in all viewports."));
            connect(gotoBtn, SIGNAL(clicked()), this, SLOT(gotoPressed()));
        } 
        else 
        {
            gotoBtn = NULL;
        }

        // duplicate button
        dupBtn = new QPushButton(""/*tr("Duplicate")*/, this);
        dupBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_DUPLICATE.c_str()));
        dupBtn->setToolTip(tr("Duplicate the selected actor(s)."));
        connect(dupBtn, SIGNAL(clicked()), this, SLOT(duplicatePressed()));

        // delete button
        deleteBtn = new QPushButton(""/*tr("Delete")*/, this);
        deleteBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_DELETE.c_str()));
        deleteBtn->setToolTip(tr("Delete the marked actor(s)"));
        connect(deleteBtn, SIGNAL(clicked()), this, SLOT(deletePressed()));

        // build up the button layout
        QHBoxLayout *hBox = new QHBoxLayout();
        hBox->addStretch(1);
        //hBox->addWidget(selectBtn);
        if (showGoto) 
        {
            hBox->addWidget(gotoBtn);
            hBox->addSpacing(2);
        }
        hBox->addWidget(dupBtn);
        hBox->addSpacing(2);
        hBox->addWidget(deleteBtn);
        hBox->addStretch(1);


        // add the controls  to the main layout
        boxLayout->addWidget(table, 1, 0);
        // only add the buttons if we're supposed to.
        if (showActions) 
        {
            boxLayout->addLayout(hBox);
        }

        // connect all our signals
        connect(table, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
        //connect(table, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
        //    this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));

        connect(&EditorEvents::getInstance(), SIGNAL(mapLibraryImported()),
            this, SLOT(clearAll()));
        // Remove search items that are being destroyed
        connect(&EditorEvents::getInstance(), SIGNAL(actorProxyAboutToBeDestroyed(proxyRefPtr)),
            this, SLOT(actorProxyAboutToBeDestroyed(proxyRefPtr)));
        connect(&EditorEvents::getInstance(), SIGNAL(selectedActors(proxyRefPtrVector &)),
            this, SLOT(selectedActors(proxyRefPtrVector &)));

        // make sure buttons and count are correct on start up
        updateResultsCount();
        doEnableButtons();
    }

    ///////////////////////////////////////////////////////////////////////////////
    ActorResultsTable::~ActorResultsTable()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::updateResultsCount() 
    {
        QString newTitle;

        if (parentBox != NULL) 
        {
            int count = table->topLevelItemCount();

            // add the count if it's greater than 0 and account for the 's'.
            if (count > 0) 
            {
                newTitle = parentBaseTitle + " (" + QString::number(count) + " Actor" +
                    ((count > 1) ? "s" : "") + ")";
            } 
            else 
            {
                newTitle = parentBaseTitle;
            }

            parentBox->setTitle(newTitle);
        }

    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::addProxies(std::vector<osg::ref_ptr<dtDAL::ActorProxy> > foundProxies)
    {
        std::vector<osg::ref_ptr<dtDAL::ActorProxy > >::const_iterator iter;
        int row = 0;

        // do something with the results
        for(iter = foundProxies.begin(); iter != foundProxies.end(); ++iter)
        {
            osg::ref_ptr<dtDAL::ActorProxy> myProxy = (*iter);

            addProxy(myProxy, false);

            //selectedActors.push_back(myProxy);
            row ++;
        }

        updateResultsCount();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::addProxy(osg::ref_ptr<dtDAL::ActorProxy> myProxy, bool updateCount)
    {
        QString name(myProxy->GetName().c_str());
        QString type(myProxy->GetActorType().GetName().c_str());
        QString category(myProxy->GetActorType().GetCategory().c_str());

        // create the tree entry
        ActorResultsTreeItem *item = new ActorResultsTreeItem(table, myProxy);
        item->setText(0, name);
        item->setText(1, category);
        item->setText(2, type);

        if (updateCount)
            updateResultsCount();
    }

    ///////////////////////////////////////////////////////////////////////////////
    ActorResultsTreeItem *ActorResultsTable::getSelectedResultTreeWidget()
    {
        ActorResultsTreeItem *returnVal = NULL;

        if (table != NULL) 
        {
            QList<QTreeWidgetItem *> list = table->selectedItems();

            if (!list.isEmpty()) 
            {
                returnVal = dynamic_cast<ActorResultsTreeItem*>(list[0]);
            }
        }

        return returnVal;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::doEnableButtons() {
        QList<QTreeWidgetItem *> list = table->selectedItems();

        // goto Button only works with one.
        if (showGoto) 
        {
            gotoBtn->setDisabled(list.size() != 1);
        }

        deleteBtn->setDisabled(list.size() == 0);
        dupBtn->setDisabled(list.size() == 0);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // SLOTS
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::clearAll()
    {
        table->clear();
        doEnableButtons();
        updateResultsCount();
    }

    ///////////////////////////////////////////////////////////////////////////////
    //void ActorResultsTable::itemDoubleClicked(QTreeWidgetItem *selectedItem, int column)
    //{
    //    // get the proxy, wrap it in an osg_ptr, and stick it in a vector.
    //    ActorResultsTreeItem *item = static_cast<ActorResultsTreeItem*>(selectedItem);
    //    std::vector<osg::ref_ptr<dtDAL::ActorProxy> > proxyVector;
    //    osg::ref_ptr<dtDAL::ActorProxy> proxyPtr = item->getProxy();
    //    proxyVector.push_back(proxyPtr);

    //    // tell the world to select it
    //    EditorEvents::getInstance().emitActorsSelected(proxyVector);
    //}

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::gotoPressed()
    {
        ActorResultsTreeItem *selection = getSelectedResultTreeWidget();

        if (selection != NULL) 
        {
            osg::ref_ptr<dtDAL::ActorProxy> proxyPtr = selection->getProxy();

            // Make sure we are in sync so that we goto the right object.
            sendSelection();

            // now tell the viewports to goto that actor
            EditorEvents::getInstance().emitGotoActor(proxyPtr);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::actorProxyAboutToBeDestroyed(osg::ref_ptr<dtDAL::ActorProxy> proxy)
    {
        QTreeWidgetItem *item;
        int index = 0;

        // iterate through our top level items until we have no more.
        while (NULL != (item = table->topLevelItem(index))) 
        {
            ActorResultsTreeItem *treeItem = static_cast<ActorResultsTreeItem *>(item);

            if (proxy == treeItem->getProxy()) 
            {
                table->takeTopLevelItem(index);
                updateResultsCount();
                doEnableButtons();
                break;  // we're done
            }

            index ++;
        }

        // NOTE - it is very likely that a delete operation also sends a selection event.  It's
        // supposed to.  So we should not have to handle our selection separately.  We'll get
        // an event.

    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::sendSelection()
    {
        if (!recurseProtectSendingSelection) 
        {
            QList<QTreeWidgetItem *> list = table->selectedItems();
            QListIterator<QTreeWidgetItem *> iter(list);
            std::vector<osg::ref_ptr<dtDAL::ActorProxy> > proxyVector;

            // move the objects to a vector for the message
            while (iter.hasNext()) 
            {
                ActorResultsTreeItem *item = static_cast<ActorResultsTreeItem*>(iter.next());
                osg::ref_ptr<dtDAL::ActorProxy> proxyPtr = item->getProxy();
                proxyVector.push_back(proxyPtr);
            }

            // tell the world to select these items - handle several recursive cases
            recurseProtectSendingSelection = true;
            if (!recurseProtectEmitSelectionChanged) 
            {
                EditorEvents::getInstance().emitActorsSelected(proxyVector);
            }
            recurseProtectSendingSelection = false;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::onSelectionChanged()
    {
        if (!recurseProtectSendingSelection) 
        {
            // always change the real selection whenever our list selection changes
            sendSelection();
            doEnableButtons();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::selectedActors(std::vector<osg::ref_ptr<dtDAL::ActorProxy> > &actors)
    {
        QTreeWidgetItem *item;
        int index = 0;
        //QTreeWidgetItem *currentItem = table->currentItem();

        if (!recurseProtectSendingSelection) 
        {
            // if we get a selection event, just clear our selection.  We're not going to
            // keep our selection in sync, since it's technically impossible.  Our list may
            // not contain all the items, in which case the user is left in an ambiguous state
            // So, just clear our selection and prevent any possible confusion.  No actions can
            // occur with no selections, so...
            //
            // Also, protect from recursive issues.
            recurseProtectEmitSelectionChanged = true;

            // clear any selections - Yes, there is a clearSelection() method, but that method also
            // resets the current item, which causes wierd keyboard focus issues that will resend
            // a selection event sometimes or cause the selection to flicker...  it's sloppy.  So,
            // the easiest thing to do was just unselect items one at a time.
            while (NULL != (item = table->topLevelItem(index))) 
            {
                if (table->isItemSelected(item)) 
                {
                    table->setItemSelected(item, false);
                }
                index ++;
            }

            recurseProtectEmitSelectionChanged = false;

            doEnableButtons();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::deletePressed()
    {
        // absolutely, positively, guarantee that the our selection is the same as the
        // rest of the system. If out of sync, the user deletes the WRONG objects!!!
        sendSelection();

        // Protect from recursive issues.
        recurseProtectEmitSelectionChanged = true;

        // delete the currently selected actors
        EditorActions::getInstance().slotEditDeleteActors();

        recurseProtectEmitSelectionChanged = false;

        updateResultsCount();
        doEnableButtons();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorResultsTable::duplicatePressed()
    {
        // absolutely, positively, guarantee that the our selection is the same as the
        // rest of the system. If out of sync, the user duplicates the WRONG objects!!!
        sendSelection();

        // Protect from recursive issues.
        recurseProtectEmitSelectionChanged = true;

        // duplicate the currently selected actors
        EditorActions::getInstance().slotEditDuplicateActors();

        recurseProtectEmitSelectionChanged = false;

        updateResultsCount();
        doEnableButtons();
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
    ActorResultsTreeItem::ActorResultsTreeItem(QTreeWidget *parent,
            osg::ref_ptr<dtDAL::ActorProxy> proxy)
        :QTreeWidgetItem(parent), myProxy(proxy)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    ActorResultsTreeItem::~ActorResultsTreeItem()
    {
    }



}
