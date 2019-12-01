/* -*-c++-*-
 * testAAR - testaarmessageprocessor (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * William E. Johnson II
 */

#ifndef DELTA_TEST_AAR_MESSAGE_PROCESSOR
#define DELTA_TEST_AAR_MESSAGE_PROCESSOR

#include <dtGame/defaultmessageprocessor.h>
#include <dtCore/sigslot.h>
#include "export.h"

// Foward declarations
namespace dtGame
{
   class LogController;
   class LogStatus;
   class LogTag;
   class LogKeyframe;
   class ServerLoggerComponent;
   class Message;
}

namespace dtCore
{
   class GameEvent;
}

namespace dtActors
{
   class TaskActor;
   class TaskActorProxy;
}

namespace dtLMS
{
   class LmsComponent;
}

class TestAARHUD;

class TEST_AAR_EXPORT TestAARMessageProcessor : public dtGame::DefaultMessageProcessor, public sigslot::has_slots<>
{
   public:

      /// Constructor
      TestAARMessageProcessor(dtLMS::LmsComponent& lmsComp, 
                              dtGame::LogController& logCtrl, 
                              dtGame::ServerLoggerComponent& srvrCtrl);

      /**
       * Override to handle messages from the input component
       * @param msg The message to handle
       */
      virtual void ProcessMessage(const dtGame::Message& msg);

      /**
       * Initializes the tasks associated with the demo
       */
      void SetupTasks();

      /**
       * Displays the current active tasks and their status to the console.
       */
      void PrintTasks();

      /**
       * Clears the actors from the game manager and recreates the player and the tasks.
       * @note This is used when going from playback to idle since playback takes control
       *  therefore owning all the actors in the scene.
       */
      void Reset();

      /**
       * Slot method called from the log controller when a status message is received
       * from the logging component.
       * @param newStatus Contains the status object received from the logging component.
       */
      void OnReceivedStatus(const dtGame::LogStatus& newStatus);

      /**
       * Slot method called from the log controller when a Tags List message is sent
       * from the logging component to the log controller.
       * @param The Tags List vector.
       */
      void OnReceivedTags(const std::vector<dtGame::LogTag>& newTagList);

      /**
       * Slot method called from the log controller when a Keyframes List message is sent
       * from the logging component to the log controller.
       * @param The Keyframes List vector.
       */
      void OnReceivedKeyframes(const std::vector<dtGame::LogKeyframe>& newKeyframeList);

      /**
       * Slot method called from the log controller when a rejection message is sent
       * from the logging component to the log controller.
       * @param The rejection message.  Note, this has the causing message in it as well.
       */
      void OnReceivedRejection(const dtGame::Message& newMessage);

      /**
       * Force a request of status, tags, keys, etc...
       */
      void RequestAllControllerUpdates();

      /**
       * Callback function called when this component enters the GM
       */
      virtual void OnAddedToGM();

      /**
       * Sets the player actor that this component references
       * @param gap The new actor proxy
       */
      void SetPlayerActor(dtGame::GameActorProxy& gap) { mPlayer = &gap; }

   protected:

      /// Destructor
      virtual ~TestAARMessageProcessor();

      /**
       * Called by the Delta3D system each time a frame is about to be rendered.
       * @deltaFrameTime The tick time between frames.
       */
      void PreFrame(const double deltaFrameTime);

   private:

      /**
       * Helper method to update the task camera
       */
      void UpdateTaskCamera();

      /**
       * Helper method to update the player
       */
      void UpdatePlayerActor(const dtGame::ActorUpdateMessage& aum);

      /**
       * Helper method that creates an actor with random movement behavior.
       * @param ignored True means the actor will be ignored from recording.
       */
      void PlaceActor(bool ignored = false);

      /**
       * Helper method to create a new actor to put in the scene.
       */
      dtCore::RefPtr<dtGame::GameActorProxy> CreateNewMovingActor(const std::string& meshName, 
         float velocity, 
         float turnRate, 
         bool bSetLocation,
         bool ignoreRecording = false);

      //Our tasks and objectives
      dtCore::RefPtr<dtActors::TaskActorProxy> mTaskMoveCameraProxy;
      dtCore::RefPtr<dtActors::TaskActor> mTaskMoveCamera;

      dtGame::LogController* mLogController;
      dtLMS::LmsComponent* mLmsComponent; 
      double mLastAutoRequestStatus;
      dtGame::ServerLoggerComponent* mServerLogger;
      dtGame::GameActorProxy* mPlayer;
};

#endif
