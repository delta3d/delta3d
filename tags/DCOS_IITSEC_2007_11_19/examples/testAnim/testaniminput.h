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
 * Bradley Anderegg
 */
#ifndef DELTA_TEST_ANIM_INPUT
#define DELTA_TEST_ANIM_INPUT

#include "export.h"
#include <dtGame/baseinputcomponent.h>
#include <dtCore/refptr.h>

namespace dtAnim
{
   class AnimationHelper;
}


class TestAnimHUD;

class TEST_ANIM_EXPORT TestAnimInput : public dtGame::BaseInputComponent
{
   public:

      // Constructor
      TestAnimInput(const std::string &name);

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
      void SetPlayerActor(dtGame::GameActorProxy &gap);
      
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
      dtCore::RefPtr<dtGame::GameActorProxy> mPlayer;
      dtCore::RefPtr<dtAnim::AnimationHelper> mAnimationHelper;
};

#endif
