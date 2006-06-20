/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author David Guthrie
 */
#ifndef DELTA_GAME_ENTRY_POINT
#define DELTA_GAME_ENTRY_POINT

#include <dtCore/refptr.h>
#include <dtUtil/exception.h>
#include <dtGame/gamemanager.h>

namespace dtCore 
{
   class Scene;
}

namespace dtGame
{
   //class GameManager;
   class GameApplication;

   /**
    * @class GameEntryPoint
    * A class for specifying the entry point into a Game so that the entire game can be defined in a 
    * Game library and loaded at runtime.
    */
   class GameEntryPoint
   {
      public:
      	GameEntryPoint() {}
      	virtual ~GameEntryPoint() {}
         
         /**
          * Called to initialize the game application.
          * @param app the current application
          * @param argc number of startup arguments.
          * @param argv array of string pointers to the arguments.
          * @throwns dtUtil::Exception if initialization fails.
          */
         virtual void Initialize(GameApplication& app, int argc, char **argv) 
            throw (dtUtil::Exception) {}
         
         /**
          * Override the method to create the game manager.
          */
         virtual dtCore::RefPtr<GameManager> CreateGameManager(dtCore::Scene& scene) { return new GameManager(scene); }
         
         /**
          * Called after all startup related code is run.
          * @param gameManager The game manager to init
          */
         virtual void OnStartup(dtGame::GameManager &gameManager) = 0;
   };

}

#endif /*DELTA_GAME_ENTRY_POINT*/
