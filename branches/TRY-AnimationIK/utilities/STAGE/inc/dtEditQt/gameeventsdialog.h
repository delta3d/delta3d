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
 */

#ifndef DELTA_GAME_EVENTS_DIALOG
#define DELTA_GAME_EVENTS_DIALOG

#include <vector>

#include <QtGui/QDialog>
#include <dtDAL/actorproxy.h>

class QTableWidget;
class QGridLayout;
class QPushButton;
class QStringList;
class QTableWidgetItem;

namespace dtDAL
{
   class GameEvent;
}

namespace dtEditQt
{
   class GameEventsDialog : public QDialog
   {
      Q_OBJECT
      
      public:
         
         /// Constructor
         GameEventsDialog(QWidget *parent = NULL);
         
         /// Destructor
         virtual ~GameEventsDialog();
         
      signals:
         
         /// This signal enables the delete button
         void GameEventSelected();
         
      public slots:
            
         /// Pop up the editor for an event
         void EditGameEvent();

         /// Pop up the editor for an event
         void CreateNewGameEvent();
         
         /// Confirm deletion of libraries
         void SpawnDeleteConfirmation();
         
         /// Handle a deletion failure
         void HandleFailure(const int code, const std::string &errorMsg = "");
         
         /// Received when a library is currently selected
         void EnableButtons();
         
         /// Disable the delete button if no libraries are selected
         void DisableButtons();
         
      private:
            
         /// The visible list of Events
         QTableWidget *mGameEventView;
         
         /// Our buttons
         QPushButton *mEditGameEvent;
         QPushButton *mDeleteGameEvent;
         
         // obtains the event names and descriptions from the map
         // and converts them to QT format
         void GetGameEventList(std::vector<QTableWidgetItem*>& eventList, 
            std::vector<QTableWidgetItem*>& eventDescs) const;


         void RefreshGameEvents();
   };
   
}
#endif
