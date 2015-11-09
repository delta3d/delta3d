/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2014 David Guthrie
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
 * David Guthrie
 */

#ifndef WALKRUNBLEND_H_
#define WALKRUNBLEND_H_

#include <dtAnim/export.h>
#include <dtAnim/animationsequence.h>
#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>
#include <dtCore/motioninterface.h>
#include <vector>
#include <limits>

namespace dtAnim
{
   //A class to simulate a walk run blend
   class DT_ANIM_EXPORT WalkRunBlend: public dtAnim::AnimationSequence
   {
   public:

      class DT_ANIM_EXPORT WRController: public dtAnim::AnimationSequence::AnimationController
      {
         public:
            typedef dtAnim::AnimationSequence::AnimationController BaseClass;

            WRController(WalkRunBlend& pWR, dtCore::VelocityInterface& mi);

            WRController(const WRController& pWR);

            void SetAnimations(dtAnim::Animatable* stand, dtAnim::Animatable* walk, dtAnim::Animatable* run = NULL);

            float GetCurrentSpeed() const;

            void SetCurrentSpeed(float pSpeed);

            /*virtual*/ void Update(float dt);

            dtAnim::Animatable* GetAnimation(unsigned i);
            unsigned GetAnimationCount() const;

            float GetAnimationInherentSpeed(unsigned i) const;
            void SetAnimationInherentSpeed(unsigned i, float speed);

            dtCore::VelocityInterface* GetMotionSpeedSource();

         protected:
            ~WRController();
         private:
            float mSpeed;

            unsigned int mLastActive;

            dtCore::ObserverPtr<dtCore::VelocityInterface> mMotionSpeedSource;

            struct AnimData
            {
               AnimData(dtAnim::Animatable*, float inherentSpeed, float initialWeight);
               dtCore::RefPtr<dtAnim::Animatable> mAnim;
               float mInherentSpeed;
               float mLastWeight;
               void SetLastWeight(float newWeight);
               void ApplyWeightChange();
               bool mWeightChanged;
            private:
            };

            std::vector<AnimData> mAnimations;
         };

         WalkRunBlend(dtCore::VelocityInterface& mi);

         void SetAnimations(dtAnim::Animatable* stand, dtAnim::Animatable* walk, dtAnim::Animatable* run);

         /**
          * Configures the walk and run blend based on their inherent speeds, i.e. the motion speed
          * implied by the animation at a speed factor of 1.0.
          * Don't call this before you call set animations or it will do nothing.
          * You can't set the speed on an animation you don't have.
          */
         void Setup(float inherentWalkSpeed, float inherentRunSpeed = std::numeric_limits<float>::max());

         WRController& GetWalkRunController();

         dtCore::RefPtr<dtAnim::Animatable> Clone(dtAnim::BaseModelWrapper* modelWrapper) const;

   protected:
      ~WalkRunBlend();

      //not implemented
      WalkRunBlend& operator=(const WalkRunBlend&);

      dtCore::RefPtr<WRController> mWRController;
   };

}



#endif /* WALKRUNBLEND_H_ */
