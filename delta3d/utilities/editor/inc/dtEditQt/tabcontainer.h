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
* @author Teague Coonan
*/
#ifndef __TabContainer_h
#define __TabContainer_h

#include <QObject>
#include <QString>
#include <QTabWidget>
#include <vector>

#include "dtEditQt/tabwrapper.h"

class QIcon;

namespace dtEditQt 
{

    /**
    * @class TabContainer
    * @brief Tab Container that holds tabs. 
    */
    class TabContainer : public QObject
    {
    public:

        enum Position{Top, Bottom};

        /**
        * Constructor
        */
        TabContainer(QWidget *parent = 0);
        /**
        * Destructor
        */
        ~TabContainer();

        /**
        * Adds a new tab
        * @param TabWrapper object
        */
        void addTab(TabWrapper *myTab);

        /**
        * Adds a new tab and uses a tab icon instead of text
        * @param TabWrapper object
        * @param std::string that represents a UIResource which is a image sourcefile path
        */
        void addTab(TabWrapper *myTab, const std::string &tabResource);

        /**
        * Sets the position of the tabs
        * @param Position position
        */
        void setPosition(Position position);

        /**
        * @return Instantiated QTabWidget
        */
        QTabWidget* getWidget();

    private:
        QTabWidget *tabC;
        std::vector<TabWrapper*> tabVector;
        Position position;
    };
}
#endif
