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
 * @author Curtiss Murphy
 */
#ifndef __GM_TUTORIAL_INPUT_COMPONENT__
#define __GM_TUTORIAL_INPUT_COMPONENT__

#include "export.h"
#include <dtCore/refptr.h>
#include <dtGame/baseinputcomponent.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventmanager.h>
#include <dtGame/logcontroller.h>

/**
 * This is an example use of the base input component for our tutorial. We will put 
 * all of our keyboard and mouse event handling in here. As a GM component, it will
 * also receive all game messages.
 */
class TUTORIAL_TANK_EXPORT InputComponent : public dtGame::BaseInputComponent
{
   public:

      // Constructor
      InputComponent(const std::string &name, bool inPlaybackMode);

      // Find our GameEvents after the map is loaded
      void SetupEvents();

      // We're going to handle key presses!
      /*virtual*/ bool HandleKeyPressed(  const dtCore::Keyboard* keyboard,
                                          Producer::KeyboardKey key,
                                          Producer::KeyCharacter character );

      /**
       * Handle messages if we want to
       */                                          
      void ProcessMessage(const dtGame::Message &message);

   protected:

      /// Destructor
      virtual ~InputComponent() { }

   private:
      /**
       * Simple helper method to fire a game event.  This method creates the game event
       * message and sends it on to the Game Manager for processing.
       * @param event The game event to fire.
       */
      void FireGameEvent(dtDAL::GameEvent &event);

      dtCore::RefPtr<dtDAL::GameEvent> mToggleEngineEvent;
      dtCore::RefPtr<dtDAL::GameEvent> mSpeedBoost;
      dtCore::RefPtr<dtDAL::GameEvent> mTankFired;
      dtCore::RefPtr<dtDAL::GameEvent> mTestShaders;
      dtCore::RefPtr<dtDAL::GameEvent> mReset;

      dtCore::RefPtr<dtGame::LogController> mLogController;
      bool mInPlaybackMode;
};

#endif

