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

#ifndef DIRECTORQT_UNDO_ADD_IMPORT_EVENT
#define DIRECTORQT_UNDO_ADD_IMPORT_EVENT

#include <dtDirectorQt/undoremoveimportevent.h>

namespace dtDirector
{
   /**
    * Undo event for adding an imported script.
    */
   class UndoAddImportEvent: public UndoRemoveImportEvent
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  editor  The editor.
       * @param[in]  script  The script that was imported.
       */
      UndoAddImportEvent(DirectorEditor* editor, const std::string& script);

      /**
       * Perform undo.
       */
      virtual void Undo();

      /**
       * Perform redo.
       */
      virtual void Redo();

   protected:

      /**
       * Destructor.
       */
      virtual ~UndoAddImportEvent();
   };
}

#endif // DIRECTORQT_UNDO_ADD_IMPORT_EVENT
