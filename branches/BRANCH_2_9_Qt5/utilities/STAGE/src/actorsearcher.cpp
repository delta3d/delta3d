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
#include <prefix/stageprefix.h>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDirModel>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QApplication>
#include <QtGui/QClipboard>

#include <QtCore/QStringList>

#include <dtCore/actorproxy.h>
#include <dtCore/actortype.h>
#include <dtCore/actorfactory.h>
#include <dtCore/map.h>
#include <dtGame/actorcomponent.h>
#include <dtUtil/log.h>

#include <dtEditQt/actorsearcher.h>
#include <dtEditQt/actorresultstable.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   ActorSearcher::ActorSearcher(QWidget* parent)
      : QWidget(parent)
      , mAnyValue("(Any)")
   {
      QVBoxLayout* vbox = new QVBoxLayout(this);
      vbox->addWidget(searchGroup());
      vbox->addWidget(resultsGroup(), 1, 0);
      vbox->setSpacing(2);
      vbox->setMargin(3);

      // connect
      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryImported()),
         this, SLOT(refreshAll()), Qt::QueuedConnection);
      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryRemoved()),
         this, SLOT(refreshAll()), Qt::QueuedConnection);
      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryAboutToBeRemoved()),
         this, SLOT(refreshAll()), Qt::QueuedConnection); // make sure the table is emptied here or crash!
      connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
         this, SLOT(refreshAll()), Qt::QueuedConnection);
      connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
         this, SLOT(refreshAll()), Qt::QueuedConnection);
      connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyCreated(dtCore::ActorPtr, bool)),
         this, SLOT(onActorCreated(dtCore::ActorPtr, bool)), Qt::QueuedConnection);
   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorSearcher::~ActorSearcher()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* ActorSearcher::searchGroup()
   {
      QGroupBox*   searchBox  = new QGroupBox(tr("Actor Search"),this);
      QVBoxLayout* vBoxLayout = new QVBoxLayout(searchBox);
      QGridLayout* gridLayout = new QGridLayout();
      QLabel*      label;

      // actor name  search
      label = new QLabel(tr("Name:"),searchBox);
      label->setAlignment(Qt::AlignRight);
      mActorNameEdit = new QLineEdit(searchBox);
      mActorNameEdit->setToolTip(QString("Restrict the search by Actor Name - '*' and '?' are legal wildcards"));
      gridLayout->addWidget(label, 0, 0);
      gridLayout->addWidget(mActorNameEdit, 0, 1);

      // build the category combo box
      label = new QLabel(tr("Category:"),searchBox);
      label->setAlignment(Qt::AlignRight);
      mCategoryBox = new QComboBox(searchBox);
      mCategoryBox->setToolTip(QString("Restrict the search by Actor Category"));
      gridLayout->addWidget(label, 1, 0);
      gridLayout->addWidget(mCategoryBox, 1, 1);

      // build the type combo box
      label = new QLabel(tr("Type:"),searchBox);
      label->setAlignment(Qt::AlignRight);
      mTypeBox = new QComboBox(searchBox);
      mTypeBox->setToolTip(QString("Restrict the search by Actor Type"));
      gridLayout->addWidget(label,2, 0);
      gridLayout->addWidget(mTypeBox, 2, 1);

      // build the class combo box
      label = new QLabel(tr("Class:"),searchBox);
      label->setAlignment(Qt::AlignRight);
      mClassBox = new QComboBox(searchBox);
      mClassBox->setToolTip(QString("Restrict the search by Actor Class Name (handles superclasses)"));
      gridLayout->addWidget(label, 3, 0);
      gridLayout->addWidget(mClassBox, 3, 1);

      // search btn
      mSearchBtn = new QPushButton(tr("Search"), searchBox);
      mSearchBtn->setToolTip(QString("Searches for Actors using search criteria"));
      connect(mSearchBtn, SIGNAL(clicked()), this, SLOT(searchPressed()));
      // clear btn
      mClearBtn = new QPushButton(tr("Clear Search"), searchBox);
      mClearBtn->setToolTip(QString("Clears search results and search criteria"));
      connect(mClearBtn, SIGNAL(clicked()), this, SLOT(refreshAll()));

      // build up the button layout
      QHBoxLayout* btnLayout = new QHBoxLayout();
      btnLayout->addStretch(1);
      btnLayout->addWidget(mSearchBtn);
      btnLayout->addSpacing(3);
      btnLayout->addWidget(mClearBtn);
      btnLayout->addStretch(1);


      // put all the piece3s in the main vertical layout
      vBoxLayout->addLayout(gridLayout);
      vBoxLayout->addSpacing(5);
      vBoxLayout->addLayout(btnLayout);
      vBoxLayout->addStretch(1);

      return searchBox;
   }

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* ActorSearcher::resultsGroup()
   {
      QGroupBox* groupBox = new QGroupBox(tr("Results"),this);

      mResultsTable = new ActorResultsTable(true, true, groupBox);

      QVBoxLayout* vbox = new QVBoxLayout(groupBox);
      vbox->addWidget(mResultsTable, 10, 0); // take the rest of the space
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
      dtCore::ActorTypeVec actorTypes;
      dtCore::ActorTypeVec::const_iterator actorTypesIter;
      std::set<dtUtil::RefString>::const_iterator setIter;
      dtCore::Map* map = EditorData::GetInstance().getCurrentMap();
      QStringList categoryList;
      QStringList typeList;

      mActorNameEdit->clear();

      mClassList.clear();

      mCategoryBox->clear();
      mTypeBox->clear();
      mClassBox->clear();

      // Remember our currently selected actors.
      std::vector<dtCore::UniqueId> selection = mResultsTable->getSelectedItems();

      // empty out our table, just in case (this MUST happen  when libraries are removed)
      mResultsTable->clearAll();

      if (map != NULL)
      {
         EditorData::GetInstance().getMainWindow()->startWaitCursor();

         // walk through all the actor types and pull out the unique categories and types
         dtCore::ActorFactory::GetInstance().GetActorTypes(actorTypes);
         for (actorTypesIter = actorTypes.begin(); actorTypesIter != actorTypes.end(); ++actorTypesIter)
         {
            dtCore::RefPtr<const dtCore::ActorType> type = (*actorTypesIter);
            // get the cat
            QString newCat(type->GetCategory().c_str());
            if (!categoryList.contains(newCat))
            {
               categoryList.append(newCat);
            }
            // get the types
            QString newType(type->GetName().c_str());
            if (!typeList.contains(newType))
            {
               typeList.append(newType);
            }
         }

         // take the category list and add it to the combo box.
         categoryList.sort();
         categoryList.prepend(mAnyValue);
         mCategoryBox->addItems(categoryList);
         mCategoryBox->setCurrentIndex(0);

         // take the type list and add it to the combo box.
         typeList.sort();
         typeList.prepend(mAnyValue);
         mTypeBox->addItems(typeList);
         mTypeBox->setCurrentIndex(0);

         // fill up the class box
         std::set<dtUtil::RefString> classSet = map->GetProxyActorClasses();
         for (setIter = classSet.begin(); setIter != classSet.end(); ++setIter)
         {
            dtUtil::RefString className = (*setIter);
            mClassList.append(QString::fromStdString(className));
         }
         mClassList.sort();
         mClassList.prepend(mAnyValue);
         mClassBox->addItems(mClassList);
         mClassBox->setCurrentIndex(0);

         EditorData::GetInstance().getMainWindow()->endWaitCursor();

         searchPressed();

         // Now attempt to restore our current selection.
         mResultsTable->setSelectedItems(selection);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorSearcher::searchPressed()
   {
      dtCore::ActorRefPtrVector foundActors;
      dtCore::ActorRefPtrVector validActors;

      dtCore::Map* map = EditorData::GetInstance().getCurrentMap();
      if (map == nullptr)
      {
         return;
      }

      EditorData::GetInstance().getMainWindow()->startWaitCursor();

      // get the search values
      QString searchName     = mActorNameEdit->text();
      QString searchCategory = mCategoryBox->currentText();
      QString searchType     = mTypeBox->currentText();
      QString searchClass    = mClassBox->currentText();

      // check for the empty selection
      if (searchCategory == mAnyValue)
      {
         searchCategory = "";
      }
      if (searchType == mAnyValue)
      {
         searchType = "";
      }
      if (searchClass == mAnyValue)
      {
         searchClass = "";
      }

      // Search
      map->FindProxies(foundActors, searchName.toStdString(), searchCategory.toStdString(),
         searchType.toStdString(), searchClass.toStdString(), dtCore::Map::Either);

      // Remove actor components.
      dtGame::ActorComponent* comp = NULL;
      dtCore::ActorRefPtrVector::iterator curIter = foundActors.begin();
      dtCore::ActorRefPtrVector::iterator endIter = foundActors.end();
      for (; curIter != endIter; ++curIter)
      {
         comp = dynamic_cast<dtGame::ActorComponent*>(curIter->get());

         // Only allow actors not actor components directly.
         if (comp == NULL)
         {
            validActors.push_back(curIter->get());
         }
      }

      QClipboard* clipboard = QApplication::clipboard();
      if (!searchName.isEmpty())
      {
         clipboard->setText(searchName);
      }
      else if (!searchType.isEmpty())
      {
         clipboard->setText(searchType);
      }

      // Empty out the table before adding items.
      mResultsTable->clearAll();
      mResultsTable->setUpdatesEnabled(false);
      mResultsTable->addActors(validActors);
      mResultsTable->setUpdatesEnabled(true);
      //showResults(foundProxies);

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ActorSearcher::onActorCreated(dtCore::ActorPtr actor, bool forceNoAdjustments)
   {
      EditorData::GetInstance().getMainWindow()->startWaitCursor();

      std::set<dtUtil::RefString>::const_iterator setIter;
      bool addedClasses = false;

      const std::set<dtUtil::RefString>& actorClasses = actor->GetActorType().GetSharedClassInfo().mClassHierarchy;

      // walk through the class hierarchy of the new object
      for (setIter = actorClasses.begin(); setIter != actorClasses.end(); ++setIter)
      {
         std::string className = (*setIter);
         QString classString = QString::fromStdString(className);
         // add any new classes that we don't already have
         if (!mClassList.contains(classString))
         {
            addedClasses = true;
            mClassList.append(classString);
         }
      }

      // only do the sort and reset box once
      if (addedClasses)
      {
         // hold previous selection - nice for the user
         QString previousSelection = mClassBox->currentText();

         mClassBox->clear();

         // add the new sorted list.
         mClassList.sort();
         mClassBox->addItems(mClassList);

         // reselect the previous
         int previousIndex = mClassBox->findText(previousSelection);
         mClassBox->setCurrentIndex(previousIndex);
      }

      EditorData::GetInstance().getMainWindow()->endWaitCursor();

      searchPressed();
   }

} // namespace dtEditQt
