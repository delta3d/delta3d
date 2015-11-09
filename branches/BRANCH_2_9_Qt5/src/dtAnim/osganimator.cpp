
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/osganimator.h>
#include <dtAnim/animationinterface.h>
#include <dtAnim/osgmodelwrapper.h>
#include <dtUtil/log.h>
// OSG
#include <osgAnimation/ActionAnimation>
#include <osgAnimation/ActionBlendIn>
#include <osgAnimation/ActionBlendOut>
#include <osgAnimation/ActionStripAnimation>
#include <osgAnimation/TimelineAnimationManager>
// STL
#include <algorithm>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   static const std::string ACTION_BLEND_IN("ActionBlendIn");
   static const std::string ACTION_BLEND_OUT("ActionBlendOut");
   static const std::string ACTION_ANIM("ActionAnimation");



   /////////////////////////////////////////////////////////////////////////////
   // CLASS VARIABLES
   /////////////////////////////////////////////////////////////////////////////
   bool OsgAnimator::sAllowBindPose = false;



   /////////////////////////////////////////////////////////////////////////////
   // HELPER PREDICATE
   /////////////////////////////////////////////////////////////////////////////
   struct FindMatchingActionPred
   {
      FindMatchingActionPred(osgAnimation::Animation* anim)
         : mAnim(anim)
      {}

      bool operator() (const osgAnimation::FrameAction& action)
      {
         bool isMatch = false;

         osgAnimation::Action* a = action.second.get();
         const std::string className = a->className();

         if (className == ACTION_ANIM)
         {
            isMatch = mAnim == static_cast<osgAnimation::ActionAnimation*>(a)->getAnimation();
         }
         else if (className == ACTION_BLEND_IN)
         {
            isMatch = mAnim == static_cast<osgAnimation::ActionBlendIn*>(a)->getAnimation();
         }
         else if (className == ACTION_BLEND_IN)
         {
            isMatch = mAnim == static_cast<osgAnimation::ActionBlendOut*>(a)->getAnimation();
         }

         if (isMatch)
         {
            mActions.push_back(a);
         }

         return isMatch;
      }

      osgAnimation::Animation* mAnim;
      OsgAnimator::OsgActionArray mActions;
   };


   
   ////////////////////////////////////////////////////////////////////////////////
   // INTERNAL CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   OsgAnimator::CycleInfo::CycleInfo(dtAnim::OsgAnimation* anim)
      : mAnim(anim)
      , mSynchronous(true)
   {
      mSynchronous = anim->IsSynchronous();
   }
   


   ////////////////////////////////////////////////////////////////////////////////
   // INTERNAL CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   OsgAnimator::ActionTimer::ActionTimer(float timer, osgAnimation::Action* action,
      osgAnimation::Animation* osgAnim, dtAnim::OsgAnimation* anim)
      : mAction(action)
      , mOsgAnim(osgAnim)
      , mAnim(anim)
      , mTimer(timer)
   {}



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   OsgAnimator::OsgAnimator(OsgModelWrapper& model, osgAnimation::BasicAnimationManager& animManager, osg::Node& animNode)
      : mPaused(false)
      , mTime(0.0)
      , mAverageCycleDuration(0.0f)
      , mModel(&model)
      , mOriginalAnimManager(&animManager)
      , mAnimManager(new osgAnimation::TimelineAnimationManager)
      , mAnimManagerNode(&animNode)
   {}

   OsgAnimator::~OsgAnimator()
   {
      mOriginalAnimManager = NULL;
      mModel = NULL;
   }

   OsgModelWrapper* OsgAnimator::GetModel()
   {
      return mModel.get();
   }

   const OsgModelWrapper* OsgAnimator::GetModel() const
   {
      return mModel.get();
   }

   struct RemoveCycleTimerPred
   {
      bool operator() (float timerValue)
      {
         return timerValue <= 0.0f;
      }
   };

   void OsgAnimator::Update(double timeDelta)
   {
      if (mPaused)
      {
         return;
      }
      
      osgAnimation::Timeline* timeline = mAnimManager->getTimeline();
      
      if (timeline->getStatus() == osgAnimation::Timeline::Play)
      {
         mTime += timeDelta;
         mAnimManager->update(mTime);

         UpdateCycleWeights();

         UpdateActionClearTimers(timeDelta);
      }
   }

   void OsgAnimator::SetAnimationTime(float time)
   {
      mTime = time;

      osgAnimation::Timeline* timeline = mAnimManager->getTimeline();

      float duration = timeline->getDuration();
      if (duration != 0.0)
      {
         unsigned int frame = (time / duration) * timeline->getNumFrames();
         timeline->gotoFrame(frame);
      }
   }

   float OsgAnimator::GetAnimationTime() const
   {
      return mTime;
   }
   
   float OsgAnimator::GetAverageCycleDuration() const
   {
      return mAverageCycleDuration;
   }

   void OsgAnimator::SetPaused(bool paused)
   {
      mPaused = paused;
   }

   bool OsgAnimator::IsPaused() const
   {
      return mPaused;
   }

   bool OsgAnimator::IsUpdatable() const
   {
      return ! mCycleInfos.empty()
         || ! mActionClearTimers.empty()
         || IsBindPoseAllowed();
   }

   void OsgAnimator::ClearAll(float fadeTime)
   {
      if (fadeTime > 0.0f)
      {
         // TODO clear poses

         dtAnim::OsgAnimation* curAnim = NULL;
         ActionTimeMap::iterator curIter = mActionClearTimers.begin();
         ActionTimeMap::iterator endIter = mActionClearTimers.end();
         for (; curIter != endIter; ++curIter)
         {
            // Set the fade time if the timer is set to infinite.
            curAnim = curIter->second->mAnim;
            float curTimer = curIter->second->mTimer;
            if (curTimer < 0.0f)
            {
               // DEBUG:
               //printf("Clearing anim: \"%s\" with %f\n", curAnim->GetName().c_str(), fadeTime);

               FadeOutAnimation(*curAnim, fadeTime);
            }
            else if (curTimer > fadeTime)
            {
               // TODO:
               // Adjust any fade-in/out actions
               // associated with the current animation.

               // TODO:
            }
         }
      }
      else
      {
         osgAnimation::Timeline* timeline = mAnimManager->getTimeline();

         if (timeline->getStatus() == osgAnimation::Timeline::Play)
         {
            timeline->stop();
         }
         timeline->clearActions();

         mActionClearTimers.clear();
         mCycleInfos.clear();

         // TODO clear poses

         // Set the time back to 0.
         SetAnimationTime(0.0f);
      }
   }

   osgAnimation::BasicAnimationManager* OsgAnimator::GetOsgOriginalAnimationManager()
   {
      return mOriginalAnimManager.get();
   }

   const osgAnimation::BasicAnimationManager* OsgAnimator::GetOsgOriginalAnimationManager() const
   {
      return mOriginalAnimManager.get();
   }

   osgAnimation::TimelineAnimationManager* OsgAnimator::GetOsgAnimationManager()
   {
      return mAnimManager.get();
   }

   const osgAnimation::TimelineAnimationManager* OsgAnimator::GetOsgAnimationManager() const
   {
      return mAnimManager.get();
   }

   void OsgAnimator::SetBindPoseAllowed(bool allow)
   {
      sAllowBindPose = allow;
   }

   bool OsgAnimator::IsBindPoseAllowed()
   {
      return sAllowBindPose;
   }

   int OsgAnimator::GetActiveAnimationCount() const
   {
      int numAnims = 0;

      osgAnimation::Timeline* timeline = mAnimManager->getTimeline();
      int numLayers = (int)(timeline->getActionLayers().size());
      for (int i = 0; i < numLayers; ++i)
      {
         numAnims += (int)(timeline->getActionLayer(i).size());
      }

      return numAnims;
   }

   bool OsgAnimator::IsPlaying() const
   {
      return mAnimManager->getTimeline()->getStatus()
         == osgAnimation::Timeline::Play;
   }

   bool OsgAnimator::PlayAction(dtAnim::OsgAnimation& anim,
      float fadeInTime, float fadeOutTime,
      float weight)
   {
      anim.Reset();
      osgAnimation::Animation* osgAnim = anim.GetOsgAnimation();

      float duration = osgAnim->getDuration();
      if (duration == 0.0f)
      {
         osgAnim->computeDuration();
         duration = osgAnim->getDuration();
      }

      dtCore::RefPtr<osgAnimation::ActionStripAnimation> action
         = new osgAnimation::ActionStripAnimation(osgAnim, fadeInTime, fadeOutTime, weight);
      action->setLoop(1);
      SetupAction(*action);
      
      osgAnimation::Timeline* timeline = mAnimManager->getTimeline();
      timeline->addActionNow(action.get(), 0);

      // Ensure that actions will have enough time to play
      // and will be cleared after they have all completed.
      dtCore::RefPtr<ActionTimer> atimer
         = new ActionTimer(duration, action, osgAnim, &anim);
      AddActionClearTimer(*atimer);

      // Start up the timeline if it is currently stopped.
      StartTimeline(*timeline);

      return osgAnim != NULL;
   }
      
   bool OsgAnimator::PlayCycle(dtAnim::OsgAnimation& anim,
      float weight, float fadeInTime)
   {
      // Prevent playing the same cycle if it is already active.
      if (GetCycleIndex(anim) >= 0)
      {
         return false;
      }

      anim.Reset();
      dtCore::RefPtr<CycleInfo> info = new CycleInfo(&anim);
      osgAnimation::Animation* osgAnim = info->mAnim->GetOsgAnimation();

      if (osgAnim == NULL)
      {
         LOG_ERROR("Internal OSG Animation is NULL for dtAnim::OsgAnimation \""
            + anim.GetName() + "\"");
         return false;
      }

      float duration = osgAnim->getDuration();
      if (duration == 0.0f)
      {
         osgAnim->computeDuration();
         duration = osgAnim->getDuration();
      }

      dtCore::RefPtr<osgAnimation::ActionStripAnimation> cycle
         = new osgAnimation::ActionStripAnimation(osgAnim, fadeInTime, 0.0f, weight);
      cycle->setLoop(0); // Infinite
      SetupAction(*cycle);

      osgAnimation::Timeline* timeline = mAnimManager->getTimeline();
      timeline->addActionNow(cycle.get(), 0);

      // Keep track of the action with an infinite clear timer
      // until it is requested to be removed.
      dtCore::RefPtr<ActionTimer> atimer
         = new ActionTimer(-1.0, cycle, osgAnim, &anim);
      AddActionClearTimer(*atimer);

      mCycleInfos.push_back(info);

      // DEBUG:
      //printf("Playing cycle: %s of %d\n\n", anim.GetName().c_str(), (int)mCycleInfos.size());

      StartTimeline(*timeline);

      UpdateCycleWeights();

      return duration != 0.0;
   }
      
   bool OsgAnimator::ClearAction(dtAnim::OsgAnimation& anim)
   {
      bool success = false;
      bool isAction = GetCycleIndex(anim) < 0;

      if (isAction)
      {
         // DEBUG:
         //printf("Clearing action: \"%s\" with %f\n\n", anim.GetName().c_str(), 0.0f);

         success = FadeOutAnimation(anim, 0.0f);
      }

      return success;
   }
      
   bool OsgAnimator::ClearCycle(dtAnim::OsgAnimation& anim, float fadeOutTime)
   {
      bool success = false;

      if (GetCycleIndex(anim) >= 0)
      {
         if (fadeOutTime < 0.0f)
         {
            fadeOutTime = 0.0f;
         }

         // DEBUG:
         //printf("Clearing cycle: \"%s\" with %f\n\n", anim.GetName().c_str(), fadeOutTime);

         if (fadeOutTime > 0.0f)
         {
            success = FadeOutAnimation(anim, fadeOutTime);
         }
         else
         {
            RemoveCycle(anim);
            success = RemoveActionsWithAnimation(anim) > 0;
         }
      }

      return success;
   }

   int OsgAnimator::GetCycleIndex(dtAnim::OsgAnimation& anim) const
   {
      int index = -1;

      CycleInfo* curInfo = NULL;
      CycleInfoArray::const_iterator curIter = mCycleInfos.begin();
      CycleInfoArray::const_iterator endIter = mCycleInfos.end();
      for (int i = 0; curIter != endIter; ++curIter, ++i)
      {
         curInfo = curIter->get();
         if (curInfo->mAnim.get() == &anim
            || curInfo->mAnim->GetOsgAnimation() == anim.GetOsgAnimation())
         {
            index = i;
            break;
         }
      }

      return index;
   }

   bool OsgAnimator::RemoveCycle(dtAnim::OsgAnimation& anim)
   {
      bool success = false;
      
      CycleInfo* curInfo = NULL;
      CycleInfoArray::iterator curIter = mCycleInfos.begin();
      CycleInfoArray::iterator endIter = mCycleInfos.end();
      CycleInfoArray::iterator foundIter = endIter;
      for (int i = 0; curIter != endIter; ++curIter, ++i)
      {
         curInfo = curIter->get();
         if (curInfo->mAnim->GetOsgAnimation() == anim.GetOsgAnimation())
         {
            foundIter = curIter;
            break;
         }
      }

      if (foundIter != endIter)
      {
         mCycleInfos.erase(foundIter);

         UpdateCycleWeights();

         success = true;
      }

      return success;
   }

   bool OsgAnimator::FadeInAnimation(dtAnim::OsgAnimation& anim, float fadeTime)
   {
      bool success = false;

      if (fadeTime > 0.0f)
      {
         osgAnimation::Animation* osgAnim = anim.GetOsgAnimation();

         dtCore::RefPtr<osgAnimation::ActionBlendIn> fadeIn
            = new osgAnimation::ActionBlendIn(osgAnim, fadeTime, osgAnim->getWeight());
         mAnimManager->getTimeline()->addActionNow(fadeIn.get());
         
         dtCore::RefPtr<ActionTimer> atimer
            = new ActionTimer(fadeTime, fadeIn, osgAnim, &anim);
         AddActionClearTimer(*atimer);

         success = true;
      }

      return success;
   }

   bool OsgAnimator::FadeOutAnimation(dtAnim::OsgAnimation& anim, float fadeTime)
   {
      bool success = false;
      
      osgAnimation::Animation* osgAnim = anim.GetOsgAnimation();

      if (fadeTime > 0.0f)
      {
         dtCore::RefPtr<osgAnimation::ActionBlendOut> fadeOut
            = new osgAnimation::ActionBlendOut(osgAnim, fadeTime);
         mAnimManager->getTimeline()->addActionNow(fadeOut.get());

         dtCore::RefPtr<ActionTimer> atimer
            = new ActionTimer(fadeTime, fadeOut, osgAnim, &anim);
         AddActionClearTimer(*atimer);
      }
      else // Fade time 0 or less.
      {
         osgAnimation::ActionStripAnimation* action = GetActionForAnimation(*osgAnim);
         if (action != NULL)
         {
            SetActionClearTimer(*action, 0.0f);

            success = true;
         }
      }

      return success;
   }

   int OsgAnimator::GetActions(dtAnim::OsgAnimation& anim, OsgActionArray& outActions)
   {
      osgAnimation::Timeline* timeline = mAnimManager->getTimeline();
      osgAnimation::Animation* osgAnim = anim.GetOsgAnimation();

      FindMatchingActionPred predFindActions(osgAnim);

      // Go through all layers and find actions that reference the animation.
      typedef osgAnimation::Timeline::ActionLayers ActionLayers;
      typedef osgAnimation::Timeline::ActionList ActionList;
      const ActionLayers& layers = timeline->getActionLayers();
      ActionLayers::const_iterator curLayer = layers.begin();
      ActionLayers::const_iterator endLayer = layers.end();
      for (; curLayer != endLayer; ++curLayer)
      {
         const ActionList& actions = curLayer->second;
         std::for_each(actions.begin(), actions.end(), predFindActions);
      }
      
      OsgActionArray& foundActions = predFindActions.mActions;
      int numActions = int(foundActions.size());

      // Return the references of the matching actions in the provided container.
      if (numActions > 0)
      {
         outActions.insert(outActions.end(), foundActions.begin(), foundActions.end());
      }

      return numActions;
   }

   const dtAnim::AnimationStateEnum& OsgAnimator::GetAnimationState(const OsgAnimation& anim) const
   {
      const dtAnim::AnimationStateEnum* state = &dtAnim::AnimationStateEnum::NONE;

      osgAnimation::Animation* osgAnim
         = const_cast<osgAnimation::Animation*>(anim.GetOsgAnimation());
      if (osgAnim != NULL)
      {
         if (mOriginalAnimManager->isPlaying(osgAnim))
         {
            state = &dtAnim::AnimationStateEnum::STEADY;

            FindMatchingActionPred predFindActions(osgAnim);
            
            typedef osgAnimation::Timeline::ActionLayers ActionLayers;
            typedef osgAnimation::Timeline::ActionList ActionList;

            // Find the action objects to be removed that are
            // relevant to the specified animation.
            const ActionLayers& layers = mAnimManager->getTimeline()->getActionLayers();
            ActionLayers::const_iterator curLayer = layers.begin();
            ActionLayers::const_iterator endLayer = layers.end();
            for (; curLayer != endLayer; ++curLayer)
            {
               const ActionList& actions = curLayer->second;
               std::for_each(actions.begin(), actions.end(), predFindActions);
            }

            if ( ! predFindActions.mActions.empty())
            {
               osgAnimation::Action* action = predFindActions.mActions.front().get();

               std::string className = action->className();
               if (className == ACTION_BLEND_IN)
               {
                  state = &dtAnim::AnimationStateEnum::FADE_IN;
               }
               else if (className == ACTION_BLEND_OUT)
               {
                  state = &dtAnim::AnimationStateEnum::FADE_OUT;
               }
            }
         }
         else
         {
            state = &dtAnim::AnimationStateEnum::STOPPED;
         }
      }

      return *state;
   }

   void OsgAnimator::UpdateActionClearTimers(float timeDelta)
   {
      osgAnimation::Timeline* timeline = mAnimManager->getTimeline();
      
      if (timeline->getStatus() == osgAnimation::Timeline::Play)
      {
         bool hasActions = mActionClearTimers.empty();

         typedef std::vector<osgAnimation::Action*> ActionArray;
         ActionArray actionsToRemove;

         float curTime = 0.0f;
         ActionTimeMap::iterator curIter = mActionClearTimers.begin();
         ActionTimeMap::iterator endIter = mActionClearTimers.end();
         for (; curIter != endIter; ++curIter)
         {
            curTime = curIter->second->mTimer;
            if (curTime > 0.0f)
            {
               curTime -= timeDelta;

               if (curTime <= 0.0f)
               {
                  actionsToRemove.push_back(curIter->first);
               }
               else
               {
                  curIter->second->mTimer = curTime;
               }
            }
         }

         // Determine if any actions have been marked to be cleared.
         if ( ! actionsToRemove.empty())
         {
            dtAnim::OsgAnimation* curAnim = NULL;
            osgAnimation::Action* curAction = NULL;
            ActionArray::iterator curIter = actionsToRemove.begin();
            ActionArray::iterator endIter = actionsToRemove.end();
            for (; curIter != endIter; ++curIter)
            {
               curAction = *curIter;

               ActionTimeMap::iterator foundIter = mActionClearTimers.find(curAction);
               if (foundIter != mActionClearTimers.end())
               {
                  // DEBUG:
                  //printf("Removing action: %s [%s]\n", curAction->getName().c_str(), curAction->className());

                  curAnim = foundIter->second->mAnim;
                  if (RemoveCycle(*curAnim))
                  {
                     // DEBUG:
                     //printf("\tCycle removed: %s\n", curAnim->GetName().c_str());
                  }

                  // DEBUG:
                  //printf("\n");

                  mActionClearTimers.erase(foundIter);

                  timeline->removeAction(curAction);
               }
            }
         }

         if (hasActions && mActionClearTimers.empty())
         {
            // Determine if there are no actions being processed.
            //int numActions = GetActiveAnimationCount();

            // DEBUG:
            //printf("Animator flush actions: %d\n", numActions);

            timeline->stop();

            // DEBUG:
            //printf("Animator stopped.\n\n");
         }
      }
   }

   void OsgAnimator::UpdateCycleWeights()
   {
      int numCycles = 0;
      float accumulatedWeight = 0.0f;
      float accumulatedDuration = 0.0f;
      
      // Average weights of all active synchronized animation cycles.
      CycleInfo* curInfo = NULL;
      osgAnimation::Animation* curAnim = NULL;
      CycleInfoArray::iterator curIter = mCycleInfos.begin();
      CycleInfoArray::iterator endIter = mCycleInfos.end();
      for (; curIter != endIter; ++curIter)
      {
         curInfo = curIter->get();
         curAnim = curInfo->mAnim->GetOsgAnimation();

         // Determine if the cycle is synchronized.
         if (curInfo->mSynchronous)
         {
            float weight = curAnim->getWeight();
            float coreDuration = curInfo->mAnim->GetOriginalDuration();

            accumulatedWeight += weight;
            accumulatedDuration += weight * coreDuration;
            ++numCycles;
         }
      }

      // Adjust the global animation cycle duration
      float lastAverageDuration = mAverageCycleDuration;
      float averageDuration = 0.0f;
      if (accumulatedWeight > 0.0f)
      {
         averageDuration = accumulatedDuration / accumulatedWeight;
      }

      bool cycleCountUpdated = numCycles != 0
         && mNumCyclesSynced != numCycles;
      mNumCyclesSynced = numCycles;

      if (averageDuration != lastAverageDuration || cycleCountUpdated)
      {
         mAverageCycleDuration = averageDuration;

         curInfo = NULL;
         curAnim = NULL;
         curIter = mCycleInfos.begin();
         endIter = mCycleInfos.end();
         for (; curIter != endIter; ++curIter)
         {
            curInfo = curIter->get();
            curAnim = curInfo->mAnim->GetOsgAnimation();

            // Operate only on the synchronized cycles.
            if (curInfo->mSynchronous)
            {
               // TODO:
               // Adjust any active fade-in or fade-out
               // objects associated with this cycle
               // since its duration is changing.

               //float oldWeight = curAnim->getWeight();

               // DEBUG:
               //printf("Setting animation weight: \"%s\" - %f to %f\n\n",
               //   curAnim->getName().c_str(), oldWeight, curInfo->mAnim->GetOriginalWeight()/numCycles);

               curAnim->setDuration(averageDuration);
               curAnim->setWeight(curInfo->mAnim->GetOriginalWeight()/numCycles);
            }
         }
      }
   }

   void OsgAnimator::StartTimeline(osgAnimation::Timeline& timeline)
   {
      // Start up the timeline if it is currently stopped.
      if (timeline.getStatus() == osgAnimation::Timeline::Stop)
      {
         // DEBUG:
         //int curFrame = timeline.getCurrentFrame();
         //double curTime = timeline.getCurrentTime();
         //printf("Animator timeline: PLAY @ %d, %f\n", curFrame, curTime);

         timeline.play();
      }
   }
   
   void OsgAnimator::SetupAction(osgAnimation::Action& action)
   {
      float duration = action.getDuration();
      if (duration != 0.0f && action.getNumFrames() == 0)
      {
         // This will force the action to set its frame count.
         action.setDuration(duration);
      }
   }

   void OsgAnimator::AddActionClearTimer(OsgAnimator::ActionTimer& actionTimer)
   {
      ActionTimeMap::iterator foundIter = mActionClearTimers.find(actionTimer.mAction);
      if (foundIter != mActionClearTimers.end())
      {
         foundIter->second = &actionTimer;
      }
      else
      {
         mActionClearTimers.insert(std::make_pair(actionTimer.mAction, &actionTimer));
      }
   }

   void OsgAnimator::SetActionClearTimer(osgAnimation::Action& action, float timeToClear)
   {
      ActionTimeMap::iterator foundIter = mActionClearTimers.find(&action);
      if (foundIter != mActionClearTimers.end())
      {
         foundIter->second->mTimer = timeToClear;
      }
   }

   bool OsgAnimator::RemoveAction(osgAnimation::Action& action)
   {
      bool success = false;

      ActionTimeMap::iterator foundIter = mActionClearTimers.find(&action);
      if (foundIter != mActionClearTimers.end())
      {
         mActionClearTimers.erase(foundIter);
         success = true;
      }

      return success;
   }

   int OsgAnimator::RemoveActionsWithAnimation(dtAnim::OsgAnimation& anim)
   {
      int successes = 0;

      typedef std::vector<osgAnimation::Action*> ActionTimerKeyArray;
      ActionTimerKeyArray timersToRemove;

      osgAnimation::Timeline* timeline = mAnimManager->getTimeline();
      osgAnimation::Animation* animToMatch = anim.GetOsgAnimation();

      // Remove actions from the timeline that relate to the animation.
      ActionTimer* curTimer = NULL;
      ActionTimeMap::iterator curIter = mActionClearTimers.begin();
      ActionTimeMap::iterator endIter = mActionClearTimers.end();
      for (; curIter != endIter; ++curIter)
      {
         curTimer = curIter->second.get();
         if (curTimer->mAnim.valid() && animToMatch == curTimer->mAnim->GetOsgAnimation())
         {
            timeline->removeAction(curTimer->mAction);

            // Keep track of the timer key values
            // so that in a final pass the timer objects
            // can also be removed.
            timersToRemove.push_back(curIter->first);

            ++successes;
         }
      }

      // Remove the clear timer objects associated with matching actions.
      ActionTimerKeyArray::iterator keyIter = timersToRemove.begin();
      for ( ; keyIter != timersToRemove.end(); ++keyIter)
      {
         ActionTimeMap::iterator foundIter = mActionClearTimers.find(*keyIter);
         if (foundIter != mActionClearTimers.end())
         {
            mActionClearTimers.erase(foundIter);
         }
      }

      return successes;
   }

   osgAnimation::ActionStripAnimation* OsgAnimator::GetActionForAnimation(osgAnimation::Animation& anim)
   {
      osgAnimation::ActionStripAnimation* result = NULL;

      osgAnimation::ActionStripAnimation* curAction = NULL;
      ActionTimeMap::iterator curIter = mActionClearTimers.begin();
      ActionTimeMap::iterator endIter = mActionClearTimers.end();
      for (; curIter != endIter; ++curIter)
      {
         curAction = dynamic_cast<osgAnimation::ActionStripAnimation*>(curIter->first);
         if (curAction != NULL && &anim == curAction->getAnimation()->getAnimation())
         {
            result = curAction;
            break;
         }
      }

      return result;
   }

   bool OsgAnimator::BlendPose(dtAnim::AnimationInterface& anim, float weight, float delay)
   {
      // TODO:

      return false;
   }

   bool OsgAnimator::ClearPose(dtAnim::AnimationInterface& anim, float delay)
   {
      // TODO:
      
      return false;
   }

} // namespace dtAnim

