/*
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
#ifndef DELTA_GAME_APPLICATION_LOADER
#define DELTA_GAME_APPLICATION_LOADER

#include <dtABC/application.h>
#include <dtUtil/librarysharingmanager.h>
#include <dtGame/export.h>
#include <dtGame/gamemanager.h>
#include <dtCore/observerptr.h>

namespace dtGame
{
   class GameEntryPoint;

   /**
    * @class GameApplication
    * Base application for a GameManager app.  It loads a game entry point library to use at startup.
    */
   class DT_GAME_EXPORT GameApplicationLoader
   {
      public:
         /**
          * Function pointer to the create function for .
          */
         typedef GameEntryPoint* (*CreateEntryPointFn)();
         /**
          * Function pointer to the destroy function implemented in the entry point library.
          * This is called just before closing the dynamic library owning the
          * entry point.
          */
         typedef void (*DestroyEntryPointFn)(GameEntryPoint*);

         /**
          * Constructor.  It takes the command line parameters.
          * @param argc the number of arguments
          * @param argv An array of string pointers with the command line data.
          * @param configFileName the name of the xml config file.  Defaults to config.xml
          * @param window a window instance ot use instead of creating one.
          */
         GameApplicationLoader(int argc, char** argv);

         ///Destructor
         ~GameApplicationLoader();

         /**
          * Configures the application, loads the game library,
          * and calls the entry point to configure the game.
          * After loading the library, it searches for the following interfaces by name:
          * - "CreateGameEntryPoint", with the signature of CreateEntryPointFn
          * - "DestroyGameEntryPoint", with the signature of DestroyEntryPointFn
          */
         void Config(const std::string& configFileName = "config.xml");

         /// Runs the internal application
         void Run();

         ///@return the platform independent library name.
         const std::string& GetGameLibraryName() const { return mLibName; }

         ///Sets the name of the library to load that has the game.  This should be a platform independent name (no lib, .so, .dll, etc.).
         void SetGameLibraryName(const std::string& newName) { mLibName = newName; }

         /// @return the game manager used by the application.
         dtGame::GameManager* GetGameManager() { return mGameManager.get(); }

         /**
          * Override the default, internally created GameManager with the
          * supplied parameter.  Useful if you have a GameManager derivative
          * this class should use instead of the default.
          * @param gameManager : the GameManager this instance should use
          */
         void SetGameManager(dtGame::GameManager* gameManager);

      protected:


      private:

         std::string mLibName;

         int mArgc;
         char** mArgv;

         dtCore::RefPtr<dtGame::GameManager> mGameManager;
         dtCore::RefPtr<dtABC::BaseABC> mApplication;
         dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> mEntryPointLib;
         dtGame::GameEntryPoint* mEntryPoint;
         CreateEntryPointFn mCreateFunction;
         DestroyEntryPointFn mDestroyFunction;
   };

}
#endif
