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
* Teague Coonan
*/
#include <prefix/dtstageprefix-src.h>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

#include <dtEditQt/tabcontainer.h>
#include <dtEditQt/tabwrapper.h>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    TabContainer::TabContainer(QWidget *parent) : QObject(parent)
    {
        tabC = new QTabWidget (parent);
    }
    ///////////////////////////////////////////////////////////////////////////////
    TabContainer::~TabContainer()
    {
        for(unsigned int i = 0; i < tabVector.size(); i++)
            delete tabVector.at(i);
 
        tabVector.clear();
    }
    ///////////////////////////////////////////////////////////////////////////////
    void TabContainer::addTab(TabWrapper *myTab)
    {
        // add the tab to the container
        tabC->addTab(myTab->getWidget(), myTab->getName());
        
        // push the tab into a vector
        tabVector.push_back(myTab);
    }
    ///////////////////////////////////////////////////////////////////////////////
    void TabContainer::addTab(TabWrapper *myTab, const std::string &tabResource)
    {
        QIcon *tabIcon = new QIcon();
        QString resource = tabResource.c_str();
        tabIcon->addPixmap(QPixmap(resource));

        // add the tab to the container
        tabC->addTab(myTab->getWidget(),*tabIcon, myTab->getName());
        
        // push the tab into a vector
        tabVector.push_back(myTab);
    }
    ///////////////////////////////////////////////////////////////////////////////
    void TabContainer::setPosition(Position position)
    {
        // set the position to either top or bottom
        tabC->setTabPosition((QTabWidget::TabPosition)position);
    }
    ///////////////////////////////////////////////////////////////////////////////
    QTabWidget *TabContainer::getWidget()
    {
        return tabC;
    }
    ///////////////////////////////////////////////////////////////////////////////
}
