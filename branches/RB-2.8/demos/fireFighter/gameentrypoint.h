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
#ifndef DELTA_FIRE_FIGHTER_GAME_ENTRY_POINT
#define DELTA_FIRE_FIGHTER_GAME_ENTRY_POINT

#include <dtGame/gameentrypoint.h>
#include <dtGame/gameapplication.h>
#include <fireFighter/export.h>

namespace dtLMS
{
   class LmsComponent;
}

class FIRE_FIGHTER_EXPORT FireFighterGameEntryPoint : public dtGame::GameEntryPoint
{
   public:

      /// Constructor
      FireFighterGameEntryPoint();

      /**
       * Called to initialize the game application.
       * @param app the current application
       */
      virtual void Initialize(dtABC::BaseABC& app, int argc, char **argv);

      /**
       * Called after all startup related code is run.
       * @param app the current application
       */
      virtual void OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gamemanager);

      /**
       * Called when it is time to shut'r down
       * @param The gm to shut down
       */
      virtual void OnShutdown(dtABC::BaseABC& app, dtGame::GameManager& gamemanager);

   protected:

      /// Destructor
      virtual ~FireFighterGameEntryPoint();

   private:
      
      bool mUseLMS; 
      dtCore::RefPtr<dtLMS::LmsComponent> mLmsComponent;
};
#endif
