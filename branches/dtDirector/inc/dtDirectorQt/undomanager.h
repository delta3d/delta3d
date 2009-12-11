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

#include <dtCore/refptr.h>
#include <dtCore/uniqueid.h>

#include <QtCore/QObject>

#include <osg/Referenced>

#include <vector>
#include <stack>

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
       * Destructor.
       */
      virtual ~UndoEvent();

      /**
       * Perform undo.
       */
      virtual void Undo();

      /**
       * Perform redo.
       */
      virtual void Redo();

   protected:

      DirectorEditor*   mEditor;
   };

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
       * Destructor.
       */
      virtual ~UndoMultipleEvent();

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

   protected:

      std::vector<dtCore::RefPtr<UndoEvent> > mEvents;
   };

   /**
    * Undo event for a property change.
    */
   class UndoPropertyEvent: public UndoEvent
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  editor  The editor.
       * @param[in]  id        The ID of the node.
       * @param[in]  propName  The property name.
       * @param[in]  oldValue  The old property value.
       * @param[in]  newValue  The new property value.
       */
      UndoPropertyEvent(DirectorEditor* editor,
                        const dtCore::UniqueId& id,
                        const std::string& propName,
                        const std::string& oldValue,
                        const std::string& newValue);

      /**
       * Destructor.
       */
      virtual ~UndoPropertyEvent();

      /**
       * Perform undo.
       */
      virtual void Undo();

      /**
       * Perform redo.
       */
      virtual void Redo();

   protected:

      dtCore::UniqueId mID;
      std::string      mPropName;
      std::string      mOldValue;
      std::string      mNewValue;
   };

   /**
    * This class handles undo and redo for the Director Editor.
    */
   class UndoManager: public QObject
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
      bool CanUndo() {return !mUndoEvents.empty();}

      /**
       * Retrieves whether there are any redo events to perform.
       */
      bool CanRedo() {return !mRedoEvents.empty();}

      /**
       * Begins and ends a group of multiple transaction events.
       */
      void BeginMultipleEvents();
      void EndMultipleEvents();

      /**
       * Property change event.
       *
       * @param[in]  id        The ID of the node.
       * @param[in]  propName  The property name.
       * @param[in]  oldValue  The old property value.
       * @param[in]  newValue  The new property value.
       */
      void PropertyChangeEvent(const dtCore::UniqueId& id, const std::string& propName, const std::string& oldValue, const std::string& newValue);

   protected:

      /**
       * Adds an undo event to the stack.
       *
       * @param[in]  event  The event to add.
       */
      void AddEvent(UndoEvent* event);

   private:

      DirectorEditor*   mEditor;

      std::stack<dtCore::RefPtr<UndoEvent> > mUndoEvents;
      std::stack<dtCore::RefPtr<UndoEvent> > mRedoEvents;

      std::stack<dtCore::RefPtr<UndoMultipleEvent> > mMultipleEventStack;
   };
}

#endif // DIRECTORQT_UNDO_MANAGER