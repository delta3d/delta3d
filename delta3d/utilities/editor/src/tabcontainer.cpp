/* 
* Delta3D Open Source Game and Simulation Engine Level Editor 
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

#include "dtEditQt/tabcontainer.h"

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    TabContainer::TabContainer(QWidget *parent) : QObject(parent)
    {
        this->tabC = new QTabWidget (parent);
    }
    ///////////////////////////////////////////////////////////////////////////////
    TabContainer::~TabContainer()
    {
        for(int i = 0; i<(signed)tabVector.size();i++)
            delete tabVector.at(i);
 
        tabVector.clear();
    }
    ///////////////////////////////////////////////////////////////////////////////
    void TabContainer::addTab(TabWrapper *myTab)
    {
        // add the tab to the container
        this->tabC->addTab(myTab->getWidget(), myTab->getName());
        
        // push the tab into a vector
        this->tabVector.push_back(myTab);
    }
    ///////////////////////////////////////////////////////////////////////////////
    void TabContainer::setPosition(Position position)
    {
        // set the position to either top or bottom
        this->tabC->setTabPosition((QTabWidget::TabPosition)position);
    }
    ///////////////////////////////////////////////////////////////////////////////
    QTabWidget *TabContainer::getWidget()
    {
        return this->tabC;
    }
    ///////////////////////////////////////////////////////////////////////////////
}
