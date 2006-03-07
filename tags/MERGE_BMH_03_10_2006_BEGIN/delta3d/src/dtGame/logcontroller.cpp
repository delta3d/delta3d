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
* @author Curtiss Murphy
*/

#include "dtGame/logcontroller.h"
#include "dtGame/loggermessages.h"
#include "dtGame/messagetype.h"
#include "dtGame/gamemanager.h"
#include "dtGame/logkeyframe.h"
#include "dtGame/logtag.h"
#include "dtGame/logstatus.h"

namespace dtGame 
{
   
   //////////////////////////////////////////////////////////////////////////
   LogController::LogController()
   {
   }
   
   //////////////////////////////////////////////////////////////////////////
   LogController::~LogController()
   {
   }
   
   //////////////////////////////////////////////////////////////////////////
   void LogController::ProcessMessage(const Message& message)
   {
      if (message.GetMessageType() == MessageType::LOG_INFO_STATUS) 
      {
         const dtGame::LogStatusMessage *statusMsg = (const dtGame::LogStatusMessage *) &message;
         mLastKnownStatus = statusMsg->GetStatus();
         _receivedStatus(mLastKnownStatus);
      }
      else if (message.GetMessageType() == MessageType::SERVER_REQUEST_REJECTED)
      {
         if (message.GetCausingMessage() != NULL) 
         {
            const MessageType &type = message.GetCausingMessage()->GetMessageType();

            // a rejected message must be addressed to us, so figure out if it's a logger 
            // message.  if so, send out to anyone listening to our signal
            if (type == MessageType::LOG_REQ_CHANGESTATE_PLAYBACK || type == MessageType::LOG_REQ_CHANGESTATE_RECORD || 
               type == MessageType::LOG_REQ_CHANGESTATE_IDLE || type == MessageType::LOG_REQ_CAPTURE_KEYFRAME ||
               type == MessageType::LOG_REQ_GET_KEYFRAMES || type == MessageType::LOG_REQ_GET_LOGFILES ||
               type == MessageType::LOG_REQ_GET_TAGS || type == MessageType::LOG_REQ_GET_STATUS ||
               type == MessageType::LOG_REQ_INSERT_TAG || type == MessageType::LOG_REQ_DELETE_LOG ||
               type == MessageType::LOG_REQ_SET_LOGFILE || type == MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL ||
               type == MessageType::LOG_INFO_KEYFRAMES || type == MessageType::LOG_INFO_LOGFILES ||
               type == MessageType::LOG_INFO_TAGS || type == MessageType::LOG_INFO_STATUS || 
               type == MessageType::LOG_COMMAND_BEGIN_LOADKEYFRAME_TRANS || type == MessageType::LOG_COMMAND_END_LOADKEYFRAME_TRANS)
            {
               // send the status out to anyone that was listening for the signal
               _receivedRejection(message);
            }
         }
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestChangeStateToPlayback()
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_CHANGESTATE_PLAYBACK);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }

   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestChangeStateToRecord()
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_CHANGESTATE_RECORD);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }

   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestChangeStateToIdle()
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_CHANGESTATE_IDLE);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }

   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestCaptureKeyframe(const LogKeyframe &keyframe)
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_CAPTURE_KEYFRAME);
      LogCaptureKeyframeMessage *pMsg = static_cast<LogCaptureKeyframeMessage *> (message.get());
      pMsg->SetKeyframe(keyframe);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }

   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestServerGetKeyframes()
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_GET_KEYFRAMES);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }

   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestServerGetLogfiles()
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_GET_LOGFILES);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }

   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestServerGetTags()
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_GET_TAGS);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }
   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestServerGetStatus()
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_GET_STATUS);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }

   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestInsertTag(const LogTag &tag)
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_INSERT_TAG);
      LogInsertTagMessage *pMsg = static_cast<LogInsertTagMessage *> (message.get());
      pMsg->SetTag(tag);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }

   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestDeleteLogFile(const std::string &logFile)
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_DELETE_LOG);
      LogDeleteLogfileMessage *pMsg = static_cast<LogDeleteLogfileMessage *> (message.get());
      pMsg->SetLogFileName(logFile);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }

   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestSetLogFile(const std::string &logFile)
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_SET_LOGFILE);
      LogSetLogfileMessage *pMsg = static_cast<LogSetLogfileMessage *> (message.get());
      pMsg->SetLogFileName(logFile);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }

   //////////////////////////////////////////////////////////////////////////
   void LogController::RequestSetAutoKeyframeInterval(const double interval)
   {
      dtCore::RefPtr<Message> message = 
         GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL);
      LogSetAutoKeyframeIntervalMessage *pMsg = static_cast<LogSetAutoKeyframeIntervalMessage *> (message.get());
      pMsg->SetAutoKeyframeInterval(interval);

      GetGameManager()->ProcessMessage(*message);
      GetGameManager()->SendMessage(*message);
   }

}
