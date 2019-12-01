/* -*-c++-*-
* testAnim - testaniminput (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2007-2008, Alion Science and Technology Corporation
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
* Bradley Anderegg
*/
#ifndef DELTA_TEST_ANIM_INPUT
#define DELTA_TEST_ANIM_INPUT

#include "export.h"
#include <dtGame/baseinputcomponent.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>

namespace dtAnim
{
   class AnimationHelper;
}


class TestAnimHUD;

class TEST_ANIM_EXPORT TestAnimInput : public dtGame::BaseInputComponent
{
   public:

      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;

      // Constructor
      TestAnimInput();

      // We're going to handle key presses!
      virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);

      // We're going to handle key releases!
      virtual bool HandleKeyReleased(const dtCore::Keyboard* keyboard, int key);

      // Handle messages from the GM
      virtual void ProcessMessage(const dtGame::Message& message);

      /**
       * Sets the player actor that this component access
       * @param gap The new actor proxy
       */
      void SetPlayerActor(dtGame::GameActorProxy& gap);
      
      void SetAnimationHelper(dtAnim::AnimationHelper& pHelper);

      //the walking speed in m/s
      void SetSpeed(float speed);
      void SetTurnRate(float turn);

   protected:

      /// Destructor
      virtual ~TestAnimInput();


   private:

      void TickLocal(float dt);

      float mSpeed, mTurnRate, mTurnDirection;
      bool mKeyIsPressed, mIsWalking, mIsTurning;
      dtCore::ObserverPtr<dtGame::GameActorProxy> mPlayer;
      dtCore::ObserverPtr<dtAnim::AnimationHelper> mAnimationHelper;
};

#endif
