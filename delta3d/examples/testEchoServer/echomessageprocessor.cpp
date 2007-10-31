/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* @author Pjotr van Amerongen
*/
#include "echomessageprocessor.h"

EchoMessageProcessor::EchoMessageProcessor()
: dtGame::DefaultMessageProcessor("EchoMessageProcessor")
{
}

EchoMessageProcessor::~EchoMessageProcessor()
{

}

void EchoMessageProcessor::ProcessMessage(const dtGame::Message& msg)
{
	if (GetGameManager() == NULL)
	{
		LOG_ERROR("This component is not assigned to a GameManager, but received a message.  It will be ignored.");         
		return;
	}  

	if (msg.GetDestination() != NULL && GetGameManager()->GetMachineInfo() != *msg.GetDestination())
	{
		if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
		{
			LOG_DEBUG("Received message has a destination set to a different GameManager than this one. It will be ignored.");         
			return;
		}
	}

	// Handle Network messages....
	// Otherwise echo message
	if (msg.GetSource() != GetGameManager()->GetMachineInfo()) 
	{
        // Network messages will be handled by NetworkComponent, so only forward other messages to DefauttMessageProcessor
		if(msg.GetMessageType() != dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION ||
			msg.GetMessageType() != dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION ||
			msg.GetMessageType() != dtGame::MessageType::NETSERVER_REJECT_CONNECTION ||
			msg.GetMessageType() != dtGame::MessageType::INFO_CLIENT_CONNECTED ||
			msg.GetMessageType() != dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT ||
			msg.GetMessageType() != dtGame::MessageType::SERVER_REQUEST_REJECTED)
		{
            DefaultMessageProcessor::ProcessMessage(msg);
		}
        
        // Echo GameEvent messages from Clients
        if(msg.GetMessageType() == dtGame::MessageType::INFO_GAME_EVENT) 
		{
			EchoMessage(msg);
		}
	}
}

void EchoMessageProcessor::EchoMessage(const dtGame::Message& msg)
{
	dtCore::RefPtr<dtGame::Message> netMessage = GetGameManager()->GetMessageFactory().CreateMessage(msg.GetMessageType());	
	netMessage->SetDestination(&msg.GetSource());

	LOG_INFO("Echoing host: "  + msg.GetSource().GetName());

    GetGameManager()->SendNetworkMessage(*netMessage);
}

