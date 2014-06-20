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

            void SetAnimations(dtAnim::Animatable* stand, dtAnim::Animatable* walk, dtAnim::Animatable* run);

            //this sets the basic necessary blend values, the others get expected values
            void SetRunWalkBasic(float inherentWalkSpeed, float walkFade, float runFade);

            void SetStand(float start, float fadeIn, float stop, float fadeOut);

            //customize the walk
            void SetWalk(float start, float fadeIn, float stop, float fadeOut);

            //customize the run
            void SetRun(float start, float fadeIn, float stop, float fadeOut);

            void SetCurrentSpeed(float pSpeed);

            /*virtual*/ void Update(float dt);

            dtAnim::Animatable* GetStand();

            dtAnim::Animatable* GetWalk();

            dtAnim::Animatable* GetRun();

            dtCore::RefPtr<WRController> CloneDerived() const;

         protected:
            ~WRController();

            float ComputeWeight(dtAnim::Animatable* pAnim, float startSpeed, float fadeIn, float stopSpeed, float fadeOut);

            float mSpeed;
            float mStandStart, mStandFadeIn, mStandStop, mStandFadeOut;
            float mWalkStart, mWalkFadeIn, mWalkStop, mWalkFadeOut;
            float mRunStart, mRunFadeIn, mRunStop, mRunFadeOut;


            dtCore::ObserverPtr<dtCore::VelocityInterface> mMotionSpeedSource;
            dtCore::RefPtr<dtAnim::Animatable> mStand;
            dtCore::RefPtr<dtAnim::Animatable> mWalk;
            dtCore::RefPtr<dtAnim::Animatable> mRun;
         };


         WalkRunBlend(dtCore::VelocityInterface& mi);

         WalkRunBlend(WRController& controller);

         void SetAnimations(dtAnim::Animatable* stand, dtAnim::Animatable* walk, dtAnim::Animatable* run);

         void SetupWithWalkSpeed(float inherentSpeed);

         WRController& GetWalkRunController();

         dtCore::RefPtr<dtAnim::Animatable> Clone(dtAnim::Cal3DModelWrapper* modelWrapper) const;

   protected:
      ~WalkRunBlend();

      dtCore::RefPtr<WRController> mWRController;
   };

}



#endif /* WALKRUNBLEND_H_ */
