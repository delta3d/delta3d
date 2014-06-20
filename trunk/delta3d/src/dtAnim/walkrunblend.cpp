
#include <dtAnim/walkrunblend.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/stringutils.h>
#include <limits>
#include <cfloat>

namespace dtAnim
{

   static dtUtil::Log& WRB_LOG_INSTANCE = dtUtil::Log::GetInstance("walkrunblend.cpp");

   WalkRunBlend::WalkRunBlend(dtCore::VelocityInterface& mi)
   {
      mWRController = new WRController(*this, mi);
      SetController(mWRController.get());
   }

   WalkRunBlend::WalkRunBlend(WalkRunBlend::WRController& controller)
      : mWRController(&controller)

   {
      SetController(mWRController.get());
   }

   void WalkRunBlend::SetAnimations(dtAnim::Animatable* stand, dtAnim::Animatable* walk, dtAnim::Animatable* run)
   {
      AddAnimation(stand);
      AddAnimation(walk);
      if (run != NULL)
      {
         AddAnimation(run);
      }
      mWRController->SetAnimations(stand, walk, run);
   }

   void WalkRunBlend::Setup(float inherentSpeed, float inherentRunSpeed)
   {
      mWRController->SetInherentSpeeds(inherentSpeed, inherentRunSpeed);
   }

   WalkRunBlend::WRController& WalkRunBlend::GetWalkRunController()
   {
      return *mWRController;
   }

   dtCore::RefPtr<dtAnim::Animatable> WalkRunBlend::Clone(dtAnim::Cal3DModelWrapper* modelWrapper) const
   {
      WalkRunBlend* wrb = new WalkRunBlend(*mWRController->CloneDerived());
      wrb->mWRController->CloneAnimations(modelWrapper);

      for (unsigned i = 0; i < mWRController->GetAnimationCount(); ++i)
      {
         dtAnim::Animatable* anim = mWRController->GetAnimation(i);
         if (anim != NULL)
            wrb->AddAnimation(anim);
      }

      return wrb;
   }

   WalkRunBlend::~WalkRunBlend()
   {
      mWRController = 0;
   }

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


   WalkRunBlend::WRController::WRController(WalkRunBlend& pWR, dtCore::VelocityInterface& mi)
   : BaseClass(pWR)
   , mSpeed(0.0f)
   , mLastActive(0)
   , mMotionSpeedSource(&mi)
   {
   }

   WalkRunBlend::WRController::WRController(const WalkRunBlend::WRController& pWR)
   : BaseClass(pWR)
   , mSpeed(pWR.mSpeed)
   , mLastActive(pWR.mLastActive)
   , mMotionSpeedSource(pWR.mMotionSpeedSource)
   , mAnimations(pWR.mAnimations)
   {

   }

   void WalkRunBlend::WRController::SetAnimations(dtAnim::Animatable* stand, dtAnim::Animatable* walk, dtAnim::Animatable* run)
   {
      mAnimations.push_back(AnimData(stand, 0.0f, 1.0f));
      // small stride walk - blend of walk and stand 38.2 to 61.8 (golden ratio)
      mAnimations.push_back(AnimData(NULL, 0.5f, 0.0f));
      mAnimations.push_back(AnimData(walk, 1.0f, 0.0f));
      mAnimations.push_back(AnimData(run, 2.0f, 0.0f));
      if (stand != NULL)
         stand->SetCurrentWeight(1.0f);
      if (walk != NULL)
         walk->SetCurrentWeight(0.0f);
      if (run != NULL)
         run->SetCurrentWeight(0.0f);
   }

   //this sets the basic necessary blend values, the others get expected values
   void WalkRunBlend::WRController::SetInherentSpeeds(float inherentWalkSpeed, float inherentRunSpeed)
   {
      if (mAnimations.size() >= 2)
      {
         mAnimations[1].mInherentSpeed = inherentWalkSpeed * 0.618;
         if (mAnimations.size() >= 3)
         {
            mAnimations[2].mInherentSpeed = inherentWalkSpeed;
            if (mAnimations.size() >= 4)
            {
               mAnimations[3].mInherentSpeed = inherentRunSpeed;
            }
         }
      }
   }

   float WalkRunBlend::WRController::GetCurrentSpeed() const
   {
      return mSpeed;
   }

   void WalkRunBlend::WRController::SetCurrentSpeed(float pSpeed)
   {
      mSpeed = pSpeed;
   }

   void WalkRunBlend::WRController::AnimData::SetLastWeight(float newWeight)
   {
      mWeightChanged = !dtUtil::Equivalent(mLastWeight, newWeight, 0.001f);
      mLastWeight = newWeight;
   }

   void WalkRunBlend::WRController::AnimData::ApplyWeightChange()
   {
      // Only assign the weight if it hasn't changed since last frame.  This gets rid of some glitches.
      if (mAnim != NULL && !mWeightChanged)
      {
         mAnim->SetCurrentWeight(mLastWeight);
      }
   }

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

   dtAnim::Animatable* WalkRunBlend::WRController::GetAnimation(unsigned i)
   {
      return mAnimations[i].mAnim;
   }

   unsigned WalkRunBlend::WRController::GetAnimationCount() const
   {
      return mAnimations.size();
   }


   dtCore::RefPtr<WalkRunBlend::WRController> WalkRunBlend::WRController::CloneDerived() const
   {
      dtCore::RefPtr<WalkRunBlend::WRController> result = new WRController(*this);
      return result;
   }

   void WalkRunBlend::WRController::CloneAnimations(dtAnim::Cal3DModelWrapper* modelWrapper)
   {
      for (unsigned i = 0 ; i < mAnimations.size(); ++i)
      {
         if (mAnimations[i].mAnim.valid())
         {
            mAnimations[i].mAnim = mAnimations[i].mAnim->Clone(modelWrapper);
         }
      }
   }


   WalkRunBlend::WRController::~WRController()
   {

   }

}



