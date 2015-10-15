/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtGame/defaultgameentrypoint.h>
#include "export.h"
#include <dtABC/baseabc.h>
#include <dtABC/application.h>
#include <dtCore/map.h>
#include <dtCore/project.h>
#include <dtCore/projectconfig.h>
#include <dtCore/refptr.h>
#include <dtAnim/animationcomponent.h>
#include <dtGame/binarylogstream.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/gamestatecomponent.h>
#include <dtUtil/configproperties.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include "guicomponent.h"
#include "inputcomponent.h"
#include "testappgamestates.h"

#include <dtPhysics/physicscomponent.h>
#include <dtGame/deadreckoningcomponent.h>


using namespace dtExample;



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class TestApp : public dtGame::DefaultGameEntryPoint
{

public:

   TestApp();

   ~TestApp() /*override*/; // older versions of VC++ don't allow override on destructors.

   /**
    * Called to initialize the game application.  You can pull any command line params here.
    */
   void Initialize(dtABC::BaseABC& app, int argc, char **argv) override;


   /**
    * Called just before your application's game loop starts.  This is your main
    * opportunity to create components, load maps, create unique actors, etc...
    */
   void OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gamemanager) override;

   void OnShutdown(dtABC::BaseABC& /*app*/, dtGame::GameManager& /*gamemanager*/) override;

   void ValidateMap(const std::string& mapToValidate);

private:

};


////////////////////////////////////////////////////////////////////////////////
extern "C" TEST_APP_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
{
   return new TestApp;
}

////////////////////////////////////////////////////////////////////////////////
extern "C" TEST_APP_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint* entryPoint)
{
   delete entryPoint;
}

////////////////////////////////////////////////////////////////////////////////
TestApp::TestApp()
{
   SetProjectPath("");
   SetMapName("TestApp");
   SetBaseMapName("BaseMap");
}

////////////////////////////////////////////////////////////////////////////////
TestApp::~TestApp()
{
}

////////////////////////////////////////////////////////////////////////////////
void TestApp::Initialize(dtABC::BaseABC& app, int argc, char** argv)
{

   srand((unsigned int)(time(0)));

   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   std::string executablePath = fileUtils.GetAbsolutePath(argv[0], true);
   std::string deltaDataEnvVar = dtUtil::GetDeltaDataPathList();
   if (deltaDataEnvVar.empty() || !dtUtil::FileUtils::GetInstance().DirExists(deltaDataEnvVar))
   {
      dtUtil::SetDataFilePathList(executablePath + "/../data;" + executablePath + "/../share/delta3d/data;" +  executablePath + "/../../data;" + executablePath + "/../Resources/delta3dData");
   }
   else
   {
      dtUtil::SetDataFilePathList(deltaDataEnvVar);
   }

   if (dtUtil::FindFileInPathList("map.xsd").empty())
   {
      LOG_INFO("No map.xsd was found in the search path: \"" + dtUtil::GetDataFilePathList() + "\"<BR>\n Hopefully it will be found in the project context.");
   }

   DefaultGameEntryPoint::Initialize(app, argc, argv);

   if (GetProjectPath().empty())
   {
      // TODO look in the mac bundle.
      std::vector<std::string> projectPaths;
      projectPaths.push_back("./examples");
      projectPaths.push_back(executablePath + "/../examples");
      projectPaths.push_back(executablePath + "/../share/delta3d/examples");
      projectPaths.push_back(executablePath + "/../../examples");
      projectPaths.push_back(executablePath + "/../Resources/examples");
      // TODO compile in the install prefix for linux?
      projectPaths.push_back("/usr/share/delta3d/examples");
      projectPaths.push_back("/usr/local/share/delta3d/examples");
      SetProjectPath(dtUtil::FindFileInPathList("data", projectPaths));
   }

}


////////////////////////////////////////////////////////////////////////////////
void TestApp::OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gameManager)
{
   DefaultGameEntryPoint::OnStartup(app, gameManager);

   // Setup Message Processor
   dtCore::RefPtr<dtGame::DefaultMessageProcessor> mp = new dtGame::DefaultMessageProcessor();
   gameManager.AddComponent(*mp, dtGame::GameManager::ComponentPriority::HIGHEST);

   // Setup Input Component
   dtCore::RefPtr<InputComponent> inputComp = new InputComponent();
   gameManager.AddComponent(*inputComp, dtGame::GameManager::ComponentPriority::NORMAL);

   // Setup Game State Component
   dtCore::RefPtr<dtGame::GameStateComponent> gameStateComp = new dtGame::GameStateComponent();
   gameManager.AddComponent(*gameStateComp, dtGame::GameManager::ComponentPriority::NORMAL);

   // Load game state transitions.
   std::string filePath = dtCore::Project::GetInstance().GetResourcePath(dtCore::ResourceDescriptor("Transitions:TestAppTransitions.xml"));
   if ( ! gameStateComp->LoadTransitions(filePath))
   {
      LOG_ERROR("Could not load the game state transitions file.");
   }

   // Setup GUI Component. This should follow the game state component setup.
   dtCore::RefPtr<GuiComponent> guiComp = new GuiComponent();
   gameManager.AddComponent(*guiComp, dtGame::GameManager::ComponentPriority::NORMAL);

   // Setup Physics
   dtCore::RefPtr<dtPhysics::PhysicsWorld> world;
   try
   {
      // See if the world exists already.
      world = &dtPhysics::PhysicsWorld::GetInstance();
   }
   catch (const dtUtil::Exception&)
   {
      world = new dtPhysics::PhysicsWorld(gameManager.GetConfiguration());
      world->Init();
   }

   dtCore::RefPtr<dtPhysics::PhysicsComponent> physicsComponent = new dtPhysics::PhysicsComponent(dtPhysics::PhysicsWorld::GetInstance(), false);
   // Keep the human kinematic cylinder from colliding with the terrain and its own shape walking shape.
   physicsComponent->SetGroupCollision(0, 6, false);
   gameManager.AddComponent(*physicsComponent, dtGame::GameManager::ComponentPriority::NORMAL);

   dtCore::RefPtr<dtAnim::AnimationComponent> animComponent = new dtAnim::AnimationComponent;
   gameManager.AddComponent(*animComponent, dtGame::GameManager::ComponentPriority::NORMAL);

   dtCore::RefPtr<dtGame::DeadReckoningComponent> drComponent = new dtGame::DeadReckoningComponent;
   gameManager.AddComponent(*drComponent, dtGame::GameManager::ComponentPriority::NORMAL);

   //setup camera
   double vfov, aspect, nearClip, farClip;
   //dtCore::DeltaWin::Resolution vec = gameManager.GetApplication().GetWindow()->GetCurrentResolution();

   dtCore::Camera* cam = gameManager.GetApplication().GetCamera();
   gameManager.GetApplication().GetCamera()->GetPerspectiveParams(vfov, aspect, nearClip, farClip);
   cam->SetPerspectiveParams(vfov, aspect, 0.5f, 15000.0f);

   cam->GetOSGCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
   cam->GetOSGCamera()->setCullingMode(osg::CullSettings::ENABLE_ALL_CULLING);
}

void TestApp::OnShutdown(dtABC::BaseABC& /*app*/, dtGame::GameManager& /*gamemanager*/)
{

}

