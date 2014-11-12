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
#include <dtGame/gameentrypoint.h>
#include "export.h"
#include <dtABC/baseabc.h>
#include <dtABC/application.h>
#include <dtCore/deltawin.h>
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
class TestApp : public dtGame::GameEntryPoint
{

public:

   TestApp();

   virtual ~TestApp();

   /**
    * Called to initialize the game application.  You can pull any command line params here.
    */
   virtual void Initialize(dtABC::BaseABC& app, int argc, char **argv);


   /**
    * Called just before your application's game loop starts.  This is your main
    * opportunity to create components, load maps, create unique actors, etc...
    */
   virtual void OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gamemanager);

   virtual void OnShutdown(dtABC::BaseABC& /*app*/, dtGame::GameManager& /*gamemanager*/);

   void ValidateMap(const std::string& mapToValidate);

private:

   /**
    * Helper method to parse command line options
    * @note This method will parse command line options
    * and set values as necessary. For instance, it will
    * set the data path of the application
    */
   void ParseCommandLineOptions(int argc, char **argv);
   std::string mProjectPath;
   std::string mMapName;
   std::string mBaseMapName;
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

   ParseCommandLineOptions(argc, argv);

   if (mProjectPath.empty())
   {
      // TODO look in the mac bundle.
      std::vector<std::string> projectPaths;
      projectPaths.push_back(executablePath + "/../examples");
      projectPaths.push_back(executablePath + "/../share/delta3d/examples");
      projectPaths.push_back(executablePath + "/../../examples");
      projectPaths.push_back(executablePath + "/../Resources/examples");
      // TODO compile in the install prefix for linux?
      projectPaths.push_back("/usr/share/delta3d/examples");
      projectPaths.push_back("/usr/local/share/delta3d/examples");
      mProjectPath = dtUtil::FindFileInPathList("data", projectPaths);
   }

}


////////////////////////////////////////////////////////////////////////////////
void TestApp::OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gameManager)
{
   if (mProjectPath.empty())
   {
      mProjectPath = gameManager.GetConfiguration().GetConfigPropertyValue("ProjectPath");
   }

   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

   dtUtil::FileInfo fi = fileUtils.GetFileInfo(mProjectPath);
   if (fi.fileType == dtUtil::DIRECTORY || fi.fileType == dtUtil::ARCHIVE)
   {
      try
      {
         dtCore::Project::GetInstance().SetContext(mProjectPath, true);
         LOG_ALWAYS("Using project context path: " + mProjectPath);
      }
      catch (dtUtil::Exception& )
      {
         throw dtGame::GameApplicationConfigException(
               "Invalid project context path: " + mProjectPath, __FILE__, __LINE__);
      }
   }
   else if (fi.fileType == dtUtil::REGULAR_FILE)
   {
      try
      {
         dtCore::Project::GetInstance().SetupFromProjectConfigFile(mProjectPath);
         LOG_ALWAYS("Using project config: " + mProjectPath);
      }
      catch (dtUtil::Exception& )
      {
         throw dtGame::GameApplicationConfigException(
               "Invalid project config file: " + mProjectPath, __FILE__, __LINE__);
      }
   }
   else
   {
      throw dtGame::GameApplicationConfigException(
            "Invalid or unknown project path: " + mProjectPath, __FILE__, __LINE__);
   }

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

   ValidateMap(mBaseMapName);
   ValidateMap(mMapName);

   dtGame::GameManager::NameVector mapNames;
   mapNames.push_back(mBaseMapName);
   mapNames.push_back(mMapName);

   gameManager.ChangeMapSet(mapNames);
   //gameManager.OpenAdditionalMapSet(mapNames);
}

void TestApp::ValidateMap(const std::string& mapToValidate)
{
   std::set<std::string> mapNames = dtCore::Project::GetInstance().GetMapNames();
   bool containsMap = false;
   for(std::set<std::string>::iterator i = mapNames.begin(); i != mapNames.end(); ++i)
      if(*i == mapToValidate)
         containsMap = true;

   if(!containsMap)
   {
      std::ostringstream oss;
      oss << "A map named: " << mapToValidate << " could not be located in the project context: "
            << mProjectPath;
      throw dtGame::GameApplicationConfigException(
            oss.str(), __FILE__, __LINE__);
   }   
}

void TestApp::OnShutdown(dtABC::BaseABC& /*app*/, dtGame::GameManager& /*gamemanager*/)
{

}

void TestApp::ParseCommandLineOptions(int argc, char** argv)
{
   osg::ArgumentParser argParser(&argc, argv);

   argParser.getApplicationUsage()->setCommandLineUsage("TestApp [options] value ...");
   argParser.getApplicationUsage()->addCommandLineOption("-h or --help","Display command line options");
   argParser.getApplicationUsage()->addCommandLineOption("--projectPath", "The path to the project config or project contexct directory.");
   argParser.getApplicationUsage()->addCommandLineOption("--mapName", "The name of the map to load in. This must be a map that is located within the project path specified");
   argParser.getApplicationUsage()->addCommandLineOption("--baseMap", "The name of the base map to load in. This must be a map that is located within the project path specified");

   if (argParser.read("-h") || argParser.read("--help") || argParser.argc() == 0)
   {
      argParser.getApplicationUsage()->write(std::cerr);
      throw dtGame::GameApplicationConfigException(
            "Command Line Error.", __FILE__, __LINE__);
   }

   argParser.read("--projectPath", mProjectPath);

   if (!argParser.read("--mapName", mMapName))
   {
      mMapName = "TestApp";
   }

   if (!argParser.read("--baseMap", mBaseMapName))
   {
      mBaseMapName = "BaseMap";
   }


   argParser.reportRemainingOptionsAsUnrecognized();
   if (argParser.errors())
   {
      std::ostringstream oss;
      argParser.writeErrorMessages(oss);
      LOG_ERROR("Command line error: " + oss.str());
   }
}

