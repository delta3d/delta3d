/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * Christopher DuBuc
 * William E. Johnson II
 */
#include "testaar.h"
#include "testaarmessagetypes.h"
#include "testaarmessageprocessor.h"
#include "testaarinput.h"
#include "testaarhud.h"

#include <dtCore/globals.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtDAL/actortype.h>
#include <dtDAL/project.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>
#include <dtGame/logcontroller.h>
#include <dtGame/binarylogstream.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/exceptionenum.h>
#include <dtLMS/lmscomponent.h>
#include <osg/ArgumentParser>
#include <osg/ApplicationUsage>

#include <ctime>

extern "C" TEST_AAR_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
{
   return new TestAAR;
}

//////////////////////////////////////////////////////////////////////////
extern "C" TEST_AAR_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint* entryPoint)
{
   delete entryPoint;
}

//////////////////////////////////////////////////////////////////////////
TestAAR::TestAAR() :  
   mLmsComponent(new dtLMS::LmsComponent("TestAARLmsComponent")), 
   mUseLMS(true)
{
   
}

//////////////////////////////////////////////////////////////////////////
TestAAR::~TestAAR()
{
   if(mUseLMS && mLmsComponent.valid())
   {
      mLmsComponent->DisconnectFromLms();
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAAR::Initialize(dtGame::GameApplication& app, int argc, char **argv)
{
   ParseCommandLineOptions(argc, argv);
   srand((unsigned int)(time(0)));

   dtCore::Transform tx(0.0f,-25.0f,10.0f,0.0f,-15.0f,0.0f);
   app.GetWindow()->SetWindowTitle("testAAR");
   app.GetCamera()->SetTransform(tx);

   mFMM = new dtCore::FlyMotionModel(app.GetKeyboard(), app.GetMouse(), false);
   mFMM->SetTarget(app.GetCamera());
}

//////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<dtGame::GameManager> TestAAR::CreateGameManager(dtCore::Scene& scene)
{
   return new dtGame::GameManager(scene);
}

//////////////////////////////////////////////////////////////////////////
void TestAAR::OnStartup(dtGame::GameManager &gameManager)
{
   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" + 
                               dtCore::GetDeltaRootPath() + "/examples/data" + ";");

   std::string context = dtCore::GetDeltaRootPath() + "/examples/testAAR/AARProject";

   try
   {
      dtDAL::Project::GetInstance().SetContext(context, true);
   }
   catch (dtUtil::Exception &e)
   {
      LOG_ERROR("Can't find the project context: " + e.What());
   }
   
   dtCore::DeltaWin *win = gameManager.GetApplication().GetWindow();

   // Add Component - Input Component
   dtCore::RefPtr<dtGame::LogController> logCtrl = new dtGame::LogController("LogController");
   dtCore::RefPtr<dtGame::BinaryLogStream> logStream = new dtGame::BinaryLogStream(gameManager.GetMessageFactory());
   dtCore::RefPtr<dtGame::ServerLoggerComponent> srvrLog = new dtGame::ServerLoggerComponent(*logStream, "ServerLogger");
   dtCore::RefPtr<TestAARHUD> hudComp = new TestAARHUD(win, *logCtrl, *mLmsComponent, *srvrLog);
   dtCore::RefPtr<TestAARInput> inputComp = new TestAARInput("TestInputComponent", *logCtrl, *hudComp);
   dtCore::RefPtr<TestAARMessageProcessor> mp = new TestAARMessageProcessor(*mLmsComponent, *logCtrl, *srvrLog);
  
   gameManager.AddComponent(*mp, dtGame::GameManager::ComponentPriority::HIGHEST);
   gameManager.AddComponent(*mLmsComponent, dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*inputComp, dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*logCtrl, dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*srvrLog, dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*hudComp, dtGame::GameManager::ComponentPriority::NORMAL);

   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(TestAARMessageType::RESET);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(TestAARMessageType::REQUEST_ALL_CONTROLLER_UPDATES);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(TestAARMessageType::PLACE_ACTOR);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(TestAARMessageType::PLACE_IGNORED_ACTOR);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(TestAARMessageType::PRINT_TASKS);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(TestAARMessageType::UPDATE_TASK_CAMERA);

   gameManager.SetStatisticsInterval(5);
   gameManager.SendMessage(*gameManager.GetMessageFactory().CreateMessage(TestAARMessageType::RESET));

   if(mUseLMS)
   {
      try
      {
         mLmsComponent->ConnectToLms();
      }
      catch (const dtUtil::Exception& e)
      {
         //could not connect to lms web page over given port; we will continue
         //the simulation however there will be no communication with the lms
         e.LogException(dtUtil::Log::LOG_WARNING);
      }
   }
}

void TestAAR::ParseCommandLineOptions(int argc, char **argv) const
{
   osg::ArgumentParser argParser(&argc, argv);

   argParser.getApplicationUsage()->setCommandLineUsage("TestAAR [options] value ...");
   argParser.getApplicationUsage()->addCommandLineOption("-lms", "Specify 1 to use LMS, or 0 otherwise");
   argParser.getApplicationUsage()->addCommandLineOption("-h or --help","Display command line options");
   argParser.getApplicationUsage()->addCommandLineOption("-d", "The datapath to be used for the application");

   int lms;
   if(argParser.read("-lms", lms))
      mUseLMS = lms;

   if(argParser.read("-h") || argParser.read("--help") || argParser.argc() == 0)
   {
      argParser.getApplicationUsage()->write(std::cerr);
      throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, 
         "Command Line Error.", __FILE__, __LINE__);
   }


   argParser.reportRemainingOptionsAsUnrecognized();
   if(argParser.errors())
   {
      argParser.writeErrorMessages(std::cout);
      throw dtUtil::Exception(dtGame::ExceptionEnum::GAME_APPLICATION_CONFIG_ERROR, 
         "Command Line Error.", __FILE__, __LINE__);
   }
}
