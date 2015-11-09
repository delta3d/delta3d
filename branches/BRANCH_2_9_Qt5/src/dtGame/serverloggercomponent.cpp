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
 * Curtiss Murphy, Matthew W. Campbell
 */
#include <prefix/dtgameprefix.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/loggermessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/machineinfo.h>
#include <dtGame/logstream.h>
#include <dtGame/basemessages.h>
#include <dtGame/loggermessages.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datetime.h>
#include <sstream>

namespace dtGame
{
   const std::string DEFAULT_LOGNAME = "D3DDefaultMessageLog";
   const std::string ServerLoggerComponent::AUTO_KEYFRAME_TIMER_NAME = "ServerLoggerKeyframeTimer";

   //////////////////////////////////////////////////////////////////////////
   ServerLoggerComponent::ServerLoggerComponent(LogStream& logStream, dtCore::SystemComponentType& name)
      : GMComponent(name)
      , mLogComponentMachineInfo(new MachineInfo("__Server Logger Component__"))
      , mPreviousLogState(&LogStateEnumeration::LOGGER_STATE_IDLE)
   {
      mLogStatus.SetStateEnum(LogStateEnumeration::LOGGER_STATE_IDLE);
      mLogStream = &logStream;
      SetLogDirectory(".");

      mNextMessage = NULL;
      mNextMessageSimTime = 0.0;

      mLogComponentMachineInfo->SetHostName("__Server Logger Component__");
      mLogComponentMachineInfo->SetIPAddress("0.0.0.0");
      mLogComponentMachineInfo->SetPing(1);

      mIgnoredMessageTypeList.insert(&dtGame::MessageType::INFO_MAPS_OPENED);
      mIgnoredMessageTypeList.insert(&dtGame::MessageType::INFO_MAPS_CLOSED);
      mIgnoredMessageTypeList.insert(&dtGame::MessageType::INFO_MAP_UNLOAD_BEGIN);
      mIgnoredMessageTypeList.insert(&dtGame::MessageType::INFO_MAP_UNLOADED);
   }

   //////////////////////////////////////////////////////////////////////////
   ServerLoggerComponent::~ServerLoggerComponent()
   {
      if (mLogStream.valid())
      {
         mLogStream->Close();
      }

      mRecordIgnoreList.clear();
      mPlaybackList.clear();
      mIgnoredMessageTypeList.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::ProcessMessage(const Message& message)
   {
      const MessageType& type = message.GetMessageType();

      if (type == MessageType::TICK_LOCAL)
      {
         ProcessTickMessage(static_cast<const TickMessage&>(message));
      }
      else if (type.GetCategory() == "Tick"  || type.GetCategory() == "System")
      {
         // do nothing.  Don't let it fall through or it will get logged!
      }
      else if (type == MessageType::LOG_REQ_GET_STATUS)
      {
         HandleStatusMessageRequest(message);
      }
      else if (type == MessageType::LOG_REQ_CHANGESTATE_PLAYBACK)
      {
         HandleChangeStatePlayback(message);
      }
      else if (type == MessageType::LOG_REQ_CHANGESTATE_RECORD)
      {
         HandleChangeStateRecord(message);
      }
      else if (type == MessageType::LOG_REQ_CHANGESTATE_IDLE)
      {
         HandleChangeStateIdle(message);
      }
      else if (type == MessageType::LOG_REQ_CAPTURE_KEYFRAME)
      {
         HandleCaptureKeyFrame(static_cast<const LogCaptureKeyframeMessage&>(message));
      }
      else if (type == MessageType::LOG_REQ_JUMP_TO_KEYFRAME)
      {
         HandleJumpToKeyFrame(static_cast<const LogJumpToKeyframeMessage&>(message));
      }
      else if (type == MessageType::LOG_REQ_GET_KEYFRAMES)
      {
         HandleRequestGetKeyFrames(message);
      }
      else if (type == MessageType::LOG_REQ_GET_LOGFILES)
      {
         HandleRequestGetLogs(message);
      }
      else if (type == MessageType::LOG_REQ_GET_TAGS)
      {
         HandleRequestGetTags(message);
      }
      else if (type == MessageType::LOG_REQ_INSERT_TAG)
      {
         HandleRequestInsertTag(static_cast<const LogInsertTagMessage&>(message));
      }
      else if (type == MessageType::LOG_REQ_DELETE_LOG)
      {
         HandleRequestDeleteLogFile(message);
      }
      else if (type == MessageType::LOG_REQ_SET_LOGFILE)
      {
         HandleRequestSetLogFile(message);
      }
      else if (type == MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL)
      {
         HandleRequestSetAutoKeyframeInterval(message);
      }
      else if (type == MessageType::INFO_MAP_LOADED)
      {
         HandleMapLoadedMessage(message);
      }
      else if (type == MessageType::LOG_REQ_ADD_IGNORED_ACTOR)
      {
         HandleAddIgnoredActorMessage(message);
      }
      else if (type == MessageType::LOG_REQ_REMOVE_IGNORED_ACTOR)
      {
         HandleRemoveIgnoredActorMessage(message);
      }
      else if (type == MessageType::LOG_REQ_CLEAR_IGNORE_LIST)
      {
         HandleClearIgnoreListMessage();
      }
      else if (type == MessageType::LOG_INFO_PLAYBACK_END_OF_MESSAGES)
      {
         // Do nothing. We don't want it to log.
      }
      else if (type == MessageType::INFO_MAP_CHANGE_BEGIN)
      {
         // Avoid recording map change events because they may cause havok
         // with lots of messages and state changes.
         if (mLogStatus.GetStateEnum() != LogStateEnumeration::LOGGER_STATE_IDLE)
         {
            mPreviousLogState = &mLogStatus.GetStateEnum();
            mPreviousRecordDuration = mLogStatus.GetCurrentRecordDuration();
            mPreviousNumberOfMessages = mLogStatus.GetNumMessages();

            //try
            //{
            //   // close any open records or playbacks.
            //   if (mLogStatus.GetStateEnum() == LogStateEnumeration::LOGGER_STATE_RECORD)
            //      mLogStream->SetRecordDuration(mLogStatus.GetCurrentRecordDuration());
            //   mLogStream->Close();
            //}
            //catch(const dtUtil::Exception &e)
            //{
            //   LOG_ERROR("FAILURE:changing state to IDLE from a map change event.");
            //   e.LogException(dtUtil::Log::LOG_ERROR);
            //}

            SetToIdleState();
            DoSendStatusMessage(NULL); // notify the world of our status change
         }
      }
      else if (type == MessageType::LOG_REQ_ADD_IGNORED_MESSAGETYPE)
      {
         HandleAddIgnoredMessageTypeMessage(message);
      }
      else if (type == MessageType::LOG_REQ_REMOVE_IGNORED_MESSAGETYPE)
      {
         HandleRemoveIgnoredMessageTypeMessage(message);
      }
      else if (type == MessageType::LOG_REQ_CLEAR_IGNORED_MESSAGETYPE_LIST)
      {
         HandleClearIgnoredMessageTypeMessage();
      }
      // One last step before passing the message to the log stream is to check
      // to see if this is a timer message for the auto keyframe capture.  If so,
      // perform a keyframe capture.
      // Note - we don't record INFO_TIMER_ELAPSED messages anymore. 
      else if (message.GetMessageType() == MessageType::INFO_TIMER_ELAPSED)
      {
         const TimerElapsedMessage& timerMsg =
            static_cast<const TimerElapsedMessage&>(message);

         if (timerMsg.GetTimerName() == ServerLoggerComponent::AUTO_KEYFRAME_TIMER_NAME)
         {
            if (mLogStatus.GetStateEnum() != LogStateEnumeration::LOGGER_STATE_RECORD)
            {
               GetGameManager()->RejectMessage(message, "Server Logger Component - Error: Attempted to "
                  "auto-capture keyframe while not in a record state.");
            }
            else
            {
               std::string time = dtUtil::DateTime::ToString(time_t(GetGameManager()->GetSimulationClockTime() / 1000000LL),
                  dtUtil::DateTime::TimeFormat::CLOCK_TIME_24_HOUR_FORMAT);

               LogKeyframe autoKeyFrame;
               autoKeyFrame.SetActiveMaps(mLogStatus.GetActiveMaps());
               autoKeyFrame.SetName("AutoKeyFrame " + time);
               autoKeyFrame.SetDescription("Auto captured keyframe.");
               autoKeyFrame.SetSimTimeStamp(mLogStatus.GetCurrentSimTime());
               DumpKeyFrame(autoKeyFrame);
            }
         }
      }

      else 
      {
         // If its not a message intended for the logger, it gets logged to the stream.
         DoRecordMessage(message);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleStatusMessageRequest(const Message& message)
   {
      DoRecordMessage(message);
      DoSendStatusMessage(message.GetDestination());
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::DoSendStatusMessage(const MachineInfo* destination)
   {
      dtCore::RefPtr<LogStatusMessage> response = static_cast<LogStatusMessage*>
            (GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_INFO_STATUS).get());

      response->SetStatus(mLogStatus);

      // Some status messages go only to a specific target (ie. the one that sent the request msg)
      if (destination != NULL)
      {
         response->SetDestination(destination);
      }

      GetGameManager()->SendMessage(*response.get());
      GetGameManager()->SendNetworkMessage(*response.get());
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleChangeStateIdle(const Message& message)
   {
      DoRecordMessage(message);

      if (mLogStatus.GetStateEnum() != LogStateEnumeration::LOGGER_STATE_IDLE)
      {
         try
         {
            // close any open records or playbacks.
            if (mLogStatus.GetStateEnum() == LogStateEnumeration::LOGGER_STATE_RECORD)
            {
               mLogStream->SetRecordDuration(mLogStatus.GetCurrentRecordDuration());
            }
            mLogStream->Close();
         }
         catch (const dtUtil::Exception& e)
         {
            // note, we keep going, because the default result of an exception is to change to idle, which is what we're doing.
            GetGameManager()->RejectMessage(message, "Server Logger Component - Error: Changing State to Idle: " + e.What());
         }

         SetToIdleState();
         DoSendStatusMessage(NULL); // notify the world of our status change
      }
      else
      {
         // send the status just to the requester.  No change, but might as well let him know.
         DoSendStatusMessage(&message.GetSource());
      }

   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleChangeStateRecord(const Message& message)
   {
      DoRecordMessage(message);

      if (mLogStatus.GetStateEnum() == LogStateEnumeration::LOGGER_STATE_PLAYBACK)
      {
         // can only change the log file in IDLE - reject and do nothing
         GetGameManager()->RejectMessage(message, "Server Logger Component - Error: Invalid "
            "attempt to Change State to Record while in Playback");
         return;
      }
      else if (mLogStatus.GetStateEnum() == LogStateEnumeration::LOGGER_STATE_IDLE)
      {
         try
         {
            mLogStream->Close(); // just for safety

            // use default name?
            if (mLogStatus.GetLogFile() == "")
            {
               mLogStatus.SetLogFile(DEFAULT_LOGNAME);
            }

            mLogStream->Create(mLogDirectory, mLogStatus.GetLogFile());
            mLogCache.insert(mLogStatus.GetLogFile());

            // insert first keyframe
            LogKeyframe firstKeyframe;
            firstKeyframe.SetActiveMaps(mLogStatus.GetActiveMaps());
            firstKeyframe.SetName("Default First Keyframe");
            firstKeyframe.SetDescription("Default first keyframe created when a new record is started");
            firstKeyframe.SetSimTimeStamp(mLogStatus.GetCurrentSimTime());
            DumpKeyFrame(firstKeyframe);

            // change state to RECORD
            mLogStatus.SetNumMessages(0);
            mLogStatus.SetCurrentRecordDuration(0.0);
            mLogStatus.SetStateEnum(LogStateEnumeration::LOGGER_STATE_RECORD);

            // If the auto capture keyframe is active, start a timer.
            if (mLogStatus.GetAutoRecordKeyframeInterval() != 0.0)
            {
               GetGameManager()->SetTimer(ServerLoggerComponent::AUTO_KEYFRAME_TIMER_NAME,NULL,
                  mLogStatus.GetAutoRecordKeyframeInterval(), true);
            }
         }
         catch (const dtUtil::Exception& e)
         {
            // if we got an error above, we revert back to IDLE and tell the world.  Not sure what else we can do
            GetGameManager()->RejectMessage(message, "Server Logger Component - Error occured "
               "trying to initialize record, will revert to IDLE.  Message: " + e.What());
            SetToIdleState();
            // clean up.  Note that it could except, but we don't care, just ignore it.
            try
            {
               mLogStream->Close();
            }
            catch (const dtUtil::Exception& e)
            {
               // LOG THE EXCEPTION
               e.LogException(dtUtil::Log::LOG_ERROR);
            } // ignore it.  Already failed, nothing we can do }
         }

         // notify the world of our new status - even after an error
         DoSendStatusMessage(NULL);
      }
      else
      {
         // send the status just to the requester.  No change, but might as well let him know.
         DoSendStatusMessage(message.GetDestination());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleChangeStatePlayback(const Message& message)
   {
      if (mLogStatus.GetStateEnum() == LogStateEnumeration::LOGGER_STATE_RECORD)
      {
         // can only change the log file in IDLE - reject and do nothing
         GetGameManager()->RejectMessage(message, "Server Logger Component - Error: Invalid "
            "attempt to Change State to Playback while in Record.");
         return;
      }
      else if (mLogStatus.GetStateEnum() == LogStateEnumeration::LOGGER_STATE_IDLE)
      {
         try
         {
            mLogStream->Close();

            if (mLogStatus.GetLogFile() == "")
            {
               mLogStatus.SetLogFile(DEFAULT_LOGNAME);
            }

            // Open the log file and read the first keyframe entry.  This contains the
            // initial state of the recorded simulation contained within the log.
            std::vector<LogKeyframe> kfList;
            mLogStream->Open(mLogDirectory,mLogStatus.GetLogFile());
            mLogStream->GetKeyFrameIndex(kfList);

//            for (size_t i = 0; i < kfList.size(); ++i)
//            {
//               LogKeyframe& kf = kfList[i];
//               std::cout << "\n\nKeyframe:" << kf.GetName() << "\n";
//               std::cout << "Offset:" << kf.GetLogFileOffset() << "\n";
//               std::cout << "Description:" << kf.GetDescription() << "\n";
//               std::cout << "Time Stamp:" << kf.GetSimTimeStamp() << std::endl;
//            }

            mLogStatus.SetCurrentRecordDuration(mLogStream->GetRecordDuration());
            if (kfList.empty())
            {
               throw dtGame::LogStreamIOException( "Malformed log.  No initial "
                  "keyframe could be found in the log.", __FILE__, __LINE__);
            }

            JumpToKeyFrame(kfList[0]);
            mLogStatus.SetStateEnum(LogStateEnumeration::LOGGER_STATE_PLAYBACK);
         }
         catch (const dtUtil::Exception& e)
         {
            e.LogException(dtUtil::Log::LOG_ERROR);
            // if we got an error above, we revert back to IDLE and tell the world.  Not sure what else we can do
            GetGameManager()->RejectMessage(message, "Server Logger Component - Error occured "
               "trying to initialize record, will revert to IDLE.  Message: " + e.What());
            SetToIdleState();
            // clean up.  Note that it could except, but we don't care, just ignore it.
            try
            {
               mLogStream->Close();
            }
            catch(const dtUtil::Exception& e2)
            {
               // LOG THE EXCEPTION
               e2.LogException(dtUtil::Log::LOG_ERROR);
            } // ignore it.  Already failed, nothing we can do }
         }

         // notify the world of our new status - even after an error
         DoSendStatusMessage(NULL);
      }
      else
      {
         // send the status just to the requester.  No change, but might as well let him know.
         DoSendStatusMessage(message.GetDestination());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleRequestSetLogFile(const Message& message)
   {
      DoRecordMessage(message);

      const dtGame::LogSetLogfileMessage* logMsg = (const dtGame::LogSetLogfileMessage*)&message;

      if (mLogStatus.GetStateEnum() != LogStateEnumeration::LOGGER_STATE_IDLE)
      {
         // can only change the log file in IDLE - reject and do nothing
         GetGameManager()->RejectMessage(message, "Server Logger Component - Error: Invalid attempt to set log file to [" +
            logMsg->GetLogFileName() + "] while in non-Idle state [" + mLogStatus.GetStateEnum().GetName() + "]");
         return;
      }
      else
      {
         mLogStatus.SetLogFile(logMsg->GetLogFileName());

         // notify the world of our status change
         DoSendStatusMessage(NULL);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleRequestGetLogs(const Message& message)
   {
      DoRecordMessage(message);

      dtCore::RefPtr<LogAvailableLogsMessage> response =
            static_cast<LogAvailableLogsMessage*>(GetGameManager()->GetMessageFactory().
                  CreateMessage(MessageType::LOG_INFO_LOGFILES).get());
      try
      {
         std::vector<std::string> logList;
         mLogStream->GetAvailableLogs(mLogDirectory,logList);
         response->SetLogList(logList);

         // Update our cache to reflect the most recent query.
         mLogCache.clear();
         for (size_t i = 0; i < logList.size(); ++i)
         {
            mLogCache.insert(logList[i]);
         }
      }
      catch (const dtUtil::Exception& e)
      {
         GetGameManager()->RejectMessage(message,
            "Server Logger Component - Could not retrieve log file list " + e.What());
      }

      GetGameManager()->SendMessage(*response.get());
      GetGameManager()->SendNetworkMessage(*response.get());
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleRequestSetAutoKeyframeInterval(const Message& message)
   {
      DoRecordMessage(message);
      const LogSetAutoKeyframeIntervalMessage& actual =
         static_cast<const LogSetAutoKeyframeIntervalMessage&>(message);

      // Need to create a timer that fires according to the autokeyframe interval.
      // We actually do not create it here, but when recording begins.  To handle the
      // request message we clear any old timers, validate the incoming data, and
      // inform the world of the change in status.  Once recording begins, the timer
      // will get set.
      float kfTime = actual.GetAutoKeyframeInterval();
      if (kfTime < 0.0f)
      {
         GetGameManager()->RejectMessage(message,
            "Server Logger Component - Auto Keyframe interval must be greater than or "
             "equal to zero.");
      }

      // clear our old timer and recreate it if in record mode
      if (mLogStatus.GetStateEnum() == LogStateEnumeration::LOGGER_STATE_RECORD)
      {
         GetGameManager()->ClearTimer(ServerLoggerComponent::AUTO_KEYFRAME_TIMER_NAME, NULL);
         if (kfTime > 0.0)
         {
            GetGameManager()->SetTimer(ServerLoggerComponent::AUTO_KEYFRAME_TIMER_NAME, NULL,
               kfTime, true);
         }
      }

      // Update the logger status and inform the world of the change.
      mLogStatus.SetAutoRecordKeyframeInterval(actual.GetAutoKeyframeInterval());
      DoSendStatusMessage(NULL);
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleRequestGetKeyFrames(const Message& message)
   {
      DoRecordMessage(message);

      dtCore::RefPtr<LogGetKeyframeListMessage> response;
      GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_INFO_KEYFRAMES, response);

      try
      {
         std::vector<LogKeyframe> kfList;
         mLogStream->GetKeyFrameIndex(kfList);
         response->SetKeyframeList(kfList);
      }
      catch(const dtUtil::Exception& e)
      {
         GetGameManager()->RejectMessage(message,
            "Server Logger Component - Could not retrieve key frame index: " + e.What());
      }

      GetGameManager()->SendMessage(*response);
      GetGameManager()->SendNetworkMessage(*response);
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleRequestGetTags(const Message& message)
   {
      DoRecordMessage(message);

      dtCore::RefPtr<LogGetTagListMessage> response =
            static_cast<LogGetTagListMessage*>(GetGameManager()->GetMessageFactory().
                  CreateMessage(MessageType::LOG_INFO_TAGS).get());
      try
      {
         std::vector<LogTag> tagList;
         mLogStream->GetTagIndex(tagList);
         response->SetTagList(tagList);
      }
      catch(const dtUtil::Exception& e)
      {
         GetGameManager()->RejectMessage(message,
            "Server Logger Component - Could not retrieve tags index: " + e.What());
      }

      GetGameManager()->SendMessage(*response.get());
      GetGameManager()->SendNetworkMessage(*response.get());
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleRequestInsertTag(const LogInsertTagMessage& message)
   {
      DoRecordMessage(message);
      if (mLogStatus.GetStateEnum() != LogStateEnumeration::LOGGER_STATE_RECORD)
      {
         GetGameManager()->RejectMessage(message, "Server Logger Component - Error: Attempted to capture keyframe "
            " while not in a record state.");
      }
      else
      {
         try
         {
            LogTag tag = message.GetTag();
            tag.SetSimTimeStamp(mLogStatus.GetCurrentSimTime());

            if (tag.GetCaptureKeyframe())
            {
               LogKeyframe kf;
               kf.SetName(tag.GetName() + "keyframecapture");
               kf.SetDescription(tag.GetDescription());
               kf.SetActiveMaps(mLogStatus.GetActiveMaps());
               kf.SetSimTimeStamp(mLogStatus.GetCurrentSimTime());
               kf.SetTagUniqueId(tag.GetUniqueId());
               DumpKeyFrame(kf);
               tag.SetKeyframeUniqueId(kf.GetUniqueId());
            }

            mLogStream->InsertTag(tag);
         }
         catch(const dtUtil::Exception& e)
         {
            GetGameManager()->RejectMessage(message,
               "Server Logger Component - Could not retrieve tags index: " + e.What());
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleRequestDeleteLogFile(const Message& message)
   {
      DoRecordMessage(message);

      const LogDeleteLogfileMessage& actual =
         static_cast<const LogDeleteLogfileMessage&>(message);

      try
      {
         mLogStream->Delete(mLogDirectory,actual.GetLogFileName());

         // Update our cache.
         std::set<std::string>::iterator itor = mLogCache.find(actual.GetLogFileName());
         if (itor != mLogCache.end())
         {
            mLogCache.erase(itor);
         }
      }
      catch(const dtUtil::Exception& e)
      {
         GetGameManager()->RejectMessage(message,
            "Server Logger Component - Could not delete log: " + e.What());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::ProcessTickMessage(const TickMessage& message)
   {
      long numKeyframeMsgsSkipped = 0;
      std::ostringstream ss;

      // at a minimum, store the current time on our log status.
      mLogStatus.SetCurrentSimTime(message.GetSimulationTime());

      if (mLogStatus.GetStateEnum() == LogStateEnumeration::LOGGER_STATE_RECORD)
      {
         mLogStatus.SetCurrentRecordDuration(mLogStatus.GetCurrentRecordDuration() +
               message.GetDeltaSimTime());
      }
      else if (mLogStatus.GetStateEnum() == LogStateEnumeration::LOGGER_STATE_PLAYBACK)
      {
         if (!mLogStream.valid() || mLogStream->IsEndOfStream())
         {
            return;
         }

         if (mNextMessage == NULL)
         {
            mNextMessage = mLogStream->ReadMessage(mNextMessageSimTime);
         }

         // while we got messages, do our stuff...
         while (mNextMessage != NULL && mNextMessageSimTime < GetGameManager()->GetSimulationTime())
         {
            const MessageType& type = mNextMessage->GetMessageType();

            // If an actor is destroyed, check and then update
            // the ignore list and the playback-join list appropriately
            // if the ID exists in those lists.
            if (type == MessageType::INFO_ACTOR_DELETED)
            {
               HandleRemovePlaybackActorMessage(*mNextMessage);
               mNextMessage->SetSource(*mLogComponentMachineInfo);
               mLogStatus.SetNumMessages(mLogStatus.GetNumMessages() + 1);
               GetGameManager()->SendMessage(*mNextMessage);
            }
            else if (type == MessageType::INFO_ACTOR_CREATED ||
               type == MessageType::INFO_ACTOR_UPDATED)
            {
               HandleAddPlaybackActorMessage(*mNextMessage);
               mNextMessage->SetSource(*mLogComponentMachineInfo);
               mLogStatus.SetNumMessages(mLogStatus.GetNumMessages() + 1);
               GetGameManager()->SendMessage(*mNextMessage);
            }
            // For keyframes, we loop to skip it all!  Later, this data may be resent to
            // create a heartbeat.  For now, we skip
            else if (type == MessageType::LOG_COMMAND_BEGIN_LOADKEYFRAME_TRANS)
            {
               // Curt todo - log keyframe data here if added to Begin message params
               LOG_DEBUG("Server Logger: Skipping keyframe... ");
               numKeyframeMsgsSkipped = 0;

               // skip all messages until end keyframe or empty (sort of an error)
               mNextMessage = mLogStream->ReadMessage(mNextMessageSimTime);
               while (mNextMessage != NULL && mNextMessage->GetMessageType() !=
                  MessageType::LOG_COMMAND_END_LOADKEYFRAME_TRANS)
               {
                  numKeyframeMsgsSkipped++; // for logging.
                  mNextMessage = mLogStream->ReadMessage(mNextMessageSimTime);
               }

               // Log either success or failure with num messages
               ss.str("");
               ss << numKeyframeMsgsSkipped;
               if (mNextMessage == NULL) // error - log it and keep going.
               {
                  // NOTE - We should get the keyframe from the begin (xcept it's not there now)
                  LOG_ERROR("Server Logger: Ran out of messages while skipping keyframe data. Skipped [" + ss.str() + "] messages.");
               }
               else
               {
                  LOG_DEBUG("Server Logger: Done skipping keyframe, skipped [" + ss.str() + "] messages.");
               }
            }
            // ignore all log messages... just logged for historical reference/debugging
            else if (type == MessageType::LOG_REQ_CHANGESTATE_PLAYBACK || type == MessageType::LOG_REQ_CHANGESTATE_RECORD ||
               type == MessageType::LOG_REQ_CHANGESTATE_IDLE || type == MessageType::LOG_REQ_CAPTURE_KEYFRAME ||
               type == MessageType::LOG_REQ_GET_KEYFRAMES || type == MessageType::LOG_REQ_GET_LOGFILES ||
               type == MessageType::LOG_REQ_GET_TAGS || type == MessageType::LOG_REQ_GET_STATUS ||
               type == MessageType::LOG_REQ_INSERT_TAG || type == MessageType::LOG_REQ_DELETE_LOG ||
               type == MessageType::LOG_REQ_SET_LOGFILE || type == MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL ||
               type == MessageType::LOG_INFO_KEYFRAMES || type == MessageType::LOG_INFO_LOGFILES ||
               type == MessageType::LOG_INFO_TAGS || type == MessageType::LOG_INFO_STATUS ||
               type == MessageType::LOG_REQ_JUMP_TO_KEYFRAME || type == MessageType::LOG_INFO_PLAYBACK_END_OF_MESSAGES)
            {
               // do nothing.
            }
            // for now, we're ignoring all command and request messages that could change the state
            // of the server.  Eventually, this needs to be handled more elegantly. Probably some sort of
            // awareness of being in playback mode on the rules component, or removing the rules component
            // all together, or maybe having a sense of remote vs local server type messages.
            else if (type == MessageType::COMMAND_LOAD_MAP || type == MessageType::COMMAND_PAUSE ||
               type == MessageType::COMMAND_RESUME || type == MessageType::COMMAND_RESTART ||
               type == MessageType::COMMAND_SET_TIME || type == MessageType::REQUEST_LOAD_MAP ||
               type == MessageType::REQUEST_PAUSE || type == MessageType::REQUEST_RESUME ||
               type == MessageType::REQUEST_RESTART || type == MessageType::REQUEST_SET_TIME )
            {
               // do nothing for now.  Maybe in the future.  For now, just ignore and don't send.
            }
            // Add other misc system oriented messages to be ignored here.
            else if (type == MessageType::INFO_TIMER_ELAPSED)
            {
               // do nothing
            }
            else
            {
               // No other complaints. So, get it ready and send it out.
               mNextMessage->SetSource(*mLogComponentMachineInfo);
               mLogStatus.SetNumMessages(mLogStatus.GetNumMessages() + 1);
               GetGameManager()->SendMessage(*mNextMessage);
               GetGameManager()->SendNetworkMessage(*mNextMessage);
            }

            // get our next message and do it all over again
            mNextMessage = mLogStream->ReadMessage(mNextMessageSimTime);
         }

         // NO MORE MESSAGES 
         if (mNextMessage == NULL)
         {
            // pause the server at the end of replay.
            GetGameManager()->SetPaused(true);

            // Notify others that we finished playback. 
            dtCore::RefPtr<dtGame::Message> endOfPlaybackMsg = GetGameManager()->GetMessageFactory().
               CreateMessage(MessageType::LOG_INFO_PLAYBACK_END_OF_MESSAGES).get();
            GetGameManager()->SendMessage(*endOfPlaybackMsg.get());
            GetGameManager()->SendNetworkMessage(*endOfPlaybackMsg.get());
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleJumpToKeyFrame(const LogJumpToKeyframeMessage& msg)
   {
      std::string keyframeName("UNKNOWN");

      try
      {
         DoRecordMessage(msg);
         if (mLogStatus.GetStateEnum() != LogStateEnumeration::LOGGER_STATE_PLAYBACK)
         {
            GetGameManager()->RejectMessage(msg, "Server Logger Component - Error: Attempted to jump to keyframe "
               " while not in the playback state.");
         }
         else
         {
            LogKeyframe kf = msg.GetKeyframe();
            keyframeName = kf.GetName();
            JumpToKeyFrame(kf);
         }
      }
      catch(const dtUtil::Exception& e)
      {
         // if we got an error above, we revert back to IDLE and tell the world.  Not sure what else we can do
         GetGameManager()->RejectMessage(msg, "Server Logger Component - Error occured "
            "trying to jump to keyframe [" + keyframeName + "] with error: " + e.ToString());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleCaptureKeyFrame(const LogCaptureKeyframeMessage& msg)
   {
      DoRecordMessage(msg);
      if (mLogStatus.GetStateEnum() != LogStateEnumeration::LOGGER_STATE_RECORD)
      {
         GetGameManager()->RejectMessage(msg, "Server Logger Component - Error: Attempted to capture keyframe "
            " while not in a record state.");
      }
      else
      {
         LogKeyframe kf = msg.GetKeyframe();
         kf.SetSimTimeStamp(mLogStatus.GetCurrentSimTime());
         DumpKeyFrame(kf);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::SetToIdleState()
   {
      GetGameManager()->ClearTimer(ServerLoggerComponent::AUTO_KEYFRAME_TIMER_NAME,NULL);
      mLogStatus.SetStateEnum(LogStateEnumeration::LOGGER_STATE_IDLE);
      mLogStatus.SetCurrentRecordDuration(0.0);
      mLogStatus.SetNumMessages(0);
      RequestDeletePlaybackActors();
   }

   //////////////////////////////////////////////////////////////////////////
   bool ServerLoggerComponent::SetLogDirectory(const std::string& dir)
   {
      if (dir.empty())
      {
         // This should throw an exception, but I'll log an error and return true or
         // false since the rest of the method looks like it is going out of it's way
         // to do that
         LOG_ERROR("The ServerLoggerComponent tried to set its log directory to an empty string.");
         return false;

         //throw dtUtil::Exception("Tried to set the log directory to an empty string.",
         //   __FILE__, __LINE__);
      }
      // Make sure we remove any trailing slashes from the path.
      std::string newPath = dir;
      if (newPath[newPath.length()-1] == '/' || newPath[newPath.length()-1] == '\\')
      {
         newPath = newPath.substr(0,newPath.length()-1);
      }

      newPath = dtUtil::FileUtils::GetInstance().GetAbsolutePath(newPath);
      if (!dtUtil::FileUtils::GetInstance().DirExists(newPath))
      {
         try
         {
            dtUtil::FileUtils::GetInstance().MakeDirectory(newPath);
         }
         catch (const dtUtil::Exception& ex)
         {
            ex.LogException(dtUtil::Log::LOG_ERROR);
            return false;
         }
      }

      mLogDirectory = dir;
      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::DumpKeyFrame(LogKeyframe& kf)
   {
      std::vector<GameActorProxy*> actors;
      std::vector<GameActorProxy*>::iterator actorItor;

      // Inserting a keyframe into the log stream effectively marks the beginning
      // of a new keyframe.  So mark it, and start writing messages.
      try
      {
         mLogStream->InsertKeyFrame(kf);
      }
      catch (const dtUtil::Exception& e)
      {
         LOG_ERROR("Caught exception while inserting keyframe: " + e.ToString());
         return;
      }

      // The first message to write is a begin keyframe transaction message.
      dtCore::RefPtr<Message> kfMsg =
            (GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_COMMAND_BEGIN_LOADKEYFRAME_TRANS).get());
      mLogStream->WriteMessage(*kfMsg.get(),mLogStatus.GetCurrentSimTime());

      GetGameManager()->GetAllGameActors(actors);
      for (actorItor = actors.begin(); actorItor != actors.end(); ++actorItor)
      {
         if (!IsActorIdInList((*actorItor)->GetId(), mRecordIgnoreList))
         {
            // For each game actor we need to build an actor update message, ask the
            // actor to fill the message with its current property state, and then
            // dump the update message to the stream.
            dtCore::RefPtr<dtGame::ActorUpdateMessage> updateMsg = static_cast<dtGame::ActorUpdateMessage*>
               (GetGameManager()->GetMessageFactory().CreateMessage(MessageType::INFO_ACTOR_UPDATED).get());

            (*actorItor)->PopulateActorUpdate(*updateMsg.get());
            mLogStream->WriteMessage(*updateMsg.get(), mLogStatus.GetCurrentSimTime());
         }
      }

      // We flag a keyframe as complete by adding a END_KEYFRAME message.
      dtCore::RefPtr<LogEndLoadKeyframeMessage> endMsg = static_cast<LogEndLoadKeyframeMessage*>
         (GetGameManager()->GetMessageFactory().CreateMessage(MessageType::LOG_COMMAND_END_LOADKEYFRAME_TRANS).get());
      endMsg->SetSuccessFlag(true);
      mLogStream->WriteMessage(*endMsg, mLogStatus.GetCurrentSimTime());
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::JumpToKeyFrame(LogKeyframe& kf)
   {
      double simTime;

      std::map< dtCore::UniqueId, dtCore::RefPtr<Message> > updateMap;
      std::map< dtCore::UniqueId, dtCore::RefPtr<Message> >::iterator updateMapItor;

      // First, position the stream at the start of the keyframe.
      mLogStream->JumpToKeyFrame(kf);

      // Now read the first message.  It should be a LOG_COMMAND_BEGIN_LOAD_KEYFRAME message.
      dtCore::RefPtr<Message> kfMsg = mLogStream->ReadMessage(simTime);
      if (!kfMsg.valid() || kfMsg->GetMessageType() != MessageType::LOG_COMMAND_BEGIN_LOADKEYFRAME_TRANS)
      {
         throw dtGame::LogStreamIOException( "Malformed keyframe detected in the log "
            "stream.  Cannot proceed.", __FILE__, __LINE__);
      }

      GetGameManager()->SendMessage(*kfMsg.get());
      GetGameManager()->SendNetworkMessage(*kfMsg.get());

      // Read all messages from the keyframe, track them, then compare them to what's
      // currently in the game.  If an actor exists that is not in the game, send a delete
      // message, else send and update/create message for the actor contained within the
      // keyframe.
      kfMsg = mLogStream->ReadMessage(simTime);
      while (kfMsg->GetMessageType() != MessageType::LOG_COMMAND_END_LOADKEYFRAME_TRANS)
      {
         // add actor to playback list
         HandleAddPlaybackActorMessage(*kfMsg);

         mLogStatus.SetNumMessages(mLogStatus.GetNumMessages() + 1);
         updateMap.insert(std::make_pair(kfMsg->GetAboutActorId(), kfMsg));
         kfMsg = mLogStream->ReadMessage(simTime);
      }

      std::vector<GameActorProxy*> gameProxies;
      std::vector<GameActorProxy*>::iterator proxyItor;
      GetGameManager()->GetAllGameActors(gameProxies);
      for (proxyItor = gameProxies.begin(); proxyItor != gameProxies.end(); ++proxyItor)
      {
         std::map<dtCore::UniqueId,dtCore::RefPtr<Message> >::iterator kfActorItor;

         GameActorProxy* proxy = static_cast<GameActorProxy*>((*proxyItor));
         kfActorItor = updateMap.find(proxy->GetId());

         if (kfActorItor == updateMap.end()
            && IsActorIdInList(proxy->GetId(), mPlaybackList))
         {
            // Since the actor is not in the keyframe delete it.  Do this by processing/sending
            // a message so remote objects will get removed as well.
            dtCore::RefPtr<Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(MessageType::INFO_ACTOR_DELETED);
            msg->SetAboutActorId(proxy->GetId());
            msg->SetDestination(&GetGameManager()->GetMachineInfo());
            GetGameManager()->SendMessage(*msg);
            GetGameManager()->SendNetworkMessage(*msg);

            // Before deleting the actor, remove its id from the playback list
            HandleRemovePlaybackActorMessage(*msg);

            // Request the GM to delete the actor
            GetGameManager()->DeleteActor(*proxy);
         }
      }

      for (updateMapItor = updateMap.begin(); updateMapItor != updateMap.end(); ++updateMapItor)
      {
         // If it is in the keyframe then send out an update message.  Note, that the
         // update message causes the actor to be created if it does not yet exist.
         updateMapItor->second->SetSource(*mLogComponentMachineInfo);
         GetGameManager()->SendMessage(*(updateMapItor->second.get()));
         GetGameManager()->SendNetworkMessage(*(updateMapItor->second.get()));
      }

      // Finally, send out the simulation time located in the keyframe and send out
      // the keyframe end message.
      dtCore::RefPtr<LogEndLoadKeyframeMessage> endMsg =
         static_cast<LogEndLoadKeyframeMessage*>(kfMsg.get());

      // reset our next message so that time and message will get updated on next tick
      mNextMessage = NULL;

      GetGameManager()->ChangeTimeSettings(simTime, GetGameManager()->GetTimeScale(), GetGameManager()->GetSimulationClockTime());
      GetGameManager()->SendMessage(*endMsg.get());
      GetGameManager()->SendNetworkMessage(*endMsg.get());
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleMapLoadedMessage(const Message& message)
   {
      DoRecordMessage(message);

      // if the message is from our GM, then we want to update our status, otherwise,
      // we ignore it (could be one of many client messages).
      if (message.GetSource() == GetGameManager()->GetMachineInfo())
      {
         const MapMessage& loadedMsg = static_cast<const MapMessage&>(message);
         LogStatus::NameVector mapNames;
         loadedMsg.GetMapNames(mapNames);
         mLogStatus.SetActiveMaps(mapNames);

         if (*mPreviousLogState != LogStateEnumeration::LOGGER_STATE_IDLE)
         {
            mLogStatus.SetStateEnum(*mPreviousLogState);
            mLogStatus.SetCurrentRecordDuration(mPreviousRecordDuration);
            mLogStatus.SetNumMessages(mPreviousNumberOfMessages);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::DoRecordMessage(const Message& message)
   {
      if (!OkToRecord(message))
      {
         return;
      }

      try
      {
         mLogStream->WriteMessage(message, mLogStatus.GetCurrentSimTime());
         mLogStatus.SetNumMessages(mLogStatus.GetNumMessages() + 1);
      }
      catch (const dtUtil::Exception& e)
      {
         std::string messageString;
         message.ToString(messageString);
         LOG_ERROR("Server Logger: Error writing message in Record mode: Message[" +
            message.GetMessageType().GetName() + "], MsgData[" +
            messageString + "], Exception[" + e.What() + "]");
      }      
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleAddPlaybackActorMessage(const Message& message)
   {
      // Make sure no ignored actors get added when they join the playback simulation.
      if (!message.GetAboutActorId().ToString().empty() &&
         !IsActorIdInList(message.GetAboutActorId(), mPlaybackList))
      {
         mPlaybackList.insert(message.GetAboutActorId());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleRemovePlaybackActorMessage(const Message& message)
   {
      std::set<dtCore::UniqueId>::iterator itor = mPlaybackList.find(message.GetAboutActorId());
      if (itor != mPlaybackList.end())
      {
         mPlaybackList.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleClearPlaybackListMessage()
   {
      mPlaybackList.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleAddIgnoredActorMessage(const Message& message)
   {
      mRecordIgnoreList.insert(message.GetAboutActorId());

      // If an actor came in on playback but is not part of the simulation and
      // changes to ignored state, remove it from the playback list since all
      // joining actor not initially logged as ignored will be shoved into the
      // playback list.
      if (mLogStatus.GetStateEnum() != LogStateEnumeration::LOGGER_STATE_PLAYBACK
         && IsActorIdInList(message.GetAboutActorId(), mPlaybackList))
      {
         HandleRemovePlaybackActorMessage(message);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleRemoveIgnoredActorMessage(const Message& message)
   {
      std::set<dtCore::UniqueId>::iterator itor = mRecordIgnoreList.find(message.GetAboutActorId());
      if (itor != mRecordIgnoreList.end())
      {
         mRecordIgnoreList.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleClearIgnoreListMessage()
   {
      mRecordIgnoreList.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   bool ServerLoggerComponent::IsActorIdInList(const dtCore::UniqueId& actorID, const std::set<dtCore::UniqueId>& checkedSet) const
   {
      return checkedSet.find(actorID) != checkedSet.end();
   }

   //////////////////////////////////////////////////////////////////////////
   bool ServerLoggerComponent::IsActorIdInList(dtCore::UniqueId actorID, std::set<dtCore::UniqueId>& checkedSet)
   {
      return checkedSet.find(actorID) != checkedSet.end();
   }

   //////////////////////////////////////////////////////////////////////////
   int ServerLoggerComponent::GetIgnoredActorCount() const
   {
      return mRecordIgnoreList.size();
   }

   //////////////////////////////////////////////////////////////////////////
   int ServerLoggerComponent::GetPlaybackActorCount() const
   {
      return mPlaybackList.size();
   }

   //////////////////////////////////////////////////////////////////////////
   bool ServerLoggerComponent::IsIgnoredActorId(const dtCore::UniqueId& id) const
   {
      return IsActorIdInList(id, mRecordIgnoreList);
   }

   //////////////////////////////////////////////////////////////////////////
   bool ServerLoggerComponent::IsPlaybackActorId(const dtCore::UniqueId& id) const
   {
      return IsActorIdInList(id, mPlaybackList);
   }

   //////////////////////////////////////////////////////////////////////////
   bool ServerLoggerComponent::IsIgnoredActorId(dtCore::UniqueId id)
   {
      return IsActorIdInList(id, mRecordIgnoreList);
   }

   //////////////////////////////////////////////////////////////////////////
   bool ServerLoggerComponent::IsPlaybackActorId(dtCore::UniqueId id)
   {
      return IsActorIdInList(id, mPlaybackList);
   }

   //////////////////////////////////////////////////////////////////////////
   int ServerLoggerComponent::RequestDeletePlaybackActors()
   {
      int deleteCount = 0;
      for (std::set<dtCore::UniqueId>::iterator itor = mPlaybackList.begin();
           itor != mPlaybackList.end();
           ++itor)
      {
         dtCore::RefPtr<Message> msg =
            GetGameManager()->GetMessageFactory().CreateMessage(MessageType::INFO_ACTOR_DELETED);
         msg->SetAboutActorId(*itor);
         msg->SetDestination(&GetGameManager()->GetMachineInfo());
         GetGameManager()->SendMessage(*msg);
         ++deleteCount;
      }

      // Clean the playback actor ID collection.
      HandleClearPlaybackListMessage();

      return deleteCount;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ServerLoggerComponent::OkToRecord(const Message& message) const
   {
      if (mLogStatus.GetStateEnum() != LogStateEnumeration::LOGGER_STATE_RECORD)
      {
         return false;
      }

      if (IsActorIdInList(message.GetAboutActorId(), mRecordIgnoreList))
      {
         return false;
      }

      if (mIgnoredMessageTypeList.find(&message.GetMessageType()) != mIgnoredMessageTypeList.end())
      {
         return false;
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleAddIgnoredMessageTypeMessage(const Message& message)
   {
      const LogIgnoreMessageTypeMessage* ignoreMessage = static_cast<const LogIgnoreMessageTypeMessage*>(&message);
      const dtGame::MessageType& msgType = GetGameManager()->GetMessageFactory().GetMessageTypeById(ignoreMessage->GetIgnoredMessageType());
      mIgnoredMessageTypeList.insert(&msgType);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleRemoveIgnoredMessageTypeMessage(const Message& message)
   {
      const LogIgnoreMessageTypeMessage* ignoreMessage = static_cast<const LogIgnoreMessageTypeMessage*>(&message);
      const dtGame::MessageType& msgType = GetGameManager()->GetMessageFactory().GetMessageTypeById(ignoreMessage->GetIgnoredMessageType());

      std::set<const dtGame::MessageType*>::iterator itr = mIgnoredMessageTypeList.find(&msgType);
      if (itr != mIgnoredMessageTypeList.end())
      {
         mIgnoredMessageTypeList.erase(itr);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerLoggerComponent::HandleClearIgnoredMessageTypeMessage()
   {
      mIgnoredMessageTypeList.clear();
   }
} // namespace dtGame
