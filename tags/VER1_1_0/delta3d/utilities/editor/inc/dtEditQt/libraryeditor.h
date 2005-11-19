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

#ifndef _LIBRARY_EDITOR_H_
#define _LIBRARY_EDITOR_H_

#include <QDialog>
#include <vector>
#include <osg/ref_ptr>
#include "dtDAL/actorproxy.h"

class QListWidget;
class QGridLayout;
class QPushButton;
class QStringList;
class QListWidgetItem;

namespace dtEditQt
{
    class LibraryEditor : public QDialog
    {
        Q_OBJECT

    public:
        
        /// Constructor
        LibraryEditor(QWidget *parent = NULL);

        /// Destructor
        virtual ~LibraryEditor();
        
    signals:

        /// This signal is emitted if no libraries in the list
        /// are selected so the delete button knows to grey 
        /// itself out
        void noLibsSelected();

        /// This signal enables the remove library button
        void librarySelected();

    public slots:
        
        /// Pop up the file browser for libraries
        void spawnFileBrowser();
        
        /// Confirm deletion of libraries
        void spawnDeleteConfirmation();

        /// Handle a deletion failure
        void handleFailure(const int code, const std::string &errorMsg = "");

        /// Received when a library is currently selected
        void enableButtons();

        /// Disable the delete button if no libraries are selected
        void disableButtons();

        /// Shift the current library up 1 position
        void shiftLibraryUp();

        /// Shift the current library down 1 position
        void shiftLibraryDown();

    private:

        /// The visible list of libraries
        QListWidget *libView;
        
        /// Our buttons
        QPushButton *deleteLib, *upLib, *downLib;

        unsigned int numActorsInScene;

        // private function to obtain the library names from the map
        // and convert them to QT format
        std::vector<QListWidgetItem*>& getMapLibNames() const;

        // private function to refresh the list of libraries after
        // the user has imported his own libraries
        void refreshLibraries();
    };

}
#endif
