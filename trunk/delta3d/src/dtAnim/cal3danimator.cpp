/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 03/28/2007
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/cal3danimator.h>
#include <dtAnim/animdriver.h>
#include <dtAnim/skeletondriver.h>
#include <dtAnim/morphdriver.h>
#include <dtAnim/osgobjects.h>
#include <dtAnim/physiquedriver.h>
#include <dtAnim/springdriver.h>
#include <dtUtil/log.h>
// CAL3D
#include <cal3d/mixer.h>
#include <cal3d/model.h>
#include <cal3d/morphtargetmixer.h>
#include <cal3d/physique.h>
#include <cal3d/springsystem.h>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   const float Cal3DAnimator::DEFAULT_MINIMUM_BLEND_TIME = 0.1f;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS VARIABLES
   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DAnimator::sAllowBindPose = false;



   /////////////////////////////////////////////////////////////////////////////
   // STATIC METHODS
   /////////////////////////////////////////////////////////////////////////////
   void Cal3DAnimator::SetBindPoseAllowed(bool allow)
   {
      sAllowBindPose = allow;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DAnimator::IsBindPoseAllowed()
   {
      return sAllowBindPose;
   }

   Cal3DAnimator::Cal3DAnimator(Cal3DModelWrapper* wrapper)
      : mWrapper(wrapper)
      , mCalModel(NULL)
      , mMixer(NULL)
      , mMinBlendTime(DEFAULT_MINIMUM_BLEND_TIME)
      , mPreDriver(0)
      , mPostDriver(0)
      , mAnimDriver(NULL)
      , mSkelDriver(NULL)
      , mMorphDriver(NULL)
      , mSpringDriver(NULL)
      , mPhysiqueDriver(NULL)
   {
      mAnimDriver = new AnimDriver(this);
      mSkelDriver = new SkeletonDriver(this);
      mMorphDriver = new MorphDriver(this);
      mSpringDriver = new SpringDriver(this);
      mPhysiqueDriver = new PhysiqueDriver(this);

      SetWrapper(wrapper);
   }


   Cal3DAnimator::~Cal3DAnimator()
   {
   }


   void Cal3DAnimator::Update(double dt)
   {
      // it is ok for a driver to be null

      if (mPreDriver.valid())      { mPreDriver->Update(dt);      }

      if (mAnimDriver.valid())     { mAnimDriver->Update(dt);     }

      if (mSkelDriver.valid())     { mSkelDriver->Update(dt);     }

      if (mMorphDriver.valid())    { mMorphDriver->Update(dt);    }

      if (mPhysiqueDriver.valid()) { mPhysiqueDriver->Update(dt); }

      if (mSpringDriver.valid())   { mSpringDriver->Update(dt);   }

      if (mPostDriver.valid())     { mPostDriver->Update(dt);     }

   }

   void Cal3DAnimator::SetWrapper(Cal3DModelWrapper* wrapper)
   {
      mWrapper = wrapper;
   
      if (mWrapper.valid())
      {
         mCalModel = mWrapper->GetCalModel();
         mMixer = mCalModel->getMixer();
      }
   }

   Cal3DModelWrapper* Cal3DAnimator::GetWrapper()
   {
      return mWrapper.get();
   }


   const Cal3DModelWrapper* Cal3DAnimator::GetWrapper() const
   {
      return mWrapper.get();
   }


   void Cal3DAnimator::SetPreDriver(ICal3DDriver* pDriver)
   {
      mPreDriver = pDriver;
   }

   void Cal3DAnimator::SetAnimationDriver(ICal3DDriver* pDriver)
   {
      mAnimDriver = pDriver;
   }

   void Cal3DAnimator::SetSkeletonDriver(ICal3DDriver* pDriver)
   {
      mSkelDriver = pDriver;
   }

   void Cal3DAnimator::SetMorphTargetDriver(ICal3DDriver* pDriver)
   {
      mMorphDriver = pDriver;
   }

   void Cal3DAnimator::SetPhysiqueDriver(ICal3DDriver* pDriver)
   {
      mPhysiqueDriver = pDriver;
   }

   void Cal3DAnimator::SetPostDriver(ICal3DDriver* pDriver)
   {
      mPostDriver = pDriver;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DAnimator::UpdateAnimation(float deltaTime)
   {
      mMixer->updateAnimation(deltaTime);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DAnimator::UpdateMorphTargetMixer(float deltaTime)
   {
      mCalModel->getMorphTargetMixer()->update(deltaTime);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DAnimator::UpdatePhysique(float deltaTime)
   {
      mCalModel->getPhysique()->update();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DAnimator::UpdateSpringSystem(float deltaTime)
   {
      mCalModel->getSpringSystem()->update(deltaTime);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DAnimator::UpdateSkeleton(float deltaTime)
   {
      if(IsUpdatable())
      {
         mMixer->updateSkeleton();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DAnimator::IsUpdatable() const
   {
      return ! mMixer->getAnimationActionList().empty()
         || ! mMixer->getAnimationCycle().empty()
         || ! mMixer->getAnimationPose().empty()
         || IsBindPoseAllowed();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DAnimator::SetAnimationTime(float time)
   {
      mMixer->setAnimationTime(time);
   }

   /////////////////////////////////////////////////////////////////////////////
   float Cal3DAnimator::GetAnimationTime() const
   {
      return mMixer->getAnimationTime();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DAnimator::SetPaused(bool paused)
   {
      // TODO:
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DAnimator::IsPaused() const
   {
      // TODO:
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DAnimator::ClearAll(float delay)
   {
      std::vector<CalAnimation*>& animList = mMixer->getAnimationVector();
      for (size_t animIndex = 0; animIndex < animList.size(); ++animIndex)
      {
         CalAnimation* currentAnim = animList[animIndex];
         if (currentAnim)
         {
            const std::string& animName = currentAnim->getCoreAnimation()->getName();
            dtAnim::AnimationInterface* animInterface = mWrapper->GetAnimation(animName);

            if (animInterface != NULL)
            {
               if (currentAnim->getType() == CalAnimation::TYPE_CYCLE)
               {
                  animInterface->ClearCycle(delay);
               }
               else if (currentAnim->getType() == CalAnimation::TYPE_ACTION)
               {
                  animInterface->ClearAction();
               }
               else if (currentAnim->getType() == CalAnimation::TYPE_POSE)
               {
                  ClearPose(*animInterface, delay);
               }
            }
            else
            {
               LOG_ERROR("Could not access interface for animation \"" + animName + "\"");
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DAnimator::SetMinimumBlendTime(float seconds)
   {
      mMinBlendTime = seconds;
   }
   
   /////////////////////////////////////////////////////////////////////////////
   float Cal3DAnimator::GetMinimumBlendTime() const
   {
      return mMinBlendTime;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DAnimator::BlendPose(dtAnim::AnimationInterface& anim, float weight, float delay)
   {
      return mMixer->blendPose(anim.GetID(), weight, delay);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DAnimator::ClearPose(dtAnim::AnimationInterface& anim, float delay)
   {
      return mMixer->clearPose(anim.GetID(), delay);
   }

} // namespace dtAnim
