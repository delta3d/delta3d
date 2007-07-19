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
 * @author Curtiss Murphy
 */
#ifndef __GM_TUTORIAL_MY_GAME_ENTRY_POINT__
#define __GM_TUTORIAL_MY_GAME_ENTRY_POINT__

#include "export.h"
#include <dtCore/refptr.h>
#include <dtGame/gameentrypoint.h>


namespace dtGame
{
   class GameManager;
   class ServerLoggerComponent;
   class LogController;
}

/**
 * Our entry point into the game.  The GameStart.exe application can load this like this:
 *     "GameStartd.exe TutorialGameActors"
 * We create our Game Manager and components in this class.
 */ 
class TUTORIAL_TANK_EXPORT MyGameEntryPoint: public dtGame::GameEntryPoint
{
   public:
      MyGameEntryPoint() { };
      virtual ~MyGameEntryPoint();

      // Called to do early initializtion.  Grab your command line params here.
      // We just use the base implementation
      virtual void Initialize(dtGame::GameApplication& app, int argc, char **argv)
         throw(dtUtil::Exception);

      // Create your game manager.
      // We just use the base implementation
      // virtual dtCore::RefPtr<dtGame::GameManager> CreateGameManager(dtCore::Scene& scene);

      // Called just before your application's game loop starts.  This is your main 
      // opportunity to create components, load maps, create unique actors, etc...
      virtual void OnStartup();
   private:
      std::string mMapName;
      dtCore::RefPtr<dtGame::LogController> mLogController;
      dtCore::RefPtr<dtGame::ServerLoggerComponent> mServerLogger;
      bool mInPlaybackMode; 
};
#endif
