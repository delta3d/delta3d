/* -*-c++-*-
 * TutorialLibrary - MyGameEntryPoint (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
   MyGameEntryPoint() {}
   virtual ~MyGameEntryPoint();

   // Called to do early initializtion.  Grab your command line params here.
   // We just use the base implementation
   virtual void Initialize(dtABC::BaseABC& app, int argc, char** argv);

   // Create your game manager.
   // We just use the base implementation
   // virtual dtCore::RefPtr<dtGame::GameManager> CreateGameManager(dtCore::Scene& scene);

   // Called just before your application's game loop starts.  This is your main 
   // opportunity to create components, load maps, create unique actors, etc...
   virtual void OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gamemanager);

private:
   std::string mMapName;
   dtCore::RefPtr<dtGame::LogController> mLogController;
   dtCore::RefPtr<dtGame::ServerLoggerComponent> mServerLogger;
   bool mInPlaybackMode; 
};

#endif // __GM_TUTORIAL_MY_GAME_ENTRY_POINT__
