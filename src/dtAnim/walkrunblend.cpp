
#include <dtAnim/walkrunblend.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/stringutils.h>
#include <limits>
#include <cfloat>

namespace dtAnim
{

   static dtUtil::Log& WRB_LOG_INSTANCE = dtUtil::Log::GetInstance("walkrunblend.cpp");

   ///////////////////////////////////////////////////////
   WalkRunBlend::WalkRunBlend(dtCore::VelocityInterface& mi)
   {
      mWRController = new WRController(*this, mi);
      SetController(mWRController.get());
   }

   ///////////////////////////////////////////////////////
   void WalkRunBlend::SetAnimations(dtAnim::Animatable* stand, dtAnim::Animatable* walk, dtAnim::Animatable* run)
   {
      if (stand != NULL)
      {
         AddAnimation(stand);
      }
      if (walk != NULL)
      {
         AddAnimation(walk);
      }
      if (run != NULL)
      {
         AddAnimation(run);
      }
      mWRController->SetAnimations(stand, walk, run);
   }

   ///////////////////////////////////////////////////////
   void WalkRunBlend::Setup(float inherentWalkSpeed, float inherentRunSpeed)
   {
      // If the animations haven't be setup yet.
      if (mWRController->GetAnimationCount() < 4)
      {
         return;
      }
      mWRController->SetAnimationInherentSpeed(1, inherentWalkSpeed * 0.618);
      mWRController->SetAnimationInherentSpeed(2, inherentWalkSpeed);
      mWRController->SetAnimationInherentSpeed(3, inherentRunSpeed);
   }

   ///////////////////////////////////////////////////////
   WalkRunBlend::WRController& WalkRunBlend::GetWalkRunController()
   {
      return *mWRController;
   }

   ///////////////////////////////////////////////////////
   dtCore::RefPtr<dtAnim::Animatable> WalkRunBlend::Clone(dtAnim::BaseModelWrapper* modelWrapper) const
   {
      WalkRunBlend* wrb = new WalkRunBlend(*mWRController->GetMotionSpeedSource());

      dtCore::RefPtr<Animatable> stand, walk, run;

      stand = mWRController->GetAnimation(0) != NULL ? mWRController->GetAnimation(0)->Clone(modelWrapper) : dtCore::RefPtr<Animatable>();
      walk = mWRController->GetAnimation(2) != NULL ? mWRController->GetAnimation(2)->Clone(modelWrapper) : dtCore::RefPtr<Animatable>();
      run = mWRController->GetAnimation(3) != NULL ? mWRController->GetAnimation(3)->Clone(modelWrapper) : dtCore::RefPtr<Animatable>();

      wrb->SetAnimations(stand, walk, run);

      for (unsigned i = 0; i < mWRController->GetAnimationCount(); ++i)
      {
         wrb->GetWalkRunController().SetAnimationInherentSpeed(i, mWRController->GetAnimationInherentSpeed(i));
      }

      return wrb;
   }

   ///////////////////////////////////////////////////////
   WalkRunBlend::~WalkRunBlend()
   {
      mWRController = 0;
   }

   ///////////////////////////////////////////////////////
   WalkRunBlend::WRController::AnimData::AnimData(dtAnim::Animatable* anim, float inherentSpeed, float initialWeight)
   :  mAnim(anim)
   ,  mInherentSpeed(inherentSpeed)
   ,  mLastWeight(initialWeight)
   ,  mWeightChanged(true) // To make it try to update the first frame
   {
      if (mAnim.valid())
      {
         mAnim->SetCurrentWeight(initialWeight);
      }
   }


   ///////////////////////////////////////////////////////
   WalkRunBlend::WRController::WRController(WalkRunBlend& pWR, dtCore::VelocityInterface& mi)
   : BaseClass(pWR)
   , mSpeed(0.0f)
   , mLastActive(0)
   , mMotionSpeedSource(&mi)
   {
   }

   ///////////////////////////////////////////////////////
   WalkRunBlend::WRController::WRController(const WalkRunBlend::WRController& pWR)
   : BaseClass(pWR)
   , mSpeed(pWR.mSpeed)
   , mLastActive(pWR.mLastActive)
   , mMotionSpeedSource(pWR.mMotionSpeedSource)
   , mAnimations(pWR.mAnimations)
   {

   }

   ///////////////////////////////////////////////////////
   void WalkRunBlend::WRController::SetAnimations(dtAnim::Animatable* stand, dtAnim::Animatable* walk, dtAnim::Animatable* run)
   {
      mAnimations.push_back(AnimData(stand, 0.0f, 1.0f));
      // small stride walk - blend of walk and stand 38.2 to 61.8 (golden ratio)
      mAnimations.push_back(AnimData(NULL, 0.618f, 0.0f));
      mAnimations.push_back(AnimData(walk, 1.0f, 0.0f));
      mAnimations.push_back(AnimData(run, 2.0f, 0.0f));
      if (stand != NULL)
         stand->SetCurrentWeight(1.0f);
      if (walk != NULL)
         walk->SetCurrentWeight(0.0f);
      if (run != NULL)
         run->SetCurrentWeight(0.0f);
   }

   ///////////////////////////////////////////////////////
   float WalkRunBlend::WRController::GetCurrentSpeed() const
   {
      return mSpeed;
   }

   ///////////////////////////////////////////////////////
   void WalkRunBlend::WRController::SetCurrentSpeed(float pSpeed)
   {
      mSpeed = pSpeed;
   }

   ///////////////////////////////////////////////////////
   void WalkRunBlend::WRController::AnimData::SetLastWeight(float newWeight)
   {
      mWeightChanged = !dtUtil::Equivalent(mLastWeight, newWeight, 0.001f);
      mLastWeight = newWeight;
   }

   ///////////////////////////////////////////////////////
   void WalkRunBlend::WRController::AnimData::ApplyWeightChange()
   {
      // Only assign the weight if it hasn't changed since last frame.  This gets rid of some glitches.
      if (mAnim != NULL && !mWeightChanged)
      {
         mAnim->SetCurrentWeight(mLastWeight);
      }
   }

   ///////////////////////////////////////////////////////
   /*virtual*/ void WalkRunBlend::WRController::Update(float dt)
   {
      //update our velocity vector
      if(mMotionSpeedSource.valid())
      {
           mSpeed = mMotionSpeedSource->GetVelocity().length();
      }
      else
      {
         WRB_LOG_INSTANCE.LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Controller has no valid velocity source.");
      }

      if (dtUtil::Equivalent(mSpeed, 0.0f, 0.008f))
      {
         if (mAnimations[0].mAnim.valid())
         {
            mAnimations[0].SetLastWeight(1.0f);
            for (unsigned i = 1; i < mAnimations.size(); ++i)
            {
               mAnimations[i].SetLastWeight(0.0f);
            }
         }
      }
      else
      {
         mAnimations[0].SetLastWeight(0.0f);

         bool foundBest = false;
         for (unsigned i = 1; i < mAnimations.size(); ++i)
         {
            if (!foundBest)
            {
               if (i < mAnimations.size()-1)
               {
                  float nSpeed = mAnimations[i].mInherentSpeed;
                  float np1Speed = mAnimations[i+1].mInherentSpeed;
                  bool np1Active = mAnimations[i+1].mLastWeight > FLT_EPSILON;
                  // The difference between and n and n+1 speeds
                  float diff = np1Speed - nSpeed;
                  float midPoint =  diff / 2.0f;
                  // The threshold to switch over from one animation channel to the next is the golden ratio.
                  float diffPct  = diff * 0.618f;

                  bool pickClosest = (mSpeed < nSpeed + midPoint && !np1Active);
                  bool switchDown = mSpeed < np1Speed - diffPct;
                  // Check mWeightChanged so that it won't allow a discontinuity to push it up to the next speed.
                  bool stickyDown = (mSpeed < nSpeed + diffPct && !mAnimations[i].mWeightChanged && mAnimations[i].mLastWeight > FLT_EPSILON);

                  if (pickClosest || switchDown || stickyDown )
                  {
                     mAnimations[i].SetLastWeight(1.0f);
                     foundBest = true;
                     WRB_LOG_INSTANCE.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Found best animation idx: " + dtUtil::ToString(i) );
                  }
                  else
                  {
                     mAnimations[i].SetLastWeight(0.0f);
                  }
               }
               else
               {
                  mAnimations[i].SetLastWeight(1.0f);
               }
            }
            else
            {
               mAnimations[i].SetLastWeight(0.0f);
            }
            WRB_LOG_INSTANCE.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, " idx "
                  + dtUtil::ToString(i) + " Body Speed: " + dtUtil::ToString(mSpeed) + " weight "
                  + dtUtil::ToString(mAnimations[i].mLastWeight));
         }

      }

      // Anim 1, for now, is a smaller stride walk that blends stand and walk.
      if (mAnimations[1].mLastWeight > 0.0f && !mAnimations[1].mWeightChanged)
      {
         // Have to short circuit the weight changed logic a bit here.
         mAnimations[0].mLastWeight = 0.382f;
         mAnimations[2].mLastWeight = 0.618f;
         for (unsigned i = 0; i < 3; i+=2)
         {
            mAnimations[i].mWeightChanged = false;
            mAnimations[i].mAnim->SetCurrentWeight(mAnimations[i].mLastWeight);
            // the 1 is correct here, just in case it catches your eye.
            mAnimations[i].mAnim->SetSpeed(mSpeed/mAnimations[1].mInherentSpeed);
            mAnimations[i].mAnim->Update(dt);
         }
      }
      else
      {
         for (unsigned i = 0; i < mAnimations.size(); ++i)
         {
            if (mAnimations[i].mAnim.valid())
            {
               float playSpeed = mAnimations[i].mInherentSpeed > FLT_EPSILON ? mSpeed/mAnimations[i].mInherentSpeed : 1.0f;

               WRB_LOG_INSTANCE.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, + " idx "
                     + dtUtil::ToString(i) + " Animation Play Speed: " + dtUtil::ToString(playSpeed) + " wgt " + dtUtil::ToString(mAnimations[i].mLastWeight));
               mAnimations[i].ApplyWeightChange();
               mAnimations[i].mAnim->SetSpeed(playSpeed);
               mAnimations[i].mAnim->Update(dt);
            }
         }
      }
   }

   ///////////////////////////////////////////////////////
   float WalkRunBlend::WRController::GetAnimationInherentSpeed(unsigned i) const
   {
      return mAnimations[i].mInherentSpeed;
   }

   ///////////////////////////////////////////////////////
   void WalkRunBlend::WRController::SetAnimationInherentSpeed(unsigned i, float speed)
   {
      mAnimations[i].mInherentSpeed = speed;
   }

   ///////////////////////////////////////////////////////
   dtAnim::Animatable* WalkRunBlend::WRController::GetAnimation(unsigned i)
   {
      return mAnimations[i].mAnim;
   }

   ///////////////////////////////////////////////////////
   unsigned WalkRunBlend::WRController::GetAnimationCount() const
   {
      return mAnimations.size();
   }

   ///////////////////////////////////////////////////////
   dtCore::VelocityInterface* WalkRunBlend::WRController::GetMotionSpeedSource()
   {
      return mMotionSpeedSource.get();
   }

   ///////////////////////////////////////////////////////
   WalkRunBlend::WRController::~WRController()
   {
   }

}



