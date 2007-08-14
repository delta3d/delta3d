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
#include "testechoserver.h"
#include <dtGame/gamemanager.h>
#include <dtNetGM/servernetworkcomponent.h>

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(AppException);
AppException AppException::INIT_ERROR("INIT_ERROR");

//////////////////////////////////////////////////////////////////////////
EchoServer::EchoServer()
: dtABC::Application("config.xml")
{
	SetName("EchoServer");
}

//////////////////////////////////////////////////////////////////////////
EchoServer::~EchoServer()
{	
	Quit();
}

//////////////////////////////////////////////////////////////////////////
void EchoServer::Config()
{
	mServerGM = new dtGame::GameManager(*GetScene());
	mServerGM->GetMachineInfo().SetName("EchoServer");

	mServerComp = new dtNetGM::ServerNetworkComponent("EchoNetwork", 1, "echoserver.log");
	mServerGM->AddComponent(*mServerComp, dtGame::GameManager::ComponentPriority::HIGHER);
	
	mEchoMessageProcessor = new EchoMessageProcessor();
	mServerGM->AddComponent(*mEchoMessageProcessor, dtGame::GameManager::ComponentPriority::HIGHEST);

	bool bServer = mServerComp->SetupServer(5555);
	if(!bServer) {
      throw dtUtil::Exception(AppException::INIT_ERROR, "\nError setting up server!!", __FILE__, __LINE__);
	}
	else {
		std::cout << "\nServer running on port 5555\n";
	}
}


void EchoServer::Quit()
{
	//shutdown the networking
	mServerComp->ShutdownNetwork();

	Application::Quit();
}
