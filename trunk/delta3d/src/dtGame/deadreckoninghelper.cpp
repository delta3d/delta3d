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
#include <prefix/dtgameprefix-src.h>
#include <dtGame/deadreckoninghelper.h>
#include <dtGame/deadreckoningcomponent.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/log.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/mathdefines.h>
#include <dtGame/gameactor.h>
#include <dtDAL/functor.h>

#include <osgSim/DOFTransform>

namespace dtGame
{
   IMPLEMENT_ENUM(DeadReckoningAlgorithm);
   DeadReckoningAlgorithm DeadReckoningAlgorithm::NONE("None");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::STATIC("Static");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::VELOCITY_ONLY("Velocity Only");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION("Velocity and Acceleration");

   const float DeadReckoningHelper::DEFAULT_MAX_SMOOTHING_TIME_ROT = 2.0f;
   const float DeadReckoningHelper::DEFAULT_MAX_SMOOTHING_TIME_POS = 8.0f;

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
   DeadReckoningHelper::DeadReckoningHelper() :
      mLastTimeTag(0.0),
      mLastTranslationUpdatedTime(0.0),
      mLastRotationUpdatedTime(0.0),
      mAverageTimeBetweenTranslationUpdates(0.0f),
      mAverageTimeBetweenRotationUpdates(0.0f),
      mMaxTranslationSmoothingTime(DEFAULT_MAX_SMOOTHING_TIME_POS),
      mMaxRotationSmoothingTime(DEFAULT_MAX_SMOOTHING_TIME_ROT),
      mTranslationElapsedTimeSinceUpdate(0.0f),
      mRotationElapsedTimeSinceUpdate(0.0f),
      mTranslationEndSmoothingTime(0.0f),
      mRotationEndSmoothingTime(0.0f),
      mMinDRAlgorithm(&DeadReckoningAlgorithm::NONE),
      mUpdateMode(&DeadReckoningHelper::UpdateMode::AUTO),
      mTranslationInitiated(false),
      mRotationInitiated(false),
      mUpdated(false),
      mTranslationUpdated(false),
      mRotationUpdated(false),
      mFlying(false),
      mRotationResolved(true)
   {}


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
   void DeadReckoningHelper::SetFlying(bool newFlying)
   {
      if (mFlying == newFlying)
         return;
      mFlying = newFlying;
      mUpdated = true;
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
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastKnownTranslation(const osg::Vec3 &vec)
   {
      // If multiple updates are received before the DR component
      // is able to process them, i.e. in the same frame, it can result in a jitter/warp
      // this if fixes the jitter.
      if (!mTranslationUpdated)
      {
         if (mTranslationInitiated)
         {
            mTransBeforeLastUpdate = mCurrentDeadReckonedTranslation;
         }
         else
         {
            mTransBeforeLastUpdate = vec;
         }
      }

      mTranslationInitiated = true;
      mLastTranslation = vec;
      mTranslationElapsedTimeSinceUpdate = 0.0;
      mTranslationUpdated = true;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastKnownRotation(const osg::Vec3 &vec)
   {
      dtCore::Transform xform;
      xform.SetRotation(vec);
      xform.GetRotation(mLastRotationMatrix);
      // If multiple updates are received before the DR component
      // is able to process them, i.e. in the same frame, it can result in a jitter/warp
      // this if fixes the jitter.
      if (!mRotationUpdated)
      {
         if (mRotationInitiated)
         {
            mRotQuatBeforeLastUpdate = mCurrentDeadReckonedRotation;
         }
         else
         {
            mLastRotationMatrix.get(mRotQuatBeforeLastUpdate);
         }
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
      // This technically suffers from the same jitter problem as the rotation and translation, but
      // because the velocity takes time to change anyway, and because if updates are coming in frequently
      // velocity blending won't do much, there is no need to fix it.
      mVelocityBeforeLastUpdate = mLastVelocity;
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
   void DeadReckoningHelper::ComputeRotationChangeWithAngularVelocity(double deltaTime, osg::Matrix& result)
   {
      //mComputedAngularRotationMatrix
      if (mAngularVelocityVector.length2() < 1e-6)
      {
         result.makeIdentity();
      }
      else
      {
         double w = sqrt (mAngularVelocityVector[0]*mAngularVelocityVector[0]+
                       mAngularVelocityVector[1]*mAngularVelocityVector[1]+
                       mAngularVelocityVector[2]*mAngularVelocityVector[2]);

         double omega00 = 0;
         double omega01 = -mAngularVelocityVector[2];
         double omega02 = mAngularVelocityVector[1];
         double omega03 = 0;

         double omega10 = mAngularVelocityVector[2];
         double omega11 = 0;
         double omega12 = -mAngularVelocityVector[0];
         double omega13 = 0;

         double omega20 = -mAngularVelocityVector[1];
         double omega21 = mAngularVelocityVector[0];
         double omega22 = 0;
         double omega23 = 0;

         double ww00 = mAngularVelocityVector[0]*mAngularVelocityVector[0];
         double ww01 = mAngularVelocityVector[0]*mAngularVelocityVector[1];
         double ww02 = mAngularVelocityVector[0]*mAngularVelocityVector[2];
         double ww03 = 0;

         double ww10 = mAngularVelocityVector[1]*mAngularVelocityVector[0];
         double ww11 = mAngularVelocityVector[1]*mAngularVelocityVector[1];
         double ww12 = mAngularVelocityVector[1]*mAngularVelocityVector[2];
         double ww13 = 0;

         double ww20 = mAngularVelocityVector[2]*mAngularVelocityVector[0];
         double ww21 = mAngularVelocityVector[2]*mAngularVelocityVector[1];
         double ww22 = mAngularVelocityVector[2]*mAngularVelocityVector[2];
         double ww23 = 0;

         double c1 = (1-cos(w*deltaTime))/(w*w);
         double c2 = cos(w*deltaTime);
         double c3 = -sin(w*deltaTime)/w;

         result.set(
         c1*ww00+c2*1+c3*omega00, c1*ww01+c2*0+c3*omega01, c1*ww02+c2*0+c3*omega02,c1*ww03+c2*0+c3*omega03,
         c1*ww10+c2*0+c3*omega10, c1*ww11+c2*1+c3*omega11, c1*ww12+c2*0+c3*omega12,c1*ww13+c2*0+c3*omega13,
         c1*ww20+c2*0+c3*omega20, c1*ww21+c2*0+c3*omega21, c1*ww22+c2*1+c3*omega22,c1*ww23+c2*0+c3*omega23,
         0, 0, 0,1
         );
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetGroundOffset(float newOffset)
   {
      mGroundClampingData.SetGroundOffset(newOffset);
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastTranslationUpdatedTime(double newUpdatedTime)
   {
      //the average of the last average and the current time since an update.
      mAverageTimeBetweenTranslationUpdates = 0.5f * (float(newUpdatedTime - mLastTranslationUpdatedTime) + mAverageTimeBetweenTranslationUpdates);
      mLastTranslationUpdatedTime = newUpdatedTime;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastRotationUpdatedTime(double newUpdatedTime)
   {
      //the average of the last average and the current time since an update.
      mAverageTimeBetweenRotationUpdates = 0.5f * (float(newUpdatedTime - mLastRotationUpdatedTime) + mAverageTimeBetweenRotationUpdates);
      mLastRotationUpdatedTime = newUpdatedTime;
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

   /////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::GetActorProperties(std::vector<dtCore::RefPtr<dtDAL::ActorProperty> >& pFillVector)
   {
      pFillVector.push_back(new dtDAL::Vec3ActorProperty("Last Known Translation", "Last Known Translation",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &DeadReckoningHelper::SetLastKnownTranslation),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &DeadReckoningHelper::GetLastKnownTranslation),
         "Sets the last know position of this Entity", "Dead Reckoning"));

      pFillVector.push_back(new dtDAL::Vec3ActorProperty("Last Known Rotation", "Last Known Rotation",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &DeadReckoningHelper::SetLastKnownRotation),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &DeadReckoningHelper::GetLastKnownRotation),
         "Sets the last known rotation of this Entity","Dead Reckoning"));

      // Note - the member vars were changed to LastKnownXYZ, but the properties were left the same
      // so as to not break MANY maps in production.
      pFillVector.push_back(new dtDAL::Vec3ActorProperty("Velocity Vector", "Velocity Vector",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &DeadReckoningHelper::SetLastKnownVelocity),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &DeadReckoningHelper::GetLastKnownVelocity),
         "Sets the last known velocity vector of this Entity", "Dead Reckoning"));

      // Note - the member vars were changed to LastKnownXYZ, but the properties were left the same
      // so as to not break MANY maps in production.
      pFillVector.push_back(new dtDAL::Vec3ActorProperty("Acceleration Vector", "Acceleration Vector",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &DeadReckoningHelper::SetLastKnownAcceleration),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &DeadReckoningHelper::GetLastKnownAcceleration),
         "Sets the last known acceleration vector of this Entity", "Dead Reckoning"));

      // Note - the member vars were changed to LastKnownXYZ, but the properties were left the same
      // so as to not break MANY maps in production.
      pFillVector.push_back(new dtDAL::Vec3ActorProperty("Angular Velocity Vector", "Angular Velocity Vector",
         dtDAL::Vec3ActorProperty::SetFuncType(this, &DeadReckoningHelper::SetLastKnownAngularVelocity),
         dtDAL::Vec3ActorProperty::GetFuncType(this, &DeadReckoningHelper::GetLastKnownAngularVelocity),
         "Sets the last known angular velocity vector of this Entity", "Dead Reckoning"));

      pFillVector.push_back(new dtDAL::EnumActorProperty<dtGame::DeadReckoningAlgorithm>("Dead Reckoning Algorithm", "Dead Reckoning Algorithm",
         dtDAL::EnumActorProperty<dtGame::DeadReckoningAlgorithm>::SetFuncType(this, &DeadReckoningHelper::SetDeadReckoningAlgorithm),
         dtDAL::EnumActorProperty<dtGame::DeadReckoningAlgorithm>::GetFuncType(this, &DeadReckoningHelper::GetDeadReckoningAlgorithm),
         "Sets the enumerated dead reckoning algorithm to use.", "Dead Reckoning"));

      pFillVector.push_back(new dtDAL::BooleanActorProperty("Flying", "Should Not Follow the Ground",
         dtDAL::BooleanActorProperty::SetFuncType(this, &DeadReckoningHelper::SetFlying),
         dtDAL::BooleanActorProperty::GetFuncType(this, &DeadReckoningHelper::IsFlying),
         "Flags if the dead-reckoning code should not make this actor follow the ground as it moves.", "Dead Reckoning"));

      pFillVector.push_back(new dtDAL::FloatActorProperty("Ground Offset", "Ground Offset",
         dtDAL::FloatActorProperty::SetFuncType(this, &DeadReckoningHelper::SetGroundOffset),
         dtDAL::FloatActorProperty::GetFuncType(this, &DeadReckoningHelper::GetGroundOffset),
         "Sets the offset from the ground this entity should have.  This only matters if it is not flying.", "Dead Reckoning"));

      pFillVector.push_back(new dtDAL::Vec3ActorProperty("Model Dimensions", "Actor Model Dimensions",
            dtDAL::Vec3ActorProperty::SetFuncType(this, &DeadReckoningHelper::SetModelDimensions),
            dtDAL::Vec3ActorProperty::GetFuncType(this, &DeadReckoningHelper::GetModelDimensions),
            "Sets the x,y,z dimensions of the model the actor loads.  This is used by the ground clamping code.", "Dead Reckoning"));

      pFillVector.push_back(new dtDAL::BooleanActorProperty("Use Model Dimensions", "Use Model Dimensions",
            dtDAL::BooleanActorProperty::SetFuncType(this, &DeadReckoningHelper::SetUseModelDimensions),
            dtDAL::BooleanActorProperty::GetFuncType(this, &DeadReckoningHelper::UseModelDimensions),
            "Should the DR Component use the currently set model dimension values when ground clamping?", "Dead Reckoning"));
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DeadReckoningHelper::DoDR(GameActor& gameActor, dtCore::Transform& xform,
         dtUtil::Log* pLogger, BaseGroundClamper::GroundClampingType*& gcType)
   {
      bool returnValue = false; // indicates we changed the transform
      if (IsFlying())
         gcType = &BaseGroundClamper::GroundClampingType::NONE;
      else if (GetGroundClampingData().GetAdjustRotationToGround())
         gcType = &BaseGroundClamper::GroundClampingType::RANGED;
      else
         gcType = &BaseGroundClamper::GroundClampingType::INTERMITTENT_SAVE_OFFSET;

      if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::NONE)
      {
         if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Dead Reckoning Algorithm set to NONE, "
                  "setting the transform to match the actor's current position.");
         }
         gameActor.GetTransform(xform, dtCore::Transformable::REL_CS);
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

            if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               std::ostringstream ss;
               ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " got an update " << std::endl
                  << "      Rotation \"" << mLastRotation  << "\" " << std::endl
                  << "      Position \"" << mLastTranslation << "\" " << std::endl
                  << "Tag Time \"" << mLastTimeTag << "\"";
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
         if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Entity does not need to be Dead Reckoned");
      }

      return returnValue;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::CalculateSmoothingTimes(const dtCore::Transform& xform)
   {
      // ROTATION
      if (GetMaxRotationSmoothingTime() < mAverageTimeBetweenRotationUpdates)
         mRotationEndSmoothingTime = GetMaxRotationSmoothingTime();
      else
         mRotationEndSmoothingTime = mAverageTimeBetweenRotationUpdates;

      // For angular acceleration, do a similar compare
      if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION &&
         (mAngularVelocityVector.length2() * (mRotationEndSmoothingTime * mRotationEndSmoothingTime)) <
            0.1 * ((mLastQuatRotation-mCurrentDeadReckonedRotation).length2()))
      {
         mRotationEndSmoothingTime = std::min(1.0f, mAverageTimeBetweenRotationUpdates);
      }

      // TRANSLATION
      if (GetMaxTranslationSmoothingTime() < mAverageTimeBetweenTranslationUpdates)
         mTranslationEndSmoothingTime = GetMaxTranslationSmoothingTime();
      else
         mTranslationEndSmoothingTime = mAverageTimeBetweenTranslationUpdates;

      osg::Vec3 pos;
      xform.GetTranslation(pos);

      //Order of magnitude check - if the entity could not possibly get to the new position
      // in max smoothing time based on the magnitude of it's velocity, then smooth quicker (ie 1 second).
      if (mLastVelocity.length2() * (mTranslationEndSmoothingTime*mTranslationEndSmoothingTime) < (mLastTranslation - pos).length2() )
         mTranslationEndSmoothingTime = std::min(1.0f, mTranslationEndSmoothingTime);
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
            mAngularVelocityVector.length2() < 1e-6)
         {
            // if we're here, we had some sort of change, however small
            isRotationChangedByAccel = true;

            // Compute The change in the rotation based Dead Reckoning matrix
            // The Dead Reckoning Matrix
            osg::Matrix angularRotation;
            ComputeRotationChangeWithAngularVelocity(mRotationElapsedTimeSinceUpdate, angularRotation);

            // Expected DR'ed rotation - Take the last rot and add the change over time
            osg::Quat angularRotAsQuat;
            angularRotation.get(angularRotAsQuat);
            drQuat = angularRotAsQuat * mLastQuatRotation; // The current DR'ed rotation

            // Previous DR'ed rotation - same, but uses where we were before the last update, so we can smooth it out...
            startRotation = angularRotAsQuat * mRotQuatBeforeLastUpdate; // The current DR'ed rotation

            // New hpr computation
            //osg::Matrix drRot = angularRotation * mLastRotationMatrix;
            //////dtUtil::MatrixUtil::Print(mComputedAngularRotationMatrix);
            // Compute change in rotation as quaternion representation
            //drRot.get(drQuat);
            //osg::Quat rotationChange = drQuat - mLastQuatRotation;
            //isRotationChangedByAccel = rotationChange.length2() > 1e-6;
            //osg::Matrix drStartRot = angularRotation * mLastRotationMatrix;
            //startRotation = mRotQuatBeforeLastUpdate + rotationChange;
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
      osg::Vec3 accelerationEffect;
      if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION)
      {
         accelerationEffect = ((mAccelerationVector * 0.5f) *
            (mTranslationElapsedTimeSinceUpdate * mTranslationElapsedTimeSinceUpdate));
      }

      // True - meaning as if we just used the best known position & velocity.
      osg::Vec3 truePositionChange;
      // Blended - meaning using the last velocity & actual drawn position.
      osg::Vec3 blendedPositionChange;

      // COMPUTE BLENDED VELOCITY - this smooths out some of the harsh blends. We do
      // it in a fraction of the time of the translation else we get big overblown curves.
      osg::Vec3 mBlendedVelocity = mLastVelocity;
      float velBlendTime = mTranslationEndSmoothingTime/3.0f;
      if ((velBlendTime > 0.0f) && (mTranslationElapsedTimeSinceUpdate < velBlendTime))
      {
         float smoothingFactor = mTranslationElapsedTimeSinceUpdate/velBlendTime;
         mBlendedVelocity = mVelocityBeforeLastUpdate * (1.0F - smoothingFactor) +
            mLastVelocity * smoothingFactor;
      }
      else
         mBlendedVelocity = mLastVelocity;


      // COMPUTE CHANGE IN POSITION
      truePositionChange = mLastVelocity * mTranslationElapsedTimeSinceUpdate + accelerationEffect;
      blendedPositionChange = mBlendedVelocity * mTranslationElapsedTimeSinceUpdate + accelerationEffect;

      osg::Vec3 drPos = mLastTranslation + truePositionChange;

      // If we still have time left in our smoothing, then
      // blend the positions.
      if ((mTranslationEndSmoothingTime > 0.0f) && (mTranslationElapsedTimeSinceUpdate < mTranslationEndSmoothingTime))
      {
         pos = mTransBeforeLastUpdate + blendedPositionChange;
         float smoothingFactor = mTranslationElapsedTimeSinceUpdate/mTranslationEndSmoothingTime;
         //a bit of smoothing.
         pos = pos + (drPos - pos) * smoothingFactor;

         if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Actor \"" << gameActor.GetUniqueId() << " - " << gameActor.GetName() << "\" has pos " << "\"" << pos << "\"";
            pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }
      }
      else // Out of time or no move, so just set it.
      {
         pos = drPos;
      }
      mCurrentDeadReckonedTranslation = pos;

      // DEBUG STUFF
      if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " target pos "
            << "\"" << drPos << "\", temp\"" << mLastTranslationUpdatedTime + mTranslationElapsedTimeSinceUpdate << "\"\n";
         ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " current pos "
            << "\"" << pos << "\", temp\"" << mLastTranslationUpdatedTime + mTranslationElapsedTimeSinceUpdate << "\"";
         pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::LogDeadReckonStarted(osg::Vec3 &unclampedTranslation, osg::Matrix& rot, dtUtil::Log* pLogger)
   {
      std::ostringstream ss;
      ss << "Actor passed optimization checks: fully dead-reckoning actor.\n"
         << "  IsFlying():                           " << IsFlying() << std::endl
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
}
