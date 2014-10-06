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

#ifndef DIRECTORQT_UNDO_MANAGER
#define DIRECTORQT_UNDO_MANAGER

#include <dtDirectorQt/export.h>
#include <dtDirectorQt/undoevent.h>
#include <dtDirectorQt/undomultipleevent.h>

#include <dtCore/refptr.h>

#include <QtCore/QObject>

#include <stack>

namespace dtDirector
{
   class DirectorEditor;

   /**
    * This class handles undo and redo for the Director Editor.
    */
   class DT_DIRECTOR_QT_EXPORT UndoManager: public QObject
   {
      Q_OBJECT
   public:

      /**
       * Constructor.
       *
       * @param[in]  editor  The editor.
       */
      UndoManager(DirectorEditor* editor);

      /**
       * Destructor.
       */
      ~UndoManager();

      /**
       * Retrieves whether the script has been modified.
       */
      bool IsModified();

      /**
       * Event handler for when the graph has been saved.
       */
      void OnSaved();

      /**
       * Reverts all changes since the last unchanged state.
       */
      void Revert();

      /**
       * Clears all undo/redo events.
       */
      void Clear();

      /**
       * Perform an undo action.
       */
      void Undo();

      /**
       * Perform a redo action.
       */
      void Redo();

      /**
       * Retrieves whether there are any undo events to perform.
       */
      bool CanUndo() const {return !mUndoEvents.empty();}

      /**
       * Retrieves whether there are any redo events to perform.
       */
      bool CanRedo() const {return !mRedoEvents.empty();}

      /**
       * Retrieves the undo or redo description.
       */
      std::string GetUndoDescription() const;
      std::string GetRedoDescription() const;

      /**
       * Retrieves the editor.
       */
      DirectorEditor* GetEditor() {return mEditor;}

      /**
       * Begins and ends a group of multiple transaction events.
       */
      void BeginMultipleEvents(const std::string& description);
      void EndMultipleEvents();

      /**
       *	Undoes the contents of a currently active multiple event stack and
       * removes that event from the manager.
       */
      void UndoCurrentMultipleEvent();

      /**
       * Adds an undo event to the stack.
       *
       * @param[in]  event  The event to add.
       */
      void AddEvent(UndoEvent* event);

   protected:

   private:

      DirectorEditor*   mEditor;
      int               mModifyIndex;

      std::stack<dtCore::RefPtr<UndoEvent> > mUndoEvents;
      std::stack<dtCore::RefPtr<UndoEvent> > mRedoEvents;

      std::stack<dtCore::RefPtr<UndoMultipleEvent> > mMultipleEventStack;
   };
}

#endif // DIRECTORQT_UNDO_MANAGER
