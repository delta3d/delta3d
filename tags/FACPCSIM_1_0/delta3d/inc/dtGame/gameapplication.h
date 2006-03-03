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
 * @author David Guthrie
 */
#ifndef DELTA_GAME_APPLICATION
#define DELTA_GAME_APPLICATION

#include <dtCore/refptr.h>
#include <dtABC/application.h>
#include <dtUtil/librarysharingmanager.h>

namespace dtGame
{
   class GameManager;
   class GameEntryPoint;
   
   /**
    * @class GameApplication
    * Base application for a GameManager app.  It loads a game entry point library to use at startup.  
    */
   class GameApplication: public dtABC::Application
   {
      DECLARE_MANAGEMENT_LAYER(GameApplication)

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

         GameApplication();
         virtual ~GameApplication();
         
         /**
          * Configure the application, load the game library,
          * and call the entry point to configure the game.
          */
         virtual void Config();
      private:
         dtCore::RefPtr<GameManager> mGameManager;
         dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> mEntryPointLib;
         dtGame::GameEntryPoint* mEntryPoint;
         CreateEntryPointFn mCreateFunction;
         DestroyEntryPointFn mDestroyFunction;
   };

}
#endif
