/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#ifndef DIRECTOR_IMPORT_EDITOR
#define DIRECTOR_IMPORT_EDITOR


#include <QtGui/QDialog>
#include <QtGui/QListWidgetItem>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

namespace dtDirector
{
   class DirectorEditor;
   class DirectorGraph;

   class ImportEditor : public QDialog
   {
      Q_OBJECT

   public:

      /**
       * Constructor.
       *
       * @param[in]  editor  The editor.
       */
      ImportEditor(DirectorEditor* editor = NULL);

      /**
       * Destructor.
       */
      virtual ~ImportEditor();

   public slots:

      /**
       * Creates and displays the file browser.
       */
      void SpawnFileBrowser();

      /**
       * Confirms the deletion of libraries.
       */
      void SpawnDeleteConfirmation();

      /**
       *	Received when a library is currently selected.
       */
      void EnableButtons(int row);

   protected:

      /**
       * Retrieves the imported script names and converts them to Qt format.
       */
      void GetScriptNames(std::vector<QListWidgetItem*>& items) const;

      /**
       *	Refreshes the import list.
       */
      void Refresh();

      DirectorEditor* mEditor;
      QString mCurrentDir;

      QListWidget* mImportView;
      QPushButton* mDeleteButton;
   };

} // namespace dtDirector

#endif // DIRECTOR_IMPORT_EDITOR
