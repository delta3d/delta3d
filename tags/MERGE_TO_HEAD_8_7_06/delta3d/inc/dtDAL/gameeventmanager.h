/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Matthew W. Campbell
 */

#ifndef DELTA_GAMEEVENTMANAGER
#define DELTA_GAMEEVENTMANAGER

#include <dtCore/refptr.h>
#include <dtCore/uniqueid.h>
#include <osg/Referenced>
#include "dtDAL/export.h"

#include <string>
#include <vector>
#include <map>

namespace dtDAL
{
   class GameEvent;

   class DT_DAL_EXPORT GameEventManager : public osg::Referenced
   {
      public:

         /**
          * Gets the single global instance of this class.
          * @return The singleton instance.
          */
         static GameEventManager &GetInstance()
         {
            if (mInstance == NULL)
               mInstance = new GameEventManager();
            return *mInstance;
         }

         /**
          * Adds a new event to the game event manager.
          * @param event The new event to register with the manager.
          */
         void AddEvent(GameEvent &event);

         /**
          * Removes an existing event from the game event manager.
          * @param event The event to remove.  If it is not currently in the
          *  manager, this method is a no-op.
          */
         void RemoveEvent(GameEvent &event);

         /**
          * Removes the game event with the specified unique id from the manager.
          * @param id The unique id of the game event to remove.  If the event is not
          *   currently in the manager, this method is a no-op.
          */
         void RemoveEvent(const dtCore::UniqueId &id);

         /**
          * Clears all the currently registered events from the manager.
          */
         void ClearAllEvents();

         /**
          * Fills the specified vector with the current list of game events in the
          * manager.
          * @param toFill The vector to fill with game events.  This is cleared before it is
          *   filled.
          */
         void GetAllEvents(std::vector<dtCore::RefPtr<GameEvent> > &toFill);

         /**
          * Gets the event specified by the unique id.
          * @param id The id of the event to retreive.
          * @return The event in question or NULL if an event with the specified unique id could
          *   not be found.
          */
         GameEvent *FindEvent(const dtCore::UniqueId &id);

         /**
          * Gets the event with the specified name.
          * @param name Name of the event to search fo.
          * @return The event with the specified name, or NULL if an event could not be found.
          * @note Since events may have the same name, this method will return the FIRST event
          *   matching the specified name.
          */
         GameEvent *FindEvent(const std::string &name);

         /**
          * Gets the number of events currently registered with the event manager.
          * @return The number of events.
          */
         unsigned int GetNumEvents() const { return mEventList.size(); }

      private:
         /**
          * Constructs the shader manager.  Since this is a singleton class, this is private.
          */
         GameEventManager();

         /**
          * Destroys the shader manager.
          */
         virtual ~GameEventManager();

         GameEventManager(const GameEventManager &rhs) { }
         GameEventManager &operator=(const GameEventManager &rhs) { return *this; }

         ///Single instance of this class.
         static dtCore::RefPtr<GameEventManager> mInstance;

         ///Map of game events currently owned by the game event manager.
         std::map<dtCore::UniqueId,dtCore::RefPtr<GameEvent> > mEventList;
   };

}

#endif
