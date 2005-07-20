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
#ifndef __TabWrapper_h
#define __TabWrapper_h

#include <QObject>
#include <QWidget>
#include <QString>

namespace dtEditQt 
{
    /**
    * @class TabWrapper
    * @brief This class is a wrapper for the QWidget class to create a generic
    *        tab to be added to a tab container. 
    */
    class TabWrapper : public QObject
    {
    public:
        /**
        * Constructor
        */
        TabWrapper(QWidget *parent = 0);
        /**
        * Destructor
        */
        ~TabWrapper();

        /**
        * Sets the name of the tab
        * @param string name
        */
        void setName(const QString &name){tabName=name;};
        /**
        * Sets a widget to this object
        * @param QWidget widget
        */
        void setWidget(QWidget *myWidget);
        /**
        * Gets the name of this tab
        * @return The name of the tab.
        */
        QString getName(){return this->tabName;}
        /**
        * Gets the widget that was set by setWidget
        * @return QWidget
        */
        QWidget *getWidget(){return this->myWidget;};

    private:
        QString tabName;
        QWidget *myWidget;
    };
}
#endif
