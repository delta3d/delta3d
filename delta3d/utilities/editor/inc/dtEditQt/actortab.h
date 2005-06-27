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
#ifndef _ActorTab_H
#define _ActorTab_H

#include <QObject>
#include <QDockWidget>
#include <QMainWindow>

class QWidget;

namespace dtEditQt 
{

    class TabWrapper;
    class TabContainer;
    class ActorBrowser;
    class ActorSearcher;
    class ActorGlobalBrowser;

    /**
    * @class ActorTab
    * @brief This class holds all actor related tabs.
    */
    class ActorTab : public QDockWidget
    {
    public:
        /**
        * Constructor
        */
        ActorTab(QWidget *parent = 0);

        /**
        * Destructor
        */
        ~ActorTab();

        /**
        * Adds tab widgets to the tab container
        */
        void addTabs();

        /**
        * gets the actor tab widget
        * @return The main QWidget control for the tab 
        */
        QWidget* getWidget();

    private:
        TabContainer *tabC;
        TabWrapper *tabActorBrowser;
        TabWrapper *tabActorSearch;
        TabWrapper *tabGlobalActor;

        ActorBrowser *actorBrowserWidget;
        ActorSearcher *actorSearchWidget;
        ActorGlobalBrowser *actorGlobalWidget;

        void closeEvent(QCloseEvent *e);
    };

}

#endif
