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

namespace dtGame
{
   IMPLEMENT_ENUM(DeadReckoningAlgorithm);
   DeadReckoningAlgorithm DeadReckoningAlgorithm::NONE("None");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::STATIC("Static");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::VELOCITY_ONLY("Velocity Only");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION("Velocity and Acceleration");

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
   DeadReckoningHelper::UpdateMode DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR("CALCULATE_AND_MOVE_ACTOR");

   //////////////////////////////////////////////////////////////////////
   DeadReckoningHelper::DeadReckoningHelper() :
	   mLastTimeTag(0.0),
      mLastTranslationUpdatedTime(0.0), 
	   mLastRotationUpdatedTime(0.0), 
	   mTimeUntilForceClamp(0.0f),
      mAverageTimeBetweenTranslationUpdates(0.0f), 
	   mAverageTimeBetweenRotationUpdates(0.0f), 
      mMaxTranslationSmoothingTime(8.0f),
      mMaxRotationSmoothingTime(2.0f),
      mTranslationCurrentSmoothingTime(0.0f), 
	   mRotationCurrentSmoothingTime(0.0f), 
      mTranslationEndSmoothingTime(0.0f),
      mRotationEndSmoothingTime(0.0f),
      mGroundOffset(0.0f),
      mMinDRAlgorithm(&DeadReckoningAlgorithm::NONE),
      mUpdateMode(&DeadReckoningHelper::UpdateMode::AUTO),
      mTranslationInitiated(false),
      mRotationInitiated(false),
      mUpdated(false), 
      mTranslationUpdated(false), 
      mRotationUpdated(false), 
      mFlying(false), 
      mRotationResolved(true),
      mUseModelDimensions(false)
   {}


//   //////////////////////////////////////////////////////////////////////
//   void DeadReckoningHelper::CreateProperties(std::vector<dtCore::RefPtr<dtDAL::ActorProperty> >& toFill)
//   {
//   }

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
   void DeadReckoningHelper::SetDeadReckoningAlgorithm(DeadReckoningAlgorithm& newAlgorithm)
   {
      if (*mMinDRAlgorithm == newAlgorithm)
         return;
      mMinDRAlgorithm = &newAlgorithm;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastKnownTranslation(const osg::Vec3 &vec)
   {
      if (mTranslationInitiated)
      {
   		mTransBeforeLastUpdate = mCurrentDeadReckonedTranslation;
   		mLastTranslation = vec;
   		mTranslationCurrentSmoothingTime = 0.0;
   		mTranslationUpdated = true;
   		mUpdated = true;
      }
      else
      {
   		mTranslationInitiated = true;
   		mTransBeforeLastUpdate = vec;
   		mLastTranslation = vec;
   		mTranslationCurrentSmoothingTime = 0.0;
   		mTranslationUpdated = true;
   		mUpdated = true;
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetLastKnownRotation(const osg::Vec3 &vec)
   {
      if (mRotationInitiated)
      {
         dtCore::Transform xform;
         xform.SetRotation(vec);
         xform.GetRotation(mLastRotationMatrix);
         //dtUtil::MatrixUtil::MatrixToHpr(mLastRotation,mLastRotationMatrix);
         mLastRotation = vec;
         mRotQuatBeforeLastUpdate = mCurrentDeadReckonedRotation;
         mLastRotationMatrix.get(mLastQuatRotation);
         mRotationCurrentSmoothingTime = 0.0;
         mRotationUpdated = true;
         mUpdated = true;
      }
      else 
      {
   		dtCore::Transform xform;
   		xform.SetRotation(vec);
   		xform.GetRotation(mLastRotationMatrix);
   		//dtUtil::MatrixUtil::MatrixToHpr(mLastRotation,mLastRotationMatrix);
   		mLastRotation = vec;
   		mLastRotationMatrix.get(mRotQuatBeforeLastUpdate);
   		mLastRotationMatrix.get(mLastQuatRotation);
   		mRotationCurrentSmoothingTime = 0.0;
   		mRotationInitiated = true;
   		mRotationUpdated = true;
   		mUpdated = true;
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetVelocityVector(const osg::Vec3 &vec)
   {
      mVelocityVector = vec;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetAccelerationVector(const osg::Vec3 &vec)
   {
      mAccelerationVector = vec;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::SetAngularVelocityVector(const osg::Vec3 &vec)
   {
      mAngularVelocityVector = vec;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::ComputeRotationChangeWithAngularVelocity(double deltaTime, osg::Matrix& result)
   {
	   //mComputedAngularRotationMatrix
      if (mAngularVelocityVector.length2() < 1e-11)
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
      mGroundOffset = newOffset;
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
      mModelDimensions = newDimensions; 
      SetUseModelDimensions(true); 
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::AddToDeadReckonDOF(const std::string &DofName, osg::Vec3& position, osg::Vec3& rateOverTime)
   {
      DeadReckoningDOF *toAdd = new DeadReckoningDOF;
      toAdd->mNext = NULL;
      toAdd->mPrev = NULL;
      toAdd->mName = DofName;
      toAdd->mCurrentTime = 0;
      toAdd->mRateOverTime.set( rateOverTime );
      toAdd->mStartLocation.set( position );

      std::list<dtCore::RefPtr<DeadReckoningDOF> >::iterator iter;
      for(iter = mDeadReckonDOFS.begin(); iter != mDeadReckonDOFS.end(); ++iter)
      {
         if((*iter)->mName == DofName)
         {
            bool HadToIter = false;
            dtCore::RefPtr<DeadReckoningDOF> GetDOFBeforeNULL = (*iter).get();
            
            while(GetDOFBeforeNULL != NULL)
            {
               if(GetDOFBeforeNULL->mNext == NULL)
                  break;
               GetDOFBeforeNULL = GetDOFBeforeNULL->mNext;
               HadToIter = true;
            }
            
            toAdd->mPrev = GetDOFBeforeNULL.get();
            toAdd->mNext = NULL;

            GetDOFBeforeNULL->mCurrentTime = 0;
            GetDOFBeforeNULL->mNext = toAdd;
            GetDOFBeforeNULL->mStartLocation = mDOFDeadReckoning->GetDOFByName(GetDOFBeforeNULL->mName)->getCurrentHPR();
            
            break;
         }
      }

      mDeadReckonDOFS.push_back(toAdd);
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::RemoveDRDOF(DeadReckoningDOF &obj)
   {
      std::list<dtCore::RefPtr<DeadReckoningDOF> >::iterator iterDOF;
       for(iterDOF = mDeadReckonDOFS.begin();iterDOF != mDeadReckonDOFS.end(); ++iterDOF)
       {
          if(  (obj.mCurrentTime   == (*iterDOF)->mCurrentTime)
             && (obj.mName          == (*iterDOF)->mName)
             && (obj.mRateOverTime  == (*iterDOF)->mRateOverTime)
             && (obj.mStartLocation == (*iterDOF)->mStartLocation))
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
      if((*iter)->mNext != NULL)
      {
         // if its in the middle of two objects
         if((*iter)->mPrev != NULL)
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
      else if((*iter)->mPrev != NULL)
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
         dtDAL::MakeFunctor(*this, &DeadReckoningHelper::SetLastKnownTranslation),
         dtDAL::MakeFunctorRet(*this, &DeadReckoningHelper::GetLastKnownTranslationByCopy),
         "Sets the last know position of this Entity", ""));

      pFillVector.push_back(new dtDAL::Vec3ActorProperty("Last Known Rotation", "Last Known Rotation",
         dtDAL::MakeFunctor(*this, &DeadReckoningHelper::SetLastKnownRotation),
         dtDAL::MakeFunctorRet(*this, &DeadReckoningHelper::GetLastKnownRotationByCopy),
         "Sets the last known rotation of this Entity", ""));

      pFillVector.push_back(new dtDAL::Vec3ActorProperty("Velocity Vector", "Velocity Vector",
         dtDAL::MakeFunctor(*this, &DeadReckoningHelper::SetVelocityVector),
         dtDAL::MakeFunctorRet(*this, &DeadReckoningHelper::GetVelocityVectorByCopy),
         "Sets the velocity vector of this Entity", ""));

      pFillVector.push_back(new dtDAL::Vec3ActorProperty("Acceleration Vector", "Acceleration Vector",
         dtDAL::MakeFunctor(*this, &DeadReckoningHelper::SetAccelerationVector),
         dtDAL::MakeFunctorRet(*this, &DeadReckoningHelper::GetAccelerationVectorByCopy),
         "Sets the acceleration vector of this Entity", ""));

      pFillVector.push_back(new dtDAL::Vec3ActorProperty("Angular Velocity Vector", "Angular Velocity Vector",
         dtDAL::MakeFunctor(*this, &DeadReckoningHelper::SetAngularVelocityVector),
         dtDAL::MakeFunctorRet(*this, &DeadReckoningHelper::GetAngularVelocityVectorByCopy),
         "Sets the acceleration vector of this Entity", ""));

      pFillVector.push_back(new dtDAL::EnumActorProperty<dtGame::DeadReckoningAlgorithm>("Dead Reckoning Algorithm", "Dead Reckoning Algorithm",
         dtDAL::MakeFunctor(*this, &DeadReckoningHelper::SetDeadReckoningAlgorithm),
         dtDAL::MakeFunctorRet(*this, &DeadReckoningHelper::GetDeadReckoningAlgorithm),
         "Sets the enumerated dead reckoning algorithm to use.", ""));

      pFillVector.push_back(new dtDAL::BooleanActorProperty("Flying", "Should Not Follow the Ground",
         dtDAL::MakeFunctor(*this, &DeadReckoningHelper::SetFlying),
         dtDAL::MakeFunctorRet(*this, &DeadReckoningHelper::IsFlying),
         "Flags if the dead-reckoning code should not make this actor follow the ground as it moves."));

      pFillVector.push_back(new dtDAL::FloatActorProperty("Ground Offset", "Ground Offset", 
         dtDAL::MakeFunctor(*this, &DeadReckoningHelper::SetGroundOffset), 
         dtDAL::MakeFunctorRet(*this, &DeadReckoningHelper::GetGroundOffset), 
         "Sets the offset from the ground this entity should have.  This only matters if it is not flying."));

      pFillVector.push_back(new dtDAL::Vec3ActorProperty("Model Dimensions", "Actor Model Dimensions", 
            dtDAL::MakeFunctor(*this, &DeadReckoningHelper::SetModelDimensions), 
            dtDAL::MakeFunctorRet(*this, &DeadReckoningHelper::GetModelDimensionsByCopy), 
            "Sets the x,y,z dimensions of the model the actor loads.  This is used by the ground clamping code."));

      pFillVector.push_back(new dtDAL::BooleanActorProperty("Use Model Dimensions", "Use Model Dimensions", 
            dtDAL::MakeFunctor(*this, &DeadReckoningHelper::SetUseModelDimensions), 
            dtDAL::MakeFunctorRet(*this, &DeadReckoningHelper::UseModelDimensions), 
            "Should the DR Component use the currently set model dimension values when ground clamping?"));
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DeadReckoningHelper::DoDR(GameActor& gameActor, dtCore::Transform& xform, 
         dtUtil::Log* pLogger, bool& bShouldGroundClamp)
   {         
      bool returnValue = false; // indicates we changed the transform
      bShouldGroundClamp = !IsFlying();

      if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::NONE)
      {
         if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
                  "Dead Reckoning Algorithm set to NONE, doing nothing.");
         }
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
      osg::Vec3& pos = xform.GetTranslation();
      osg::Matrix& rot = xform.GetRotation();

      osg::Vec3 unclampedTranslation = pos;
      unclampedTranslation.z() = mLastTranslation.z();

      //avoid the sqrtf by using length2.
      //we went to see if all this dr and ground clamping stuff has to be done.      
     if ( IsUpdated() || 
         mLastTranslation != unclampedTranslation ||
         !mRotationResolved ||
         mVelocityVector.length2() > 1e-2f ||
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
               ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " got an HLA update " << std::endl
                  << "      ROTATE   [" << mLastRotation[0] << ", " << mLastRotation[1] << ", " << mLastRotation[2] << "] " << std::endl 
                  << "      POSITION [" << mLastTranslation[0] << ", " << mLastTranslation[1] << ", " << mLastTranslation[2] << "] " << std::endl 
                  << "Tag Time [" << mLastTimeTag << "]";
   			   pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());               
   			}
         }

         // RESOLVE ROTATION
         DeadReckonTheRotation(rot, xform);

         // POSITION SMOOTHING
         DeadReckonThePosition(pos, pLogger, gameActor);

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
         mRotationEndSmoothingTime = 1.0;
      }

      // TRANSLATION
      if (GetMaxTranslationSmoothingTime() < mAverageTimeBetweenTranslationUpdates)
         mTranslationEndSmoothingTime = GetMaxTranslationSmoothingTime();
      else 
         mTranslationEndSmoothingTime = mAverageTimeBetweenTranslationUpdates;      

      //If the player could not possible get to the new position in 10 seconds
      //based on the magnitude of it's velocity vector, then just warp the entity in 1 second.
      if (mVelocityVector.length2() * (mTranslationEndSmoothingTime*mTranslationEndSmoothingTime) < (mLastTranslation - xform.GetTranslation()).length2() )
         mTranslationEndSmoothingTime = 1.0;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::DeadReckonTheRotation(osg::Matrix &rot, dtCore::Transform &xform)
   {
      osg::Quat newRot;
      osg::Quat drQuat = mLastQuatRotation; // velocity only just uses the last. 
      bool isRotationChangedByAccel = false;
      osg::Quat startRotation = mRotQuatBeforeLastUpdate;

      if (!mRotationResolved) // mRotationResolved is never set when using Accel
      {

         // For vel and Accel, we use the angular velocity to compute a dead reckoning matrix to slerp to
         if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION)
         {
            // Compute The change in the rotation based Dead Reckoning matrix
            // The Dead Reckoning Matrix
            osg::Matrix angularRotation;
            ComputeRotationChangeWithAngularVelocity(mRotationCurrentSmoothingTime, angularRotation);

            // New hpr computation 
            osg::Matrix drRot = angularRotation * mLastRotationMatrix;
            //dtUtil::MatrixUtil::Print(mComputedAngularRotationMatrix);
            // Compute change in rotation as quaternion representation 
            drRot.get(drQuat);
            osg::Quat rotationChange = drQuat - mLastQuatRotation;
            isRotationChangedByAccel = rotationChange.length2() > 1e-6;
            startRotation = mRotQuatBeforeLastUpdate + rotationChange;
         }

         // If there is a difference in the rotations and we still have time to smooth, then 
         // slerp between the two quats: 1) the old rotation plus the expected change using angular
         //    velocity and 2) the desired new rotation
         if (isRotationChangedByAccel || mRotationCurrentSmoothingTime <  mRotationEndSmoothingTime)
         {
            float smoothingFactor = mRotationCurrentSmoothingTime/mRotationEndSmoothingTime;
            dtUtil::Clamp(smoothingFactor, 0.0f, 1.0f);
            newRot.slerp(smoothingFactor, startRotation, drQuat);
            rot.set(newRot);
         }
         else // Either smoothing time is done or the current rotation equals the desired rotation
         {
            newRot = drQuat;
            mRotationResolved = true;
         }

         // we finished DR, so update the rotation values on the helper and transform
         rot.set(newRot);
         mCurrentDeadReckonedRotation = newRot;
         xform.GetRotation(mCurrentAttitudeVector);
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningHelper::DeadReckonThePosition( osg::Vec3& pos, dtUtil::Log* pLogger, GameActor &gameActor )
   {
      osg::Vec3 positionChange;
      if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_ONLY)
      {
         positionChange = mVelocityVector * mTranslationCurrentSmoothingTime;
      }
      else
      {
         positionChange = mVelocityVector * mTranslationCurrentSmoothingTime +
            ((mAccelerationVector * 0.5f) * (mTranslationCurrentSmoothingTime * mTranslationCurrentSmoothingTime));
      }
      osg::Vec3 drPos = mLastTranslation + positionChange;

      // If the position has changed a little, and we still have time left in our smoothing, then
      // blend the positions.
      if (positionChange.length2()>1e-3 && mTranslationCurrentSmoothingTime < mTranslationEndSmoothingTime)
      {
         pos = mTransBeforeLastUpdate + positionChange;
         float smoothingFactor = mTranslationCurrentSmoothingTime/mTranslationEndSmoothingTime;
         //a bit of smoothing.
         pos = pos + (drPos - pos) * smoothingFactor;

         if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Actor [" << gameActor.GetUniqueId() << " - " << gameActor.GetName() << "] has pos " << "[" << pos << "]";
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
            << "[" << drPos << "], temp[" << mLastTranslationUpdatedTime + mTranslationCurrentSmoothingTime << "]\n";
         ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " current pos "
            << "[" << pos << "], temp[" << mLastTranslationUpdatedTime + mTranslationCurrentSmoothingTime << "]";
         pLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, ss.str().c_str());               
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
         << "  mVelocityVector.length2():     " << mVelocityVector.length2() << std::endl
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
