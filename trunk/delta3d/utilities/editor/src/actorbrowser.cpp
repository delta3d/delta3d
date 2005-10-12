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

#include <QDirModel>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStringList>
#include <QTreeView>
#include <QTreeWidget>

#include "dtEditQt/actorbrowser.h"
#include "dtEditQt/viewportmanager.h"
#include "dtDAL/librarymanager.h"
#include <dtUtil/log.h>
#include "dtDAL/map.h"
#include "dtEditQt/actortypetreewidget.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/mainwindow.h"

namespace dtEditQt
{

    ///////////////////////////////////////////////////////////////////////////////
    ActorBrowser::ActorBrowser(QWidget *parent)
        :QWidget(parent), rootActorType(NULL), rootNodeWasExpanded(false)
    {
        setupGUI();
        connect(&EditorEvents::getInstance(), SIGNAL(mapLibraryImported()),
            this, SLOT(refreshActorTypes()));
        connect(&EditorEvents::getInstance(), SIGNAL(mapLibraryRemoved()),
            this, SLOT(refreshActorTypes()));
        connect(&EditorEvents::getInstance(), SIGNAL(currentMapChanged()),
            this, SLOT(refreshActorTypes()));
        connect(&EditorEvents::getInstance(), SIGNAL(projectChanged()),
            this, SLOT(refreshActorTypes()));
        connect(&EditorEvents::getInstance(), SIGNAL(mapLibraryAboutToBeRemoved()),
            this, SLOT(clearActorTypesTree()));
    }

    ///////////////////////////////////////////////////////////////////////////////
    ActorBrowser::~ActorBrowser()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorBrowser::setupGUI()
    {
        QGroupBox *groupBox = new QGroupBox(tr("Actor Type"), this);
        QVBoxLayout *vBox = new QVBoxLayout(groupBox);
        //vBox->setSpacing(2);
        //vBox->setMargin(3);

        // create root
        tree = new QTreeWidget(groupBox);
        tree->setColumnCount(1);
        connect(tree, SIGNAL(itemSelectionChanged()), this, SLOT(treeSelectionChanged()));
        tree->header()->hide();
        vBox->addWidget(tree);

        createActorBtn = new QPushButton(tr("Create Actor"), this);
        connect(createActorBtn, SIGNAL(clicked()), this, SLOT(createActorPressed()));

        QHBoxLayout *btnLayout = new QHBoxLayout();
        btnLayout->addStretch(1);
        btnLayout->addWidget(createActorBtn);
        btnLayout->addStretch(1);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
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

        rootActorType = NULL;
        tree->clear();
        actorTypes.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorBrowser::reloadActors()
    {
        EditorData::getInstance().getMainWindow()->startWaitCursor();

        // resets everything and marks the current expansion
        clearActorTypesTree();

        dtDAL::LibraryManager::GetInstance().GetActorTypes(actorTypes);

        // recreate our root.
        rootActorType = new ActorTypeTreeWidget(tree, tr("Actor Types"));

        // iterate through the actor types and create all the internal nodes.
        for(unsigned int i = 0; i < actorTypes.size(); i++)
        {
            if (actorTypes[i] != NULL)
            {
                QString fullCategory(tr(actorTypes[i].get()->GetCategory().c_str()));

                if (!fullCategory.isNull())
                {
                    QStringList subCategories = fullCategory.split(tr(ActorTypeTreeWidget::CATEGORY_SEPARATOR.c_str()),
                        QString::SkipEmptyParts);
                    QMutableStringListIterator *listIterator = new QMutableStringListIterator(subCategories);
                    rootActorType->recursivelyAddCategoryAndActorTypeAsChildren(listIterator, actorTypes[i].get());

                }
            }

        }

        // Now, go back and try to re-expand items and restore our scroll position
        restorePreviousExpansion();

        EditorData::getInstance().getMainWindow()->endWaitCursor();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorBrowser::handleEnableCreateActor()
    {
        ActorTypeTreeWidget *selectedWidget = getSelectedActorTreeWidget();

        // if we have a leaf, then enable the button
        if (selectedWidget != NULL && selectedWidget->isLeafNode())
        {
            createActorBtn->setEnabled(true);
            return;
        }

        // disable the button if we got here.
        if (createActorBtn != NULL)
        {
            createActorBtn->setEnabled(false);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    ActorTypeTreeWidget *ActorBrowser::getSelectedActorTreeWidget()
    {
        ActorTypeTreeWidget *returnVal = NULL;

        if (tree != NULL)
        {
            QList<QTreeWidgetItem *> list = tree->selectedItems();

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
        if (tree != NULL && rootActorType != NULL)
        {
            // we trap the root node separately to make the tree walking easier.
            rootNodeWasExpanded = tree->isItemExpanded(rootActorType);

            // clear out previous marks
            expandedActorTypeNames.clear();

            // start recursion
            recurseMarkCurrentExpansion(rootActorType, expandedActorTypeNames);

            // also store the last location of the scroll bar... so that they go back
            // to where they were next time.
            lastScrollBarLocation = tree->verticalScrollBar()->sliderPosition();
        }

    }

    /////////////////////////////////////////////////////////////////////////////////
    void ActorBrowser::recurseMarkCurrentExpansion(ActorTypeTreeWidget *parent,
        core::tree<QString> &currentTree)
    {
        for (int i = 0; i < parent->childCount(); i++)
        {
            ActorTypeTreeWidget *child = dynamic_cast<ActorTypeTreeWidget*>(parent->child(i));

            // if we have children, then we could potentially be expanded...
            if (child != NULL && child->childCount() > 0)
            {

                if (tree->isItemExpanded(child))
                {
                    // add it to our list
                    core::tree<QString> &insertedItem = currentTree.
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
        if (tree != NULL && rootActorType != NULL) {
            // re-expand the root node separately to make the tree walking easier.
            if (rootNodeWasExpanded)
                tree->expandItem(rootActorType);

            recurseRestorePreviousExpansion(rootActorType, expandedActorTypeNames);

            // Put the scroll bar back where it was last time
            tree->verticalScrollBar()->setSliderPosition(lastScrollBarLocation);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void ActorBrowser::recurseRestorePreviousExpansion(ActorTypeTreeWidget *parent,
        core::tree<QString> &currentTree)
    {
        // walk through the children...
        for (core::tree<QString>::const_iterator iter = currentTree.in(); iter != currentTree.end(); ++iter) {
            QString name = (*iter);

            // Try to find a control with this name in our model
            for (int i = 0; i < parent->childCount(); i ++) {
                ActorTypeTreeWidget *child = dynamic_cast<ActorTypeTreeWidget*>(parent->child(i));
                // found a match!  expand it
                if (child->getCategoryOrName() == name) {
                    tree->expandItem(child);

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
        LOG_INFO("User Created an Actor - Slot");
        dtEditQt::ActorTypeTreeWidget *selectedWidget = getSelectedActorTreeWidget();

        // if we have an actor type, then create the proxy and emit the signal
        if (selectedWidget != NULL && selectedWidget->getActorType() != NULL) {
            EditorData::getInstance().getMainWindow()->startWaitCursor();

            // create our new object
            osg::ref_ptr<dtDAL::ActorProxy> proxy =
                    dtDAL::LibraryManager::GetInstance().CreateActorProxy(*selectedWidget->getActorType()).get();

            if (proxy.valid())
            {
                // add the new proxy to the map
                osg::ref_ptr<dtDAL::Map> mapPtr = EditorData::getInstance().getCurrentMap();
                if (mapPtr.valid())
                {
                    mapPtr->AddProxy(*(proxy.get()));
                }

                // let the world know that a new proxy exists
                EditorEvents::getInstance().emitBeginChangeTransaction();
                EditorEvents::getInstance().emitActorProxyCreated(proxy.get(), false);
                ViewportManager::getInstance().placeProxyInFrontOfCamera(proxy.get());
                EditorEvents::getInstance().emitEndChangeTransaction();

                // Now, let the world that it should select the new actor proxy.
                std::vector<osg::ref_ptr<dtDAL::ActorProxy> > actors;
                actors.push_back(proxy.get());
                EditorEvents::getInstance().emitActorsSelected(actors);
            }

            EditorData::getInstance().getMainWindow()->endWaitCursor();
        }

    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorBrowser::treeSelectionChanged()
    {
        handleEnableCreateActor();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorBrowser::refreshActorTypes()
    {
        reloadActors();
    }

}
