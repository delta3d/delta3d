/* -*-c++-*-
 * testAAR - testaarinput (.h & .cpp) - Using 'The MIT License'
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Christopher DuBuc
 * William E. Johnson II
 */

#ifndef DELTA_TEST_AAR_INPUT
#define DELTA_TEST_AAR_INPUT

#include "export.h"
#include <dtGame/baseinputcomponent.h>
#include <dtGame/logstatus.h>
#include <dtGame/logcontroller.h>


class TEST_APP_EXPORT InputComponent : public dtGame::BaseInputComponent
{
   public:

      // Constructor
      InputComponent();

      /*override*/ bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);

      /*override*/ bool HandleKeyReleased(const dtCore::Keyboard* keyboard, int key);

      // General message handler
      virtual void ProcessMessage(const dtGame::Message& message);

   protected:

      /// Destructor
      virtual ~InputComponent();

      /**
       * Tell the log controller to insert a tag
       */
      void InsertTag();

      /**
       * Tell the log controller to insert a keyframe
       */
      void InsertKeyFrame();

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
      void SendPlayerUpdateMsg(const std::string& paramName, const float value);
      
      double mSimSpeedFactor;
      dtGame::LogController* mLogController;
};

#endif
