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

#ifndef __DELTA_CAL3DANIMATOR_H__
#define __DELTA_CAL3DANIMATOR_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtAnim/animationupdaterinterface.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/ical3ddriver.h>
#include <dtCore/observerptr.h>
#include <osg/Referenced>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
class CalMixer;
class CalModel;



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3DAnimator: public osg::Referenced, virtual public dtAnim::AnimationUpdaterInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      static const float DEFAULT_MINIMUM_BLEND_TIME;

      Cal3DAnimator(Cal3DModelWrapper* wrapper);

      Cal3DModelWrapper* GetWrapper();
      const Cal3DModelWrapper* GetWrapper() const;

      ///Give the Animator a new Cal3DModelWrapper to operate on.
      void SetWrapper(Cal3DModelWrapper* wrapper);

      // these are listed in the order in which they are called on update
      void SetPreDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetPreDriver() const {return mPreDriver.get(); }

      void SetAnimationDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetAnimationDriver() const {return mAnimDriver.get(); }

      void SetSkeletonDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetSkeletonDriver() const {return mSkelDriver.get(); }

      void SetMorphTargetDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetMorphTargetDriver() const {return mMorphDriver.get(); }

      void SetPhysiqueDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetPhysiqueDriver() const {return mPhysiqueDriver.get(); }

      void SetSpringDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetSpringDriver() const {return mSpringDriver.get(); }

      void SetPostDriver(ICal3DDriver* pDriver);
      ICal3DDriver* GetPostDriver() const {return mPostDriver.get(); }

      virtual void Update(double timeDelta);
      
      /// Update just the Cal3D's animation using the mixer
      void UpdateAnimation(float deltaTime);

      /// Update just Cal3D's skeleton using the mixer
      void UpdateSkeleton(float deltaTime);

      /// Update the CalModel's morph target mixer
      void UpdateMorphTargetMixer(float deltaTime);

      /// Update the CalModel's physique
      void UpdatePhysique(float deltaTime);

      /// Update the CalModel's spring system
      void UpdateSpringSystem(float deltaTime);

      /// Remove all existing animations from the mixer
      virtual void ClearAll(float delay = 0.0);

      /// sets the offset time used in synchronized looping animations.
      virtual void SetAnimationTime(float time);
      /// @return the offset time used when playing looping animations.
      virtual float GetAnimationTime() const;

      virtual void SetPaused(bool paused);

      virtual bool IsPaused() const;

      /**
       * Determines if animation updates can be performed depending if any queued
       * animations are present. If bind poses are allowed then this will always
       * return TRUE; this is because an update with no animation will return to
       * the neutral bind pose.
       */
      virtual bool IsUpdatable() const;

      void SetMinimumBlendTime(float seconds);
      float GetMinimumBlendTime() const;

      /*virtual*/ bool BlendPose(dtAnim::AnimationInterface& anim, float weight, float delay);
      /*virtual*/ bool ClearPose(dtAnim::AnimationInterface& anim, float delay);
      
      /**
       * Globally set whether characters should be allowed to go back to bind pose
       * when animations have completed.
       */
      static void SetBindPoseAllowed(bool allow);
      static bool IsBindPoseAllowed();

   protected:
      virtual ~Cal3DAnimator();

   private:
      dtCore::ObserverPtr<dtAnim::Cal3DModelWrapper> mWrapper;
      CalModel* mCalModel;
      CalMixer* mMixer;
      float mMinBlendTime;

      dtCore::RefPtr<ICal3DDriver> mPreDriver;
      dtCore::RefPtr<ICal3DDriver> mPostDriver;
      dtCore::RefPtr<ICal3DDriver> mAnimDriver;
      dtCore::RefPtr<ICal3DDriver> mSkelDriver;
      dtCore::RefPtr<ICal3DDriver> mMorphDriver;
      dtCore::RefPtr<ICal3DDriver> mSpringDriver;
      dtCore::RefPtr<ICal3DDriver> mPhysiqueDriver;

      // Class variables
      static bool sAllowBindPose;
   };

} // namespace dtAnim

#endif // __DELTA_CAL3DANIMATOR_H__
