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

#ifndef DELTA_GAME_EVENT_DIALOG
#define DELTA_GAME_EVENT_DIALOG

#include <dtCore/refptr.h>
#include <dtDAL/gameevent.h>
#include <QtGui/QDialog>

class QLineEdit;
class QCloseEvent;
class QPushButton;
class QTextEdit;

namespace dtEditQt
{
   class GameEventDialog : public QDialog
   {
      Q_OBJECT

      public:

         /// Constructor
         GameEventDialog(QWidget *parent, dtDAL::GameEvent& event, bool isNew);

         /// Destructor
         virtual ~GameEventDialog() { }

         inline dtDAL::GameEvent& GetEvent() { return *mGameEvent; } 

      public slots:

         /// slot for receiving the text changing signal
         void Edited(const QString &newText);

         /// slot for applying the changes made
         void ApplyChanges();

      private:
         QPushButton *mOKButton;
         QLineEdit *mNameEdit;
         QTextEdit *mDescEdit;
         dtCore::RefPtr<dtDAL::GameEvent> mGameEvent;
   };

}
#endif


