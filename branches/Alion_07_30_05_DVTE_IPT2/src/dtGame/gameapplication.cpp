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
#include <prefix/dtgameprefix-src.h>
#include <dtUtil/log.h>

#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>
#include <dtGame/gameentrypoint.h>
#include <dtGame/exceptionenum.h>

#include <dtUtil/exception.h>
//#include <dtAudio/audiomanager.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/generickeyboardlistener.h>
#include <dtABC/application.h>

namespace dtGame
{
   IMPLEMENT_MANAGEMENT_LAYER(GameApplication)

   GameApplication::GameApplication(int argc, char** argv): 
   dtABC::Application("config.xml"),
      mArgc(argc),
      mArgv(argv),
      mCreateFunction(NULL),
      mDestroyFunction(NULL)
   {
      RegisterInstance(this);
      mKeyboard->RemoveKeyboardListener(GetKeyboardListener());
   }

   GameApplication::~GameApplication()
   {
      try
      {
         mEntryPoint->OnShutdown(*mGameManager);
      }
      catch(const dtUtil::Exception &e)
      {
         e.LogException(dtUtil::Log::LOG_ALWAYS);
      }
      catch(...)
      {
         LOG_ALWAYS("Unknown exception caught in the destructor of GameApplication");
      }

      DeregisterInstance(this);

      GetScene()->RemoveAllDrawables();

      if (mGameManager.valid())
         mGameManager = NULL;

      if (mDestroyFunction != NULL)
      {
         mDestroyFunction(mEntryPoint);
         mEntryPoint = NULL;
      }
   }

   void GameApplication::Config()
   {
      Application::Config();
      dtUtil::LibrarySharingManager& lsm = dtUtil::LibrarySharingManager::GetInstance();
      std::string libName = GetGameLibraryName();

      if (libName.empty())
      {
         throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR,
            "The game library name must be set before configuring a application.", __FILE__, __LINE__);
      }

      std::ostringstream msg;
      try
      {
         mEntryPointLib = lsm.LoadSharedLibrary(libName);
      }
      catch (dtUtil::Exception)
      {
         msg.str("");
         msg << "Unable to load game library " << libName;
         throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, 
            msg.str(), __FILE__, __LINE__);
      }

      dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS createAddr;
      dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS destroyAddr;
      createAddr = mEntryPointLib->FindSymbol("CreateGameEntryPoint");
      destroyAddr = mEntryPointLib->FindSymbol("DestroyGameEntryPoint");

      //Make sure the plugin actually implemented these functions and they
      //have been exported.
      if(createAddr == NULL)
      {
         msg.str("");
         msg << "Game libraries must implement the function " <<
                " \"CreateGameEntryPoint.\"";
         throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, 
            msg.str(), __FILE__, __LINE__);
      }

      if(destroyAddr == NULL)
      {
         msg.str("");
         msg << "Game libraries must implement the function " <<
                " \"DestroyGameEntryPoint.\"";
         throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, 
            msg.str(), __FILE__, __LINE__);
      }

      //Well we made it here so that means the plugin was loaded
      //successfully and the create and destroy functions were found.

      #if (__GNUC__ == 3 && __GNUC_MINOR__ <= 4)
      mCreateFunction  = (CreateEntryPointFn)createAddr;
      mDestroyFunction = (DestroyEntryPointFn)destroyAddr;
      #else
      mCreateFunction  = reinterpret_cast<CreateEntryPointFn>(createAddr);
      mDestroyFunction = reinterpret_cast<DestroyEntryPointFn>(destroyAddr);
      #endif

      mEntryPoint = mCreateFunction();

      try
      {
         mEntryPoint->Initialize(*this, mArgc, mArgv);
         mGameManager = mEntryPoint->CreateGameManager(*GetScene());

         if(mGameManager == NULL)
         {
            msg.str("");
            msg << " " << " \"GameEntryPoint failed to create Game Manager.\"";
            exit(-1);
         }
         
         mGameManager->SetApplication(*this);
         mEntryPoint->OnStartup(*mGameManager);
      }
      catch(const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR);

         if(ex.TypeEnum() == dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR)
            exit(-1);
         else
            throw ex;
      }
   }

}
