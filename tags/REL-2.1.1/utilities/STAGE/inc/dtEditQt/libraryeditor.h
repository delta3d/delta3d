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

#ifndef DELTA_LIBRARY_EDITOR
#define DELTA_LIBRARY_EDITOR

#include <QtGui/QDialog>
#include <vector>
#include <dtDAL/actorproxy.h>

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
         void getMapLibNames(std::vector<QListWidgetItem*>& items) const;

         // private function to refresh the list of libraries after
         // the user has imported his own libraries
         void refreshLibraries();
   };

}
#endif
