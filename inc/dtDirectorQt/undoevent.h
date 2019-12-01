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

#ifndef DIRECTORQT_UNDO_EVENT
#define DIRECTORQT_UNDO_EVENT

#include <osg/Referenced>
#include <vector>
#include <string>

namespace dtDirector
{
   class DirectorEditor;

   /**
    * This class is a specific undo event.
    */
   class UndoEvent: public osg::Referenced
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  editor  The editor.
       */
      UndoEvent(DirectorEditor* editor);

      /**
       * Set the description of the event.
       *
       * @param[in]  description  The description of the undo action.
       */
      void SetDescription(const std::string& description);

      /**
       * Retrieves the description of the event.
       */
      const std::string& GetDescription() const;

      /**
       * Perform undo.
       */
      virtual void Undo() = 0;

      /**
       * Perform redo.
       */
      virtual void Redo() = 0;

   protected:

      /**
       * Destructor.
       */
      virtual ~UndoEvent() = 0;

      DirectorEditor*   mEditor;
      std::string       mDescription;
   };
}

#endif // DIRECTORQT_UNDO_EVENT
