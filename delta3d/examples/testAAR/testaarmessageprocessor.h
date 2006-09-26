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
 * William E. Johnson II
 */
#ifndef DELTA_TEST_AAR_MESSAGE_PROCESSOR
#define DELTA_TEST_AAR_MESSAGE_PROCESSOR

#include <dtGame/defaultmessageprocessor.h>
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

namespace dtDAL
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

class TEST_AAR_EXPORT TestAARMessageProcessor : public dtGame::DefaultMessageProcessor
{
   public:

      /// Constructor
      TestAARMessageProcessor(dtLMS::LmsComponent &lmsComp, 
                                 dtGame::LogController &logCtrl, 
                                 dtGame::ServerLoggerComponent &srvrCtrl);

      /**
       * Override to handle messages from the input component
       * @param msg The message to handle
       */
      virtual void ProcessMessage(const dtGame::Message &msg);

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
      void OnReceivedStatus(const dtGame::LogStatus &newStatus);

      /**
       * Slot method called from the log controller when a Tags List message is sent
       * from the logging component to the log controller.
       * @param The Tags List vector.
       */
      void OnReceivedTags(const std::vector<dtGame::LogTag> &newTagList);

      /**
       * Slot method called from the log controller when a Keyframes List message is sent
       * from the logging component to the log controller.
       * @param The Keyframes List vector.
       */
      void OnReceivedKeyframes(const std::vector<dtGame::LogKeyframe> &newKeyframeList);

      /**
       * Slot method called from the log controller when a rejection message is sent
       * from the logging component to the log controller.
       * @param The rejection message.  Note, this has the causing message in it as well.
       */
      void OnReceivedRejection(const dtGame::Message &newMessage);

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
      void SetPlayerActor(dtGame::GameActorProxy &gap) { mPlayer = &gap; }

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
      void UpdatePlayerActor(const dtGame::ActorUpdateMessage &aum);

      /**
       * Helper method that creates an actor with random movement behavior.
       */
      void PlaceActor();

      /**
       * Helper method to create a new actor to put in the scene.
       */
      dtCore::RefPtr<dtGame::GameActorProxy> CreateNewMovingActor(const std::string &meshName, 
         float velocity, 
         float turnRate, 
         bool bSetLocation);

      //Our tasks and objectives
      dtCore::RefPtr<dtActors::TaskActorProxy> mTaskMoveCameraProxy;
      dtCore::RefPtr<dtActors::TaskActor> mTaskMoveCamera;

      dtGame::LogController *mLogController;
      dtLMS::LmsComponent *mLmsComponent; 
      double mLastAutoRequestStatus;
      dtGame::ServerLoggerComponent *mServerLogger;
      dtGame::GameActorProxy *mPlayer;
};

#endif
