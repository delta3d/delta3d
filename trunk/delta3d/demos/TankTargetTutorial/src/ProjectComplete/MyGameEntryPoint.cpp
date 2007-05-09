/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation & MOVES Institute
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
 * @author Curtiss Murphy
 * @author Chris Osborn
 */
#include <sstream> 
 
#include "MyGameEntryPoint.h"
#include "InputComponent.h"
#include "HUDComponent.h"
#include "TargetChanged.h"
#include "TankActor.h"

#ifdef HLA
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/hlacomponentconfig.h>
#endif

#include <dtGame/gamemanager.h>
#include <dtGame/defaultmessageprocessor.h>
//#include <dtGame/rulescomponent.h>
#include <dtGame/gameapplication.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/binarylogstream.h>
#include <dtGame/logtag.h>
#include <dtGame/logkeyframe.h>
#include <dtGame/logstatus.h>
#include <dtGame/loggermessages.h>
#include <dtGame/logcontroller.h>
#include <dtGame/serverloggercomponent.h>

#include <dtCore/camera.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/keyboard.h>
#include <dtCore/globals.h>
#include <dtCore/deltawin.h>

#include <dtDAL/project.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtABC/application.h>

#include <osg/ArgumentParser>
#include <osg/ApplicationUsage>

//////////////////////////////////////////////////////////////////////////
extern "C" TUTORIAL_TANK_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
{
   return new MyGameEntryPoint;
}

//////////////////////////////////////////////////////////////////////////
extern "C" TUTORIAL_TANK_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint* entryPoint)
{
   delete entryPoint;
}


//////////////////////////////////////////////////////////////////////////
MyGameEntryPoint::~MyGameEntryPoint()
{
}

//////////////////////////////////////////////////////////////////////////
void MyGameEntryPoint::Initialize(dtGame::GameApplication& app, int argc, char **argv)
         throw(dtUtil::Exception)
{
   osg::ArgumentParser parser(&argc, argv);

   parser.getApplicationUsage()->addCommandLineOption("-h or --help","Display command line options");
   parser.getApplicationUsage()->addCommandLineOption("--mapName", "The name of the map to load in. This must be a map that is located within the project path specified");
   parser.getApplicationUsage()->addCommandLineOption("--startPlayback", "Set this with a 1 if you want to run in playback mode.  You need to have previously recorded or it won't do anything.");

   if (parser.read("-h") || parser.read("--help") || parser.read("-?") || parser.read("--?") ||
      parser.argc() == 0)
   {
      parser.getApplicationUsage()->write(std::cerr);
      throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, "Command Line Error.", __FILE__, __LINE__);
   }
   
   mMapName.reserve(512);
   
   if (!parser.read("--mapName", mMapName))
   {
      mMapName = "mapone";
   }

   // Start in Playback mode or not.
   int tempValue = 0;
   if(!parser.read("--startPlayback", tempValue))
      mInPlaybackMode = false;
   else
   {
      mInPlaybackMode = (tempValue == 1) ? true : false;
      if (mInPlaybackMode)
         LOG_ERROR("NOTE - Enabling Playback mode. If you have not already done so, please consider using a different map without the tank actor in it ('--mapName mapone_playback'). Playback will not be able to control a local instance of the tank properly."); 
   }
   //  --startPlayback 1 --mapName mapone_playback
}

//////////////////////////////////////////////////////////////////////////
void MyGameEntryPoint::OnStartup(dtGame::GameManager &gameManager)
{
   dtABC::Application& app = gameManager.GetApplication();

   // init our file path so it can find GUI Scheme
   // add extra data paths here if you need them
   dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList() + 
      ";" + dtCore::GetDeltaRootPath() + "/examples/data;");   

   dtDAL::Project::GetInstance().SetContext("StageProject");

   // Load the map we created in STAGE.
   gameManager.ChangeMap(mMapName);

   // Add Component - DefaultMessageProcessor 
   dtGame::DefaultMessageProcessor *dmp = new dtGame::DefaultMessageProcessor("DefaultMessageProcessor");
   gameManager.AddComponent(*dmp,dtGame::GameManager::ComponentPriority::HIGHEST);

   // Register our messages with the Game Manager message factory - part 5
   TutorialMessageType::RegisterMessageTypes( gameManager.GetMessageFactory() );

   // Add Component - Rules Component
   //dtCore::RefPtr<dtGame::RulesComponent> rc = new dtGame::RulesComponent("Rules");
   //gameManager.AddComponent(*rc, dtGame::GameManager::ComponentPriority::NORMAL);

   // Add Component - Input Component
   dtCore::RefPtr<InputComponent> inputComp = new InputComponent("InputComponent", mInPlaybackMode);
   gameManager.AddComponent(*inputComp, dtGame::GameManager::ComponentPriority::NORMAL);
 
#ifdef HLA
   // Add Component - HLAComponent 
   dtCore::RefPtr<dtHLAGM::HLAComponent> hlaComp = new dtHLAGM::HLAComponent("HLAComponent");
   gameManager.AddComponent(*hlaComp, dtGame::GameManager::ComponentPriority::NORMAL);

   //Load HLA Configuration
   dtHLAGM::HLAComponentConfig hlaCC;
   dtDAL::ResourceDescriptor rd("Federations:HLAMapping.xml", "Federations:HLAMapping.xml");
   const std::string mappingPath = dtDAL::Project::GetInstance().GetResourcePath(rd);
   if (!mappingPath.empty())
   {
      hlaCC.LoadConfiguration(*hlaComp, mappingPath);
   }
   else
   {
      std::ostringstream ss;
      ss << "Unable to load HLA Mapping resource " << rd.GetDisplayName();
      throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, ss.str(), __FILE__, __LINE__);
   }  

   dtDAL::ResourceDescriptor rd2("Federations:RPR-FOM.fed", "Federations:RPR-FOM.fed");
   const std::string fedPath = dtDAL::Project::GetInstance().GetResourcePath(rd2);

   if (!fedPath.empty())
   {
      hlaComp->JoinFederationExecution("Tutorial", fedPath, "Tank Tutorial");
   }
   else
   {
      std::ostringstream ss;
      ss << "Unable to load HLA Mapping resource " << rd2.GetDisplayName();
      throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, ss.str(), __FILE__, __LINE__);
   }  
#endif

   // Add Component - HUD Component
   dtCore::RefPtr<HUDComponent> hudComp = new HUDComponent(app.GetWindow(), "HUDComponent");
   gameManager.AddComponent(*hudComp, dtGame::GameManager::ComponentPriority::NORMAL);

   // offset our camera a little back and above the tank.
   //dtCore::Transform tx(0.0f, 0.7f, 2.2f, 0.0f, 0.0f, 0.0f);
   //app.GetCamera()->SetTransform(tx); 

   gameManager.GetScene().UseSceneLight(true);

   // Attach our camera to the tank from the map
   std::vector< dtCore::RefPtr<dtDAL::ActorProxy> > tanks;
   gameManager.FindActorsByName("HoverTank", tanks);
   if (tanks.size() > 0 && tanks[0].valid())
   {
      if (mInPlaybackMode)
         LOG_ALWAYS("Error: The tank actor is already loaded into the world during playback.  This will prevent the tank from receiving Playback messages.  Try loading a different map using --mapName <map>");
   }

   // Allow the fly motion model to move the camera around independent of the tank.
   dtCore::FlyMotionModel *fmm = new dtCore::FlyMotionModel(app.GetKeyboard(), app.GetMouse(), false);
   fmm->SetMaximumFlySpeed(15);
   fmm->SetTarget(app.GetCamera());

   app.GetWindow()->SetWindowTitle("Tutorial");

   // Add the AAR behaviors.
   dtGame::BinaryLogStream *logStream = new dtGame::BinaryLogStream(gameManager.GetMessageFactory());
   mServerLogger = new dtGame::ServerLoggerComponent(*logStream, "ServerLoggerComponent");
   mLogController = new dtGame::LogController("LogController");
   gameManager.AddComponent(*mServerLogger.get(), dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*mLogController.get(), dtGame::GameManager::ComponentPriority::NORMAL);
   if (mInPlaybackMode)
   {
      mLogController->RequestChangeStateToPlayback();
      mLogController->RequestServerGetKeyframes();
   }
   else
      mLogController->RequestSetAutoKeyframeInterval(20.0f);

   // Ignore constant actors from being recorded. This protects them from deletion over 
   // ServerLoggerComponent state changes; such as transitions from PLAYBACK to IDLE states.
   //mLogController->RequestAddIgnoredActor(mInputComp->GetTerrainActor().GetId());

   gameManager.DebugStatisticsTurnOn(true, false, 10, true);
}
