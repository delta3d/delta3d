/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2009 MOVES Institute
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
* Michael Guerrero
*/

#include "testentrypoint.h"

#include "testactorlibraryregistry.h"
#include "proceduralanimationactor.h"
#include "proceduralanimationcomponent.h"

#include <dtUtil/mathdefines.h>
#include <dtUtil/exception.h>

#include <dtCore/globals.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/fpsmotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>

#include <dtDAL/project.h>

#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>
#include <dtGame/exceptionenum.h>

extern "C" TEST_ANIM_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
{
   return new TestProceduralAnimation;
}

//////////////////////////////////////////////////////////////////////////
extern "C" TEST_ANIM_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint* entryPoint)
{
   delete entryPoint;
}

//////////////////////////////////////////////////////////////////////////
TestProceduralAnimation::TestProceduralAnimation()
   : dtGame::GameEntryPoint()
   , mPerformanceTest(false)
   , mMotionModel(NULL)
{
}

//////////////////////////////////////////////////////////////////////////
TestProceduralAnimation::~TestProceduralAnimation()
{
}

//////////////////////////////////////////////////////////////////////////
void TestProceduralAnimation::Initialize(dtGame::GameApplication& app, int argc, char** argv)
{ 
   if (argc > 1)
   {
      std::string arg1(argv[1]);
      if (arg1 == "--performanceTest")
      {
         mPerformanceTest = true;
      }
      else
      {
         std::cout << std::endl<< "Usage: " << argv[0] << " testAnim [--performanceTest]" << std::endl;
         throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR,"Usage error", __FILE__, __LINE__);
      }
   }

   if (mPerformanceTest)
   {
      dtCore::FlyMotionModel* motionModel = new dtCore::FlyMotionModel(app.GetKeyboard(), app.GetMouse());
      motionModel->SetTarget(app.GetCamera());

      // Customize the motion settings?
      //mFMM->SetMaximumFlySpeed(5.0f);
      //mFMM->SetMaximumTurnSpeed(10.0f);

      mMotionModel = motionModel;
   }
   else
   {
      dtCore::FPSMotionModel* motionModel = new dtCore::FPSMotionModel(app.GetKeyboard(), app.GetMouse());
      motionModel->SetScene(app.GetScene());
      mMotionModel = motionModel;
   }

   mMotionModel->SetTarget(app.GetCamera());
}

//////////////////////////////////////////////////////////////////////////
void TestProceduralAnimation::OnStartup(dtGame::GameApplication& app)
{     
   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" + dtCore::GetDeltaRootPath() + 
      "/examples/data" + ";" + dtCore::GetDeltaRootPath() + "/examples/testProceduralAnimation/"); 

   typedef std::vector<dtDAL::ActorProxy*> ProxyContainer;
   ProxyContainer proxies;
   ProxyContainer groundActor;

   // Make sure the game manager knows about IK actors
   dtGame::GameManager& gameManager = *app.GetGameManager();
   gameManager.LoadActorRegistry("testProceduralAnimation");

   ProceduralAnimationComponent* animationComponent = new ProceduralAnimationComponent();
   gameManager.AddComponent(*animationComponent, dtGame::GameManager::ComponentPriority::NORMAL);

   try
   {
      // Ideally this would come from a config file...

      // Get the screen size
      int x, y, width, height;
      app.GetWindow()->GetPosition(x, y, width, height);
      app.GetWindow()->SetPosition(x, y, 1024, 768);
      app.GetWindow()->SetWindowTitle("TestProceduralAnimation");

      std::string context = dtCore::GetDeltaRootPath() + "/examples/data/demoMap";
      dtDAL::Project::GetInstance().SetContext(context, true);

      if (!mPerformanceTest)
      {
         gameManager.ChangeMap("TestProceduralAnimation");
      }
      else
      {
         // The performance test doesn't use a map
         animationComponent->InitializePerformanceTest();
      }
   }
   catch (dtUtil::Exception& e)
   {
      LOG_ERROR("Can't find the project context or load the map. Exception follows.");
      e.LogException(dtUtil::Log::LOG_ERROR);
   }

   app.SetNextStatisticsType();
}


