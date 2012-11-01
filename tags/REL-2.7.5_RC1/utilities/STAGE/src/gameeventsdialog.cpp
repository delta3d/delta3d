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
 * David A. Guthrie
 * William E. Johnson II
 * Curtiss Murphy
 */

#include <prefix/stageprefix.h>

#include <dtEditQt/editoractions.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/gameeventdialog.h>
#include <dtEditQt/gameeventsdialog.h>

#include <dtCore/actorproperty.h>
#include <dtCore/actorproxy.h>
#include <dtCore/datatype.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventactorproperty.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/map.h>

#include <dtUtil/log.h>

#include <QtCore/QStringList>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QTableWidgetItem>
#include <QtGui/QVBoxLayout>

using std::vector;
using dtCore::GameEvent;
using dtCore::Map;
using dtCore::BaseActorObject;
using dtCore::ActorProperty;

namespace dtEditQt
{

   //////////////////////////////////////////////////////////////////
   GameEventsDialog::GameEventsDialog(QWidget* parent)
      : QDialog(parent)
   {
      setWindowTitle(tr("Game Event Editor"));

      QGroupBox*   groupBox = new QGroupBox(tr("Game Events"), this);
      QGridLayout* gridLayout = new QGridLayout(groupBox);

      mGameEventView = new QTreeWidget(groupBox);
      //mGameEventView = new QTableWidget(groupBox);
      mGameEventView->setSelectionMode(QAbstractItemView::SingleSelection);
      mGameEventView->setSelectionBehavior(QAbstractItemView::SelectRows);
      mGameEventView->setAlternatingRowColors(true);
      mGameEventView->setEditTriggers(QAbstractItemView::NoEditTriggers);

      mGameEventView->setRootIsDecorated(false);
      mGameEventView->setSortingEnabled(true);

      //mGameEventView->setRowCount(eventNames.size());
      mGameEventView->setColumnCount(2);
      mGameEventView->header()->setResizeMode(QHeaderView::Stretch);
      mGameEventView->header()->setFixedHeight(20);

      // set the headers
      QStringList headerLabels;
      headerLabels << "Event" << "Description";
      mGameEventView->setHeaderLabels(headerLabels);
      mGameEventView->header()->setClickable(true);

      gridLayout->addWidget(mGameEventView, 0, 0);

      // create the buttons, default delete to disabled
      QHBoxLayout* buttonLayout = new QHBoxLayout;
      QPushButton* newEvent     = new QPushButton(tr("New Event"),this);
      QPushButton* close        = new QPushButton(tr("Close"),this);
      mEditGameEvent            = new QPushButton(tr("Edit Event"),this);
      mDeleteGameEvent          = new QPushButton(tr("Delete Event"),this);

      newEvent->setDefault(false);
      mEditGameEvent->setDefault(true);

      mEditGameEvent->setDisabled(true);
      mDeleteGameEvent->setDisabled(true);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(newEvent);
      buttonLayout->addWidget(mEditGameEvent);
      buttonLayout->addWidget(mDeleteGameEvent);
      buttonLayout->addWidget(close);
      buttonLayout->addStretch(1);

      // make the connections
      connect(mGameEventView,   SIGNAL(itemSelectionChanged()), this, SLOT(EnableButtons()));
      connect(newEvent,         SIGNAL(clicked()),              this, SLOT(CreateNewGameEvent()));
      connect(mEditGameEvent,   SIGNAL(clicked()),              this, SLOT(EditGameEvent()));
      connect(mDeleteGameEvent, SIGNAL(clicked()),              this, SLOT(SpawnDeleteConfirmation()));
      connect(close,            SIGNAL(clicked()),              this, SLOT(close()));
      connect(this,             SIGNAL(GameEventSelected()),    this, SLOT(EnableButtons()));
      //connect(mGameEventView,   SIGNAL(itemSelectionChanged()), this, SLOT(EnableButtons()));

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      setTabOrder(newEvent, mEditGameEvent);
      setTabOrder(mEditGameEvent, mDeleteGameEvent);
      setTabOrder(mDeleteGameEvent, close);

      RefreshGameEvents();
   }

   //////////////////////////////////////////////////////////////////
   GameEventsDialog::~GameEventsDialog()
   {
   }

   //////////////////////////////////////////////////////////////////
   EventTreeItem* GameEventsDialog::GetSelectedEventTreeItem()
   {
      EventTreeItem* returnVal = NULL;

      if (mGameEventView != NULL)
      {
         QList<QTreeWidgetItem*> list = mGameEventView->selectedItems();

         if (!list.isEmpty())
         {
            returnVal = dynamic_cast<EventTreeItem*>(list[0]);
         }
      }

      return returnVal;
   }

   //////////////////////////////////////////////////////////////////
   void GameEventsDialog::EditGameEvent()
   {
      EventTreeItem* selection = GetSelectedEventTreeItem();
      if (selection == NULL)
      {
         return;
      }
      GameEvent* eventToEdit = selection->GetEvent();

      GameEventDialog editDialog(this, *eventToEdit, false);
      if (editDialog.exec() == QDialog::Accepted)
      {
         EditorEvents::GetInstance().emitGameEventEdited();
         Map& curMap = *EditorData::GetInstance().getCurrentMap();
         curMap.SetModified(true);
         RefreshGameEvents();
         //select the event just edited.
      }
   }

   //////////////////////////////////////////////////////////////////
   void GameEventsDialog::CreateNewGameEvent()
   {
      dtCore::RefPtr<dtCore::GameEvent> newEvent = new dtCore::GameEvent();

      GameEventDialog editDialog(this, *newEvent, true);
      if (editDialog.exec() == QDialog::Accepted)
      {
         //select the event just edited.
         Map& curMap = *EditorData::GetInstance().getCurrentMap();
         curMap.GetEventManager().AddEvent(*newEvent);
         EditorEvents::GetInstance().emitGameEventAdded();
         RefreshGameEvents();
      }
   }

   //////////////////////////////////////////////////////////////////
   void GameEventsDialog::SpawnDeleteConfirmation()
   {
      Map& curMap = *EditorData::GetInstance().getCurrentMap();

      EventTreeItem* selection = GetSelectedEventTreeItem();
      if (selection == NULL)
      {
         return;
      }
      GameEvent* eventToDelete = selection->GetEvent();

      vector< dtCore::RefPtr<BaseActorObject> > proxies;
      curMap.GetAllProxies(proxies);

      vector<ActorProperty*> properties;
      vector<dtCore::GameEventActorProperty*> eventPropsToClear;
      // fail if actors are in the library
      for (unsigned int j = 0; j < proxies.size(); ++j)
      {
         proxies[j]->GetPropertyList(properties);
         for (unsigned int k = 0; k < properties.size(); ++k)
         {
            ActorProperty* prop = properties[k];
            if (prop->GetDataType() == dtCore::DataType::GAME_EVENT)
            {
               dtCore::GameEventActorProperty* geProp = static_cast<dtCore::GameEventActorProperty*>(prop);
               GameEvent* propEvent = geProp->GetValue();
               if (propEvent != NULL && *propEvent == *eventToDelete)
               {
                  eventPropsToClear.push_back(geProp);
               }
            }
         }
      }

      if (eventPropsToClear.empty())
      {
         if (QMessageBox::question(this, tr("Confirm deletion"),
            tr("Are you sure you want to delete this game event?"),
            tr("&Yes"), tr("&No"), QString::null, 1) == 0)
         {
            EditorEvents::GetInstance().emitGameEventAboutToBeRemoved();
            curMap.GetEventManager().RemoveEvent(*eventToDelete);
            EditorEvents::GetInstance().emitGameEventRemoved();
         }
      }
      else if (QMessageBox::question(this, tr("Confirm deletion"),
         tr("Actors are referencing this game event. Would you still like to delete it?"),
         tr("&Yes"), tr("&No"), QString::null, 1) == 0)
      {
         for (unsigned int i = 0; i < eventPropsToClear.size(); ++i)
         {
            eventPropsToClear[i]->SetValue(NULL);
         }

         EditorEvents::GetInstance().emitGameEventAboutToBeRemoved();
         curMap.GetEventManager().RemoveEvent(*eventToDelete);
         EditorEvents::GetInstance().emitGameEventRemoved();
      }
      RefreshGameEvents();
   }

   //////////////////////////////////////////////////////////////////
   void GameEventsDialog::HandleFailure(const int code, const std::string& errorMsg)
   {
   }

   //////////////////////////////////////////////////////////////////
   void GameEventsDialog::EnableButtons()
   {
      EventTreeItem* selection = GetSelectedEventTreeItem();
      mDeleteGameEvent->setDisabled(selection == NULL);
      mEditGameEvent->setDisabled(selection == NULL);
   }

   //////////////////////////////////////////////////////////////////
   void GameEventsDialog::DisableButtons()
   {
      EnableButtons();
   }

   //////////////////////////////////////////////////////////////////
   //Populate the list box with the current events.
   //vector<QTableWidgetItem*> eventNames, eventDescs;
   //GetGameEventList(eventNames, eventDescs);
   //void GameEventsDialog::GetGameEventList(std::vector<QTableWidgetItem*>& eventList,
   //   std::vector<QTableWidgetItem*>& eventDescs) const
   //{
   //}

   void GameEventsDialog::RefreshGameEvents()
   {
      EventTreeItem* selectedItem = GetSelectedEventTreeItem();
      Map* currentMap = EditorData::GetInstance().getCurrentMap();
      if (currentMap == NULL)
      {
         return;
      }

      // Clear out the current list
      mGameEventView->clear();

      vector<GameEvent*> events;
      currentMap->GetEventManager().GetAllEvents(events);

      // For each event, create a new entry in our table with an event on it.
      for (unsigned int i = 0; i < events.size(); ++i)
      {
         QString name(events[i]->GetName().c_str());
         QString description(events[i]->GetDescription().c_str());

         // create the tree entry
         EventTreeItem* item = new EventTreeItem(mGameEventView, events[i]);
         item->setText(0, name);
         item->setText(1, description);
      }

      // Reselect the previous selection, if we can.
      if (selectedItem != NULL)
      {
         // loop through the new items and compare events - if same, then set selected
         EventTreeItem* item;
         int index = 0;
         while (NULL != (item = (EventTreeItem*) mGameEventView->topLevelItem(index)))
         {
            if (item->GetEvent() == selectedItem->GetEvent())
            {
               mGameEventView->setItemSelected(item, true);
            }
            ++index;
         }

      }

      // Make sure we set our buttons appropriately
      EnableButtons();
   }

} // namespace dtEditQt
