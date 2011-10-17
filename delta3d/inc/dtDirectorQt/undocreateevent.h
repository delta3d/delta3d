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

#ifndef DIRECTORQT_UNDO_CREATE_EVENT
#define DIRECTORQT_UNDO_CREATE_EVENT

#include <dtDirectorQt/undodeleteevent.h>

namespace dtDirector
{
   struct ID;

   /**
    * Undo event for a property change.
    */
   class UndoCreateEvent: public UndoDeleteEvent
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  editor    The editor.
       * @param[in]  nodeID    The ID of the node being deleted.
       * @param[in]  parentID  The ID of the parent graph that owns this node.
       */
      UndoCreateEvent(DirectorEditor* editor,
                      const dtDirector::ID& nodeID,
                      const dtDirector::ID& parentID);

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
      virtual ~UndoCreateEvent();
   };
}

#endif // DIRECTORQT_UNDO_CREATE_EVENT
