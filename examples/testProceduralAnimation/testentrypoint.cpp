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

#include <dtUtil/datapathutils.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/exception.h>

#include <dtCore/flymotionmodel.h>
#include <dtCore/fpsmotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>

#include <dtCore/project.h>

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
void TestProceduralAnimation::Initialize(dtABC::BaseABC& app, int argc, char** argv)
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
         throw dtGame::GameApplicationConfigException("Usage error", __FILE__, __LINE__);
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
      dtCore::FlyMotionModel* motionModel = new dtCore::FlyMotionModel(app.GetKeyboard(), app.GetMouse());
      mMotionModel = motionModel;
      motionModel->SetMaximumFlySpeed(1.0f);
      motionModel->SetMaximumTurnSpeed(5.0f);
   }

   mMotionModel->SetTarget(app.GetCamera());
}

//////////////////////////////////////////////////////////////////////////
void TestProceduralAnimation::OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gameManager)
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" + dtUtil::GetDeltaRootPath() +
      "/examples/data" + ";" + dtUtil::GetDeltaRootPath() + "/examples/testProceduralAnimation/");

   typedef dtCore::ActorPtrVector ProxyContainer;
   ProxyContainer proxies;
   ProxyContainer groundActor;

   // Make sure the game manager knows about IK actors
   gameManager.LoadActorRegistry("testProceduralAnimation");

   ProceduralAnimationComponent* animationComponent = new ProceduralAnimationComponent();
   gameManager.AddComponent(*animationComponent, dtGame::GameManager::ComponentPriority::NORMAL);

   gameManager.AddComponent(*new dtAnim::AnimationComponent, dtGame::GameManager::ComponentPriority::NORMAL);

   try
   {
      // Ideally this would come from a config file...

      // Get the screen size
      int x, y, width, height;
      app.GetWindow()->GetPosition(x, y, width, height);
      app.GetWindow()->SetPosition(x, y, 1024, 768);
      app.GetWindow()->SetWindowTitle("TestProceduralAnimation");

      std::string context = dtUtil::GetDeltaRootPath() + "/examples/data";
      dtCore::Project::GetInstance().SetContext(context, true);

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
   catch (const dtUtil::Exception& e)
   {
      LOG_ERROR("Can't find the project context or load the map. Exception follows.");
      e.LogException(dtUtil::Log::LOG_ERROR);
   }

}
