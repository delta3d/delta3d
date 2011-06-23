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

#ifndef DELTA_LIBRARY_PATHS_EDITOR
#define DELTA_LIBRARY_PATHS_EDITOR

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
   class DT_QT_EXPORT LibraryPathsEditor : public QDialog
   {
      Q_OBJECT

   public:
      /// Constructor
      LibraryPathsEditor(QWidget* parent, const std::string& fileBrowserStartDir);

      /// Destructor
      virtual ~LibraryPathsEditor();

      /// Are any items selected?
      bool AnyItemsSelected() const;

   public slots:
      /// Pop up the file browser for libraries
      void SpawnFileBrowser();

      /// Confirm deletion of libraries
      void SpawnDeleteConfirmation();

      /// Received when the path selection state changes
      void RefreshButtons();

      /// Shift the current path up 1 position
      void ShiftPathUp();

      /// Shift the current path down 1 position
      void ShiftPathDown();

   private:
      /// The visible list of paths
      QListWidget* pathView;

      /// Our buttons
      QPushButton* deletePath;
      QPushButton* upPath;
      QPushButton* downPath;

      std::string mFileBrowserDir;
      // private function to obtain the library paths from the map
      // and convert them to QT format
      void GetPathNames(std::vector<QListWidgetItem*>& items) const;
      void RefreshPaths();
   };

} // namespace dtEditQt

#endif // DELTA_LIBRARY_PATHS_EDITOR
