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

#ifndef DIRECTORQT_UNDO_MULTIPLE_EVENT
#define DIRECTORQT_UNDO_MULTIPLE_EVENT

#include <dtDirectorQt/undoevent.h>

#include <dtCore/refptr.h>

#include <vector>

namespace dtDirector
{
   class DirectorEditor;

   /**
    * An undo event that holds multiple undo events.
    */
   class UndoMultipleEvent: public UndoEvent
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  editor  The editor.
       */
      UndoMultipleEvent(DirectorEditor* editor);

      /**
       * Perform undo.
       */
      virtual void Undo();

      /**
       * Perform redo.
       */
      virtual void Redo();

      /**
       * Adds an event to the stack.
       *
       * @param[in]  event  The event to add.
       */
      void AddEvent(UndoEvent* event);

      /**
       * Retrieves whether this event contains events.
       */
      bool HasEvents() {return !mEvents.empty();}


   protected:
         
      /**
       * Destructor.
       */
      virtual ~UndoMultipleEvent();

      std::vector<dtCore::RefPtr<UndoEvent> > mEvents;
   };
}

#endif // DIRECTORQT_UNDO_MULTIPLE_EVENT
