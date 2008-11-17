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
* Curtiss Murphy
*/
#include <prefix/dtstageprefix-src.h>
#include <QtGui/QComboBox>
#include <QtGui/QDirModel>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTreeView>

#include <QtCore/QStringList>

#include <dtDAL/actorproxy.h>
#include <dtDAL/actortype.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>

#include <dtUtil/log.h>

#include <dtEditQt/actorsearcher.h>
#include <dtEditQt/actorresultstable.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>

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
        connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryImported()),
            this, SLOT(refreshAll()));
        connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryRemoved()),
            this, SLOT(refreshAll()));
        connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryAboutToBeRemoved()),
           this, SLOT(refreshAll())); // make sure the table is emptied here or crash!
        connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()), 
            this, SLOT(refreshAll()));
        connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()), 
            this, SLOT(refreshAll()));
        connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyCreated(ActorProxyRefPtr, bool)), 
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
        std::vector<const dtDAL::ActorType*> actorTypes;
        std::vector<const dtDAL::ActorType*>::const_iterator actorTypesIter;
        std::set<std::string>::const_iterator setIter;
        dtDAL::Map *map = EditorData::GetInstance().getCurrentMap();
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
            EditorData::GetInstance().getMainWindow()->startWaitCursor();

            // walk through all the actor types and pull out the unique categories and types
            dtDAL::LibraryManager::GetInstance().GetActorTypes(actorTypes);
            for(actorTypesIter = actorTypes.begin(); actorTypesIter != actorTypes.end(); ++actorTypesIter)
            {
                dtCore::RefPtr<const dtDAL::ActorType> type = (*actorTypesIter);
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

            EditorData::GetInstance().getMainWindow()->endWaitCursor();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorSearcher::searchPressed()
    {
        EditorData::GetInstance().getMainWindow()->startWaitCursor();

        LOG_ERROR("User pressed the search button");
        std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > foundProxies;
        dtDAL::Map *map = EditorData::GetInstance().getCurrentMap();

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

        EditorData::GetInstance().getMainWindow()->endWaitCursor();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ActorSearcher::onActorProxyCreated(dtCore::RefPtr<dtDAL::ActorProxy> proxy, bool forceNoAdjustments)
    {
        EditorData::GetInstance().getMainWindow()->startWaitCursor();

        std::set<std::string>::const_iterator setIter;
        bool addedClasses = false;

        const std::set<std::string>& actorClasses = proxy->GetClassHierarchy();

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

        EditorData::GetInstance().getMainWindow()->endWaitCursor();
    }

}
