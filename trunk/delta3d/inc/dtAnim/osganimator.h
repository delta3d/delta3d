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
 */

#ifndef __DELTA_OSGANIMATOR_H__
#define __DELTA_OSGANIMATOR_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtAnim/animationupdaterinterface.h>
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/TimelineAnimationManager>
#include <osg/Referenced>
#include <dtCore/refptr.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtAnim
{
   class AnimationStateEnum;
   class OsgAnimation;
   class OsgModelWrapper;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgAnimator: public osg::Referenced, virtual public dtAnim::AnimationUpdaterInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      OsgAnimator(dtAnim::OsgModelWrapper& model, osgAnimation::BasicAnimationManager& animManager, osg::Node& animNode);

      dtAnim::OsgModelWrapper* GetModel();
      const dtAnim::OsgModelWrapper* GetModel() const;

      virtual void Update(double timeDelta);

      /// sets the offset time used in synchronized looping animations.
      virtual void SetAnimationTime(float time);

      /// @return the offset time used when playing looping animations.
      virtual float GetAnimationTime() const;

      // @return Average duration of synchronized cycle animations.
      float GetAverageCycleDuration() const;
      
      virtual void SetPaused(bool paused);

      virtual bool IsPaused() const;

      /**
       * Determines if animation updates can be performed depending if any queued
       * animations are present. If bind poses are allowed then this will always
       * return TRUE; this is because an update with no animation will return to
       * the neutral bind pose.
       */
      virtual bool IsUpdatable() const;

      virtual void ClearAll(float delay = 0.0);

      osgAnimation::BasicAnimationManager* GetOsgOriginalAnimationManager();
      const osgAnimation::BasicAnimationManager* GetOsgOriginalAnimationManager() const;

      osgAnimation::TimelineAnimationManager* GetOsgAnimationManager();
      const osgAnimation::TimelineAnimationManager* GetOsgAnimationManager() const;
      
      /**
       * Globally set whether characters should be allowed to go back to bind pose
       * when animations have completed.
       */
      static void SetBindPoseAllowed(bool allow);
      static bool IsBindPoseAllowed();

      int GetActiveAnimationCount() const;

      bool IsPlaying() const;

      bool PlayAction(dtAnim::OsgAnimation& anim,
         float fadeInTime, float fadeOutTime,
         float weight = 1.0f);
      
      bool PlayCycle(dtAnim::OsgAnimation& anim,
         float weight, float fadeInTime);
      
      bool ClearAction(dtAnim::OsgAnimation& anim);
      
      bool ClearCycle(dtAnim::OsgAnimation& anim, float fadeOutTime);

      typedef std::vector<dtCore::RefPtr<osgAnimation::Action> > OsgActionArray;
      int GetActions(dtAnim::OsgAnimation& anim, OsgActionArray& outActions);

      const dtAnim::AnimationStateEnum& GetAnimationState(const OsgAnimation& anim) const;

      /*virtual*/ bool BlendPose(dtAnim::AnimationInterface& anim, float weight, float delay);
      /*virtual*/ bool ClearPose(dtAnim::AnimationInterface& anim, float delay);

   protected:
      virtual ~OsgAnimator();

      class ActionTimer : public osg::Referenced
      {
      public:
         ActionTimer(float timer = 0.0f, osgAnimation::Action* action = NULL,
            osgAnimation::Animation* osgAnim = NULL,
            dtAnim::OsgAnimation* anim = NULL);

         dtCore::RefPtr<osgAnimation::Action> mAction;
         dtCore::RefPtr<osgAnimation::Animation> mOsgAnim;
         dtCore::RefPtr<dtAnim::OsgAnimation> mAnim;
         float mTimer;

      protected:
         virtual ~ActionTimer() {}
      };

      int GetCycleIndex(dtAnim::OsgAnimation& anim) const;
      
      bool RemoveCycle(dtAnim::OsgAnimation& anim);

      bool FadeInAnimation(dtAnim::OsgAnimation& anim, float fadeTime);

      bool FadeOutAnimation(dtAnim::OsgAnimation& anim, float fadeTime);
      
      void UpdateActionClearTimers(float timeDelta);

      void UpdateCycleWeights();

      void StartTimeline(osgAnimation::Timeline& timeline);

      void SetupAction(osgAnimation::Action& action);

      void AddActionClearTimer(ActionTimer& actionTimer);
      
      void SetActionClearTimer(osgAnimation::Action& action, float timeToClear);

      bool RemoveAction(osgAnimation::Action& action);

      int RemoveActionsWithAnimation(dtAnim::OsgAnimation& anim);

      osgAnimation::ActionStripAnimation* GetActionForAnimation(osgAnimation::Animation& anim);

   private:
      bool mPaused;
      int mNumCyclesSynced;
      double mTime;
      float mAverageCycleDuration;
      dtCore::RefPtr<dtAnim::OsgModelWrapper> mModel;
      dtCore::RefPtr<osgAnimation::BasicAnimationManager> mOriginalAnimManager;
      dtCore::RefPtr<osgAnimation::TimelineAnimationManager> mAnimManager;
      dtCore::RefPtr<osg::Node> mAnimManagerNode;

      class CycleInfo : public osg::Referenced
      {
      public:
         CycleInfo(dtAnim::OsgAnimation* anim = NULL);

         dtCore::RefPtr<dtAnim::OsgAnimation> mAnim;
         dtCore::RefPtr<osgAnimation::Animation> mOsgAnim;
         bool mSynchronous;

      protected:
         virtual ~CycleInfo() {}
      };

      typedef std::vector<dtCore::RefPtr<CycleInfo> > CycleInfoArray;
      CycleInfoArray mCycleInfos;

      typedef std::map<osgAnimation::Action*, dtCore::RefPtr<ActionTimer> > ActionTimeMap;
      ActionTimeMap mActionClearTimers;

      // Class variables
      static bool sAllowBindPose;
   };

} // namespace dtAnim

#endif // __DELTA_OSGANIMATOR_H__
