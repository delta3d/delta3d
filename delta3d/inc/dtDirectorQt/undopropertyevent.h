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

#ifndef DIRECTORQT_UNDO_PROPERTY_EVENT
#define DIRECTORQT_UNDO_PROPERTY_EVENT

#include <dtDirectorQt/undoevent.h>
#include <dtDirector/node.h>

namespace dtCore
{
   class ActorProperty;
}

namespace dtDirector
{
   class DirectorEditor;

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
                        const dtDirector::ID& id,
                        const std::string& propName,
                        const std::string& oldValue,
                        const std::string& newValue);

      /**
       * Retrieves the property for the event.
       */
      dtCore::ActorProperty* GetProperty();

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
      virtual ~UndoPropertyEvent();

      dtDirector::ID   mID;
      std::string      mPropName;
      std::string      mOldValue;
      std::string      mNewValue;
   };
}

#endif // DIRECTORQT_UNDO_PROPERTY_EVENT
