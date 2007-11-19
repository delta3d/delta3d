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
 * William E. Johnson II
 */

#ifndef DELTA_LIBRARY_PATHS_EDITOR
#define DELTA_LIBRARY_PATHS_EDITOR

#include <QtGui/QDialog>
#include <vector>

class QListWidget;
class QGridLayout;
class QPushButton;
class QStringList;
class QListWidgetItem;

namespace dtEditQt
{
   class LibraryPathsEditor : public QDialog
   {
         Q_OBJECT
         
      public:
         
         /// Constructor
         LibraryPathsEditor(QWidget *parent = NULL);
         
         /// Destructor
         virtual ~LibraryPathsEditor();
         
      signals:
         
         /// This signal is emitted if no libraries in the list
         /// are selected so the delete button knows to grey 
         /// itself out
         void noPathsSelected();
      
         /// This signal enables the remove library button
         void pathSelected();
      
      public slots:
         
         /// Pop up the file browser for libraries
         void spawnFileBrowser();
      
         /// Confirm deletion of libraries
         void spawnDeleteConfirmation();
      
         /// Received when a path is currently selected
         void enableButtons();
      
         /// Disable the delete button if no paths are selected
         void disableButtons();
      
         /// Shift the current path up 1 position
         void shiftPathUp();
      
         /// Shift the current path down 1 position
         void shiftPathDown();
      
      private:
         
         /// The visible list of paths
         QListWidget *pathView;
      
         /// Our buttons
         QPushButton *deletePath, *upPath, *downPath;
      
         unsigned int numActorsInScene;
      
         // private function to obtain the library paths from the map
         // and convert them to QT format
         void getPathNames(std::vector<QListWidgetItem*>& items) const;
         void refreshPaths();
   };

}
#endif
