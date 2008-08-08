/* -*-c++-*-
* Delta3D Simulation Training And Game Editor (STAGE)
* STAGE - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
* 
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
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

         /// Are any items selected?
         bool AnyItemsSelected() const;

      public slots:
         
         /// Pop up the file browser for libraries
         void spawnFileBrowser();
      
         /// Confirm deletion of libraries
         void spawnDeleteConfirmation();
      
         /// Received when the path selection state changes
         void refreshButtons();

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
