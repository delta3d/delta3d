
#include <dtAnim/walkrunblend.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/stringutils.h>
#include <cfloat>

namespace dtAnim
{

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

   void WalkRunBlend::SetupWithWalkSpeed(float inherentSpeed)
   {
      mWRController->SetRunWalkBasic(inherentSpeed, inherentSpeed* 0.5, inherentSpeed * 0.5);
   }

   WalkRunBlend::WRController& WalkRunBlend::GetWalkRunController()
   {
      return *mWRController;
   }

   dtCore::RefPtr<dtAnim::Animatable> WalkRunBlend::Clone(dtAnim::Cal3DModelWrapper* modelWrapper) const
   {
      WalkRunBlend* wrb = new WalkRunBlend(*mWRController->CloneDerived());

      if(mWRController->GetWalk() && mWRController->GetRun())
      {
         wrb->SetAnimations(mWRController->GetStand()->Clone(modelWrapper).get(), mWRController->GetWalk()->Clone(modelWrapper).get(), mWRController->GetRun()->Clone(modelWrapper).get());
      }
      return wrb;
   }

   WalkRunBlend::~WalkRunBlend()
   {
      mWRController = 0;
   }


   WalkRunBlend::WRController::WRController(WalkRunBlend& pWR, dtCore::VelocityInterface& mi)
   : BaseClass(pWR)
   , mSpeed(0.0f)
   , mStandStart(0.000000f)//in m/s
   , mStandFadeIn(0.0f)
   , mStandStop(0.25) // in m/s
   , mStandFadeOut(0.15f)
   , mWalkStart(0.000001f)//in m/s
   , mWalkFadeIn(0.15f)
   , mWalkStop(1.5f)//in m/s
   , mWalkFadeOut(0.15f)
   , mRunStart(1.35f)//in m/s
   , mRunFadeIn(0.15f)
   , mRunStop(std::numeric_limits<float>::max()) //we don't want to stop running
   , mRunFadeOut(0.0f)
   , mMotionSpeedSource(&mi)
   {
   }

   WalkRunBlend::WRController::WRController(const WalkRunBlend::WRController& pWR)
   : BaseClass(pWR)
   , mSpeed(pWR.mSpeed)
   , mStandStart(pWR.mStandStart)
   , mStandFadeIn(pWR.mStandFadeIn)
   , mStandStop(pWR.mStandStop)
   , mStandFadeOut(pWR.mStandFadeOut)
   , mWalkStart(pWR.mWalkStart)
   , mWalkFadeIn(pWR.mWalkFadeIn)
   , mWalkStop(pWR.mWalkStop)
   , mWalkFadeOut(pWR.mWalkFadeOut)
   , mRunStart(pWR.mRunStart)
   , mRunFadeIn(pWR.mRunFadeIn)
   , mRunStop(pWR.mRunStop)
   , mRunFadeOut(pWR.mRunFadeOut)
   , mMotionSpeedSource(pWR.mMotionSpeedSource)
   {

   }

   void WalkRunBlend::WRController::SetAnimations(dtAnim::Animatable* stand, dtAnim::Animatable* walk, dtAnim::Animatable* run)
   {
      mStand = stand;
      mRun = run;
      mWalk = walk;
   }

   //this sets the basic necessary blend values, the others get expected values
   void WalkRunBlend::WRController::SetRunWalkBasic(float inherentWalkSpeed, float walkFade, float runFade)
   {
      mStandStart = 0.0f;
      mStandFadeOut = walkFade;
      mStandStop = walkFade;
      mWalkStart = FLT_EPSILON;
      mWalkFadeIn = walkFade;
      mWalkStop= inherentWalkSpeed * 2.0f;
      mWalkFadeOut = walkFade;
      mRunStart = inherentWalkSpeed * 1.05;
      mRunFadeIn = runFade;
      mRunStop = std::numeric_limits<float>::max();
      mRunFadeOut = 0.0f;
   }

   void WalkRunBlend::WRController::SetStand(float start, float fadeIn, float stop, float fadeOut)
   {
      mStandStart = start;
      mStandFadeIn = fadeIn;
      mStandStop = stop;
      mStandFadeOut = fadeOut;
   }

   //customize the walk
   void WalkRunBlend::WRController::SetWalk(float start, float fadeIn, float stop, float fadeOut)
   {
      mWalkStart = start;
      mWalkFadeIn = fadeIn;
      mWalkStop = stop;
      mWalkFadeOut = fadeOut;
   }

   //customize the run
   void WalkRunBlend::WRController::SetRun(float start, float fadeIn, float stop, float fadeOut)
   {
      mRunStart = start;
      mRunFadeIn = fadeIn;
      mRunStop = stop;
      mRunFadeOut = fadeOut;
   }

   void WalkRunBlend::WRController::SetCurrentSpeed(float pSpeed)
   {
      mSpeed = pSpeed;
   }

   /*virtual*/ void WalkRunBlend::WRController::Update(float dt)
   {
      if (!mStand.valid() || !mWalk.valid() || !mRun.valid())
      {
         //A warning was already printed out that this won't work, so just make sure we don't crash.
         return;
      }

      //update our velocity vector
      if(mMotionSpeedSource.valid())
      {
         mSpeed = mMotionSpeedSource->GetVelocity().length();
      }
      else
      {
         LOG_ERROR("Controller has no valid parent pointer");
      }


      float standWeight = ComputeWeight(mStand.get(), mStandStart, mStandFadeIn, mStandStop, mStandFadeOut);
      float walkWeight = ComputeWeight(mWalk.get(), mWalkStart, mWalkFadeIn, mWalkStop, mWalkFadeOut);
      float runWeight = ComputeWeight(mRun.get(), mRunStart, mRunFadeIn, mRunStop, mRunFadeOut);

      if (standWeight <= FLT_EPSILON && walkWeight <= FLT_EPSILON && runWeight <= FLT_EPSILON)
      {
         walkWeight = 1.0f;
         LOG_ERROR("dtCore::VelocityInterface with all 0 weights");
      }

      LOGN_DEBUG("walkrunblend.cpp", "Animiation Speed: " + dtUtil::ToString(mSpeed) + " weights "
            + " " + dtUtil::ToString(standWeight)
      + " " + dtUtil::ToString(walkWeight)
      + " " + dtUtil::ToString(runWeight)
      );

      mStand->SetCurrentWeight(standWeight);
      mWalk->SetCurrentWeight(walkWeight);

      mStand->Update(dt);

      if (mRun == NULL)
      {
         walkWeight = walkWeight + runWeight;
         dtUtil::Clamp(walkWeight, 0.0f, 1.0f);
      }
      else
      {
         mRun->Update(dt);
      }

      mWalk->Update(dt);

   }

   dtAnim::Animatable* WalkRunBlend::WRController::GetStand()
   {
      return mStand.get();
   }

   dtAnim::Animatable* WalkRunBlend::WRController::GetWalk()
   {
      return mWalk.get();
   }

   dtAnim::Animatable* WalkRunBlend::WRController::GetRun()
   {
      return mRun.get();
   }

   dtCore::RefPtr<WalkRunBlend::WRController> WalkRunBlend::WRController::CloneDerived() const
   {
      return new WRController(*this);
   }

   WalkRunBlend::WRController::~WRController()
   {

   }

   float WalkRunBlend::WRController::ComputeWeight(dtAnim::Animatable* pAnim, float startSpeed, float fadeIn, float stopSpeed, float fadeOut)
   {
      //we will have the default imply mSpeed is between startSpeed and stopSpeed
      //which basically just saves us another if check
      float weight = 1.0f;

      //if we are out of bounds
      if(mSpeed < startSpeed || mSpeed > stopSpeed)
      {
         weight = 0.00f;
      }
      else if(mSpeed < startSpeed + fadeIn) //else if we are fading in
      {
         weight = (mSpeed - startSpeed) / fadeIn;
      }
      else if(mSpeed > (stopSpeed - fadeOut)) //else we are fading out
      {
         weight = (stopSpeed - mSpeed) / fadeOut;
      }

      dtUtil::Clamp(weight, 0.0f, 1.0f);
      return weight;
   }

}



