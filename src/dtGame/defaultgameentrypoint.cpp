/*
 * defaultgameentrypoint.cpp
 *
 *  Created on: Sep 28, 2015
 *      Author: david
 */

#include <dtGame/defaultgameentrypoint.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/gamemanager.h>
#include <dtCore/project.h>
#include <dtCore/projectconfig.h>
#include <dtUtil/configproperties.h>
#include <sstream>
#include <iostream>

namespace dtGame
{

   DefaultGameEntryPoint::DefaultGameEntryPoint()
   : mProjectPath()
   , mMapName()
   , mBaseMapName()
   , mMapIsRequired(true)
   , mArgc(0)
   {
   }

   DefaultGameEntryPoint::~DefaultGameEntryPoint()
   {
   }

   void DefaultGameEntryPoint::Initialize(dtABC::BaseABC& app, int argc, char **argv)
   {
      GetOrCreateArgParser(argc, argv);
      ParseDefaultCommandLineOptions();
   }

   DT_IMPLEMENT_ACCESSOR(DefaultGameEntryPoint, std::string, ProjectPath)
   DT_IMPLEMENT_ACCESSOR(DefaultGameEntryPoint, std::string, MapName)
   DT_IMPLEMENT_ACCESSOR(DefaultGameEntryPoint, bool, MapIsRequired)
   DT_IMPLEMENT_ACCESSOR(DefaultGameEntryPoint, std::string, BaseMapName)

   osg::ArgumentParser* DefaultGameEntryPoint::GetOrCreateArgParser(int argc, char **argv)
   {
      if (!mArgParser)
      {
         mArgc = argc;
         mArgParser.reset(new osg::ArgumentParser(&mArgc, argv));
      }
      return mArgParser.get();
   }

   osg::ArgumentParser* DefaultGameEntryPoint::GetArgParser()
   {
      return mArgParser.get();
   }

   void DefaultGameEntryPoint::ParseDefaultCommandLineOptions()
   {
      if (!mArgParser)
      {
         throw dtGame::GameApplicationConfigException(
               "Tried to parse the command line without initializing the command line parser.", __FILE__, __LINE__);
      }

      osg::ApplicationUsage* usage = mArgParser->getApplicationUsage();
      if (usage->getCommandLineUsage().empty())
      {
         usage->setCommandLineUsage("delta3d [options] value ...");
      }

      usage->addCommandLineOption("-h or --help","Display command line options");
      usage->addCommandLineOption("--projectPath", "The path to the project config or project contexct directory.");
      usage->addCommandLineOption("--mapName", "The name of the map to load in. This must be a map that is located within the project path specified");
      usage->addCommandLineOption("--baseMap", "The name of the base map to load in. This must be a map that is located within the project path specified");

      if (mArgParser->read("-h") || mArgParser->read("-?") || mArgParser->read("--help") || mArgParser->argc() == 0)
      {
         usage->write(std::cerr);
         throw dtGame::GameApplicationConfigException(
               "Command Line Error.", __FILE__, __LINE__);
      }

      mArgParser->read("--projectPath", mProjectPath);

      if (!mArgParser->read("--mapName", mMapName) && mMapName.empty() && mMapIsRequired)
      {
         usage->write(std::cerr);
         throw dtGame::GameApplicationConfigException(
               "Command Line Error.  No map name was provided.", __FILE__, __LINE__);
      }

      mArgParser->read("--baseMap", mBaseMapName);
   }

   void DefaultGameEntryPoint::SetupProjectContext()
   {

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      dtUtil::FileInfo fi = fileUtils.GetFileInfo(mProjectPath);
      if (fi.fileType == dtUtil::DIRECTORY || fi.fileType == dtUtil::ARCHIVE)
      {
         try
         {
            dtCore::Project::GetInstance().SetContext(GetProjectPath(), true);
            LOGN_INFO("defaultgameentrypoint.cpp", "Using project context path: " + mProjectPath);
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
            LOGN_INFO("defaultgameentrypoint.cpp", "Using project config: " + mProjectPath);
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
   }

   void DefaultGameEntryPoint::OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gameManager)
   {
      GetArgParser()->reportRemainingOptionsAsUnrecognized();
      if (GetArgParser()->errors())
      {
         std::ostringstream oss;
         GetArgParser()->writeErrorMessages(oss);
         LOG_ERROR("Command line error: " + oss.str());
      }

      if (mProjectPath.empty())
      {
         SetProjectPath(gameManager.GetConfiguration().GetConfigPropertyValue("ProjectPath"));
      }

      SetupProjectContext();

      dtGame::GameManager::NameVector mapNames;

      if (mMapIsRequired)
      {
         if (!GetBaseMapName().empty())
         {
            ValidateMap(GetBaseMapName());
            mapNames.push_back(GetBaseMapName());
         }
         ValidateMap(GetMapName());
         mapNames.push_back(GetMapName());
         LoadMaps(gameManager, mapNames);
      }
   }

   void DefaultGameEntryPoint::LoadMaps(dtGame::GameManager& gameManager, dtGame::GameManager::NameVector& mapNames)
   {
      gameManager.ChangeMapSet(mapNames);
   }

   void DefaultGameEntryPoint::ValidateMap(const std::string& mapToValidate)
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



} /* namespace dtGame */
