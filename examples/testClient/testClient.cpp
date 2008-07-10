/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2007, MOVES Institute
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
*  Pjotr van Amerongen
*/

#include "testClient.h"
#include "clientmessageprocessor.h"
#include <dtNetGM/clientnetworkcomponent.h>
#include <dtNetGM/machineinfomessage.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventmanager.h>
#include <dtCore/deltawin.h>
#include <dtGame/basemessages.h>


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
	mClientGM = new dtGame::GameManager(*GetScene());
	mClientGM->GetMachineInfo().SetName("EchoClient");

	mClientNetwComp = new dtNetGM::ClientNetworkComponent("EchoNetwork", 1, "echoclient.log");
	mClientGM->AddComponent(*mClientNetwComp, dtGame::GameManager::ComponentPriority::HIGHER);

    mMessageProcessor = new ClientMessageProcessor();
	mClientGM->AddComponent(*mMessageProcessor, dtGame::GameManager::ComponentPriority::HIGHEST);
	
	bool bConnected = mClientNetwComp->SetupClient("localhost", 5555);
	if(bConnected) {
		dtCore::RefPtr<dtNetGM::MachineInfoMessage> msg;
        mClientGM->GetMessageFactory().CreateMessage(dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION, msg);	
		msg->SetMachineInfo(mClientGM->GetMachineInfo());
		msg->SetDestination(NULL); // We don't know our server yet......

		mClientGM->SendNetworkMessage(*msg);
        LOG_INFO("Requesting connection.");
	}
	else {
		LOG_ERROR("Error setting up client connection");
		return;
	}

	mEvent = new dtDAL::GameEvent("TestMessage");
	dtDAL::GameEventManager::GetInstance().AddEvent(*(mEvent.get()));

   dtABC::Application::Config();
   dtABC::Application::GetWindow()->SetWindowTitle("testClient");
}


void EchoClient::PreFrame(const double deltaFrameTime)
{
    static int iCounter = 0;

    // Create a single message / second @ 60 Hz.
    if(++iCounter%60 != 0) {
        return;
    }

    if(mClientNetwComp->IsConnectedClient())
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
    if(mClientNetwComp->IsConnectedClient())
    {
	    //notify server we are quitting
	    dtCore::RefPtr<dtGame::Message> msg = mClientGM->GetMessageFactory().CreateMessage(dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT);				
	    mClientGM->SendNetworkMessage(*msg);
    }

	//shutdown the networking
	mClientNetwComp->ShutdownNetwork();

	Application::Quit();
}

