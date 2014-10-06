/* -*-c++-*-
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
 * Matthew W. Campbell
 */
#ifndef DELTA_GAMEEVENT
#define DELTA_GAMEEVENT

#include <string>

#include <dtCore/export.h>
#include <dtCore/base.h>

#include <dtUtil/getsetmacros.h>

namespace dtCore
{
   /**
    * This is a simple data class which represents a game event.  Game events
    * are used as a simple means to notify the game simulation that something occured.
    * Events themselves are not meant to contain specific data about the event.
    */
   class DT_CORE_EXPORT GameEvent : public dtCore::Base
   {
      public:

         /**
          * Constructs an event with no name.
          * @note Game events are generally useless without a name to signal
          *    what the even is.  Although events are guarenteed to be unique
          *    using their unique identifier, it logically makes no sense to
          *    leave an event unnamed.
          */
         GameEvent();

         /**
          * Constructs the game event with the specified name and optional
          * description.
          * @param name The name to give to this event.
          * @param description An optional description to give to the event.
          */
         GameEvent(const std::string& name, const dtUtil::RefString& description=dtUtil::RefString());

         DT_DECLARE_ACCESSOR(dtUtil::RefString, Description);

         bool operator==(const GameEvent& toCompare) const;

      protected: 
         /// Destructor
         virtual ~GameEvent() { }
   };
}

#endif
