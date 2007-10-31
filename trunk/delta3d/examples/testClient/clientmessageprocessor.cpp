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
#include "clientmessageprocessor.h"

ClientMessageProcessor::ClientMessageProcessor()
: dtGame::DefaultMessageProcessor("ClientMessageProcessor")
{
	SetName("ClientMessageProcessor");
}

ClientMessageProcessor::~ClientMessageProcessor()
{

}

void ClientMessageProcessor::ProcessMessage(const dtGame::Message& msg)
{ 
	if (GetGameManager() == NULL)
	{
		LOG_ERROR("This component is not assigned to a GameManager, but received a message.  It will be ignored.");         
		return;
	}  

	if (msg.GetDestination() != NULL && GetGameManager()->GetMachineInfo() != *msg.GetDestination())
	{
        LOG_DEBUG("Received message has a destination set to a different GameManager than this one. It will be ignored.");
	}

	if(msg.GetSource() != GetGameManager()->GetMachineInfo()) {
        LOG_INFO(msg.GetMessageType().GetName() + " message received from: " + msg.GetSource().GetName() + " [" + msg.GetSource().GetHostName() + "]");        
    }
    
    if(msg.GetMessageType() == dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT || 
        msg.GetMessageType() == dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION || 
        msg.GetMessageType() == dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION || 
        msg.GetMessageType() == dtGame::MessageType::NETSERVER_REJECT_CONNECTION || 
        msg.GetMessageType() == dtGame::MessageType::INFO_CLIENT_CONNECTED)
    {
        // These messages will be automatically processed by NetworkComponent
    }
    else 
    {
        DefaultMessageProcessor::ProcessMessage(msg);
    }
}
