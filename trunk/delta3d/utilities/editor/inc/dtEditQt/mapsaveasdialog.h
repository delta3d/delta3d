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
* @author William E. Johnson II
*/

#ifndef _MAP_SAVE_AS_DIALOG_H_
#define _MAP_SAVE_AS_DIALOG_H_

#include <QDialog>

class QLineEdit;
class QTextEdit;
class QCloseEvent;
class QPushButton;

namespace dtDAL 
{
    class Map;
}

namespace dtEditQt
{
    class MapSaveAsDialog : public QDialog
    {
        Q_OBJECT

    public:

        /// Constructor
        MapSaveAsDialog(QWidget *parent = NULL);

        /// Destructor
        virtual ~MapSaveAsDialog() { }

        /// Retrieves the name of the map the user entered
        std::string getMapName(); 

        /// Retrieves the filename of the map
        std::string getMapFileName();

        /// Retrieves the description of the map
        std::string getMapDescription();

    public slots:

        /// slot for receiving the text changing signal
        void edited(const QString &newText);

    private:

        QPushButton *okButton;
        QLineEdit *nameEdit;
        QTextEdit *descEdit;
        QLineEdit *fileEdit;
    };

}
#endif
