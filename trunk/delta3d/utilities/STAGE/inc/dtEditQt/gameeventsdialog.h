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
 */

#ifndef DELTA_GAME_EVENTS_DIALOG
#define DELTA_GAME_EVENTS_DIALOG

#include <vector>

#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>

class QTableWidget;
class QGridLayout;
class QPushButton;
class QStringList;
class QTableWidgetItem;

namespace dtCore
{
   class GameEvent;
}

namespace dtEditQt
{

   /**
    * @class EventTreeItem
    * @brief This is a utility table that shows a list of actor Proxies and allows some
    * basic behavior on them such as select and goto.
    */
   class EventTreeItem : public QTreeWidgetItem
   {
   public:
      EventTreeItem(QTreeWidget* parent, dtCore::GameEvent* gameEvent)
         : QTreeWidgetItem(parent)
         , mGameEvent(gameEvent)
      {
      }

      virtual ~EventTreeItem() { }

      dtCore::GameEvent* GetEvent() { return mGameEvent; }

   private:
      dtCore::GameEvent* mGameEvent;
   };

   ///////////////////////////////////////////////////////
   class GameEventsDialog : public QDialog
   {
      Q_OBJECT

   public:
      /// Constructor
      GameEventsDialog(QWidget* parent = NULL);

      /// Destructor
      virtual ~GameEventsDialog();

      virtual EventTreeItem* GetSelectedEventTreeItem();

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
      void HandleFailure(const int code, const std::string& errorMsg = "");

      /// Received when a library is currently selected
      void EnableButtons();

      /// Disable the delete button if no libraries are selected
      void DisableButtons();

   private:
      /// The visible list of Events
      //QTableWidget* mGameEventView;
      QTreeWidget* mGameEventView;

      /// Our buttons
      QPushButton* mEditGameEvent;
      QPushButton* mDeleteGameEvent;

      // obtains the event names and descriptions from the map
      // and converts them to QT format
      //void GetGameEventList(std::vector<QTableWidgetItem*>& eventList,
      //   std::vector<QTableWidgetItem*>& eventDescs) const;

      void RefreshGameEvents();
   };

} // namespace dtEditQt

#endif // DELTA_GAME_EVENTS_DIALOG
