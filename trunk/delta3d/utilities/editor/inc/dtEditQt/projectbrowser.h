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
* @author Teague Coonan
*/

#ifndef __ProjectBrowser_h
#define __ProjectBrowser_h

#include <QWidget>

class QGroupBox;
class QPushButton;

namespace dtEditQt 
{

    /**
    * @class ProjectBrowser
    * @brief This class provides the user with a mechanism to browse and manipulate
    *        their current project
    */
    class ProjectBrowser : public QWidget
    {
        Q_OBJECT

    public:
        /**
        * Constructor
        */
        ProjectBrowser(QWidget *parent = 0);
        /**
        * Destructor
        */
        ~ProjectBrowser();

    private slots:
        /**
        * Slot - Used as a mechanism to trigger a refresh if the user believes
        *        something has changed or been added to the project.
        */
        void refreshButtonClicked();

    private:
        /**
        * This defines the layout for projects
        * @return a QGroupBox layout widget
        */
        QGroupBox *projectGroup();
        QPushButton *refreshButton;
    };
}

#endif
