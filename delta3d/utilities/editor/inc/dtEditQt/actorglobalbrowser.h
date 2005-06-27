/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2005, BMH Associates, Inc. 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
 * @author Curtiss Murphy
*/

#ifndef DELTA_ACTORGLOBALBROWSER
#define DELTA_ACTORGLOBALBROWSER

#include <QWidget>
#include <osg/ref_ptr>
#include <vector>
#include "dtDAL/actorproxy.h"

class QComboBox;
class QGroupBox;
class QLineEdit;
class QPushButton;

namespace dtEditQt 
{
    // forward declare
    class ActorResultsTable;

    /**
     * @class ActorGlobalBrowser 
     * @brief This class allows the user to browse through the global actors.
     */
    class ActorGlobalBrowser: public QWidget
    {
      Q_OBJECT
        public:
            /**
             * Constructor
             */
            ActorGlobalBrowser(QWidget *parent = 0);

            /**
             * Destructor
             */
            ~ActorGlobalBrowser();

        public slots:
            /**
             *  Reloads the global browser data
             */
            void refreshAll();

            /** 
             * When a new actor is created, we may have to add to the global list 
             */
            void onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy> proxy);

            /**
             * Called when an actor proxy is deleted
             */
            void onActorProxyDestroyed(osg::ref_ptr<dtDAL::ActorProxy> proxy);

        private:
            QGroupBox *resultsGroup();
            ActorResultsTable *resultsTable;
    };
}

#endif
