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
#ifndef DELTA_FIRE_FIGHTER_GAME_STATE
#define DELTA_FIRE_FIGHTER_GAME_STATE

#include <dtUtil/enumeration.h>
#include <fireFighter/export.h>

class FIRE_FIGHTER_EXPORT GameState : public dtUtil::Enumeration
{
   DECLARE_ENUM(GameState);

   public:

      static GameState STATE_MENU;
      static GameState STATE_INTRO;
      static GameState STATE_RUNNING;
      static GameState STATE_DEBRIEF;
      static GameState STATE_UNKNOWN;
   
   private:

      GameState(const std::string &name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
};

#endif
