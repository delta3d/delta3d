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

#include <QtGui/QComboBox>
#include <QtGui/QDirModel>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtCore/QStringList>
#include <QtGui/QTreeView>
#include "dtDAL/actorproxy.h"
#include "dtDAL/actortype.h"
#include "dtDAL/librarymanager.h"
#include <dtUtil/log.h>
#include "dtDAL/map.h"
#include "dtEditQt/actorsearcher.h"
#include "dtEditQt/actorresultstable.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/mainwindow.h"

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    ActorSearcher::ActorSearcher(QWidget *parent)
        :QWidget(parent), anyValue("(Any)")
    {
        QVBoxLayout *vbox = new QVBoxLayout(this);
        vbox->addWidget(searchGroup());
        vbox->addWidget(resultsGroup(), 1, 0);
        vbox->setSpacing(2);
        vbox->setMargin(3);

        // connect 
        connect(&EditorEvents::getInstance(), SIGNAL(mapLibraryImported()),
            this, SLOT(refreshAll()));
        connect(&EditorEvents::getInstance(), SIGNAL(mapLibraryRemoved()),
            this, SLOT(refreshAll()));
        connect(&EditorEvents::getInstance(), SIGNAL(currentMapChanged()), 
            this, SLOT(refreshAll()));
        connect(&EditorEvents::getInstance(), SIGNAL(projectChanged()), 
            this, SLOT(refreshAll()));
        connect(&EditorEvents::getInstance(), SIGNAL(LibraryAboutToBeRemoved()),
            this, SLOT(refreshAll())); // make sure the table is emptied here or crash!
        connect(&EditorEvents::getInstance(), SIGNAL(actorProxyCreated(ActorProxyRefPtr, bool)), 
            this, SLOT(onActorProxyCreated(ActorProxyRefPtr, bool)));   
    }

    ///////////////////////////////////////////////////////////////////////////////
    ActorSearcher::~ActorSearcher()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *ActorSearcher::searchGroup()
    {
        QGroupBox *searchBox = new QGroupBox(tr("Actor Search"),this);
        QVBoxLayout *vBoxLayout = new QVBoxLayout(searchBox);
        QGridLayout *gridLayout = new QGridLayout();
        QLabel *label;

        // actor name  search
        label = new QLabel(tr("Name:"),searchBox);
        label->setAlignment(Qt::AlignRight);
        actorNameEdit = new QLineEdit(searchBox);
        actorNameEdit->setToolTip(QString("Restrict the search by Actor Name - '*' and '?' are legal wildcards"));
        gridLayout->addWidget(label, 0, 0);
        gridLayout->addWidget(actorNameEdit, 0, 1);

        // build the category combo box
        label = new QLabel(tr("Category:"),searchBox);
        label->setAlignment(Qt::AlignRight);
        categoryBox = new QComboBox(searchBox);
        categoryBox->setToolTip(QString("Restrict the search by Actor Category"));
        gridLayout->addWidget(label, 1, 0);
        gridLayout->addWidget(categoryBox, 1, 1);

        // build the type combo box
        label = new QLabel(tr("Type:"),searchBox);
        label->setAlignment(Qt::AlignRight);
        typeBox = new QComboBox(searchBox);
        typeBox->setToolTip(QString("Restrict the search by Actor Type"));
        gridLayout->addWidget(label,2, 0);
        gridLayout->addWidget(typeBox, 2, 1);

        // build the class combo box
        label = new QLabel(tr("Class:"),searchBox);
        label->setAlignment(Qt::AlignRight);
        classBox = new QComboBox(searchBox);
        classBox->setToolTip(QString("Restrict the search by Actor Class Name (handles superclasses)"));
        gridLayout->addWidget(label, 3, 0);
        gridLayout->addWidget(classBox, 3, 1);

        // search btn
        searchBtn = new QPushButton(tr("Search"), searchBox);
        searchBtn->setToolTip(QString("Searches for Actors using search criteria"));
        connect(searchBtn, SIGNAL(clicked()), this, SLOT(searchPressed()));
        // clear btn
        clearBtn = new QPushButton(tr("Clear Search"), searchBox);
        clearBtn->setToolTip(QString("Clears search results and search criteria"));
        connect(clearBtn, SIGNAL(clicked()), this, SLOT(refreshAll()));

        // build up the button layout
        QHBoxLayout *btnLayout = new QHBoxLayout();
        btnLayout->addStretch(1);
        btnLayout->addWidget(searchBtn);
        btnLayout->addSpacing(3);
        btnLayout->addWidget(clearBtn);
        btnLayout->addStretch(1);


        // put all the piece3s in the main vertical layout
        vBoxLayout->addLayout(gridLayout);
        vBoxLayout->addSpacing(5);
        vBoxLayout->addLayout(btnLayout);
        vBoxLayout->addStretch(1);

        return searchBox;
    }

    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *ActorSearcher::resultsGroup()
    {
        QGroupBox *groupBox = new QGroupBox(tr("Results"),this);

        resultsTable = new ActorResultsTable(true, true, groupBox);

        QVBoxLayout *vbox = new QVBoxLayout(groupBox);
        vbox->addWidget(resultsTable, 10, 0); // take the rest of the space
        vbox->setSpacing(0);
        vbox->setMargin(1);

        return groupBox;
    }

    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    // SLOTS
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    void ActorSearcher::refreshAll()
    {
        std::vector<dtCore::RefPtr<dtDAL::ActorType> > actorTypes;
        std::vector<dtCore::RefPtr<dtDAL::ActorType> >::const_iterator actorTypesIter;
        std::set<std::string>::const_iterator setIter;
        dtDAL::Map *map = EditorData::getInstance().getCurrentMap().get();
        QStringList categoryList;
        QStringList typeList;

        actorNameEdit->clear();

        classList.clear();

        // clear the category box - there's no clear all.... 
        while (categoryBox->count() > 0) 
        {
            categoryBox->removeItem(0);
        }
        // clear the type box - there's no clear all.... 
        while (typeBox->count() > 0) 
        {
            typeBox->removeItem(0);
        }
        // clear the type box - there's no clear all.... 
        while (classBox->count() > 0) 
        {
            classBox->removeItem(0);
        }

        // empty out our table, just in case (this MUST happen  when libraries are removed)
        resultsTable->clearAll();

        if (map != NULL) 
        {
            EditorData::getInstance().getMainWindow()->startWaitCursor();

            // walk through all the actor types and pull out the unique categories and types
            dtDAL::LibraryManager::GetInstance().GetActorTypes(actorTypes);
            for(actorTypesIter = actorTypes.begin(); actorTypesIter != actorTypes.end(); ++actorTypesIter)
            {
                dtCore::RefPtr<dtDAL::ActorType> type = (*actorTypesIter);
                // get the cat
                QString newCat(type->GetCategory().c_str());
                if (!categoryList.contains(newCat))
                    categoryList.append(newCat);
                // get the types
                QString newType(type->GetName().c_str());
                if (!typeList.contains(newType)) 
                    typeList.append(newType);
            }

            // take the category list and add it to the combo box.
            categoryList.sort();
            categoryList.prepend(anyValue);
            categoryBox->addItems(categoryList);
            categoryBox->setCurrentIndex(0);

            // take the type list and add it to the combo box.
            typeList.sort();
            typeList.prepend(anyValue);
            typeBox->addItems(typeList);
            typeBox->setCurrentIndex(0);

            // fill up the class box
            std::set<std::string> classSet = map->GetProxyActorClasses();
            for (setIter = classSet.begin(); setIter != classSet.end(); ++setIter)
            {
                std::string className = (*setIter);
                classList.append(QString::fromStdString(className));
            }
            classList.sort();
            classList.prepend(anyValue);
            classBox->addItems(classList);
            classBox->setCurrentIndex(0);

            EditorData::getInstance().getMainWindow()->endWaitCursor();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorSearcher::searchPressed()
    {
        EditorData::getInstance().getMainWindow()->startWaitCursor();

        LOG_ERROR("User pressed the search button");
        std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > foundProxies;
        dtDAL::Map *map = EditorData::getInstance().getCurrentMap().get();

        // get the search values
        QString searchName = actorNameEdit->text();
        QString searchCategory = categoryBox->currentText();
        QString searchType = typeBox->currentText();
        QString searchClass = classBox->currentText();

        // check for the empty selection
        if (searchCategory == anyValue)
            searchCategory = "";
        if (searchType == anyValue)
            searchType = "";
        if (searchClass == anyValue)
            searchClass = "";
        
        // search
        map->FindProxies(foundProxies, searchName.toStdString(), searchCategory.toStdString(), 
            searchType.toStdString(), searchClass.toStdString(), dtDAL::Map::Either);

        // empty out our table before we add stuff
        resultsTable->clearAll();
        resultsTable->setUpdatesEnabled(false);
        resultsTable->addProxies(foundProxies);
        resultsTable->setUpdatesEnabled(true);
        //showResults(foundProxies);

        EditorData::getInstance().getMainWindow()->endWaitCursor();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorSearcher::onActorProxyCreated(dtCore::RefPtr<dtDAL::ActorProxy> proxy, bool forceNoAdjustments)
    {
        EditorData::getInstance().getMainWindow()->startWaitCursor();

        std::set<std::string>::const_iterator setIter;
        bool addedClasses = false;

        std::set<std::string> actorClasses = proxy->GetClassHierarchy();

        // walk through the class hierarchy of the new object
        for (setIter = actorClasses.begin(); setIter != actorClasses.end(); ++setIter)
        {
            std::string className = (*setIter);
            QString classString = QString::fromStdString(className);
            // add any new classes that we don't already have
            if (!classList.contains(classString)) 
            {
                addedClasses = true;
                classList.append(classString);
            }
        }

        // only do the sort and reset box once
        if (addedClasses) 
        {
            // hold previous selection - nice for the user
            QString previousSelection = classBox->currentText();

            // clear the type box - there's no clear all.... 
            while (classBox->count() > 0) 
            {
                classBox->removeItem(0);
            }

            // add the new sorted list.
            classList.sort();
            classBox->addItems(classList);

            // reselect the previous
            int previousIndex = classBox->findText(previousSelection);
            classBox->setCurrentIndex(previousIndex);
        }

        EditorData::getInstance().getMainWindow()->endWaitCursor();
    }

}
