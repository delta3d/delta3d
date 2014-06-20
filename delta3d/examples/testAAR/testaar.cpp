/* -*-c++-*-
 * testAAR - testaar (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 * Christopher DuBuc
 * William E. Johnson II
 */

#include "testaar.h"
#include "testaarmessagetypes.h"
#include "testaarmessageprocessor.h"
#include "testaarinput.h"
#include "testaarhud.h"

#include <dtCore/flymotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtCore/actortype.h>
#include <dtCore/project.h>
#include <dtUtil/datapathutils.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>
#include <dtGame/logcontroller.h>
#include <dtGame/binarylogstream.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/exceptionenum.h>
#include <dtLMS/lmscomponent.h>
#include <osg/ArgumentParser>
#include <osg/ApplicationUsage>

#include <iostream>

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
TestAAR::TestAAR()
   : mLmsComponent(new dtLMS::LmsComponent())
   , mUseLMS(false)
{

}

//////////////////////////////////////////////////////////////////////////
TestAAR::~TestAAR()
{
   if (mUseLMS && mLmsComponent.valid())
   {
      mLmsComponent->DisconnectFromLms();
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAAR::Initialize(dtABC::BaseABC& app, int argc, char** argv)
{
   ParseCommandLineOptions(argc, argv);
   srand((unsigned int)(time(0)));

   dtCore::Transform tx(0.0f,-25.0f,10.0f,0.0f,-15.0f,0.0f);
   app.GetWindow()->SetWindowTitle("testAAR");
   app.GetCamera()->SetTransform(tx);

   mFMM = new dtCore::FlyMotionModel(app.GetKeyboard(), app.GetMouse(), dtCore::FlyMotionModel::OPTION_REQUIRE_MOUSE_DOWN);
   mFMM->SetTarget(app.GetCamera());
}


//////////////////////////////////////////////////////////////////////////
void TestAAR::OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gameManager)
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaRootPath() + "/examples/data/;" +
                               dtUtil::GetDeltaRootPath() + "/examples/data/gui/;" +
                               dataPath + ";" );

   std::string context = dtUtil::GetDeltaRootPath() + "/examples/testAAR/AARProject";

   try
   {
      dtCore::Project::GetInstance().SetContext(context, true);
   }
   catch (dtUtil::Exception& e)
   {
      LOG_ERROR("Can't find the project context: " + e.What());
   }

   // Add Component - Input Component
   dtCore::RefPtr<dtGame::LogController> logCtrl = new dtGame::LogController();
   dtCore::RefPtr<dtGame::BinaryLogStream> logStream = new dtGame::BinaryLogStream(gameManager.GetMessageFactory());
   dtCore::RefPtr<dtGame::ServerLoggerComponent> srvrLog = new dtGame::ServerLoggerComponent(*logStream);
   dtCore::RefPtr<TestAARHUD> hudComp = new TestAARHUD(app, *logCtrl, *mLmsComponent, *srvrLog);

   dtCore::RefPtr<TestAARInput> inputComp = new TestAARInput(*logCtrl, *hudComp);
   dtCore::RefPtr<TestAARMessageProcessor> mp = new TestAARMessageProcessor(*mLmsComponent, *logCtrl, *srvrLog);

   gameManager.AddComponent(*mp, dtGame::GameManager::ComponentPriority::HIGHEST);
   gameManager.AddComponent(*mLmsComponent, dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*inputComp, dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*logCtrl, dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*srvrLog, dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*hudComp, dtGame::GameManager::ComponentPriority::NORMAL);

   gameManager.DebugStatisticsTurnOn(false, false, 5);
   gameManager.SendMessage(*gameManager.GetMessageFactory().CreateMessage(TestAARMessageType::RESET));

   if (mUseLMS)
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

void TestAAR::ParseCommandLineOptions(int argc, char** argv) const
{
   osg::ArgumentParser argParser(&argc, argv);

   argParser.getApplicationUsage()->setCommandLineUsage("TestAAR [options] value ...");
   argParser.getApplicationUsage()->addCommandLineOption("-lms", "Specify 1 to use LMS, or 0 otherwise");
   argParser.getApplicationUsage()->addCommandLineOption("-h or --help","Display command line options");
   argParser.getApplicationUsage()->addCommandLineOption("-d", "The datapath to be used for the application");

   int lms;
   if (argParser.read("-lms", lms))
   {
      mUseLMS = (lms == 1) ? true : false;
   }

   if (argParser.read("-h") || argParser.read("--help") || argParser.argc() == 0)
   {
      argParser.getApplicationUsage()->write(std::cerr);
      throw dtGame::GameApplicationConfigException(
         "Command Line Error.", __FILE__, __LINE__);
   }

   argParser.reportRemainingOptionsAsUnrecognized();
   if (argParser.errors())
   {
      std::ostringstream oss;
      argParser.writeErrorMessages(oss);
      LOG_ERROR("Command line error: " + oss.str());
   }
}
