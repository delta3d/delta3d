/* -*-c++-*-
 * TutorialLibrary - InputComponent (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 * @author Curtiss Murphy
 */

#ifndef __GM_TUTORIAL_INPUT_COMPONENT__
#define __GM_TUTORIAL_INPUT_COMPONENT__

#include "export.h"
#include <dtCore/refptr.h>
#include <dtGame/baseinputcomponent.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventmanager.h>
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
   InputComponent(const std::string& name, bool inPlaybackMode);

   // Find our GameEvents after the map is loaded
   void SetupEvents();

   // We're going to handle key presses!
   virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);

   /**
    * Handle messages if we want to
    */
   void ProcessMessage(const dtGame::Message& message);

protected:
   /// Destructor
   virtual ~InputComponent() {}

private:
   /**
    * Simple helper method to fire a game event.  This method creates the game event
    * message and sends it on to the Game Manager for processing.
    * @param event The game event to fire.
    */
   void FireGameEvent(const dtCore::GameEvent& event);

   dtCore::RefPtr<dtCore::GameEvent> mToggleEngineEvent;
   dtCore::RefPtr<dtCore::GameEvent> mSpeedBoost;
   dtCore::RefPtr<dtCore::GameEvent> mTankFired;
   dtCore::RefPtr<dtCore::GameEvent> mTestShaders;
   dtCore::RefPtr<dtCore::GameEvent> mReset;

   dtCore::RefPtr<dtGame::LogController> mLogController;
   bool mInPlaybackMode;
};

#endif // __GM_TUTORIAL_INPUT_COMPONENT__
