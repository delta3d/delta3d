/* -*-c++-*-
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
 * Christopher DuBuc
 * William E. Johnson II
 */
#ifndef DELTA_TEST_AAR_INPUT
#define DELTA_TEST_AAR_INPUT

#include "export.h"
#include <dtGame/baseinputcomponent.h>
#include <dtGame/logstatus.h>

// Foward declarations
namespace dtDAL
{
   class GameEvent;
}

namespace dtGame
{
   class LogController;
}

class TestAARGameEvent;
class TestAARHUD;

class TEST_AAR_EXPORT TestAARInput : public dtGame::BaseInputComponent
{
   public:

      // Constructor
      TestAARInput(const std::string &name, dtGame::LogController &logCtrl, TestAARHUD &hudComp);

      // We're going to handle key presses!
      virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key,Producer::KeyCharacter character);

      // We're going to handle key releases!
      virtual bool HandleKeyReleased(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key,Producer::KeyCharacter character);

      // Handle messages from the GM
      virtual void ProcessMessage(const dtGame::Message &message);

      /**
       * Sets the player actor that this component access
       * @param gap The new actor proxy
       */
      void SetPlayerActor(dtGame::GameActorProxy &gap) { mPlayer = &gap; }

   protected:

      /// Destructor
      virtual ~TestAARInput();

      /**
       * Tell the log controller to insert a tag
       */
      void InsertTag();

      /**
       * Tell the log controller to insert a keyframe
       */
      void InsertKeyFrame();

      /**
       * Utility method to fire an event message
       */
      void FireEvent(dtDAL::GameEvent &event);

      /**
       * Method that will go to the previous key frame
       */
      void GotoPreviousKeyframe();

      /**
       * Method that will go to the next keyframe
       */
      void GotoNextKeyframe();

   private:

      /**
       * Helper method that creates and sends an ActorUpdateMessage about the player
       * @param paramName The name of the update parameter
       * @param value The value of the update parameter
       */
      void SendPlayerUpdateMsg(const std::string &paramName, const float value); 
      
      bool mKeyIsPressed;
      double mSimSpeedFactor;
      const dtGame::LogStateEnumeration *mAppMode;
      TestAARHUD *mHudGUI;
      dtGame::GameActorProxy *mPlayer;
      bool mVelocityUpdated;
      bool mTurnRateUpdated;
      dtGame::LogController *mLogController;
};

#endif
