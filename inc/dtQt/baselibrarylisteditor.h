/* -*-c++-*-
 * Delta3D
 * Copyright 2009, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 * William E. Johnson II
 */

#ifndef DTQT_BASE_LIBRARY_EDITOR
#define DTQT_BASE_LIBRARY_EDITOR

#include <dtQt/export.h>
#include <QtGui/QDialog>
#include <vector>

class QListWidget;
class QGridLayout;
class QPushButton;
class QStringList;
class QListWidgetItem;

namespace dtQt
{
   class DT_QT_EXPORT BaseLibraryListEditor : public QDialog
   {
      Q_OBJECT

   public:
      enum Errors
      {
         ERROR_LIB_NOT_LOADED = 0,
         ERROR_OBJECTS_IN_LIB_EXIST,
         ERROR_INVALID_LIB,
         ERROR_UNKNOWN
      };

      /// Constructor
      BaseLibraryListEditor(QWidget* parent = NULL);

      /// Destructor
      virtual ~BaseLibraryListEditor();

      void showEvent(QShowEvent*);

   protected:
      /**
       * Call this from SpawnFileBrowser to get a library to use. It returns the system independent name of the library
       * first then the full path to the library.
       */
      std::pair<std::string, std::string> SelectLibraryToOpen(const std::string& startingDir);

      // private function to obtain the library names from the map
      // and convert them to QT format
      virtual void GetLibraryNames(std::vector<QListWidgetItem*>& items) const = 0;

      QListWidget& GetLibraryListWidget() { return *mLibView; };

      // function to refresh the list of libraries after
      // the user has imported his own libraries
      void RefreshLibraries();

   public slots:
      /**
       *  Pop up the file browser for libraries and adds it to the list.
       *  You can call SelectLibraryToOpen to pick the file
       */
      virtual void SpawnFileBrowser() = 0;

      /// Confirm deletion of libraries
      virtual void SpawnDeleteConfirmation() = 0;

      /// Handle a deletion failure
      void HandleFailure(const int code, const std::string& errorMsg = "");

      /// Received when a library is currently selected
      void EnableButtons(int row);

      /// Shift the current library up 1 position
      virtual void ShiftLibraryUp();

      /// Shift the current library down 1 position
      virtual void ShiftLibraryDown();

   private:
      /// The visible list of libraries
      QListWidget* mLibView;

      /// Our buttons
      QPushButton* mDeleteLib;
      QPushButton* mUpLib;
      QPushButton* mDownLib;

   };
}
#endif // DELTA_LIBRARY_EDITOR
