/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * David Guthrie, Curtiss Murphy
 */
#include <prefix/dtgameprefix.h>
#include <dtGame/deadreckoninghelper.h>

#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/propertymacros.h>

#include <dtGame/deadreckoningcomponent.h>
#include <dtGame/gameactor.h>

#include <dtUtil/log.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/mathdefines.h>

#include <osg/io_utils>  //for Vec3 streaming
#include <osgSim/DOFTransform>

namespace dtGame
{
   const dtGame::ActorComponent::ACType DeadReckoningHelper::TYPE("DeadReckoningActComp");

   IMPLEMENT_ENUM(DeadReckoningAlgorithm);
   DeadReckoningAlgorithm DeadReckoningAlgorithm::NONE("None");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::STATIC("Static");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::VELOCITY_ONLY("Velocity Only");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION("Velocity and Acceleration");

   const float DeadReckoningHelper::DEFAULT_MAX_SMOOTHING_TIME_ROT = 2.0f;
   const float DeadReckoningHelper::DEFAULT_MAX_SMOOTHING_TIME_POS = 4.0f;

   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_POSITION("Position");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_POSITIONRATE("PositionRate");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_EXTENSION("Extension");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_EXTENSIONRATE("ExtensionRate");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_X("LocationX");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_XRATE("LocationXRate");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_Y("LocationY");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_YRATE("LocationYRate");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_Z("LocationZ");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ZRATE("LocationZRate");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_AZIMUTH("Azimuth");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_AZIMUTHRATE("AzimuthRate");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ELEVATION("Elevation");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ELEVATIONRATE("ElevationRate");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ROTATION("Rotation");
   const std::string DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ROTATIONRATE("RotationRate");

   IMPLEMENT_ENUM(DeadReckoningHelper::UpdateMode);
   DeadReckoningHelper::UpdateMode DeadReckoningHelper::UpdateMode::AUTO("AUTO");
   DeadReckoningHelper::UpdateMode DeadReckoningHelper::UpdateMode::CALCULATE_ONLY("CALCULATE_ONLY");
   DeadReckoningHelper::UpdateMode
      DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR("CALCULATE_AND_MOVE_ACTOR");

   //////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////
   /// A wrapper for private data to prevent includes wherever deadreckoninghelper.h is used - uses the pimple pattern (like system.cpp)
   class DeadReckoningHelperImpl
   {
   public:
      DeadReckoningHelperImpl() 
         : mPosSplineXA(0.0f)
         , mPosSplineXB(0.0f)
         , mPosSplineXC(0.0f)
         , mPosSplineXD(0.0f)
         , mPosSplineYA(0.0f)
         , mPosSplineYB(0.0f)
         , mPosSplineYC(0.0f)
         , mPosSplineYD(0.0f)
         , mPosSplineZA(0.0f)
         , mPosSplineZB(0.0f)
         , mPosSplineZC(0.0f)
         , mPosSplineZD(0.0f)
         , mCurTimeDelta(0.0f)
      {  
      }
      ~DeadReckoningHelperImpl() 
      { 
      }

      /// Called when the trans or vel changes to recompute the parametric values used during spline blending.  
      void RecomputeTransSplineValues(const osg::Vec3 &transBeforeLastUpdate, const osg::Vec3 &velBeforeLastUpdate, 
         const osg::Vec3 &lastTrans, const osg::Vec3 &lastVel, float transEndSmoothingTime, 
         const osg::Vec3 &currentAccel)
      {        

         osg::Vec3 distanceFromAcceleration = currentAccel * 0.5f * transEndSmoothingTime * transEndSmoothingTime;
         osg::Vec3 projectForwardDistance = distanceFromAcceleration + lastVel * transEndSmoothingTime;
         mSplineEndLocation = lastTrans + projectForwardDistance;

         ///////////////////////////////////////////////////
         // CUBIC BEZIER SPLINE IMPLEMENTATION
         float velNormalizer = (transEndSmoothingTime > 0.0f) ? 1.0 * transEndSmoothingTime : 1.0f;
         osg::Vec3 coord0; // Start pos
         osg::Vec3 coord1; // First Control - Start Pos + 1/3 velBeforeLastUpdate 
         osg::Vec3 coord3; // End Pos
         osg::Vec3 coord2; // Second Control - End Pos - 1/3 lastvel
         coord0 = transBeforeLastUpdate;
         coord1 = transBeforeLastUpdate + (velBeforeLastUpdate * velNormalizer) * 0.33333f;
         coord3 = mSplineEndLocation; //lastTrans + projectForwardDistance;
         osg::Vec3 estimatedVel = lastVel + currentAccel * transEndSmoothingTime;
         coord2 = coord3 - (estimatedVel * velNormalizer * 0.33333f);

         // Note, the variables are setup as XA, XB, XC, ... to support multiple algorithms... (see below)
         mPosSplineXA = coord0.x();
         mPosSplineXB = coord1.x();
         mPosSplineXC = coord2.x();
         mPosSplineXD = coord3.x();

         mPosSplineYA = coord0.y();
         mPosSplineYB = coord1.y();
         mPosSplineYC = coord2.y();
         mPosSplineYD = coord3.y();

         mPosSplineZA = coord0.z();
         mPosSplineZB = coord1.z();
         mPosSplineZC = coord2.z();
         mPosSplineZD = coord3.z();


         /// Below are 2 alternate methods that were fully implemented but discarded because the above
         // technique works better. 

         /////////////////////////////////////////////////
         // CATMULL-ROM SPLINE Implementation
         // The following method is an implementation of Catmull-Rom splines. 
         // We will pre-compute four values, ABCD, and use them in a simple equation: 
         //    pos = A*t^3 + B*t^2 + C*t + D;
         // This equation should interpret between coord 1 and coord 2 along a smooth curve which 
         // factors in coord0 and coord 3. 
         // Note that coord 0 and coord 3 are complicated to compute - required to get the incoming and outgoing
         // velocity to be fairly close.  
         /*
         osg::Vec3 coord0; // Start pos - velBeforeLastUpdate
         coord0 = mNextSplineStartLocation;//transBeforeLastUpdate - velBeforeLastUpdate * transEndSmoothingTime; // mNextSplineStartLocation; // 
         osg::Vec3 coord1; // Start Pos
         coord1 = transBeforeLastUpdate;
         // Coord 2 and 3 will seem wierd because we also project forward along our velocity to 
         // where we 'should' be at this time. We don't want to be chasing behind
         osg::Vec3 coord2 = mSplineEndLocation; //lastTrans + projectForwardDistance; //End Pos (desired)
         osg::Vec3 coord3 = coord2 + projectForwardDistance + distanceFromAcceleration * 4.0f; // End Pos plus projected (again)

         // Given these 4 coords, we can compute the 12 parameters to our parametric equation.
         mPosSplineXA = 0.5f * (-coord0.x() + 3.0f * coord1.x() - 3.0f * coord2.x() + coord3.x());
         mPosSplineXB = coord0.x() - 2.5f*coord1.x() + 2.0f*coord2.x() - 0.5f*coord3.x();
         mPosSplineXC = 0.5f * (-coord0.x() + coord2.x());
         mPosSplineXD = coord1.x();

         mPosSplineYA = 0.5f * (-coord0.y() + 3.0f * coord1.y() - 3.0f * coord2.y() + coord3.y());
         mPosSplineYB = coord0.y() - 2.5f*coord1.y() + 2.0f*coord2.y() - 0.5f*coord3.y();
         mPosSplineYC = 0.5f * (-coord0.y() + coord2.y());
         mPosSplineYD = coord1.y();

         mPosSplineZA = 0.5f * (-coord0.z() + 3.0f * coord1.z() - 3.0f * coord2.z() + coord3.z());
         mPosSplineZB = coord0.z() - 2.5f*coord1.z() + 2.0f*coord2.z() - 0.5f*coord3.z();
         mPosSplineZC = 0.5f * (-coord0.z() + coord2.z());
         mPosSplineZD = coord1.z();

         mNextSplineStartLocation = lastTrans - distanceFromAcceleration;
         */

         /* 
         Alternate method 2 - has smooth curves, but the curves have visual discontinuities between them
         This technique uses 4 coordinates from a start point and initial velocity and an end point and final vel. 
         The following link goes over the math along with visuals. It explains the process pretty well:
         http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/
         */

         /*
         // A 2nd alternate method follows. This is the bezier cubic spline method from the following
         // articles. It is basically what was used above, but doesn't work out quite as nicely.
         // 1) Nick Caldwell from Gamedev.net entitled 'Defeating Lag with Cubic Splines' published 2/14/2000.
         // 2) http://www.tinaja.com/glib/cubemath.pdf - 'The Math Behind Bezier Cubic Splines'
         // Note - There are some minor typos/math errors in Nick Caldwells paper

         // we re-normalize the velocity based on the smoothing time & a weight, else the control points become too strong
         float velNormalizer = (transEndSmoothingTime > 0.0f) ? 0.1 * transEndSmoothingTime : 1.0f;
         osg::Vec3 distanceFromAcceleration = currentAccel * 0.5f * transEndSmoothingTime * transEndSmoothingTime;
         osg::Vec3 coord0; // Start pos
         coord0 = transBeforeLastUpdate;
         osg::Vec3 coord1; // Start Pos + velBeforeLastUpdate
         coord1 = transBeforeLastUpdate + velBeforeLastUpdate * velNormalizer;
         osg::Vec3 coord2; // coord3 - (lastvel * time)    // 
         coord2 = lastTrans + distanceFromAcceleration;
         osg::Vec3 coord3; // End Pos = last trans + (lastvel * time) + distanceFromAcceleration. //project forward
         coord3 = coord2 + lastVel * velNormalizer;
         // Given these 4 coords, we can compute the 12 parameters to our parametric equation.
         mPosSplineXA = coord3.x() - 3.0f * coord2.x() + 3.0f * coord1.x() - coord0.x();
         mPosSplineXB = 3.0f * coord2.x() - 6.0f * coord1.x() + 3.0f * coord0.x();
         mPosSplineXC = 3.0f * coord1.x() - 3.0f * coord0.x();
         mPosSplineXD = coord0.x();
         // etc for YA, YB, ... ZA, ZB, ...
         */
      }

      // The following variables are used to compute the 'cubic spline' that represents the blended position
      float mPosSplineXA, mPosSplineXB, mPosSplineXC, mPosSplineXD; // x spline pre-compute values
      float mPosSplineYA, mPosSplineYB, mPosSplineYC, mPosSplineYD; // y spline pre-compute values
      float mPosSplineZA, mPosSplineZB, mPosSplineZC, mPosSplineZD; // z spline pre-compute values
      osg::Vec3 mPreviousInstantVel;
      float mCurTimeDelta; // Tracks how long this process step is for. Used to compute instant vel.
      osg::Vec3 mSplineEndLocation; // The target goal of the spline == the end point at time T.
      //osg::Vec3 mNextSplineStartLocation; // The starting point of the last spline == coord1 becomes coord0 next time.
   };



   //////////////////////////////////////////////////////////////////////
   DeadReckoningHelper::DeadReckoningHelper()
   : ActorComponent(TYPE)
   , mAutoRegisterWithGMComponent(true)
   , mGroundClampType(&GroundClampTypeEnum::KEEP_ABOVE)
   , mLastTranslationUpdatedTime(0.0)
   , mLastRotationUpdatedTime(0.0)
   , mAverageTimeBetweenTranslationUpdates(0.0f)
   , mAverageTimeBetweenRotationUpdates(0.0f)
   , mMaxTranslationSmoothingTime(DEFAULT_MAX_SMOOTHING_TIME_POS)
   , mMaxRotationSmoothingTime(DEFAULT_MAX_SMOOTHING_TIME_ROT)
   , mUseFixedSmoothingTime(false)
   , mFixedSmoothingTime(1.0f)
   , mTranslationElapsedTimeSinceUpdate(0.0f)
   , mRotationElapsedTimeSinceUpdate(0.0f)
   , mTranslationEndSmoothingTime(0.0f)
   , mRotationEndSmoothingTime(0.0f)
   , mMinDRAlgorithm(&DeadReckoningAlgorithm::STATIC)
   , mUpdateMode(&DeadReckoningHelper::UpdateMode::AUTO)
   , mTranslationInitiated(false)
   , mRotationInitiated(false)
   , mUpdated(false)
   , mTranslationUpdated(false)
   , mRotationUpdated(false)
   //, mFlying(false)
   , mRotationResolved(true)
   , mUseCubicSplineTransBlend(false)
   , mExtraDataUpdated(false)
   , mForceUprightRotation(false)
   {
      mDRImpl = new DeadReckoningHelperImpl();
   }

   ////////////////////////////////////////////////////////////////////////////////
   DeadReckoningHelper::~DeadReckoningHelper()
   {
      delete mDRImpl;
   }

   DT_IMPLEMENT_ACCESSOR(DeadReckoningHelper, bool, AutoRegisterWithGMComponent);

   // GROUND CLAMP TYPE PROPERTY
   DT_IMPLEMENT_ACCESSOR_GETTER(DeadReckoningHelper, dtUtil::EnumerationPointer<GroundClampTypeEnum>, GroundClampType);


   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::OnEnteredWorld()
   {
      mExtraDataUpdated = false;

      if (mAutoRegisterWithGMComponent)
      {
         RegisterWithGMComponent();
      }
   }

   /// Called when the parent actor leaves the "world".
   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::OnRemovedFromWorld()
   {
      if (mAutoRegisterWithGMComponent)
      {
         UnregisterWithGMComponent();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::RegisterWithGMComponent()
   {
      dtGame::DeadReckoningComponent* drc = NULL;

      GameActor* ga = NULL;
      GetOwner(ga);
      GameActorProxy& act = ga->GetGameActorProxy();

      act.GetGameManager()->
         GetComponentByName(dtGame::DeadReckoningComponent::DEFAULT_NAME, drc);

      if (drc != NULL)
      {
         drc->RegisterActor(act, *this);
      }
      else
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Actor \"%s\"\"%s\" unable to find DeadReckoningComponent.",
            act.GetName().c_str(), act.GetId().ToString().c_str());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::UnregisterWithGMComponent()
   {
      dtGame::DeadReckoningComponent* drc = NULL;

      GameActor* ga = NULL;
      GetOwner(ga);
      GameActorProxy& act = ga->GetGameActorProxy();

      act.GetGameManager()->
         GetComponentByName(dtGame::DeadReckoningComponent::DEFAULT_NAME, drc);

      if (drc != NULL)
      {
         drc->UnregisterActor(act);
      }
      else
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Actor \"%s\"\"%s\" unable to find DeadReckoningComponent.",
            act.GetName().c_str(), act.GetId().ToString().c_str());
      }
   }

   //////////////////////////////////////////////////////////////////////
   DeadReckoningHelper::UpdateMode& DeadReckoningHelper::GetEffectiveUpdateMode(bool isRemote) const
   {
      if (*mUpdateMode == DeadReckoningHelper::UpdateMode::AUTO)
      {
         if (isRemote)
            return DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR;
         else
            return DeadReckoningHelper::UpdateMode::CALCULATE_ONLY;
      }
      return *mUpdateMode;
   }

   //////////////////////////////////////////////////////////////////////
   bool DeadReckoningHelper::IsExtraDataUpdated()
   {
      return mExtraDataUpdated;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetExtraDataUpdated(bool newValue)
   {
      mExtraDataUpdated = newValue;
   }

   //////////////////////////////////////////////////////////////////////
   bool DeadReckoningHelper::IsFlyingDeprecatedProperty()
   {
      bool result = (GroundClampTypeEnum::NONE == GetGroundClampType());
      return result;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetFlyingDeprecatedProperty(bool newFlying)
   {
      // THIS METHOD SUPPORTS THE DEPRECATED PROPERTY IN EXISTING MAPS. USE SetGroundClampType() instead.
      (newFlying) ?
         (SetGroundClampType(GroundClampTypeEnum::NONE)) :
         (SetGroundClampType(GroundClampTypeEnum::KEEP_ABOVE));
   }

   //////////////////////////////////////////////////////////////////////
   DEPRECATE_FUNC bool DeadReckoningHelper::IsFlying()
   {
      DEPRECATE("void DeadReckoningHelper::IsFlying()", 
         " GroundClampTypeEnum DeadReckoningHelper::GetGroundClampType()");
      return IsFlyingDeprecatedProperty();
   }

   //////////////////////////////////////////////////////////////////////
   DEPRECATE_FUNC void DeadReckoningHelper::SetFlying(bool newFlying)
   {
      DEPRECATE("void DeadReckoningHelper::SetFlying(bool)", 
         " bool DeadReckoningHelper::SetGroundClampType(GroundClampTypeEnum)");
      SetFlyingDeprecatedProperty(newFlying);
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetGroundClampType(GroundClampTypeEnum& newType)
   {
      if (newType == GetGroundClampType())
         return;

      mGroundClampingData.SetGroundClampType(newType);
      mGroundClampType = newType;
      mUpdated = true;
      mExtraDataUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetAdjustRotationToGround(bool newAdjust)
   {
      if (mGroundClampingData.GetAdjustRotationToGround() == newAdjust)
         return;
      mGroundClampingData.SetAdjustRotationToGround(newAdjust);
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetDeadReckoningAlgorithm(DeadReckoningAlgorithm& newAlgorithm)
   {
      if (mMinDRAlgorithm == &newAlgorithm)
         return;
      mMinDRAlgorithm = &newAlgorithm;
      mUpdated = true;

      mExtraDataUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastKnownTranslation(const osg::Vec3 &vec)
   {
      if (mTranslationInitiated)
      {
         mTransBeforeLastUpdate = mCurrentDeadReckonedTranslation;
      }
      else
      {
         mTransBeforeLastUpdate = vec;
      }

      mTranslationInitiated = true;
      mLastTranslation = vec;
      mTranslationElapsedTimeSinceUpdate = 0.0;
      mTranslationUpdated = true;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastKnownRotation(const osg::Vec3 &newRot)
   {
      // Some objects only care about heading, so we zero out pitch/yaw.
      osg::Vec3 vec = (!mForceUprightRotation) ? (newRot) : (osg::Vec3(newRot.x(), 0.0f, 0.0f));

      dtCore::Transform xform;
      xform.SetRotation(vec);
      xform.GetRotation(mLastRotationMatrix);

      if (mRotationInitiated)
      {
         mRotQuatBeforeLastUpdate = mCurrentDeadReckonedRotation;
      }
      else
      {
         mLastRotationMatrix.get(mRotQuatBeforeLastUpdate);
      }

      mLastRotation = vec;
      mLastRotationMatrix.get(mLastQuatRotation);
      mRotationElapsedTimeSinceUpdate = 0.0;
      mRotationInitiated = true;
      mRotationUpdated = true;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastKnownVelocity(const osg::Vec3 &vec)
   {
      //mVelocityBeforeLastUpdate = mLastVelocity;
      // Clamp the instant velocity to never be more than 4x the last velocity. 
      // This prevents a rare issue that occurs when toggling between 
      // different DR modes where the values are bogus and it shoots WAY off. 
      float lengthInstantVel = mDRImpl->mPreviousInstantVel.length();
      float lengthLastVelocity = mLastVelocity.length();
      if (lengthInstantVel > 0.01f && lengthInstantVel > 4.0f * lengthLastVelocity)
      {
         // Tamp the velocity down so it's not so extreme.
         mVelocityBeforeLastUpdate = mDRImpl->mPreviousInstantVel * 2.0f * (lengthLastVelocity / lengthInstantVel); 
      }
      else 
      {
         mVelocityBeforeLastUpdate = mDRImpl->mPreviousInstantVel; 
      }


      mLastVelocity = vec;
      mTranslationElapsedTimeSinceUpdate = 0.0;
      // If velocity is updated, the effect is the same as if the trans was updated
      mTranslationUpdated = true;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastKnownAcceleration(const osg::Vec3 &vec)
   {
      mAccelerationVector = vec;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastKnownAngularVelocity(const osg::Vec3 &vec)
   {
      mAngularVelocityVector = vec;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   const osg::Vec3& DeadReckoningHelper::GetCurrentInstantVelocity() const
   {
      return mDRImpl->mPreviousInstantVel;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetForceUprightRotation(bool newValue)
   {
      mForceUprightRotation = newValue;
   }

   //////////////////////////////////////////////////////////////////////
   bool DeadReckoningHelper::GetForceUprightRotation() const
   {
      return mForceUprightRotation;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetGroundOffset(float newOffset)
   {
      mGroundClampingData.SetGroundOffset(newOffset);
      mUpdated = true;

      mExtraDataUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastTranslationUpdatedTime(double newUpdatedTime)
   {
      //the average of the last average and the current time since an update.
      float timeDelta = float(newUpdatedTime - mLastTranslationUpdatedTime);
      mAverageTimeBetweenTranslationUpdates = 0.5f * timeDelta + 0.5f * mAverageTimeBetweenTranslationUpdates;
      mLastTranslationUpdatedTime = newUpdatedTime;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastRotationUpdatedTime(double newUpdatedTime)
   {
      //the average of the last average and the current time since an update.
      float timeDelta = float(newUpdatedTime - mLastRotationUpdatedTime);
      mAverageTimeBetweenRotationUpdates = 0.5f * timeDelta + 0.5f * mAverageTimeBetweenRotationUpdates;
      mLastRotationUpdatedTime = newUpdatedTime;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetTranslationElapsedTimeSinceUpdate(float value)
   { 
      /// Compute time delta for this step of DR. Should be the same as DeltaTime in the component
      mDRImpl->mCurTimeDelta = dtUtil::Max(value - mTranslationElapsedTimeSinceUpdate, 0.0f);

      mTranslationElapsedTimeSinceUpdate = value; 
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetModelDimensions(const osg::Vec3& newDimensions)
   {
      mGroundClampingData.SetModelDimensions(newDimensions);
      SetUseModelDimensions(true);
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::AddToDeadReckonDOF(const std::string &dofName,
      const osg::Vec3& position,
      const osg::Vec3& rateOverTime,
      const std::string& metricName)
   {
      dtCore::RefPtr<DeadReckoningDOF> newDOF = new DeadReckoningDOF();
      newDOF->mNext = NULL;
      newDOF->mPrev = NULL;
      newDOF->mName = dofName;
      newDOF->mMetricName = metricName;
      newDOF->mCurrentTime = 0;
      newDOF->mRateOverTime.set( rateOverTime );
      newDOF->mStartLocation.set( position );

      std::list<dtCore::RefPtr<DeadReckoningDOF> >::iterator iter;
      for (iter = mDeadReckonDOFS.begin(); iter != mDeadReckonDOFS.end(); ++iter)
      {
         // does the linking of the object to ADD, so we add the next pointer and validate it
         // no matter what the dead reckon dof gets pushed onto the list at the end
         // of the function.
         if((*iter)->mName == dofName)
         {
            DeadReckoningDOF* currentDOF = (*iter).get();

            while(currentDOF != NULL)
            {
               if(currentDOF->mNext == NULL)
               {
                  break;
               }
               currentDOF = currentDOF->mNext;
            }

            newDOF->mPrev = currentDOF;
            newDOF->mNext = NULL;

            currentDOF->mCurrentTime = 0;
            currentDOF->mNext = newDOF.get();
            currentDOF->mStartLocation = mDOFDeadReckoning->GetDOFTransform(currentDOF->mName)->getCurrentHPR();

            break;
         }
      }

      mDeadReckonDOFS.push_back(newDOF);
   }

   //////////////////////////////////////////////////////////////////////
   // Used for removing area by name.
   struct RemoveByName
   {
   public:
      RemoveByName(const std::string & name) : mName(name)
      {

      }

      bool operator()(dtCore::RefPtr<DeadReckoningHelper::DeadReckoningDOF>& toCheck)
      {
         return (mName == toCheck->mName);
      }

   private:
      const std::string& mName;
   };

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::RemoveAllDRDOFByName(const std::string& removeName)
   {
      mDeadReckonDOFS.remove_if(RemoveByName(removeName));
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::RemoveDRDOF(DeadReckoningDOF &obj)
   {
      std::list<dtCore::RefPtr<DeadReckoningDOF> >::iterator iterDOF;
      for (iterDOF = mDeadReckonDOFS.begin();iterDOF != mDeadReckonDOFS.end(); ++iterDOF)
      {
         // If the same pointer or the values match, then remove this object from the list.
         if ( &obj == *iterDOF
            ||((obj.mCurrentTime   == (*iterDOF)->mCurrentTime)
            && (obj.mName          == (*iterDOF)->mName)
            && (obj.mRateOverTime  == (*iterDOF)->mRateOverTime)
            && (obj.mStartLocation == (*iterDOF)->mStartLocation)))
         {
            RemoveDRDOF(iterDOF);
            return;
         }
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::RemoveDRDOF(std::list<dtCore::RefPtr<DeadReckoningDOF> >::iterator &iter)
   {
      // if theres more after it
      if ((*iter)->mNext != NULL)
      {
         // if its in the middle of two objects
         if ((*iter)->mPrev != NULL)
         {
            (*iter)->mPrev->mNext = (*iter)->mNext;
            (*iter)->mNext->mPrev = (*iter)->mPrev;
         }
         // else its in the front
         else
         {
            (*iter)->mNext->mPrev = NULL;
         }
      }
      // if there is more before it
      else if ((*iter)->mPrev != NULL)
      {
         // its last
        (*iter)->mPrev->mNext = NULL;
      }
      // none before or after the object, first one in list
      else
      {
         // dont need to link objects
      }
      mDeadReckonDOFS.erase(iter);
      return;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::OnAddedToActor(dtGame::GameActor& actor)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::OnRemovedFromActor(dtGame::GameActor& actor)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::GetPartialUpdateProperties(std::vector<dtUtil::RefString>& propNamesToFill)
   {
      propNamesToFill.reserve(propNamesToFill.size() + 5U);
      propNamesToFill.push_back(DeadReckoningHelper::PROPERTY_LAST_KNOWN_TRANSLATION);
      propNamesToFill.push_back(DeadReckoningHelper::PROPERTY_LAST_KNOWN_ROTATION);
      propNamesToFill.push_back(DeadReckoningHelper::PROPERTY_VELOCITY_VECTOR);
      propNamesToFill.push_back(DeadReckoningHelper::PROPERTY_ANGULAR_VELOCITY_VECTOR);
      propNamesToFill.push_back(DeadReckoningHelper::PROPERTY_ACCELERATION_VECTOR);
   }

   ////////////////////////////////////////////////////////////////////////////////
   // PROPERTY NAME DECLARATIONS
   ////////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString DeadReckoningHelper::PROPERTY_LAST_KNOWN_TRANSLATION("Last Known Translation");
   const dtUtil::RefString DeadReckoningHelper::PROPERTY_LAST_KNOWN_ROTATION("Last Known Rotation");
   const dtUtil::RefString DeadReckoningHelper::PROPERTY_VELOCITY_VECTOR("Velocity Vector");
   const dtUtil::RefString DeadReckoningHelper::PROPERTY_ACCELERATION_VECTOR("Acceleration Vector");
   const dtUtil::RefString DeadReckoningHelper::PROPERTY_ANGULAR_VELOCITY_VECTOR("Angular Velocity Vector");
   const dtUtil::RefString DeadReckoningHelper::PROPERTY_DEAD_RECKONING_ALGORITHM("Dead Reckoning Algorithm");
   //const dtUtil::RefString DeadReckoningHelper::PROPERTY_FLYING("Flying");
   const dtUtil::RefString DeadReckoningHelper::PROPERTY_GROUND_CLAMP_TYPE("GroundClampType");
   const dtUtil::RefString DeadReckoningHelper::PROPERTY_GROUND_OFFSET("Ground Offset");

   ////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::BuildPropertyMap()
   {
      dtGame::GameActor* actor;
      GetOwner(actor);

      static const dtUtil::RefString DEADRECKONING_GROUP = "Dead Reckoning";
      typedef dtDAL::PropertyRegHelper<DeadReckoningHelper&, DeadReckoningHelper> PropRegType;
      PropRegType propRegHelper(*this, this, DEADRECKONING_GROUP);//"Dead Reckoning");


      DT_REGISTER_PROPERTY_WITH_NAME(LastKnownTranslation, PROPERTY_LAST_KNOWN_TRANSLATION, 
         "Sets the last know position of this Entity", PropRegType, propRegHelper);

      // Last Known Rotation - See the header for GetInternalLastKnownRotationInXYZ() for why this is wierd.
      DT_REGISTER_PROPERTY_WITH_NAME(InternalLastKnownRotationInXYZ, PROPERTY_LAST_KNOWN_ROTATION, 
         "Sets the last known rotation of this Entity", PropRegType, propRegHelper);

      // Note - the member vars were changed to LastKnownXYZ, but the properties were left the same
      // so as to not break MANY maps in production.
      DT_REGISTER_PROPERTY_WITH_NAME(LastKnownVelocity, PROPERTY_VELOCITY_VECTOR, 
         "Sets the last known velocity vector of this Entity", PropRegType, propRegHelper);

      // Note - the member vars were changed to LastKnownXYZ, but the properties were left the same
      // so as to not break MANY maps in production.
      DT_REGISTER_PROPERTY_WITH_NAME(LastKnownAcceleration, PROPERTY_ACCELERATION_VECTOR, 
         "Sets the last known acceleration vector of this Entity", PropRegType, propRegHelper);

      // Note - the member vars were changed to LastKnownXYZ, but the properties were left the same
      // so as to not break MANY maps in production.
      DT_REGISTER_PROPERTY_WITH_NAME(LastKnownAngularVelocity, PROPERTY_ANGULAR_VELOCITY_VECTOR, 
         "Sets the last known angular velocity vector of this Entity", PropRegType, propRegHelper);
      DT_REGISTER_PROPERTY_WITH_NAME(DeadReckoningAlgorithm, PROPERTY_DEAD_RECKONING_ALGORITHM, 
         "Sets the enumerated dead reckoning algorithm to use.", PropRegType, propRegHelper);

      // IsFlying was changed to the enumeration below. The property is depracated and creatd on the fly.
      // Doesn't use macro cause the Getter is called IsFlying
      //AddProperty(new dtDAL::BooleanActorProperty(PROPERTY_FLYING, "Should Not Follow the Ground",
      //   dtDAL::BooleanActorProperty::SetFuncType(this, &DeadReckoningHelper::SetFlying),
      //   dtDAL::BooleanActorProperty::GetFuncType(this, &DeadReckoningHelper::IsFlying),
      //   "If flying is true, then it won't ground clamp. Also useful for hovering or jumping vehicles. ", DEADRECKONING_GROUP));

      // Doesn't use macro cause the Getter is called IsFlying
      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(GroundClampType, PROPERTY_GROUND_CLAMP_TYPE, "Ground Clamp Type",
         "What type of ground clamping should be performed. This replaced the Flying property.", 
         PropRegType, propRegHelper);


      DT_REGISTER_PROPERTY_WITH_NAME(GroundOffset, PROPERTY_GROUND_OFFSET, 
         "Sets the offset from the ground this entity should have.  This only matters if it is not flying.", PropRegType, propRegHelper);


      // The following 'properties' are usually set dynamically at runtime by computing the bounding dimensions
      // of the object. Making them real properties would be very misleading for people working on a map file. So, 
      // these were removed.  In the future, if someone strongly thinks these should be properties, it should be revisited 
      // by DG and CMM.
      /*
      AddProperty(new dtDAL::Vec3ActorProperty("Model Dimensions", "Actor Model Dimensions",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &DeadReckoningHelper::SetModelDimensions),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &DeadReckoningHelper::GetModelDimensions),
         "Sets the x,y,z dimensions of the model the actor loads.  This is used by the ground clamping code.", DEADRECKONING_GROUP));

      AddProperty(new dtDAL::BooleanActorProperty("Use Model Dimensions", "Use Model Dimensions",
         dtDAL::BooleanActorProperty::SetFuncType(this, &DeadReckoningHelper::SetUseModelDimensions),
         dtDAL::BooleanActorProperty::GetFuncType(this, &DeadReckoningHelper::UseModelDimensions),
         "Should the DR Component use the currently set model dimension values when ground clamping?", DEADRECKONING_GROUP));
         */

   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DeadReckoningHelper::DoDR(GameActor& gameActor, dtCore::Transform& xform,
         dtUtil::Log* pLogger, BaseGroundClamper::GroundClampRangeType*& gcType)
   {
      bool returnValue = false; // indicates we changed the transform
      if (GetGroundClampType() == GroundClampTypeEnum::NONE)
      {
         gcType = &BaseGroundClamper::GroundClampRangeType::NONE;
      }
      else if (GetGroundClampingData().GetAdjustRotationToGround())
      {
         gcType = &BaseGroundClamper::GroundClampRangeType::RANGED;
      }
      else
      {
         gcType = &BaseGroundClamper::GroundClampRangeType::INTERMITTENT_SAVE_OFFSET;
      }

      if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::NONE)
      {
         if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Dead Reckoning Algorithm set to NONE, "
                  "setting the transform to match the actor's current position.");
         }
         gameActor.GetTransform(xform, dtCore::Transformable::REL_CS);
         // It's set to NONE, so no ground clamping..
         gcType = &BaseGroundClamper::GroundClampRangeType::NONE;
      }
      else if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::STATIC)
      {
         if (IsUpdated())
         {
            DRStatic(gameActor, xform, pLogger);
            returnValue = true;
         }
      }
      else
      {
         returnValue = DRVelocityAcceleration(gameActor, xform, pLogger);
      }

      return returnValue;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::DRStatic(GameActor& gameActor, dtCore::Transform& xform, dtUtil::Log* pLogger)
   {
      if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Dead Reckoning actor as STATIC.  New position is " <<  mLastTranslation
            << ".  New Rotation is " << mLastRotation << ".";
         pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }

      xform.SetTranslation(mLastTranslation);
      xform.SetRotation(mLastRotationMatrix);

      mTransBeforeLastUpdate = mLastTranslation;
      mRotQuatBeforeLastUpdate = mLastQuatRotation;

      mRotationResolved = true;

      mCurrentDeadReckonedTranslation = mLastTranslation;
      xform.GetRotation(mCurrentAttitudeVector);
      mLastRotationMatrix.get(mCurrentDeadReckonedRotation);

      mDRImpl->mPreviousInstantVel.set(0.0f, 0.0f, 0.0f);
   }

   //////////////////////////////////////////////////////////////////////
   bool DeadReckoningHelper::DRVelocityAcceleration(GameActor& gameActor, dtCore::Transform& xform, dtUtil::Log* pLogger)
   {
      bool returnValue = false; // indicates that we made a change to the transform
      osg::Vec3 pos;
      xform.GetTranslation(pos);
      osg::Matrix rot;
      xform.GetRotation(rot);

      osg::Vec3 unclampedTranslation = pos;

      // removed because the transform is now initialized to the
      // the current DR'd position.
      //if (!mFlying)
      //   unclampedTranslation.z() = mLastTranslation.z();

      //avoid the sqrtf by using length2.
      //we went to see if all this dr and ground clamping stuff has to be done.
      if ( IsUpdated() ||
         mLastTranslation != unclampedTranslation ||
         !mRotationResolved ||
         mLastVelocity.length2() > 1e-2f ||
         (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION
            && mAccelerationVector.length2() > 1e-2f)||
         (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION
            && mAngularVelocityVector.length2() > 1e-5f))
      {
         if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            LogDeadReckonStarted(unclampedTranslation, rot, pLogger);

         // if we got an update, then we need to recalculate our smoothing
         if (IsUpdated())
         {
            CalculateSmoothingTimes(xform);

            // Note - call RecomputeTransSplineValues even if not using splines, because it computes mSplineEndLocation
            if (mUseCubicSplineTransBlend && GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION)
            {  // Use Accel
               mDRImpl->RecomputeTransSplineValues(mTransBeforeLastUpdate,mVelocityBeforeLastUpdate, 
                  mLastTranslation, mLastVelocity, mTranslationEndSmoothingTime, mAccelerationVector);
            }
            else if (mUseCubicSplineTransBlend) // No accel
            {
               osg::Vec3 zeroAccel;
               mDRImpl->RecomputeTransSplineValues(mTransBeforeLastUpdate,mVelocityBeforeLastUpdate, 
                  mLastTranslation, mLastVelocity, mTranslationEndSmoothingTime, zeroAccel);
            }

            if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               std::ostringstream ss;
               ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " got an update " << std::endl
                  << "      Rotation \"" << mLastRotation  << "\" " << std::endl
                  << "      Position \"" << mLastTranslation << "\" " << std::endl;
               pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
            }
         }

         // RESOLVE ROTATION
         DeadReckonTheRotation(xform);

         // POSITION SMOOTHING
         DeadReckonThePosition(pos, pLogger, gameActor);
         xform.SetTranslation(pos);

         returnValue = true;
      }
      else
      {
         mDRImpl->mPreviousInstantVel.set(0.0f, 0.0f, 0.0f);

         if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Entity does not need to be Dead Reckoned");
      }

      return returnValue;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::CalculateSmoothingTimes(const dtCore::Transform& xform)
   {
      // Dev Note - When the blend time changes drastically (ex 0.1 one time then 
      // 1.0 the next), it can inject significant issues when DR'ing - whether with 
      // catmull-rom/Bezier splines or linear. Recommended use case is for mAlwaysUseMaxSmoothingTime = true

      // ROTATION
      if (mUseFixedSmoothingTime)
      {
         mRotationEndSmoothingTime = GetFixedSmoothingTime();
      }
      else 
      {
         mRotationEndSmoothingTime = GetMaxRotationSmoothingTime();
         // Use our avg update time if it's smaller than our max
         if (GetMaxRotationSmoothingTime() > mAverageTimeBetweenRotationUpdates)
            mRotationEndSmoothingTime = mAverageTimeBetweenRotationUpdates;

         // Way-Out-Of-Bounds check For angular acceleration. 
         if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION &&
            (mAngularVelocityVector.length2() * (mRotationEndSmoothingTime * mRotationEndSmoothingTime)) <
            0.1 * ((mLastQuatRotation-mCurrentDeadReckonedRotation).length2()))
         {
            mRotationEndSmoothingTime = std::min(1.0f, mRotationEndSmoothingTime);
            //mRotationEndSmoothingTime = std::min(1.0f, mAverageTimeBetweenRotationUpdates);
         }
      }

      // TRANSLATION
      if (mUseFixedSmoothingTime)
      {
         mTranslationEndSmoothingTime = GetFixedSmoothingTime();
      }
      else 
      {
         mTranslationEndSmoothingTime = GetMaxTranslationSmoothingTime();
         // Use our avg update time if it's smaller than our max
         if (GetMaxTranslationSmoothingTime() > mAverageTimeBetweenTranslationUpdates)
         {
            mTranslationEndSmoothingTime = mAverageTimeBetweenTranslationUpdates;
         }

         osg::Vec3 pos;
         xform.GetTranslation(pos);

         //Order of magnitude check - if the entity could not possibly get to the new position
         // in max smoothing time based on the magnitude of it's velocity, then smooth quicker (ie 1 second).
         if (mLastVelocity.length2() * (mTranslationEndSmoothingTime*mTranslationEndSmoothingTime) < (mLastTranslation - pos).length2() )
         {
            mTranslationEndSmoothingTime = std::min(1.0f, mTranslationEndSmoothingTime);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::DeadReckonTheRotation(dtCore::Transform &xform)
   {
      osg::Quat newRot;
      osg::Quat drQuat = mLastQuatRotation; // velocity only just uses the last.
      bool isRotationChangedByAccel = false;
      osg::Quat startRotation = mRotQuatBeforeLastUpdate;

      if (!mRotationResolved) // mRotationResolved is never set when using Accel
      {

         // For vel and Accel, we use the angular velocity to compute a dead reckoning matrix to slerp to
         // assuming that we have an angular velocity at all...
         if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION &&
            mAngularVelocityVector.length2() > 1e-6)
         {
            // if we're here, we had some sort of change, however small
            isRotationChangedByAccel = true;

            // Compute The change in the rotation based Dead Reckoning matrix
            // The Dead Reckoning Matrix
            osg::Matrix angularRotation;
            float actualRotationTime = std::min(mRotationElapsedTimeSinceUpdate, mRotationEndSmoothingTime);

            osg::Vec3 angVelAxis(mAngularVelocityVector);
            float angVelMag = angVelAxis.normalize(); // returns the length
            // rotation around the axis is magnitude of ang vel * time.
            float rotationAngle = angVelMag * actualRotationTime;
            osg::Quat rotationFromAngVel(rotationAngle, angVelAxis);
            
            // Expected DR'ed rotation - Take the last rot and add the change over time
            drQuat = rotationFromAngVel * mLastQuatRotation; // The current DR'ed rotation
            // Previous DR'ed rotation - same, but uses where we were before the last update, so we can smooth it out...
            startRotation = rotationFromAngVel * mRotQuatBeforeLastUpdate; // The current DR'ed rotation
         }

         // If there is a difference in the rotations and we still have time to smooth, then
         // slerp between the two quats: 1) the old rotation plus the expected change using angular
         //    velocity and 2) the desired new rotation
         if ((mRotationEndSmoothingTime > 0.0f) && (mRotationElapsedTimeSinceUpdate <  mRotationEndSmoothingTime))
         {
            float smoothingFactor = mRotationElapsedTimeSinceUpdate/mRotationEndSmoothingTime;
            dtUtil::Clamp(smoothingFactor, 0.0f, 1.0f);
            newRot.slerp(smoothingFactor, startRotation, drQuat);
         }
         else // Either smoothing time is done or the current rotation equals the desired rotation
         {
            newRot = drQuat;
            mRotationResolved = !isRotationChangedByAccel; //true;
         }

         // we finished DR, so update the rotation values on the helper and transform
         xform.SetRotation(newRot);
         mCurrentDeadReckonedRotation = newRot;
         xform.GetRotation(mCurrentAttitudeVector);
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::DeadReckonThePosition( osg::Vec3& pos, dtUtil::Log* pLogger, GameActor &gameActor )
   {
      // Do we just need to project or do we need more smoothing?
      bool pastTheSmoothingTime = (mTranslationEndSmoothingTime <= 0.0f) || 
         (mTranslationElapsedTimeSinceUpdate >= mTranslationEndSmoothingTime);

      if (pastTheSmoothingTime) 
      {  
         // no need to blend, so just project forward using trivial equation for motion. 
         // Simple formula - P' = P + V*T + 1/2*A*T
         osg::Vec3 accelerationEffect;
         if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION)
         {
            accelerationEffect = (mAccelerationVector * 0.5f) * (mTranslationElapsedTimeSinceUpdate * mTranslationElapsedTimeSinceUpdate); 
         }
         pos = mLastTranslation + mLastVelocity * mTranslationElapsedTimeSinceUpdate + accelerationEffect;

      }
      // Still Smoothing - SPLINES
      else if (mUseCubicSplineTransBlend)
      {
         DeadReckonThePositionUsingSplines(pos, pLogger, gameActor);
      }
      // Still Smoothing - PROJECTIVE VELOCITY BLENDING
      else 
      {
         DeadReckonThePositionUsingLinearBlend(pos, pLogger, gameActor);
      }
       
      // Compute our instantaneous velocity for this frame == change in pos / time. Used when we get a new vel update.
      if (mDRImpl->mCurTimeDelta > 0.0f) // if delta <= 0 then just use prev values
      {
         osg::Vec3 instantVel = (pos - mCurrentDeadReckonedTranslation) / mDRImpl->mCurTimeDelta;
         mDRImpl->mPreviousInstantVel = instantVel;
      }

      mCurrentDeadReckonedTranslation = pos;

      // DEBUG STUFF
      if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " current pos "
            << "\"" << pos << "\", temp\"" << mLastTranslationUpdatedTime + mTranslationElapsedTimeSinceUpdate << "\"";
         pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::DeadReckonThePositionUsingSplines(osg::Vec3& pos, dtUtil::Log* pLogger, GameActor &gameActor)
   {
      //////////////////////////////////////////////////
      // With cubic splines, we work with 4 points and several pre-computed values
      // Then, we interpret along the cubic spline based on time T. 
      // See DRImpl->RecomputeTransSplineValues() for the actual algorithm
      //////////////////////////////////////////////////

      // The formula for X, Y, and Z is ... x = A*t^3 + B*t^2 + C*t + D.
      // Note - t is normalized between 0 and 1. 
      // Note - Acceleration is accounted for in RecomputeTransSplineValues()
      // Note - if mTranslationEndSmoothingTime changes often, this may cause anomalies
      // due to the velocity of the changes in T. Set mRotationEndSmoothingTime to improve.
      float timeT = mTranslationElapsedTimeSinceUpdate/(mTranslationEndSmoothingTime);
      float timeTT = timeT * timeT;
      float timeTTT = timeTT * timeT;

      // The following is for Bezier Cubic Splines
      float timeTInverse = (1.0f - timeT);
      float timeTTInverse = timeTInverse * timeTInverse;
      float timeTTTInverse = timeTInverse * timeTTInverse;
      pos.x() = timeTTTInverse * mDRImpl->mPosSplineXA + 3.0f * timeTTInverse * timeT * mDRImpl->mPosSplineXB + 
         3.0f * timeTInverse * timeTT * mDRImpl->mPosSplineXC + timeTTT * mDRImpl->mPosSplineXD;
      pos.y() = timeTTTInverse * mDRImpl->mPosSplineYA + 3.0f * timeTTInverse * timeT * mDRImpl->mPosSplineYB + 
         3.0f * timeTInverse * timeTT * mDRImpl->mPosSplineYC + timeTTT * mDRImpl->mPosSplineYD;
      pos.z() = timeTTTInverse * mDRImpl->mPosSplineZA + 3.0f * timeTTInverse * timeT * mDRImpl->mPosSplineZB + 
         3.0f * timeTInverse * timeTT * mDRImpl->mPosSplineZC + timeTTT * mDRImpl->mPosSplineZD;

      // The following is left for testing with Catmull-Rom Cubic Splines to DR the trans.
      /*
      pos.x() = mDRImpl->mPosSplineXA * timeTTT + mDRImpl->mPosSplineXD +
         mDRImpl->mPosSplineXB * timeTT + mDRImpl->mPosSplineXC * timeT;
      pos.y() = mDRImpl->mPosSplineYA * timeTTT + mDRImpl->mPosSplineYD +
         mDRImpl->mPosSplineYB * timeTT + mDRImpl->mPosSplineYC * timeT;
      pos.z() = mDRImpl->mPosSplineZA * timeTTT + mDRImpl->mPosSplineZD +
         mDRImpl->mPosSplineZB * timeTT + mDRImpl->mPosSplineZC * timeT;
      */

      if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Actor \"" << gameActor.GetUniqueId() << " - " << gameActor.GetName() << "\" has pos " << "\"" << pos << "\"";
         pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }

      // Note - at the end of the bezier cubic spline, it should match the mLastVelocity. 
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::DeadReckonThePositionUsingLinearBlend(osg::Vec3& pos, dtUtil::Log* pLogger, GameActor& gameActor)
   {
      float smoothingFactor = mTranslationElapsedTimeSinceUpdate/mTranslationEndSmoothingTime;

      // This DR is very simple. Use velocity/Accel and project forward from published loc and 
      // from our last actual DR pos, then blend between them over time. 

      // COMPUTE LAST KNOWN CHANGE - as if we just used the best known position & velocity.
      osg::Vec3 lastKnownPosChange = mLastTranslation + mLastVelocity * mTranslationElapsedTimeSinceUpdate; // Add Accel later. 

      // COMPUTE BLENDED VELOCITY - Lerp the two velocities and use that for movement. 
      // This majorly reduces the oscillations. 
      osg::Vec3 mBlendedVelocity = mVelocityBeforeLastUpdate + 
         (mLastVelocity - mVelocityBeforeLastUpdate) * smoothingFactor;
      osg::Vec3 velBlendedPos = mTransBeforeLastUpdate + mBlendedVelocity * mTranslationElapsedTimeSinceUpdate;// Add Accel later;

      // BLEND THE TWO - lerp between the last known and blended velocity 
      pos = velBlendedPos + (lastKnownPosChange - velBlendedPos) * smoothingFactor;
      // ADD ACCEL - do at end because it applies to both projectives anyway.
      if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION)
      {
         pos += ((mAccelerationVector * 0.5f) * (mTranslationElapsedTimeSinceUpdate * mTranslationElapsedTimeSinceUpdate));
      }

      if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Actor \"" << gameActor.GetUniqueId() << " - " << gameActor.GetName() << "\" has pos " << "\"" << pos << "\"";
         pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::LogDeadReckonStarted(osg::Vec3 &unclampedTranslation, osg::Matrix& rot, dtUtil::Log* pLogger)
   {
      std::ostringstream ss;
      ss << "Actor passed optimization checks: fully dead-reckoning actor.\n"
         << "  mLastTranslation:                     " << mLastTranslation << std::endl
         << "  unclampedTranslation:                 " << unclampedTranslation << std::endl
         << "  mLastVelocity.length2():     " << mLastVelocity.length2() << std::endl
         << "  mAccelerationVector.length2(): " << mAccelerationVector.length2() << std::endl
         << "  rot Matrix is: " << rot << std::endl
         << "  mLastRotationMatrix is: " << mLastRotationMatrix << std::endl;

      if (!mRotationResolved)
      {
         ss << "rot Matrix is: " << rot << std::endl;
         ss << "mLastRotationMatrix is: " << mLastRotationMatrix << std::endl;
      }

      pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetInternalLastKnownRotationInXYZ(const osg::Vec3 &vec)
   {
      SetLastKnownRotation(osg::Vec3(vec[2], vec[0], vec[1]));
   }

   ////////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 DeadReckoningHelper::GetInternalLastKnownRotationInXYZ() const
   {
      const osg::Vec3& result = GetLastKnownRotation();
      return osg::Vec3(result[1], result[2], result[0]);
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::ActorProperty> DeadReckoningHelper::GetDeprecatedProperty(const std::string& name)
   {
      static const dtUtil::RefString DEADRECKONING_GROUP = "Dead Reckoning";
      dtCore::RefPtr<dtDAL::ActorProperty> result;
      if (name == "Flying")
      {
         result = new dtDAL::BooleanActorProperty(name, "Should Not Follow the Ground",
            dtDAL::BooleanActorProperty::SetFuncType(this, &DeadReckoningHelper::SetFlyingDeprecatedProperty),
            dtDAL::BooleanActorProperty::GetFuncType(this, &DeadReckoningHelper::IsFlyingDeprecatedProperty),
            "DEPRECATED - USE GROUNDCLAMPTYPE INSTEAD! ", DEADRECKONING_GROUP);
      }
      return result;
   }


}
