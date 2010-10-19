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

#ifndef DIRECTOR_LIBRARY_EDITOR
#define DIRECTOR_LIBRARY_EDITOR


#include <dtQt/baselibrarylisteditor.h>

namespace dtDirector
{
   class DirectorEditor;

   class LibraryEditor : public dtQt::BaseLibraryListEditor
   {
      Q_OBJECT

   public:

      /**
       * Constructor.
       *
       * @param[in]  editor  The editor.
       */
      LibraryEditor(DirectorEditor* editor = NULL);

      /**
       * Destructor.
       */
      virtual ~LibraryEditor();

   public slots:

      /**
       * Creates and displays the file browser.
       */
      virtual void SpawnFileBrowser();

      /**
       * Confirms the deletion of libraries.
       */
      virtual void SpawnDeleteConfirmation();

      /**
       * Shifts a library up.
       */
      virtual void ShiftLibraryUp();

      /**
       * Shifts a library down.
       */
      virtual void ShiftLibraryDown();

   protected:

      /**
       * Retrieves the library names and converts them to Qt format.
       */
      virtual void GetLibraryNames(std::vector<QListWidgetItem*>& items) const;

      DirectorEditor* mEditor;
      std::string mCurrentDir;
   };

} // namespace dtDirector

#endif // DIRECTOR_LIBRARY_EDITOR
