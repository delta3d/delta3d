/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * David A. Guthrie
 * William E. Johnson II
 */

#include <prefix/dtstageprefix-src.h>

#include <QtCore/QStringList>
#include <QtGui/QTableWidget>
#include <QtGui/QTableWidgetItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QtGui/QMainWindow>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>

#include <dtEditQt/gameeventsdialog.h>
#include <dtEditQt/gameeventdialog.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editoractions.h>

#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventmanager.h>
#include <dtDAL/map.h>
#include <dtDAL/datatype.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/enginepropertytypes.h>

#include <dtUtil/log.h>

using std::vector;
using dtDAL::GameEvent;
using dtDAL::Map;
using dtDAL::ActorProxy;
using dtDAL::ActorProperty;

namespace dtEditQt
{
   GameEventsDialog::GameEventsDialog(QWidget *parent): QDialog(parent)
   {
      setWindowTitle(tr("Game Event Editor"));
      
      QGroupBox *groupBox = new QGroupBox(tr("Game Events"), this);
      QGridLayout *gridLayout = new QGridLayout(groupBox);      
      
      mGameEventView = new QTableWidget(groupBox);
      mGameEventView->setSelectionMode(QAbstractItemView::SingleSelection);
      mGameEventView->setSelectionBehavior(QAbstractItemView::SelectRows);
      mGameEventView->setAlternatingRowColors(true);
      mGameEventView->setEditTriggers(QAbstractItemView::NoEditTriggers);

      gridLayout->addWidget(mGameEventView, 0, 0);
                  
      // create the buttons, default delete to disabled
      QHBoxLayout *buttonLayout = new QHBoxLayout;
      QPushButton *newEvent = new QPushButton(tr("New Event"),this);
      QPushButton *close = new QPushButton(tr("Close"),this);
      mEditGameEvent = new QPushButton(tr("Edit Event"),this);
      mDeleteGameEvent = new QPushButton(tr("Delete Event"),this);
      
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
      connect(newEvent,         SIGNAL(clicked()),              this, SLOT(CreateNewGameEvent()));
      connect(mEditGameEvent,   SIGNAL(clicked()),              this, SLOT(EditGameEvent()));
      connect(mDeleteGameEvent, SIGNAL(clicked()),              this, SLOT(SpawnDeleteConfirmation()));
      connect(close,            SIGNAL(clicked()),              this, SLOT(close()));
      connect(this,             SIGNAL(GameEventSelected()),    this, SLOT(EnableButtons()));
      connect(mGameEventView,   SIGNAL(itemSelectionChanged()), this, SLOT(EnableButtons()));

      QVBoxLayout *mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      setTabOrder(newEvent, mEditGameEvent);
      setTabOrder(mEditGameEvent, mDeleteGameEvent);
      setTabOrder(mDeleteGameEvent, close);
      
      RefreshGameEvents();
   }

   GameEventsDialog::~GameEventsDialog()
   {
   }
                  
   void GameEventsDialog::EditGameEvent()
   {
      if (mGameEventView->currentItem() == NULL)
         return;
         
      std::string eventNameToEdit = mGameEventView->currentItem()->text().toStdString();
      
      Map& curMap = *EditorData::GetInstance().getCurrentMap();
      GameEvent* eventToEdit = curMap.GetEventManager().FindEvent(eventNameToEdit);

      if (eventToEdit == NULL)
      {
         LOG_ERROR("Event named \"" + eventNameToEdit + 
            "\" does not exist in the Event Manager, but it was selected to edit.");
         return;
      }

      GameEventDialog editDialog(this, *eventToEdit, false);
      if (editDialog.exec() == QDialog::Accepted) 
      {
         EditorEvents::GetInstance().emitGameEventEdited();
         curMap.SetModified(true);
         RefreshGameEvents();
         //select the event just edited.
      }
   }

   void GameEventsDialog::CreateNewGameEvent()
   {
      dtCore::RefPtr<dtDAL::GameEvent> newEvent = new dtDAL::GameEvent();

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
   
   void GameEventsDialog::SpawnDeleteConfirmation()
   {
      if (mGameEventView->currentItem() == NULL)
         return;
         
      Map& curMap = *EditorData::GetInstance().getCurrentMap();

      std::string eventNameToDelete = mGameEventView->currentItem()->text().toStdString();
      
      GameEvent* eventToDelete = curMap.GetEventManager().FindEvent(eventNameToDelete);
      
      if (eventToDelete == NULL)
      {
         LOG_ERROR("Event named \"" + eventNameToDelete + 
            "\" does not exist in the Event Manager.");
         return;
      }
      
      vector<dtCore::RefPtr<ActorProxy> > proxies;
      curMap.GetAllProxies(proxies);

      vector<ActorProperty*> properties;
      vector<dtDAL::GameEventActorProperty*> eventPropsToClear;
      // fail if actors are in the library
      for (unsigned int j = 0; j < proxies.size(); ++j)
      {
         proxies[j]->GetPropertyList(properties);
         for (unsigned int k = 0; k < properties.size(); ++k)
         {
            ActorProperty* prop = properties[k];
            if (prop->GetDataType() == dtDAL::DataType::GAME_EVENT)
            {
               dtDAL::GameEventActorProperty* geProp = static_cast<dtDAL::GameEventActorProperty*>(prop);
               GameEvent* propEvent = geProp->GetValue();
               if (propEvent != NULL && *propEvent == *eventToDelete)
                  eventPropsToClear.push_back(geProp);
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
   
   void GameEventsDialog::HandleFailure(const int code, const std::string &errorMsg)
   {
   }
   
   void GameEventsDialog::EnableButtons()
   {
      mDeleteGameEvent->setDisabled(false);
      mEditGameEvent->setDisabled(false);
   }
   
   void GameEventsDialog::DisableButtons()
   {
      mDeleteGameEvent->setDisabled(true);
      mEditGameEvent->setDisabled(true);
   }
         
   void GameEventsDialog::GetGameEventList(std::vector<QTableWidgetItem*>& eventList, 
      std::vector<QTableWidgetItem*>& eventDescs) const
   {
      eventList.clear();
      Map* currentMap = EditorData::GetInstance().getCurrentMap();
      if (currentMap == NULL)
         return;
      
      vector<GameEvent*> events;
      currentMap->GetEventManager().GetAllEvents(events);
      
      for(unsigned int i = 0; i < events.size(); i++)
      {
         QTableWidgetItem *n = new QTableWidgetItem;
         QTableWidgetItem *d = new QTableWidgetItem;

         n->setText(tr(events[i]->GetName().c_str()));
         d->setText(tr(events[i]->GetDescription().c_str()));

         eventList.push_back(n);
         eventDescs.push_back(d);
      }
   }
   
   void GameEventsDialog::RefreshGameEvents()
   {
      //Populate the list box with the current events.
      vector<QTableWidgetItem*> eventNames, eventDescs;
      GetGameEventList(eventNames, eventDescs);
      
      mGameEventView->setRowCount(eventNames.size());
      mGameEventView->setColumnCount(2);
      mGameEventView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
      mGameEventView->horizontalHeader()->setFixedHeight(20);
      mGameEventView->verticalHeader()->setHidden(true);

      QStringList names;
      names.push_back(tr("Event"));
      names.push_back(tr("Description"));
      mGameEventView->setHorizontalHeaderLabels(names);

      names.clear();

      for(unsigned int i = 0; i < eventNames.size(); ++i)
      {
         mGameEventView->setItem(i, 0, eventNames[i]);
         mGameEventView->setItem(i, 1, eventDescs[i]);
         names.push_back(tr(""));
      }
      mGameEventView->setVerticalHeaderLabels(names);
      
      if(mGameEventView->currentItem() == NULL)
         DisableButtons();
      else
         mGameEventView->setItemSelected(mGameEventView->currentItem(), true);

   }
}
