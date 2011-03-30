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
#include <prefix/dtgameprefix.h>
#include <dtUtil/log.h>

#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>
#include <dtCore/deltawin.h>
#include <dtCore/shadermanager.h>

#include <dtGame/gameentrypoint.h>
#include <dtGame/exceptionenum.h>

#include <dtUtil/exception.h>
#include <dtCore/scene.h>
#include <osgViewer/CompositeViewer> // for parent class's forward declaration
#include <dtCore/keyboard.h>

#include <iostream>
#include <sstream>

namespace dtGame
{
   IMPLEMENT_MANAGEMENT_LAYER(GameApplication)

   GameApplication::GameApplication(int argc, char** argv, const std::string& configFileName, dtCore::DeltaWin* window)
      : dtABC::Application(configFileName, window)
      , mArgc(argc)
      , mArgv(argv)
      , mEntryPoint(NULL)
      , mCreateFunction(NULL)
      , mDestroyFunction(NULL)
   {
      RegisterInstance(this);
      GetKeyboard()->RemoveKeyboardListener(GetKeyboardListener());
   }

   /////////////////////////////////////////////////////////////////////////////
   GameApplication::~GameApplication()
   {
      if (mEntryPoint != NULL)
      {
         try
         {
            mEntryPoint->OnShutdown(*this);
         }
         catch (const dtUtil::Exception& e)
         {
            e.LogException(dtUtil::Log::LOG_ALWAYS);
         }
         catch (...)
         {
            LOG_ALWAYS("Unknown exception caught in the destructor of GameApplication");
         }
      }

      DeregisterInstance(this);

      GetScene()->RemoveAllDrawables();

      dtCore::ShaderManager::GetInstance().Clear();

      if (mGameManager.valid())
      {
         mGameManager->Shutdown();
         mGameManager = NULL;
      }

      if (mDestroyFunction != NULL)
      {
         mDestroyFunction(mEntryPoint);
         mDestroyFunction = NULL;
         mEntryPoint = NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void GameApplication::Config()
   {
      dtUtil::LibrarySharingManager& lsm = dtUtil::LibrarySharingManager::GetInstance();
      std::string libName = GetGameLibraryName();

      if (libName.empty())
      {
         throw dtGame::GameApplicationConfigException(
            "The game library name must be set before configuring a application.", __FILE__, __LINE__);
      }

      std::ostringstream msg;
      try
      {
         mEntryPointLib = lsm.LoadSharedLibrary(libName);
      }
      catch (const dtUtil::Exception&)
      {
         msg.str("");
         msg << "Unable to load game library " << libName;
         throw dtGame::GameApplicationConfigException(
            msg.str(), __FILE__, __LINE__);
      }

      dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS createAddr;
      dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS destroyAddr;
      createAddr = mEntryPointLib->FindSymbol("CreateGameEntryPoint");
      destroyAddr = mEntryPointLib->FindSymbol("DestroyGameEntryPoint");

      // Make sure the plugin actually implemented these functions and they
      // have been exported.
      if (createAddr == NULL)
      {
         msg.str("");
         msg << "Game libraries must implement the function " <<
                " \"CreateGameEntryPoint.\"";
         throw dtGame::GameApplicationConfigException(
            msg.str(), __FILE__, __LINE__);
      }

      if (destroyAddr == NULL)
      {
         msg.str("");
         msg << "Game libraries must implement the function " <<
                " \"DestroyGameEntryPoint.\"";
         throw dtGame::GameApplicationConfigException(
            msg.str(), __FILE__, __LINE__);
      }

      // Well we made it here so that means the plugin was loaded
      // successfully and the create and destroy functions were found.

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
         mGameManager = new dtGame::GameManager(*GetScene());
         if (mGameManager == NULL)
         {
            msg.str("");
            msg << " " << " \"GameEntryPoint failed to create Game Manager.\"";
            exit(-1);
         }

         mEntryPoint->Initialize(*this, mArgc, mArgv);
         Application::Config();
         //GetCompositeViewer()->setUpThreading();

         mGameManager->SetApplication(*this);
         mEntryPoint->OnStartup(*this);
      }
      catch (const dtGame::GameApplicationConfigException& ex)
      {
         // force console logging to be on so you don't miss this critical exception.
         dtUtil::Log::GetInstance().SetOutputStreamBit(dtUtil::Log::STANDARD);
         ex.LogException(dtUtil::Log::LOG_ERROR);
         std::cerr << "The Game Manager got an exception and will now shut down ... " << std::endl;

         exit(-1);
      }
      catch (const dtUtil::Exception& ex)
      {
         dtUtil::Log::GetInstance().SetOutputStreamBit(dtUtil::Log::STANDARD);
         ex.LogException(dtUtil::Log::LOG_ERROR);
         std::cerr << "The Game Manager got an exception and will now shut down ... " << std::endl;
         throw;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void GameApplication::SetGameManager(dtGame::GameManager& gameManager)
   {
      mGameManager = &gameManager;
   }

} // namespace dtGame
