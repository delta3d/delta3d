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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtGame/gameentrypoint.h>
#include "export.h"
#include <dtABC/baseabc.h>
#include <dtCore/map.h>
#include <dtCore/project.h>
#include <dtCore/projectconfig.h>
#include <dtCore/refptr.h>
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

   private:

      /**
       * Helper method to parse command line options
       * @note This method will parse command line options
       * and set values as necessary. For instance, it will
       * set the data path of the application
       */
      void ParseCommandLineOptions(int argc, char **argv);
      std::string mProjectPath;

      dtCore::RefPtr<dtCore::Map> mMap;
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

   if (dtUtil::FindFileInPathList("map.xsd").empty())
   {
      LOG_INFO("No map.xsd was found in the search path: \"" + dtUtil::GetDataFilePathList() + "\"<BR>\n Hopefully it will be found in the project context.");
   }

   // The commandline might indicate the project context as a parameter.
   ParseCommandLineOptions(argc, argv);

   // If the paths of this code block are necessary, add them to
   // the .dtproj file. Be sure that all paths are valid since
   // exceptions are thrown if invalid and the project file will
   // fail to process any subsequent paths.
   /*if (mProjectPath.empty())
   {
         // TODO look in the mac bundle.
         std::vector<std::string> projectPaths;
         projectPaths.push_back(executablePath + "/../examples");
         projectPaths.push_back(executablePath + "/../share/delta3d/examples");
         projectPaths.push_back(executablePath + "/../../examples");
         // TODO compile in the install prefix for linux?
         projectPaths.push_back("/usr/share/delta3d/examples");
         projectPaths.push_back("/usr/local/share/delta3d/examples");
         mProjectPath = dtUtil::FindFileInPathList("data", projectPaths);
   }*/

}


////////////////////////////////////////////////////////////////////////////////
void TestApp::OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gameManager)
{
   // If the command line has not indicated the project context...
   if (mProjectPath.empty())
   {
      // ...search for it within the porperties of the config.xml.
      mProjectPath = gameManager.GetConfiguration().GetConfigPropertyValue("ProjectPath");
   }

   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   dtUtil::FileInfo fi = fileUtils.GetFileInfo(mProjectPath);

   // Determine if the project context was specified as a directory or archive.
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
   // Otherwise, determine if the project context has been
   // specified as a .dtproj file.
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
   else // The project context could not be found as a file or directory.
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

   // Setup GUI Component
   dtCore::RefPtr<GuiComponent> guiComp = new GuiComponent();
   gameManager.AddComponent(*guiComp, dtGame::GameManager::ComponentPriority::NORMAL);

   // Setup Game State Component
   dtCore::RefPtr<dtGame::GameStateComponent> gameStateComp = new dtGame::GameStateComponent();
   gameManager.AddComponent(*gameStateComp, dtGame::GameManager::ComponentPriority::NORMAL);
   
   // Load game state transitions.
   std::string filePath = dtCore::Project::GetInstance().GetResourcePath(dtCore::ResourceDescriptor("Transitions:TestAppTransitions.xml"));
   if ( ! gameStateComp->LoadTransitions(filePath))
   {
      LOG_ERROR("Could not load the game state transitions file.");
   }

   // Load the map for this application.
   mMap = &app.LoadMap("TestApp", false);;
   if ( ! mMap.valid())
   {
      LOG_ERROR("Map file for TestApp could not be found.");
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

   if (argParser.read("-h") || argParser.read("--help") || argParser.argc() == 0)
   {
      argParser.getApplicationUsage()->write(std::cerr);
      throw dtGame::GameApplicationConfigException(
         "Command Line Error.", __FILE__, __LINE__);
   }

   argParser.read("--projectPath", mProjectPath);


   argParser.reportRemainingOptionsAsUnrecognized();
   if (argParser.errors())
   {
      std::ostringstream oss;
      argParser.writeErrorMessages(oss);
      LOG_ERROR("Command line error: " + oss.str());
   }
}

