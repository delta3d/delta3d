/* -*-c++-*-
 * testClient - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007, MOVES Institute
 * Copyright (C) 2014, Caper Holdings, LLC
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
 *  Francesco Argese
 *  Pjotr van Amerongen
 *  David Guthrie
 */

#include "testClient.h"
#include "clientmessageprocessor.h"
#include <dtNetGM/clientnetworkcomponent.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/deltawin.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>

#include <iostream>

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(AppException);
AppException AppException::INIT_ERROR("INIT_ERROR");

//////////////////////////////////////////////////////////////////////////
EchoClient::EchoClient()
: dtABC::Application("config.xml")
{
}

//////////////////////////////////////////////////////////////////////////
EchoClient::~EchoClient()
{
   Quit();
}

//////////////////////////////////////////////////////////////////////////
void EchoClient::Config()
{
   dtABC::Application::Config();
   dtABC::Application::GetWindow()->SetWindowTitle("testClient");

   mClientGM = new dtGame::GameManager(*GetScene());
   mClientGM->GetMachineInfo().SetName("EchoClient");

   mClientNetwComp = new dtNetGM::ClientNetworkComponent("EchoNetwork", 1, "echoclient.log");
   mClientGM->AddComponent(*mClientNetwComp, dtGame::GameManager::ComponentPriority::HIGHER);

   mMessageProcessor = new ClientMessageProcessor();
   mClientGM->AddComponent(*mMessageProcessor, dtGame::GameManager::ComponentPriority::HIGHEST);

   mEvent = new dtCore::GameEvent("TestMessage");
   dtCore::GameEventManager::GetInstance().AddEvent(*(mEvent.get()));

   bool bConnected = mClientNetwComp->SetupClient("localhost", 5555);
   if (bConnected)
   {
      mClientNetwComp->SendRequestConnectionMessage();
      LOG_INFO("Requesting connection.");
   }
   else
   {
      LOG_ERROR("Error setting up client connection");
      return;
   }

   
}

bool EchoClient::KeyPressed(const dtCore::Keyboard *kb, int key)
{
   bool verdict = Application::KeyPressed(kb, key);
   if (verdict)
   {
      return verdict;
   }

   switch (key)
   {
   case 'D':
      std::cout << "It was pressed 'D'" << std::endl;

      if (mClientNetwComp->IsConnectedClient())
      {
         // notify server we are quitting
         dtCore::RefPtr<dtGame::Message> msg = mClientGM->GetMessageFactory().CreateMessage(dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT);
         mClientGM->SendNetworkMessage(*msg);

         // Disconnect client from server --> After this statement there are problems in next connections!!!
         mClientNetwComp->Disconnect();
      }

      break;
   case 'C':
   {
      std::cout << "It was pressed 'C'" << std::endl;

      if(!mClientNetwComp->IsConnectedClient())
      {
         bool bConnected = mClientNetwComp->SetupClient("localhost", 5555);

         if (bConnected)
         {
            mClientNetwComp->SendRequestConnectionMessage();
            LOG_INFO("Requesting connection.");
         }
         else
         {
            LOG_ERROR("Error setting up client connection");
         }
      }
      break;
   }
   default:
      break;
   }

   return verdict;
}


void EchoClient::PreFrame(const double deltaFrameTime)
{
   static int iCounter = 0;

   // Create a single message / second @ 60 Hz.
   if (++iCounter % 60 != 0)
   {
      return;
   }

   if (mClientNetwComp && mClientNetwComp->IsConnectedClient())
   {
      dtCore::RefPtr<dtGame::GameEventMessage> eventMsg;
      mClientGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT, eventMsg);
      eventMsg->SetGameEvent(*(mEvent.get()));

      eventMsg->SetDestination(mClientNetwComp->GetServer());
      mClientGM->SendNetworkMessage(*eventMsg);
   }
}

void EchoClient::Quit()
{
   if (mClientNetwComp->IsConnectedClient())
   {
      // notify server we are quitting
      dtCore::RefPtr<dtGame::Message> msg = mClientGM->GetMessageFactory().CreateMessage(dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT);
      mClientGM->SendNetworkMessage(*msg);
   }

   // shutdown the networking
   mClientNetwComp->ShutdownNetwork();

   Application::Quit();
}

