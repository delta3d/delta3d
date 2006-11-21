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
 * William E. Johnson II
 */
#ifndef DELTA_TEST_AAR_GAME_EVENT
#define DELTA_TEST_AAR_GAME_EVENT

#include <dtCore/refptr.h>
#include <dtDAL/gameevent.h>
#include <dtCore/refptr.h>
#include "export.h"

class TEST_AAR_EXPORT TestAARGameEvent
{
   public:
   
      static dtCore::RefPtr<dtDAL::GameEvent> EVENT_START_RECORD;
      static dtCore::RefPtr<dtDAL::GameEvent> EVENT_BOX_PLACED;
      static dtCore::RefPtr<dtDAL::GameEvent> EVENT_PLAYER_LEFT;
      static dtCore::RefPtr<dtDAL::GameEvent> EVENT_PLAYER_RIGHT;
      static dtCore::RefPtr<dtDAL::GameEvent> EVENT_PLAYER_FORWARD;
      static dtCore::RefPtr<dtDAL::GameEvent> EVENT_PLAYER_BACKWARD;

      /**
       * Static function called to ensure the statics are initialized and 
       * added to the event manager properly
       */
      static void InitEvents();

   protected:
      /// Constructor
      TestAARGameEvent();

      /// Destructor
      virtual ~TestAARGameEvent();
};
#endif
