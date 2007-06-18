/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * William E. Johnson II
 */
#include <fireFighter/gameentrypoint.h>
#include <fireFighter/messages.h>
#include <fireFighter/messagetype.h>
#include <fireFighter/hudcomponent.h>
#include <fireFighter/gamestate.h>
#include <fireFighter/inputcomponent.h>
#include <fireFighter/exception.h>
#include <dtCore/deltawin.h>
#include <dtCore/camera.h>
#include <dtCore/globals.h>
#include <dtDAL/project.h>
#include <dtABC/application.h>
#include <dtAudio/audiomanager.h>
#include <dtLMS/lmscomponent.h>
#include <dtGame/defaultmessageprocessor.h>
#include <osg/ArgumentParser>
#include <osg/ApplicationUsage>

using dtCore::RefPtr;

extern "C" FIRE_FIGHTER_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
{
   return new FireFighterGameEntryPoint;
}

extern "C" FIRE_FIGHTER_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint* entryPoint)
{
   delete entryPoint;
}

FireFighterGameEntryPoint::FireFighterGameEntryPoint() : mUseLMS(true)
{
   dtAudio::AudioManager::Instantiate();
   dtAudio::AudioManager::GetInstance().Config(AudioConfigData(32));
}

FireFighterGameEntryPoint::~FireFighterGameEntryPoint()
{
   dtAudio::AudioManager::Destroy();
}

void FireFighterGameEntryPoint::Initialize(dtGame::GameApplication& app, int argc, char **argv)
{
   // Parse the command line options
   osg::ArgumentParser parser(&argc, argv);
   parser.getApplicationUsage()->setCommandLineUsage("Fire Fighter Application [options] value ...");
   parser.getApplicationUsage()->addCommandLineOption("-h or --help","Display command line options");
   parser.getApplicationUsage()->addCommandLineOption("--useLms", "Pass 1 to indicate you wish to communicate with an external LMS, or 0 to ignore.");

   if(parser.read("-h") || parser.read("--help"))
   {
      parser.getApplicationUsage()->write(std::cerr);
      throw dtUtil::Exception(ExceptionEnum::COMMAND_LINE_EXCEPTION, "Command Line Error.", 
         __FILE__, __LINE__);
   }

   int commandLineParam = 0;
   if(parser.read("--useLMS", commandLineParam))
   {
      mUseLMS = commandLineParam ? true : false; 
   }

   parser.reportRemainingOptionsAsUnrecognized();
   if(parser.errors())
   {
      std::ostringstream oss;
      parser.writeErrorMessages(oss);
      LOG_ERROR("Command line error: " + oss.str());
   }

   app.GetWindow()->SetFullScreenMode(false);

   app.GetWindow()->SetWindowTitle("Fire Fighter Application");

   dtDAL::Project::GetInstance().SetContext("demos/fireFighter/FireFighterProject");
   dtCore::SetDataFilePathList(dtCore::GetDataFilePathList() + ";" + 
      dtDAL::Project::GetInstance().GetContext() + "/CEGUI");
}

void FireFighterGameEntryPoint::OnStartup()
{
   dtGame::GameManager& gameManager = *GetGameManager();
   // Make sure we can hear audio
   gameManager.GetApplication().GetCamera()->AddChild(dtAudio::AudioManager::GetListener());

   // Register the messages of the game with the game manager
   gameManager.GetMessageFactory().RegisterMessageType<GameStateChangedMessage>(MessageType::GAME_STATE_CHANGED);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(MessageType::ITEM_INTERSECTED);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(MessageType::ITEM_ACQUIRED);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(MessageType::ITEM_SELECTED);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(MessageType::ITEM_ACTIVATED);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(MessageType::ITEM_DEACTIVATED);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(MessageType::MISSION_COMPLETE);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(MessageType::MISSION_FAILED);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(MessageType::HELP_WINDOW_OPENED);
   gameManager.GetMessageFactory().RegisterMessageType<dtGame::Message>(MessageType::HELP_WINDOW_CLOSED);

   // Create the components and add them to the game manager
   RefPtr<HUDComponent>   hudComp   = new HUDComponent(*gameManager.GetApplication().GetWindow());
   RefPtr<InputComponent> inputComp = new InputComponent;
   RefPtr<dtGame::DefaultMessageProcessor> dmp = new dtGame::DefaultMessageProcessor("DefaultMessageProcessor");
   mLmsComponent = new dtLMS::LmsComponent("LMSComponent");

   gameManager.AddComponent(*hudComp,       dtGame::GameManager::ComponentPriority::HIGHER);
   gameManager.AddComponent(*inputComp,     dtGame::GameManager::ComponentPriority::NORMAL);
   gameManager.AddComponent(*dmp,           dtGame::GameManager::ComponentPriority::HIGHEST);
   gameManager.AddComponent(*mLmsComponent, dtGame::GameManager::ComponentPriority::NORMAL);

   // Connect to the LMS
   if(mUseLMS)
   {
      try
      {
         mLmsComponent->ConnectToLms();
      }
      catch(const dtUtil::Exception &e) 
      {
         // Failed to connect to the LMS, log the exception and continue with
         // the game as normal.
         e.LogException(dtUtil::Log::LOG_ALWAYS);
      }
   }

   // Send the message to switch to the menu
   RefPtr<dtGame::Message> msg = gameManager.GetMessageFactory().CreateMessage(MessageType::GAME_STATE_CHANGED);
   GameStateChangedMessage &gscm = static_cast<GameStateChangedMessage&>(*msg);
   gscm.SetOldState(GameState::STATE_UNKNOWN);
   gscm.SetNewState(GameState::STATE_MENU);
   gameManager.SendMessage(gscm);
}

void FireFighterGameEntryPoint::OnShutdown()
{  
   dtGame::GameManager& gameManager = *GetGameManager();

   if(mUseLMS && mLmsComponent.valid())
      mLmsComponent->DisconnectFromLms();

   gameManager.CloseCurrentMap();
}
