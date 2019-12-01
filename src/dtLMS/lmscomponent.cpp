/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Christopher DuBuc
 */
#include <dtLMS/lmscomponent.h>

#include <dtCore/booleanactorproperty.h>
#include <dtCore/floatactorproperty.h>

#include <dtGame/gameactorproxy.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>

#include <dtLMS/lmsclientsocket.h>
#include <dtLMS/lmsmessagetype.h>
#include <dtLMS/lmsmessagevalue.h>

namespace dtLMS
{
   const dtCore::RefPtr<dtCore::SystemComponentType> LmsComponent::TYPE(new dtCore::SystemComponentType("LmsComponent","GMComponents",
         "Integration with Learning Management Systems.",
         dtGame::GMComponent::BaseGMComponentType));
   const std::string LmsComponent::DEFAULT_NAME(TYPE->GetName());

   LmsComponent::LmsComponent(dtCore::SystemComponentType& type)
      : dtGame::TaskComponent(type)
      , mClientSocket(NULL)
      , mHost("localhost")
      , mPort(4444)
      , mReverseBytes(true)
      , mNeedValidSocket(true)
   {
   }

   ///////////////////////////////////////////////////////////////////////
   LmsComponent::LmsComponent(const std::string& host, int port, bool reverseBytes)
      : dtGame::TaskComponent(*TYPE)
      , mClientSocket(NULL)
      , mHost(host)
      , mPort(port)
      , mReverseBytes(reverseBytes)
      , mNeedValidSocket(true)
   {

   }
   ///////////////////////////////////////////////////////////////////////
   void LmsComponent::ProcessMessage(const dtGame::Message& message)
   {
      // call parent class' ProcessMessage method
      dtGame::TaskComponent::ProcessMessage(message);

      // we are only interested in actor update messages
      if (message.GetMessageType() != dtGame::MessageType::INFO_ACTOR_UPDATED)
      {
         return;
      }

      // get the GameActorProxy that the message refers to
      const dtCore::UniqueId& id = message.GetAboutActorId();
      dtGame::GameActorProxy* proxy = GetGameManager()->FindGameActorById(id);
      if (proxy == NULL)
      {
         // parent class will have logged a warning, we will just return
         return;
      }

      // does this task require notifying an LMS of its changes in status?
      bool notifyLms = false;
      dtCore::ActorProperty* prop = proxy->GetProperty("NotifyLMSOnUpdate");
      if (prop != NULL)
      {
         notifyLms = static_cast<dtCore::BooleanActorProperty*>(prop)->GetValue();
      }

      if (notifyLms)
      {
         //send lms message
         if (mClientSocket != NULL && mClientSocket->GetClientState() == &LmsConnectionState::CONNECTED)
         {
            try
            {
               SendLmsUpdate(*proxy);
            }
            catch (const dtUtil::Exception& e)
            {
               e.LogException(dtUtil::Log::LOG_ERROR);
            }
         }
      }

   }
   ///////////////////////////////////////////////////////////////////////
   void LmsComponent::ConnectToLms()
   {
      mClientSocket = new LmsClientSocket(mHost, mPort, mReverseBytes);
      mClientSocket->Connect();
   }

   ///////////////////////////////////////////////////////////////////////
   void LmsComponent::DisconnectFromLms()
   {
      if (mClientSocket != NULL)
      {
         if (mClientSocket->GetClientState() == &LmsConnectionState::CONNECTED)
         {
            try
            {
               mClientSocket->Disconnect();
            }
            catch (dtUtil::Exception& e)
            {
               e.LogException(dtUtil::Log::LOG_WARNING);
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////
   void LmsComponent::SendLmsUpdate(dtGame::GameActorProxy& actor)
   {
      // if the client socket is not connected, then return (exceptions should have been
      // thrown elsewhere)
      if (mNeedValidSocket)
      {
         if (mClientSocket == NULL)
         {
            return;
         }
         else if (mClientSocket->GetClientState() != &LmsConnectionState::CONNECTED)
         {
            return;
         }
      }
      else
      {
         mMessageVector.clear();
      }

      // if this lms task is not yet being tracked for changes, then start tracking it;
      std::string taskID = actor.GetName();

      std::map<std::string, LmsTaskStatus>::iterator currentTask = mPreviousTaskStatus.find(taskID);
      if (currentTask == mPreviousTaskStatus.end())
      {
         // LmsTaskStatus initializes with GetCompleted = false and GetScore = 0
         currentTask = mPreviousTaskStatus.insert(std::make_pair(taskID, LmsTaskStatus())).first;
      }

      // if the completion status changed, then send message to LMS;
      // also we will remember the last value we sent so that we can
      // just send when changed
      dtCore::BooleanActorProperty* prop = static_cast<dtCore::BooleanActorProperty*>(actor.GetProperty("Complete"));
      bool taskIsComplete = prop->GetValue();

      if (taskIsComplete != currentTask->second.GetCompleted())
      {
         if (mNeedValidSocket)
         {
            mClientSocket->SendLmsMessage(TranslateObjectiveCompleteMessage(taskID, taskIsComplete));
         }
         else
         {
            mMessageVector.push_back(TranslateObjectiveCompleteMessage(taskID, taskIsComplete));
         }

         currentTask->second.SetCompleted(taskIsComplete); //remember last "complete" value we sent
      }

      // if the score changed, then send message to LMS;
      // also we will remember the last value we sent so that we can
      // just send when changed
      float taskScore = static_cast<dtCore::FloatActorProperty*>(actor.GetProperty("Score"))->GetValue();
      if (taskScore != currentTask->second.GetScore())
      {
         if (mNeedValidSocket)
         {
            mClientSocket->SendLmsMessage(TranslateObjectiveScoreMessage(taskID, taskScore));
         }
         else
         {
            mMessageVector.push_back(TranslateObjectiveScoreMessage(taskID, taskScore));
         }
         currentTask->second.SetScore(taskScore); //remember last "score" value we sent
      }
   }

   ///////////////////////////////////////////////////////////////////////
   LmsMessage LmsComponent::TranslateObjectiveCompleteMessage(const std::string& taskID, bool taskIsComplete)
   {
      std::string messageValue;

      if (taskIsComplete)
      {
         messageValue = LmsMessageValue::ObjectiveCompletionValue::COMPLETE.GetName();
      }
      else
      {
         messageValue = LmsMessageValue::ObjectiveCompletionValue::INCOMPLETE.GetName();
      }

      return LmsMessage(mClientSocket != NULL ? mClientSocket->GetClientID() : "",
         LmsMessageType::OBJECTIVE_COMPLETION, messageValue, taskID);
   }

   ///////////////////////////////////////////////////////////////////////
   LmsMessage LmsComponent::TranslateObjectiveScoreMessage(const std::string& taskID, float taskScore)
   {
      //convert score float value to string
      std::ostringstream oss;
      oss << taskScore;

      return LmsMessage(mClientSocket != NULL ? mClientSocket->GetClientID() : "",
         LmsMessageType::OBJECTIVE_SCORE, oss.str(), taskID);
   }

   ///////////////////////////////////////////////////////////////////////
   LmsComponent::~LmsComponent()
   {
      if (mClientSocket != NULL)
      {
         if (mClientSocket->GetClientState() == &LmsConnectionState::CONNECTED)
         {
            DisconnectFromLms();
         }

         delete mClientSocket;
         mClientSocket = NULL;
      }
   }

   ///////////////////////////////////////////////////////////////////////
   void LmsComponent::GetMessageVector(std::vector<LmsMessage>& toFill)
   {
      toFill.clear();
      for (unsigned int i = 0; i < mMessageVector.size(); ++i)
      {
         toFill.push_back(mMessageVector[i]);
      }
   }
} // namespace dtLMS
