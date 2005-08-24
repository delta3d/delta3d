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

#ifndef DELTA_ACTORSEARCHER
#define DELTA_ACTORSEARCHER

#include <QWidget>
#include <osg/ref_ptr>
#include <vector>
#include "dtDAL/actorproxy.h"
#include "dtEditQt/typedefs.h"

class QComboBox;
class QGroupBox;
class QLineEdit;
class QPushButton;

//namespace dtDAL {
//    class ActorProxy;
//}

namespace dtEditQt {
    // forward declare
    class ActorResultsTable;

    /**
     * @class ActorSearcher 
     * @brief This class allows the user to search for actors in the map
     */
    class ActorSearcher : public QWidget
    {
      Q_OBJECT
        public:
            /**
            * Constructor
            */
            ActorSearcher(QWidget *parent = 0);

            /**
            * Destructor
            */
            ~ActorSearcher();

        public slots:
            /**
            * Slot - handles the event when the search button is pressed 
            */
            void searchPressed();

            /** 
            * Called when maps, projects, or libraries change.  It should clear 
            * any results and the various search combo boxes.
            */
            void refreshAll();

            /** 
            * When a new actor is created, we have to add it's classes to our class list.
            */
            void onActorProxyCreated(proxyRefPtr proxy, bool forceNoAdjustments);

        private:
            QGroupBox *resultsGroup();
            QGroupBox *searchGroup();

            QComboBox *categoryBox;
            QComboBox *typeBox;
            QComboBox *classBox;
            QPushButton *searchBtn;
            QPushButton *clearBtn;
            QLineEdit *actorNameEdit;
            QLineEdit *actorCategoryEdit;

            ActorResultsTable *resultsTable;
            QString anyValue;
            QStringList classList;
    };
}

#endif
