/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright 2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * @author Curtiss Murphy
 */
#include <prefix/dtgameprefix.h>

#include <dtGame/drpublishingactcomp.h>

#include <dtUtil/mathdefines.h>
#include <dtCore/enginepropertytypes.h>
#include <dtCore/propertymacros.h>
#include <dtGame/basemessages.h>
#include <dtGame/gameactor.h>
#include <dtGame/invokable.h>
#include <dtGame/messagetype.h>
#include <dtGame/message.h>
#include <dtCore/transform.h>
#include <osg/Geode>
//#include <iostream>


namespace dtGame
{
   const float DRPublishingActComp::TIME_BETWEEN_UPDATES(10.0f);

   ////////////////////////////////////////////////////////////////////////////////
   DRPublishingActComp::DRPublishingActComp(bool requiresDRHelper/*=true*/)
      : ActorComponent(TYPE)
      , mVelocityAverageFrameCount(1)
      , mVelocityClampMagnitude(0.1f)
      , mMaxUpdateSendRate(5.0f)
      , mPublishLinearVelocity(true)
      , mPublishAngularVelocity(true)
      , mRequiresDRHelper(requiresDRHelper)
      , mTimeUntilNextFullUpdate(0.0f)
      , mSecsSinceLastUpdateSent(0.0f)
      , mVelocityMagThreshold(1.0f)
      , mVelocityDotThreshold(0.9f)
      , mForceUpdateNextChance(false)
      , mUseVelocityInDRUpdateDecision(false)
      , mMaxRotationError(1.0f) // 2.0
      , mMaxRotationError2(mMaxRotationError * mMaxRotationError)
      , mMaxTranslationError(0.02f)//(0.15f)
      , mMaxTranslationError2(mMaxTranslationError * mMaxTranslationError)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   DRPublishingActComp::~DRPublishingActComp()
   {
   }


   ////////////////////////////////////////////////////////////////////////////////
   // PROPERTY MACROS
   // These macros define the Getter and Setter method body for each property
   ////////////////////////////////////////////////////////////////////////////////

   DT_IMPLEMENT_ACCESSOR(DRPublishingActComp, dtCore::RefPtr<dtCore::VelocityInterface>, VelocitySource);
   DT_IMPLEMENT_ACCESSOR(DRPublishingActComp, dtCore::RefPtr<dtCore::AccelerationInterface>, AccelSource);
   DT_IMPLEMENT_ACCESSOR(DRPublishingActComp, dtCore::RefPtr<dtCore::AngularVelocityInterface>, AngVelSource);

   DT_IMPLEMENT_ACCESSOR(DRPublishingActComp, float, VelocityClampMagnitude);

   DT_IMPLEMENT_ACCESSOR(DRPublishingActComp, int, VelocityAverageFrameCount);

   DT_IMPLEMENT_ACCESSOR_GETTER(DRPublishingActComp, float, MaxUpdateSendRate); // Setter is implemented below

   DT_IMPLEMENT_ACCESSOR(DRPublishingActComp, bool, PublishLinearVelocity);

   DT_IMPLEMENT_ACCESSOR_GETTER(DRPublishingActComp, bool, PublishAngularVelocity);

   ////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetPublishAngularVelocity(bool publishAngularVelocity)
   {
      mPublishAngularVelocity = publishAngularVelocity;

      // If we are now off, then set our last known value to 0,0,0, in case it was previously set,
      // That way, we don't have to continue setting it to zero every time we publish.
      if (!mPublishAngularVelocity && IsDeadReckoningActorComponentValid())
      {
         osg::Vec3 zeroAngularVelocity;
         SetAngularVelocity(zeroAngularVelocity);
         GetDeadReckoningActorComponent()->SetLastKnownAngularVelocity(zeroAngularVelocity);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::OnAddedToActor(dtGame::GameActor& actor)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::OnRemovedFromActor(dtGame::GameActor& actor)
   {
   }


   ////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::UpdatePublishingData(const dtGame::TickMessage& tickMessage)
   {
      // Note - We do this behavior for local actors, but it happens during Tick Remote. 

      double elapsedTime = tickMessage.GetDeltaSimTime();
      bool forceUpdate = false;
      bool fullUpdate = false;

      // UPDATE OUR DR TIME
      mTimeUntilNextFullUpdate -= elapsedTime;
      mSecsSinceLastUpdateSent += elapsedTime; // We can only send out an update so many times a second.

      dtGame::GameActorProxy* actor = NULL;
      GetOwner(actor);

      dtCore::Transformable* tx = NULL;
      actor->GetDrawable(tx);

      dtCore::Transform xform;
      tx->GetTransform(xform);
      osg::Vec3 rot;
      xform.GetRotation(rot);
      osg::Vec3 pos;
      xform.GetTranslation(pos);

      // Have to update instant velocity even if we don't publish
      CalculateCurrentVelocity(elapsedTime, pos, rot);
      if (mAngVelSource.valid())
      {
         SetAngularVelocity(mAngVelSource->GetAngularVelocity());
      }

      if (mTimeUntilNextFullUpdate <= 0.0f)
      {
         fullUpdate = true;
         forceUpdate = true;
      }

      // If the extra settings on DR Helper changed (like flying), then we need a full update
      if (IsDeadReckoningActorComponentValid() && GetDeadReckoningActorComponent()->IsExtraDataUpdated())
      {
         forceUpdate = true;
         fullUpdate = true;

         GetDeadReckoningActorComponent()->SetExtraDataUpdated(false);
      }

      // Check for update
      if (!fullUpdate)
      {
         // The normal case. The one that makes the decision most of the time. 
         forceUpdate = ShouldForceUpdate(pos, rot);
      }

      if (forceUpdate)
      {
         mForceUpdateNextChance = false; // reset, since we are publishing

         // Previously, this was done at the start of the frame, before applying physics.
         // Because the behavior was pulled into the Actor Component, the behavior now happens 
         // after the owner actor is ticked. Theoretically, this should be OK, because the physics
         // forces do not apply until after this frame. However, if you are manually setting velocity
         // or position, it might behave differently than it did before.
         SetLastKnownValuesBeforePublish(pos, rot);

         // Since we are about to publish, set our time since last publish back to 0.
         // This allows us to immediately send out a change the exact moment it happens (ex if we
         // were moving slowly and hadn't updated recently).
         mSecsSinceLastUpdateSent = 0.0f;

         // If it is almost time to do a full update and our entity wants to do a partial update anyway, 
         // then go ahead and do a full update now. This prevents the heart beat from causing 
         // discontinuities in the update rate - mainly for vehicles that publish quickly and regularly
         // The logic should cause an update at between 9.5 - 10.5 seconds assuming a 10s heart beat
         if (!fullUpdate && mTimeUntilNextFullUpdate < TIME_BETWEEN_UPDATES * 0.1f)
         {
            fullUpdate = true;
         }

         if (fullUpdate)
         {
            // Reset our timer.
            ResetFullUpdateTimer(false);

            actor->NotifyFullActorUpdate();
         }
         else
         {
            actor->NotifyPartialActorUpdate();
         }
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::OnEnteredWorld()
   {
      dtGame::GameActorProxy* actor = NULL;
      GetOwner(actor);

      // LOCAL ACTOR -  do our setup
      if (!actor->IsRemote())
      {
         // Now we register for tick remote, so that we guarantee it happens AFTER our actor 
         // It also needs to run AFTER the DeadReckoningComponent
         mTickInvokable = actor->RegisterForMessages(dtGame::MessageType::TICK_REMOTE, dtUtil::MakeFunctor(&DRPublishingActComp::UpdatePublishingData, this));


         ResetFullUpdateTimer(true);
      }

      // Lookup the DR Helper from our actor. If we need it, and don't have it, then report an error one time.
      // This flag allows developers to use the DRPublishing component to JUST do heartbeats, without actual Dead reckoning
      if (mRequiresDRHelper && !IsDeadReckoningActorComponentValid())
      {
         dtGame::DeadReckoningActorComponent* deadReckoningHelper = NULL;
         actor->GetComponent(deadReckoningHelper);
         mDeadReckoningActorComponent = deadReckoningHelper;
         if (!mDeadReckoningActorComponent.valid())
         {
            std::string error = std::string("Actor \"") + actor->GetName() + "\" is setup to use a Dead Reckoning Helper in the DRPublishingActComp but doesn't have one. If you want one, add it before adding this component. If you don't want one, pass false to the constructor.";
            LOGN_ERROR("DRPublishingActComp.cpp", error);
         }
      }

      // Now that we have a dead reckoning helper, we need to call our set update send rate 
      // method because it initializes the DR Helper smoothing times. 
      SetMaxUpdateSendRate(GetMaxUpdateSendRate());

   }

   ////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::OnRemovedFromWorld()
   {
      dtGame::GameActorProxy* actor = NULL;
      GetOwner(actor);

      // LOCAL ACTOR - cleanup
      if (mTickInvokable.valid())
      {
         actor->UnregisterForMessages(dtGame::MessageType::TICK_REMOTE, mTickInvokable->GetName());
      }

      mDeadReckoningActorComponent = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::BuildPropertyMap()
   {
      //static const dtUtil::RefString GROUPNAME = "DR Publishing";

      typedef dtCore::PropertyRegHelper<DRPublishingActComp> PropRegType;
      PropRegType propRegHelper(*this, this, "DR Publishing");

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(VelocityAverageFrameCount, "VelocityAveragingFrameCount", "Velocity Averaging Frame Count",
         "This actor computes it's current velocity by averaging the change in position over the given number of frames.", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(MaxUpdateSendRate, "DesiredNumUpdatesPerSec", "Desired Number of Updates Per Second",
         "The desired number of updates per second - the actual frequently will be less if the motion of the vehicle doesn't deviate from the dead-reckoned values.", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(MaxTranslationError, "MaxTranslationError", "Maximum distance of allowed translation error",
         "Maximum distance of translation error between the actual and dead-reckoned prediction before an update will be sent.", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(MaxRotationError, "MaxRotationError", "Maximum allowed rotation error in degrees",
         "Maximum amount of rotation error in degrees between the actual and dead-reckoned prediction before an update will be sent.", PropRegType, propRegHelper);
   }

   //////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::ForceUpdateAtNextOpportunity()
   {
      mForceUpdateNextChance = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::ForceFullUpdateAtNextOpportunity()
   {
      mTimeUntilNextFullUpdate = 0.0f;
   }

   //////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetSecsSinceLastUpdateSent(float secsSinceLastUpdateSent)
   {
      mSecsSinceLastUpdateSent = secsSinceLastUpdateSent;
   }

   //////////////////////////////////////////////////////////////////////
   float DRPublishingActComp::GetSecsSinceLastUpdateSent() const
   {
      return mSecsSinceLastUpdateSent;
   }

   //////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetMaxUpdateSendRate(float maxUpdateSendRate)
   {
      mMaxUpdateSendRate = maxUpdateSendRate;

      // The DR helper should be kept in the loop about the max send rate. 
      if (IsDeadReckoningActorComponentValid() && maxUpdateSendRate > 0.0f)
      {
         // Note - AlwaysUseFixedSmoothingTime is controlled via the BaseEntity and a config option
         float updateRate = dtUtil::Max(0.01f, dtUtil::Min(1.0f, 1.00f/maxUpdateSendRate));
         GetDeadReckoningActorComponent()->SetFixedSmoothingTime(updateRate);
      }

   }

   //////////////////////////////////////////////////////////////////////
   float DRPublishingActComp::GetVelocityMagnitudeUpdateThreshold() const
   {
      return mVelocityMagThreshold;
   }

   //////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetVelocityDotProductUpdateThreshold(float thresh)
   {
      mVelocityDotThreshold = thresh;
   }

   //////////////////////////////////////////////////////////////////////
   float DRPublishingActComp::GetVelocityDotProductUpdateThreshold() const
   {
      return mVelocityMagThreshold;
   }

   //////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetUseVelocityInDRUpdateDecision(bool value)
   {
      mUseVelocityInDRUpdateDecision = value;
   }

   //////////////////////////////////////////////////////////////////////
   bool DRPublishingActComp::GetUseVelocityInDRUpdateDecision() const
   {
      return mUseVelocityInDRUpdateDecision;
   }

   //////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetVelocity(const osg::Vec3& vec)
   { 
      mVelocity = vec;
   }

   //////////////////////////////////////////////////////////////////////
   osg::Vec3 DRPublishingActComp::GetVelocity() const
   { 
      return mVelocity;
   }

   //////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetAcceleration(const osg::Vec3& vec)
   { 
      mAcceleration = vec; 
   }

   //////////////////////////////////////////////////////////////////////
   osg::Vec3 DRPublishingActComp::GetAcceleration() const
   { 
      return mAcceleration; 
   }

   //////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetAngularVelocity(const osg::Vec3& vec)
   { 
      mAngularVelocity = vec; 
   }

   //////////////////////////////////////////////////////////////////////
   osg::Vec3 DRPublishingActComp::GetAngularVelocity() const
   { 
      return mAngularVelocity; 
   }

   //////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetMaxTranslationError(float distance)
   {
      mMaxTranslationError = distance;
      mMaxTranslationError2 = distance * distance;
   }

   //////////////////////////////////////////////////////////////////////
   float DRPublishingActComp::GetMaxTranslationError() const 
   { 
      return mMaxTranslationError; 
   }

   //////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetMaxRotationError(float rotation)
   {
      mMaxRotationError = rotation;
      mMaxRotationError2 = rotation * rotation;
   }

   //////////////////////////////////////////////////////////////////////
   float DRPublishingActComp::GetMaxRotationError() const 
   { 
      return mMaxRotationError; 
   }

   //////////////////////////////////////////////////////////////////////
   float DRPublishingActComp::GetTimeUntilNextFullUpdate() const 
   { 
      return mTimeUntilNextFullUpdate; 
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::ResetFullUpdateTimer(bool doBigRandomOffset)
   {
      mTimeUntilNextFullUpdate = TIME_BETWEEN_UPDATES;

      if (doBigRandomOffset)
      {
         // We publish full updates periodically, but we want to randomize their start point, 
         // so all actors loaded in a map don't do full updates on the same frame. 
         float halfUpdateTime = 0.5f * TIME_BETWEEN_UPDATES;
         mTimeUntilNextFullUpdate += dtUtil::RandFloat(-halfUpdateTime, halfUpdateTime);
      }
      else
      {
         // Added a slight randomize in all cases to help spread them out.
         float smallRandomness = 0.1f * TIME_BETWEEN_UPDATES;
         mTimeUntilNextFullUpdate += (dtUtil::RandFloat(0.0f, smallRandomness)) ;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetAccumulatedAcceleration(const osg::Vec3 &newValue)
   { 
      mAccumulatedAcceleration = newValue; 
   }

   ///////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 DRPublishingActComp::GetAccumulatedAcceleration() const
   { 
      return mAccumulatedAcceleration; 
   }

   ///////////////////////////////////////////////////////////////////////////////////
   dtGame::DeadReckoningActorComponent* DRPublishingActComp::GetDeadReckoningActorComponent()
   { 
      return mDeadReckoningActorComponent.get();
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetDeadReckoningActorComponent(dtGame::DeadReckoningActorComponent* drHelper) 
   { 
      mDeadReckoningActorComponent = drHelper; 
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool DRPublishingActComp::IsDeadReckoningActorComponentValid() const
   { 
      return (mDeadReckoningActorComponent.valid()); 
   }


   ///////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::SetLastKnownValuesBeforePublish(const osg::Vec3& pos, const osg::Vec3& rot)
   {
      // We can't do this without a helper. Reported as an error in OnEnteredWorld().
      if (!IsDeadReckoningActorComponentValid())
      {
         return; 
      }

      GetDeadReckoningActorComponent()->SetLastKnownTranslation(pos);
      GetDeadReckoningActorComponent()->SetLastKnownRotation(rot);


      // Linear Velocity & acceleration - push the current value to the Last Known
      if (mPublishLinearVelocity)
      {
         // VELOCITY 
         osg::Vec3 velocity = GetVelocity();
         GetDeadReckoningActorComponent()->SetLastKnownVelocity(velocity);


         // ACCELERATION

         /// DAMPEN THE ACCELERATION TO PREVENT WILD SWINGS WITH DEAD RECKONING
         // Dampen the acceleration before publication if the vehicle is making drastic changes 
         // in direction. With drastic changes, the acceleration will cause the Dead Reckoned 
         // pos to oscillate wildly. Whereas it will improve DR on smooth curves such as a circle.
         // The math is: take the current accel and the non-scaled accel from the last publish;
         // normalize them; dot them and use the product to scale our current Acceleration. 
         osg::Vec3 curAccel = GetAcceleration();
         curAccel.normalize();
         float accelDotProduct = curAccel * mAccelerationCalculatedForLastPublish; // dot product
         SetAcceleration(GetAcceleration() * dtUtil::Max(0.0f, accelDotProduct));
         mAccelerationCalculatedForLastPublish = curAccel; // Hold for next time (pre-normalized)

         // Acceleration is paired with velocity
         GetDeadReckoningActorComponent()->SetLastKnownAcceleration(GetAcceleration());
      }

      // Angular Velocity - push the current value to the Last Known
      if (mPublishAngularVelocity)
      {
         osg::Vec3 angularVelocity = GetAngularVelocity();
         if (angularVelocity.length2() < 0.00001)  // If close to 0, set to 0 to prevent wiggling/shaking
         {
            angularVelocity = osg::Vec3(0.0f, 0.0f, 0.0f);
         }
         GetDeadReckoningActorComponent()->SetLastKnownAngularVelocity(angularVelocity);
      }

   }

   ///////////////////////////////////////////////////////////////////////////////////
   void DRPublishingActComp::CalculateCurrentVelocity(float deltaTime, const osg::Vec3& pos, const osg::Vec3& rot)
   {
      if (deltaTime <= FLT_EPSILON)
      {
         return;
      }

      // We can't do this without a helper. Reported as an error in OnEnteredWorld().
      if (!IsDeadReckoningActorComponentValid())
      {
         return; 
      }

      if (mPublishLinearVelocity) // If not publishing, then don't do anything.
      {
         osg::Vec3 previousAccumulatedLinearVelocity = mAccumulatedLinearVelocity;

         if (mVelocitySource.valid())
         {
            mAccumulatedLinearVelocity = mVelocitySource->GetVelocity();
         }
         else
         {
            // Note - we used to grab the velocity from the physics engines, but there were sometimes
            // discontinuities reported by the various engines, so that was removed in favor of a simple
            // differential of position.
            osg::Vec3 distanceMoved = pos - mLastPos;
            osg::Vec3 instantVelocity = distanceMoved / deltaTime;

            // Compute Vel - either the instant Vel or a blended value over a couple of frames. Blended Velocities tend to make acceleration less useful
            if (mVelocityAverageFrameCount <= 1)
            {
               mAccumulatedLinearVelocity = instantVelocity;
            }
            else
            {
               float instantVelWeight = 1.0f / float(mVelocityAverageFrameCount);
               mAccumulatedLinearVelocity = instantVelocity * instantVelWeight + mAccumulatedLinearVelocity * (1.0f - instantVelWeight);
            }

            if (mVelocityClampMagnitude > FLT_EPSILON)
            {
               // Clamping to provide some better stability at low speeds.
               float velMagnitude2 = mAccumulatedLinearVelocity.length2();
               if (velMagnitude2 < mVelocityClampMagnitude * mVelocityClampMagnitude)
               {
                  mAccumulatedLinearVelocity = osg::Vec3(0.0f, 0.0f, 0.0f);
               }
            }
         }

         if (mAccelSource.valid())
         {
            mAccumulatedAcceleration = mAccelSource->GetAcceleration();
         }
         else
         {
            // Compute our acceleration as the instantaneous differential of the velocity
            // Acceleration is dampened before publication - see SetLastKnownValuesBeforePublish().
            osg::Vec3 changeInVelocity = mAccumulatedLinearVelocity - previousAccumulatedLinearVelocity; /*instantVelocity - mAccumulatedLinearVelocity*/;
            mAccumulatedAcceleration = changeInVelocity / deltaTime;
         }

         // Many vehicles get a slight jitter up/down while running. If you allow the z acceleration to
         // be published, the vehicle will go all over the place nutty. So, we zero it out.
         // This is not an ideal solution, but is workable because vehicles that really do have a lot of
         // z acceleration are probably flying and by definition are not as close to other objects so the z accel
         // is less visually apparent.
         //mAccumulatedAcceleration.z() = 0.0f;  // Removed because we use this for more than vehicles.

         SetVelocity(mAccumulatedLinearVelocity);
         SetAcceleration(mAccumulatedAcceleration);

         mLastPos = pos; 
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool DRPublishingActComp::ShouldForceUpdate(const osg::Vec3& pos, const osg::Vec3& rot)
   {
      // We can't do this without a helper. Reported as an error in OnEnteredWorld().
      if (!IsDeadReckoningActorComponentValid())
      {
         return false; 
      }

      bool forceUpdateResult = false; // if full update set, we assume we will publish
      bool enoughTimeHasPassed = (mMaxUpdateSendRate > 0.0f &&
         (mSecsSinceLastUpdateSent >= 1.0f / mMaxUpdateSendRate));

      if (enoughTimeHasPassed)
      {
         // The actor requested an update once time allows
         if (mForceUpdateNextChance)
         {
            forceUpdateResult = true;
            //mForceUpdateNextChance = false; reset happens in TickLocal to account for Full Updates.
         }
         
         // If no DR is occuring, then we don't want to check.
         else if (GetDeadReckoningActorComponent()->GetDeadReckoningAlgorithm() != dtGame::DeadReckoningAlgorithm::NONE)
         {
            // check to see if it's moved or turned enough to warrant one.
            osg::Vec3 distanceMoved = pos - GetDeadReckoningActorComponent()->GetCurrentDeadReckonedTranslation();
            osg::Vec3 distanceTurned = rot - GetDeadReckoningActorComponent()->GetCurrentDeadReckonedRotation();
            if (distanceMoved.length2() > mMaxTranslationError2 || distanceTurned.length2() > mMaxRotationError2)
            {
               // Note the rotation check isn't perfect (ie, not a quaternion), so you might get
               // an extra update, but it's very close and is very cheap processor wise.
               forceUpdateResult = true;
            }
            // We passed pos/rot check, now check velocity
            else if (GetUseVelocityInDRUpdateDecision())
            {
               osg::Vec3 oldVel = GetDeadReckoningActorComponent()->GetLastKnownVelocity();
               osg::Vec3 curVel = GetVelocity();
               float oldMag = oldVel.normalize();
               float curMag = curVel.normalize();
               float velMagChange = dtUtil::Abs(curMag - oldMag);
               if (velMagChange > mVelocityMagThreshold)
               {
                  forceUpdateResult = true;
                  LOGN_DEBUG("DRPublishingActComp.cpp", "Forcing update based on velocity magnitude.")
               }
               else
               {
                  float dotProd = oldVel * curVel;
                  if (dotProd < mVelocityDotThreshold)
                  {
                     forceUpdateResult = true;
                     LOGN_DEBUG("DRPublishingActComp.cpp", "Forcing update based on velocity angle.")
                  }
               }
            }
         }
      }

      return forceUpdateResult;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   float DRPublishingActComp::GetPercentageChangeDifference(float startValue, float newValue) const
   {
      if(std::abs(startValue) < 0.01f && std::abs(newValue) < 0.01f)
      {
         return 1.0;
      }

      if(startValue == 0)
      {
         startValue = 1.0f;
      }

      return std::abs((((newValue - startValue) / startValue) * 100.0f));
   }

} // namespace dtGame
