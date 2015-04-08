/* -*-c++-*-
 * TutorialLibrary - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
#  include <dtHLAGM/hlacomponent.h>
#  include <dtHLAGM/hlacomponentconfig.h>
#endif

#include <dtGame/gamemanager.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/defaultnetworkpublishingcomponent.h>
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
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>

#include <dtUtil/datapathutils.h>

#include <dtCore/project.h>
#include <dtCore/resourcedescriptor.h>

#include <osg/ArgumentParser>
#include <osg/ApplicationUsage>

#include <iostream>

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
void MyGameEntryPoint::Initialize(dtABC::BaseABC& app, int argc, char** argv)
{
   osg::ArgumentParser parser(&argc, argv);

   parser.getApplicationUsage()->addCommandLineOption("-h or --help","Display command line options");
   parser.getApplicationUsage()->addCommandLineOption("--mapName", "The name of the map to load in. This must be a map that is located within the project path specified");
   parser.getApplicationUsage()->addCommandLineOption("--startPlayback", "Set this with a 1 if you want to run in playback mode.  You need to have previously recorded or it won't do anything.");

   if (parser.read("-h") || parser.read("--help") || parser.read("-?") || parser.read("--?") ||
      parser.argc() == 0)
   {
      parser.getApplicationUsage()->write(std::cerr);
      throw dtGame::GameApplicationConfigException("Command Line Error.", __FILE__, __LINE__);
   }

   mMapName.reserve(512);

   if (!parser.read("--mapName", mMapName))
   {
      mMapName = "mapone";
   }

   // Start in Playback mode or not.
   int tempValue = 0;
   if (!parser.read("--startPlayback", tempValue))
   {
      mInPlaybackMode = false;
   }
   else
   {
      mInPlaybackMode = (tempValue == 1) ? true : false;
      if (mInPlaybackMode)
      {
         LOG_ERROR("NOTE - Enabling Playback mode. If you have not already done so, please consider using a different map without the tank actor in it ('--mapName mapone_playback'). Playback will not be able to control a local instance of the tank properly.");
      }
   }
   //  --startPlayback 1 --mapName mapone_playback
}

//////////////////////////////////////////////////////////////////////////
void MyGameEntryPoint::OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gamemanager)
{
   // init our file path so it can find GUI Scheme
   // add extra data paths here if you need them
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(
                              dtUtil::GetDeltaRootPath() + "/examples/data;" +
                              dtUtil::GetDeltaRootPath() + "/examples/data/gui/;" +
                              dataPath + ";");

   LOG_ALWAYS("Path list is: " + dtUtil::GetDataFilePathList() );

   dtCore::Project::GetInstance().SetContext("StageProject");

   // Load the map we created in STAGE.
   gamemanager.ChangeMap(mMapName);

   // Add Component - DefaultMessageProcessor.  Applies updates about remote actors, needed for networing and logging and playback.
   dtCore::RefPtr<dtGame::DefaultMessageProcessor> dmp = new dtGame::DefaultMessageProcessor();
   gamemanager.AddComponent(*dmp,dtGame::GameManager::ComponentPriority::HIGHEST);

   // Add Component - DefaultNetworkPublishingComponent  Forwards messages to the network.
   dtCore::RefPtr<dtGame::DefaultNetworkPublishingComponent> dnp = new dtGame::DefaultNetworkPublishingComponent();
   gamemanager.AddComponent(*dnp,dtGame::GameManager::ComponentPriority::HIGHEST);

   // Add Component - Input Component
   dtCore::RefPtr<InputComponent> inputComp = new InputComponent("InputComponent", mInPlaybackMode);
   gamemanager.AddComponent(*inputComp, dtGame::GameManager::ComponentPriority::NORMAL);

#ifdef HLA
   // Add Component - HLAComponent
   dtCore::RefPtr<dtHLAGM::HLAComponent> hlaComp = new dtHLAGM::HLAComponent();
   gamemanager.AddComponent(*hlaComp, dtGame::GameManager::ComponentPriority::NORMAL);

   //Load HLA Configuration
   dtHLAGM::HLAComponentConfig hlaCC;
   dtCore::ResourceDescriptor rd("Federations:HLAMapping.xml", "Federations:HLAMapping.xml");
   const std::string mappingPath = dtCore::Project::GetInstance().GetResourcePath(rd);
   if (!mappingPath.empty())
   {
      hlaCC.LoadConfiguration(*hlaComp, mappingPath);
   }
   else
   {
      std::ostringstream ss;
      ss << "Unable to load HLA Mapping resource " << rd.GetDisplayName();
      throw dtGame::GameApplicationConfigException( ss.str(), __FILE__, __LINE__);
   }

   dtCore::ResourceDescriptor rd2("Federations:RPR-FOM.fed", "Federations:RPR-FOM.fed");
   const std::string fedPath = dtCore::Project::GetInstance().GetResourcePath(rd2);

   if (!fedPath.empty())
   {
      hlaComp->JoinFederationExecution("Tutorial", fedPath, "Tank Tutorial");
   }
   else
   {
      std::ostringstream ss;
      ss << "Unable to load HLA Mapping resource " << rd2.GetDisplayName();
      throw dtGame::GameApplicationConfigException(ss.str(), __FILE__, __LINE__);
   }
#endif

   // Add Component - HUD Component
   dtCore::RefPtr<HUDComponent> hudComp = new HUDComponent(app,
                                                           "HUDComponent");
   gamemanager.AddComponent(*hudComp, dtGame::GameManager::ComponentPriority::NORMAL);

   // offset our camera a little back and above the tank.
   //dtCore::Transform tx(0.0f, 0.7f, 2.2f, 0.0f, 0.0f, 0.0f);
   //app.GetCamera()->SetTransform(tx);

   gamemanager.GetScene().UseSceneLight(true);

   // Attach our camera to the tank from the map
   std::vector<dtCore::BaseActorObject*> tanks;
   gamemanager.FindActorsByName("HoverTank", tanks);
   if (tanks.size() > 0 && tanks[0] != NULL)
   {
      if (mInPlaybackMode)
      {
         LOG_ALWAYS("Error: The tank actor is already loaded into the world during playback.  This will prevent the tank from receiving Playback messages.  Try loading a different map using --mapName <map>");
      }
   }

   // Allow the fly motion model to move the camera around independent of the tank.
   dtCore::FlyMotionModel* fmm = new dtCore::FlyMotionModel(app.GetKeyboard(), app.GetMouse(),
                                                            dtCore::FlyMotionModel::OPTION_USE_CURSOR_KEYS |
                                                            dtCore::FlyMotionModel::OPTION_REQUIRE_MOUSE_DOWN );
   fmm->SetMaximumFlySpeed(15);
   fmm->SetTarget(app.GetCamera());

   app.GetWindow()->SetWindowTitle("Delta3D Tank Tutorial");

   // Add the AAR behaviors.
   dtGame::BinaryLogStream* logStream = new dtGame::BinaryLogStream(gamemanager.GetMessageFactory());
   mServerLogger = new dtGame::ServerLoggerComponent(*logStream, "ServerLoggerComponent");
   mLogController = new dtGame::LogController("LogController");
   gamemanager.AddComponent(*mServerLogger, dtGame::GameManager::ComponentPriority::NORMAL);
   gamemanager.AddComponent(*mLogController, dtGame::GameManager::ComponentPriority::NORMAL);
   if (mInPlaybackMode)
   {
      mLogController->RequestChangeStateToPlayback();
      mLogController->RequestServerGetKeyframes();
   }
   else
   {
      mLogController->RequestSetAutoKeyframeInterval(20.0f);
   }

   // Ignore constant actors from being recorded. This protects them from deletion over
   // ServerLoggerComponent state changes; such as transitions from PLAYBACK to IDLE states.
   //mLogController->RequestAddIgnoredActor(mInputComp->GetTerrainActor().GetId());

   gamemanager.DebugStatisticsTurnOn(true, true, 30, true);
}
