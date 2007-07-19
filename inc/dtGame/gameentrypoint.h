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
 * David Guthrie
 */
#ifndef DELTA_GAME_ENTRY_POINT
#define DELTA_GAME_ENTRY_POINT

#include <dtCore/refptr.h>
#include <dtUtil/exception.h>
#include <dtUtil/breakoverride.h>
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
      	
         /** 
          * The destructor of the GameEntryPoint.
          * @note This method will automatically call GameManager::Shutdown() for you.
          */
         virtual ~GameEntryPoint() 
         {
            mGameManager->Shutdown();
            mGameManager = NULL;
         }
         
         /**
          * Called to initialize the game application.  This gets called from the
          * dtGame::GameApplication::Config() method, but before
          * dtABC::Application::Config().  This allows some defining of the 
          * default Camera and Window, such as setting up stencil buffers.
          * @param app the current application
          * @param argc number of startup arguments.
          * @param argv array of string pointers to the arguments.
          * @throws dtUtil::Exception if initialization fails.
          */
         virtual void Initialize(GameApplication& app, int argc, char **argv) { }
         
         /**
          * Override this method to create your own custom GameManager, otherwise
          * leave it as is and it will return back the default GameManager for you.
          */
         virtual dtCore::ObserverPtr<GameManager> CreateGameManager(dtCore::Scene& scene) 
         { 
            mGameManager = new GameManager(scene);
            return mGameManager.get();
         }
         
         /**
          * Called after all startup related code is run.  At this point, the
          * dtGame::GameApplication has been configured and ready to be used.
          * Override this method to perform any specific start up functionality that
          * the GameEntryPoint needs to do.
          */
         virtual void OnStartup() = 0;

         /**
          * This is the notice to the GameEntryPoint that the application is
          * quitting.  Perform any cleanup that needs to be done here.  
          * @note This is called from the GameApplication destructor and is followed
          * by the GameEntryPoint destructor.
          */
         virtual void OnShutdown() { }

         /**
          * A accessor method to the game manager
          * If custom behavior is desired, this can be overridden
          * @return mGameManager
          */
         virtual dtGame::GameManager* GetGameManager() { return mGameManager.get(); }

         /**
          * Sets the game manager in case custom behavior is desired
          * @param gm The new game manager to use
          */
         virtual void SetGameManager(dtGame::GameManager &gm) { mGameManager = &gm; }

      private:

         dtCore::RefPtr<dtGame::GameManager> mGameManager;

         // Deprecated in version 1.5
         BREAK_OVERRIDE(OnStartup(dtGame::GameManager&));
         BREAK_OVERRIDE(OnShutdown(dtGame::GameManager&));
   };

}

#endif /*DELTA_GAME_ENTRY_POINT*/
