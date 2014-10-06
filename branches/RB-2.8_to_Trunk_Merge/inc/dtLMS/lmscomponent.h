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
#ifndef DELTA_LMS_COMPONENT
#define DELTA_LMS_COMPONENT

#include <dtGame/taskcomponent.h>
#include <dtLMS/lmstaskstatus.h>
#include <dtLMS/lmsmessage.h>
#include <dtLMS/export.h>
#include <map>

// Forward declarations
namespace dtGame
{
   class GameActorProxy;
   class Message;
}

namespace dtLMS
{
   class LmsClientSocket;

   /**
    * This class is a sub-class of the standard TaskComponent. It adds the functionality
    * to forward task update messages to an lms system via an lms messaging applet.
    */
   class DT_LMS_EXPORT LmsComponent : public dtGame::TaskComponent
   {
   public:

      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;
      static const std::string DEFAULT_NAME;

      /**
       * Constructs the LmsComponent
       */
      LmsComponent(dtCore::SystemComponentType& type = *TYPE);

      /**
       * Constructs the LmsComponent
       *
       * @param host The host machine that the lms messaging applet is running on.
       * @param port The port on the host machine that the lms messaging applet is listening on.
       * @param reverseBytes If true, Informs the server that it needs to reverse the byte order
       *    for multi-byte messages (big/little endian issue)
       */
      LmsComponent(const std::string& host, int port, bool reverseBytes);

      /**
       * Called when a message arrives at the Game Manager.  These messages are trapped
       * anytime an actor create,destroy or update message arrives and are used to update
       * this components list of managed task actors. Overrides a method in TaskComponent.
       *
       * @param message The message being propogated by the game manager.
       */
      virtual void ProcessMessage(const dtGame::Message& message);

      /**
       * This attempts to connect to an lms messaging applet running on the specified host over
       * the specified socket. If successful, mClientSocket::GetClientState() will be set to
       * LmsConnectionState::CONNECTED.
       */
      void ConnectToLms();

      /**
       * This method disconnects the LmsComponent from the lms messaging applet.
       */
      void DisconnectFromLms();

      ///accessor for mClientSocket
      LmsClientSocket* GetLmsClientSocket() { return mClientSocket; }

      /// const accessor for mClientSocket
      const LmsClientSocket* GetLmsClientSocket() const { return mClientSocket; }

      void GetMessageVector(std::vector<LmsMessage>& toFill);

   protected:

      void SetNeedValidSocket(bool need) { mNeedValidSocket = need; }
      bool GetNeedValidSocket() const    { return mNeedValidSocket; }

      /**
       * Destructs the LmsComponent.
       */
      virtual ~LmsComponent();

      /**
       * This method will send an update message to the lms messaging applet. It will
       * check to see which properties of a task have been updated, and send a separate
       * lms message for each one.
       *
       * @param proxy The GameActorProxy containing the task that has been updated.
       */
      void SendLmsUpdate(dtGame::GameActorProxy& parent);

      /**
       * This utility method creates an 'OBJECTIVE_COMPLETION' type LmsMessage object that can be
       * sent to the lms via the mClientSocket::SendLmsMessage() method.
       *
       * @param taskID The unique name of the task.
       * @param taskIsComplete The completion status of the task.
       * @return An LmsMessage object that is understood by the
       * mClientSocket::SendLmsMessage() method.
       */
      LmsMessage TranslateObjectiveCompleteMessage(const std::string& taskID, bool taskIsComplete);

      /**
       * This utility method creates an 'OBJECTIVE_SCORE' type LmsMessage object that can be
       * sent to the lms via the mClientSocket::SendLmsMessage() method.
       *
       * @param taskID The unique name of the task.
       * @param taskScore The current raw score of the task.
       * @return An LmsMessage object that is understood by the
       * mClientSocket::SendLmsMessage() method.
       */
      LmsMessage TranslateObjectiveScoreMessage(const std::string& taskID, float taskScore);

   private:

      std::vector<LmsMessage> mMessageVector;

      ///networking code
      LmsClientSocket* mClientSocket;

      ///stores the previous task values, so that we know what has changed when
      ///we receive a task update message; this is needed because we only
      ///want to send messages to the lms when something changes
      std::map<std::string, LmsTaskStatus> mPreviousTaskStatus;

      ///generally you can just keep the default values for the following:
      std::string mHost; ///host machine where lms messaging applet is running (defaults to 'localhost')
      int mPort; ///port on host machine that lms messaging applet is listening on (defaults to 4444)
      bool mReverseBytes; ///used to inform server that it needs to reverse the byte order (big/little endian issue)
      bool mNeedValidSocket;
   };
} // namespace dtLMS

#endif // DELTA_LMS_COMPONENT
