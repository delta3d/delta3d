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

#include <dtUtil/log.h>

#include "dtGame/gamemanager.h"
#include "dtGame/gameapplication.h"
#include "dtGame/gameentrypoint.h"
#include "dtGame/exceptionenum.h"
#include <dtUtil/exception.h>
#include <dtCore/camera.h>
#include <dtABC/application.h>
#include <dtABC/applicationkeyboardlistener.h>
#include <dtABC/applicationmouselistener.h>

namespace dtGame
{
   IMPLEMENT_MANAGEMENT_LAYER(GameApplication)

   GameApplication::GameApplication(int argc, char** argv): dtABC::Application(), mArgc(argc), mArgv(argv)
   {
      RegisterInstance(this);
   }

   GameApplication::~GameApplication()
   {
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
         EXCEPT(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR,
            "The game library name must be set before configuring a application.");
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
         EXCEPT(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, msg.str());
      }

      dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS createAddr;
      dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS destroyAddr;
      createAddr = mEntryPointLib->FindSymbol("CreateGameEntryPoint");
      destroyAddr = mEntryPointLib->FindSymbol("DestroyGameEntryPoint");

      //Make sure the plugin actually implemented these functions and they
      //have been exported.
      if (createAddr == NULL)
      {
         msg.str("");
         msg << "Game libraries must implement the function " <<
                " \"CreateGameEntryPoint.\"";
         EXCEPT(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, msg.str());
      }

      if (destroyAddr== NULL)
      {
         msg.str("");
         msg << "Game libraries must implement the function " <<
                " \"DestroyGameEntryPoint.\"";
         EXCEPT(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, msg.str());
      }

      //Well we made it here so that means the plugin was loaded
      //successfully and the create and destroy functions were found.
      mCreateFunction =  (CreateEntryPointFn)createAddr;
      mDestroyFunction = (DestroyEntryPointFn)destroyAddr;
      mEntryPoint =  mCreateFunction();

      try
      {
         mEntryPoint->Initialize(*this, mArgc, mArgv);
         mGameManager = mEntryPoint->CreateGameManager(*GetScene());
      }
      catch (const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR);
         throw ex;
      }

      if (mGameManager == NULL)
      {
         msg.str("");
         msg << " " <<
                " \"GameEntryPoint failed to create Game Manager.\"";
         EXCEPT(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR ,msg.str());
      }

      mGameManager->SetApplication(*this);

      mEntryPoint->OnStartup();
   }

}
