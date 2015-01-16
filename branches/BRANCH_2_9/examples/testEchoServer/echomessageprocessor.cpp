/* -*-c++-*-
 * testEchoServer - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007, MOVES Institute
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
 * @author Pjotr van Amerongen
 */

#include "echomessageprocessor.h"
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>

EchoMessageProcessor::EchoMessageProcessor()
   : dtGame::DefaultMessageProcessor()
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
      if (msg.GetMessageType() != dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION ||
          msg.GetMessageType() != dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION  ||
          msg.GetMessageType() != dtGame::MessageType::NETSERVER_REJECT_CONNECTION  ||
          msg.GetMessageType() != dtGame::MessageType::INFO_CLIENT_CONNECTED        ||
          msg.GetMessageType() != dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT  ||
          msg.GetMessageType() != dtGame::MessageType::SERVER_REQUEST_REJECTED)
      {
         DefaultMessageProcessor::ProcessMessage(msg);
      }

      // Echo GameEvent messages from Clients
      if (msg.GetMessageType() == dtGame::MessageType::INFO_GAME_EVENT)
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

