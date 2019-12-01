/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
#ifndef DELTA_FIRE_FIGHTER_MESSAGE_TYPE
#define DELTA_FIRE_FIGHTER_MESSAGE_TYPE

#include <dtGame/messagetype.h>
#include <fireFighter/export.h>

DT_DECLARE_MESSAGE_TYPE_CLASS_BEGIN(FireFighterMessageType, FIRE_FIGHTER_EXPORT)
   static const FireFighterMessageType GAME_STATE_CHANGED;
      static const FireFighterMessageType ITEM_ACQUIRED;
      static const FireFighterMessageType ITEM_ACTIVATED;
      static const FireFighterMessageType ITEM_DEACTIVATED;
      //static const FireFighterMessageType ITEM_USED;
      static const FireFighterMessageType ITEM_SELECTED;
      static const FireFighterMessageType ITEM_INTERSECTED;
      static const FireFighterMessageType MISSION_COMPLETE;
      static const FireFighterMessageType MISSION_FAILED;
      static const FireFighterMessageType HELP_WINDOW_OPENED;
      static const FireFighterMessageType HELP_WINDOW_CLOSED;
DT_DECLARE_MESSAGE_TYPE_CLASS_END()

#endif
