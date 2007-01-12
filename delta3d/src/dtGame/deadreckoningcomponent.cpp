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
 * David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <algorithm>

#include <dtGame/deadreckoningcomponent.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>
#include <dtCore/isector.h>
#include <dtDAL/actortype.h>
#include <dtGame/gameactor.h>
#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>
#include <dtGame/exceptionenum.h>

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

   //////////////////////////////////////////////////////////////////////
   DeadReckoningHelper::DeadReckoningHelper() :
      mTranslationInitiated(false),
      mRotationInitiated(false),
      mUpdated(false), 
	   mTranslationUpdated(false), 
	   mRotationUpdated(false), 
      mFlying(false), 
	   mLastTimeTag(0.0),
      mLastTranslationUpdatedTime(0.0), 
	   mLastRotationUpdatedTime(0.0), 
      mAverageTimeBetweenTranslationUpdates(0.0f), 
	   mAverageTimeBetweenRotationUpdates(0.0f), 
      mMaxTranslationSmoothingSteps(8.0f),
      mMaxRotationSmoothingSteps(2.0f),
      mTranslationSmoothingSteps(0.0f), 
	   mRotationSmoothingSteps(0.0f), 
      mCurrentTotalTranslationSmoothingSteps(0.0f),
      mCurrentTotalRotationSmoothingSteps(0.0f),
      mGroundOffset(0.0f),
      mRotationResolved(true),
      mMinDRAlgorithm(&DeadReckoningAlgorithm::NONE)
   {}


//   //////////////////////////////////////////////////////////////////////
//   void DeadReckoningHelper::CreateProperties(std::vector<dtCore::RefPtr<dtDAL::ActorProperty> >& toFill)
//   {
//   }

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
   		if (!mFlying)
   			mLastTranslation[2] = mTransBeforeLastUpdate[2];
   		mTranslationSmoothingSteps = 0.0;
   		mTranslationUpdated = true;
   		mUpdated = true;
      }
      else
      {
   		mTranslationInitiated = true;
   		mTransBeforeLastUpdate = vec;
   		mCurrentDeadReckonedTranslation = vec;
   		mLastTranslation = vec;
   		mTranslationSmoothingSteps = 0.0;
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
         mRotationSmoothingSteps = 0.0;
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
   		mRotationSmoothingSteps = 0.0;
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
   void DeadReckoningHelper::SetDeadReckoningMatrix(double deltaTime)
   {
	   //mDeadReckoningMatrix
      if (mAngularVelocityVector.length2() < 1e-11)
      {
         mDeadReckoningMatrix.makeIdentity();
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
         
         mDeadReckoningMatrix.set(
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

   void DeadReckoningHelper::SetLastTranslationUpdatedTime(double newUpdatedTime)
   {
      //the average of the last average and the current time since an update.
      mAverageTimeBetweenTranslationUpdates = 0.5f * (float(newUpdatedTime - mLastTranslationUpdatedTime) + mAverageTimeBetweenTranslationUpdates); 
      mLastTranslationUpdatedTime = newUpdatedTime;
   }

   void DeadReckoningHelper::SetLastRotationUpdatedTime(double newUpdatedTime)
   {
      //the average of the last average and the current time since an update.
      mAverageTimeBetweenRotationUpdates = 0.5f * (float(newUpdatedTime - mLastRotationUpdatedTime) + mAverageTimeBetweenRotationUpdates); 
      mLastRotationUpdatedTime = newUpdatedTime;
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

   //////////////////////////////////////////////////////////////////////
   const std::string DeadReckoningComponent::DEFAULT_NAME("Dead Reckoning Component");

   //////////////////////////////////////////////////////////////////////
   const float DeadReckoningComponent::ForceClampTime(1.25f);

   //////////////////////////////////////////////////////////////////////
   DeadReckoningComponent::DeadReckoningComponent(const std::string& name): dtGame::GMComponent(name),
      mTimeUntilForceClamp(ForceClampTime),
      mHighResClampRange(0.0f), mHighResClampRange2(0.0f)
   {
      mLogger = &dtUtil::Log::GetInstance("deadreckoningcomponent.cpp");
   }

   //////////////////////////////////////////////////////////////////////
   DeadReckoningComponent::~DeadReckoningComponent()
   {
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::ProcessMessage(const dtGame::Message& message)
   {
      if (message.GetMessageType() == dtGame::MessageType::TICK_REMOTE)
      {
         TickRemote(static_cast<const dtGame::TickMessage&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
      {
         dtGame::GameActorProxy* mActor = GetGameManager()->FindGameActorById(message.GetAboutActorId());
         if (mActor != NULL)
            UnregisterActor(*mActor);
            
         if (mEyePointActor.valid() && message.GetAboutActorId() == mEyePointActor->GetUniqueId())
         {
            mEyePointActor = NULL;
         }
         
         if (mTerrainActor.valid() && message.GetAboutActorId() == mTerrainActor->GetUniqueId())
         {
            mTerrainActor = NULL;
         }
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::SetTerrainActor(dtCore::Transformable* newTerrain)
   {
      mTerrainActor = newTerrain;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::SetEyePointActor(dtCore::Transformable* newEyePointActor)
   {
      mEyePointActor = newEyePointActor;
   }
   
   //////////////////////////////////////////////////////////////////////
   dtCore::Isector& DeadReckoningComponent::GetGroundClampIsector()
   {
      return *mIsector;
   }   
   
   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::RegisterActor(dtGame::GameActorProxy& toRegister, DeadReckoningHelper& helper) 
   {
      if (!mRegisteredActors.insert(std::make_pair(toRegister.GetId(), &helper)).second)
      {
         throw dtUtil::Exception(ExceptionEnum::DEAD_RECKONING_EXCEPTION,
            "Actor \"" + toRegister.GetName() +
            "\" is already registered with a helper in the DeadReckoingComponent with name \"" +
            GetName() +  ".\"" , __FILE__, __LINE__);
      }
      else if (helper.IsUpdated())
      {
         dtCore::Transform xform;
         toRegister.GetGameActor().GetTransform(xform);
         xform.SetTranslation(helper.GetLastKnownTranslation());
         xform.SetRotation(helper.GetLastKnownRotation());
         toRegister.GetGameActor().SetTransform(xform);
         helper.mTransBeforeLastUpdate = helper.GetLastKnownTranslation();
         helper.mRotQuatBeforeLastUpdate = helper.mLastQuatRotation;
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::UnregisterActor(dtGame::GameActorProxy& toRegister)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<DeadReckoningHelper> >::iterator itor;
      itor = mRegisteredActors.find(toRegister.GetId());
      if (itor != mRegisteredActors.end())
      {
         mRegisteredActors.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////
   const DeadReckoningHelper* DeadReckoningComponent::GetHelperForProxy(dtGame::GameActorProxy &proxy) const
   {
      std::map<dtCore::UniqueId,
         dtCore::RefPtr<DeadReckoningHelper> >::const_iterator itor = mRegisteredActors.find(proxy.GetId());
      
      return itor == mRegisteredActors.end() ? NULL : itor->second.get();
   }

   //////////////////////////////////////////////////////////////////////
   bool DeadReckoningComponent::IsRegisteredActor(dtGame::GameActorProxy& gameActorProxy)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<DeadReckoningHelper> >::iterator itor;
      itor = mRegisteredActors.find(gameActorProxy.GetId());
      return itor != mRegisteredActors.end();
   }

   //////////////////////////////////////////////////////////////////////
   double DeadReckoningComponent::GetTerrainZIntersectionPoint(dtCore::DeltaDrawable& terrainActor, const osg::Vec3& point,
      osg::Vec3& groundNormalOut)
   {
      mIsector->Reset();

      mIsector->SetStartPosition(osg::Vec3(point[0], point[1], point[2] + 100.0f));
      mIsector->SetEndPosition(osg::Vec3(point[0], point[1], point[2] - 100.0f));

      mIsector->SetGeometry(&terrainActor);

      if (mIsector->Update())
      {
         osg::Vec3 hp;
         mIsector->GetHitPoint(hp);
         mIsector->GetHitPointNormal(groundNormalOut);

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Found a hit - old z " << point.z() << " new z " << hp.z();
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }

         return hp.z();
      }
      //if no hits are found, just return the original value.
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
	     std::ostringstream ss;
	     ss << "Found no hit";
	     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }
      groundNormalOut.set(0.0f, 0.0f, 1.0f);
      return point.z();
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::ClampToGroundThreePoint(float timeSinceUpdate, dtCore::Transform& xform,
      dtGame::GameActorProxy& gameActorProxy)
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Using three point ground clamping.");
      }

      osg::Vec3& position = xform.GetTranslation();
      osg::Matrix& rotation = xform.GetRotation();

      std::vector<float> dimensions;
      gameActorProxy.GetGameActor().GetCollisionGeomDimensions(dimensions);

      //make points for the front center and back corners in relative coordinates.
      osg::Vec3 point1(0.0f, dimensions[1] / 2, 0.0f),
         point2(dimensions[0] / 2, -(dimensions[1] / 2), 0.0f),
         point3(-(dimensions[0] / 2), -(dimensions[1] / 2), 0.0f);

      osg::Matrix m;
      xform.Get(m);

      //convert points to absolute space.
      point1 = point1 * m;
      point2 = point2 * m;
      point3 = point3 * m;

      osg::Vec3 groundNormal;
      dtCore::Transformable& terrain = *mTerrainActor;
      point1.z() = GetTerrainZIntersectionPoint(terrain, point1, groundNormal);
      point2.z() = GetTerrainZIntersectionPoint(terrain, point2, groundNormal);
      point3.z() = GetTerrainZIntersectionPoint(terrain, point3, groundNormal);

      float averageZ = (point1.z() + point2.z() + point3.z()) / 3;

      if (averageZ >= position.z())
      {
         //move the actor position up to the ground.
         position.z() = averageZ;
      }
      else
      {
         position.z() = averageZ;
      }

      osg::Vec3 ab = point1 - point3;
      osg::Vec3 ac = point1 - point2;

      osg::Vec3 normal = ab ^ ac;
      normal.normalize();

      osg::Vec3 oldNormal ( 0, 0, 1 );

      oldNormal = osg::Matrix::transform3x3(oldNormal, rotation);
      osg::Matrix normalRot;
      normalRot.makeRotate(oldNormal, normal);

      rotation = rotation * normalRot;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::ClampToGroundOnePoint(float timeSinceUpdate, dtCore::Transform& xform)
   {
      osg::Vec3& position = xform.GetTranslation();
      osg::Matrix& rotation = xform.GetRotation();
      
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Using one point ground clamping.");
      }

      //make points for the front center and back corners in relative coordinates.
      osg::Vec3 point(0.0f, 0.0f, 0.0f);

      osg::Matrix m;
      xform.Get(m);

      //convert points to absolute space.
      point = point * m;

      osg::Vec3 normal;

      point.z() = GetTerrainZIntersectionPoint(*GetTerrainActor(), point, normal);

      if (point.z() >= position.z())
      {
         //move the actor position up to the ground.
         position.z() = point.z();
      }
      else
      {
         position.z() = point.z();
      }

      normal.normalize();

      osg::Vec3 oldNormal ( 0, 0, 1 );

      oldNormal = osg::Matrix::transform3x3(oldNormal, rotation);
      osg::Matrix normalRot;
      normalRot.makeRotate(oldNormal, normal);

      rotation = rotation * normalRot;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::ClampToGround(float timeSinceUpdate, dtCore::Transform& xform,
      dtGame::GameActorProxy& gameActorProxy, DeadReckoningHelper& helper)
   {
      if (GetTerrainActor() == NULL)
         return;

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Ground clamping actor.");
      }

      osg::Vec3& position = xform.GetTranslation();

      if (mIsector->GetUseEyePoint() && GetHighResGroundClampingRange() > 0.0f)
      {
         const osg::Vec3 vec = mIsector->GetEyePoint();

         if ((position - vec).length2() > mHighResClampRange2)
            ClampToGroundOnePoint(timeSinceUpdate, xform);
         else
            ClampToGroundThreePoint(timeSinceUpdate, xform, gameActorProxy);
      }
      else
      {
         ClampToGroundThreePoint(timeSinceUpdate, xform, gameActorProxy);
      }
      
      position.z() += helper.GetGroundOffset();
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::DRStatic(DeadReckoningHelper& helper, const double timeSinceTranslationUpdate, const double timeSinceRotationUpdate, GameActor& gameActor, dtCore::Transform& xform)
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Dead Reckoning actor as STATIC.  New position is " <<  helper.mLastTranslation  
            << ".  New Rotation is " << helper.mLastRotation << ".";
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }

      xform.SetTranslation(helper.mLastTranslation);
      xform.SetRotation(helper.mLastRotationMatrix);

      helper.mTransBeforeLastUpdate = helper.mLastTranslation;
      helper.mRotQuatBeforeLastUpdate = helper.mLastQuatRotation;

      helper.mRotationResolved = true;
      if (!helper.IsFlying() && gameActor.GetCollisionGeomType() == &dtCore::Transformable::CollisionGeomType::CUBE)
      {
         ClampToGround(timeSinceTranslationUpdate, xform, gameActor.GetGameActorProxy(), helper);
      }

	   helper.mCurrentDeadReckonedTranslation = xform.GetTranslation();
      xform.GetRotation().get(helper.mCurrentDeadReckonedRotation);
      gameActor.SetTransform(xform);
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::DRVelocityAcceleration(DeadReckoningHelper& helper, const float deltaTime, const double timeSinceTranslationUpdate, const double timeSinceRotationUpdate,const bool forceClamp, GameActor& gameActor, dtCore::Transform& xform)
   {
      osg::Vec3& pos = xform.GetTranslation();
      osg::Matrix& rot = xform.GetRotation();

      osg::Vec3 unclampedTranslation = pos;
      unclampedTranslation.z() = helper.mLastTranslation.z();

      //avoid the sqrtf by using length2.
      //we went to see if all this dr and ground clamping stuff has to be done.
      if ((forceClamp && !helper.IsFlying()) || 
         helper.IsUpdated() || 
         helper.mLastTranslation != unclampedTranslation ||
         !helper.mRotationResolved ||
         helper.mVelocityVector.length2() > 1e-2f ||
         (helper.GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION
            && helper.mAccelerationVector.length2() > 1e-2f)||
			(helper.GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION
            && helper.mAngularVelocityVector.length2() > 1e-5f))
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Actor passed optimization checks: fully dead-reckoning actor.\n" 
               << "  forceClamp:                           " << forceClamp << std::endl
               << "  IsFlying():                           " << helper.IsFlying() << std::endl
               << "  mLastTranslation:                     " << helper.mLastTranslation << std::endl
               << "  unclampedTranslation:                 " << unclampedTranslation << std::endl
               << "  helper.mVelocityVector.length2():     " << helper.mVelocityVector.length2() << std::endl
               << "  helper.mAccelerationVector.length2(): " << helper.mAccelerationVector.length2() << std::endl
               << "  rot Matrix is: " << rot << std::endl
               << "  mLastRotationMatrix is: " << helper.mLastRotationMatrix << std::endl;
            
            if (!helper.mRotationResolved)
            {
               ss << "rot Matrix is: " << rot << std::endl;
               ss << "mLastRotationMatrix is: " << helper.mLastRotationMatrix << std::endl;
            }
            
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }
         osg::Vec3 positionChange;
         if (helper.GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_ONLY)
         {
            positionChange = helper.mVelocityVector * timeSinceTranslationUpdate;
         }
         else
         {
            positionChange = helper.mVelocityVector * timeSinceTranslationUpdate +
               ((helper.mAccelerationVector * 0.5f) * (timeSinceTranslationUpdate * timeSinceTranslationUpdate));
         }

         osg::Vec3 drPos = helper.mLastTranslation + positionChange;

         osg::Quat newRot;

         if (helper.IsUpdated())
         {
            CalculateTotalSmoothingSteps(helper, xform);
   			if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   			{
                  std::ostringstream ss;
   			   osg::Vec3 temp;
   			   temp[0] = helper.mLastRotation[0];
   			   temp[1] = helper.mLastRotation[1];
   			   temp[2] = helper.mLastRotation[2];
                  ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " HLA update "
                     << "\"" << temp << "Tag Time " << helper.mLastTimeTag << "\"";
   			   mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
                     ss.str().c_str());               
   			}
   			if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   			{
                  std::ostringstream ss;
   			   osg::Vec3 temp;
   			   temp[0] = helper.mLastTranslation[0];
   			   temp[1] = helper.mLastTranslation[1];
   			   temp[2] = helper.mLastTranslation[2];
                  ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " HLA update "
                     << "\"" << temp << "Tag Time " << helper.mLastTimeTag << "\"";
   			   mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
                     ss.str().c_str());               
   			}
         }

         /************************************************************************************/
         /*
          * 1) Compute rotationChange
          * a) Compute Dead Reckoning matrix
          */
         helper.SetDeadReckoningMatrix(timeSinceRotationUpdate);
         /*
          *  b) New hpr computation
          *  drRot = mDRMatrix*mLastAttitudeMatrix; 
          */
         osg::Matrix drRot = helper.mDeadReckoningMatrix*helper.mLastRotationMatrix;

         /* std::cout << "Temps " << helper.mLastRotationUpdatedTime + helper.mRotationSmoothingSteps << std::endl;
          std::cout << "Matrice de Dead Reckoning" << std::endl;
          dtUtil::MatrixUtil::Print(helper.mDeadReckoningMatrix);
          std::cout << "Matrice mLastRotationMatrix" << std::endl;
          dtUtil::MatrixUtil::Print(helper.mLastRotationMatrix);*/

         /*
          * c) get quaternion representation (drRot,drQuat);
          */
         osg::Quat drQuat ;
         drRot.get(drQuat);
         /*
          * d) rotationChange = drQuat - mLastQuatAttitude;
          */
         osg::Quat rotationChange = drQuat - helper.mLastQuatRotation;

      	if (rotationChange.length2()>1e-8 && helper.mRotationSmoothingSteps <  helper.mCurrentTotalRotationSmoothingSteps)
         {
            /*
             * 2) Smooth : 
             * newRot.slerp(smoothingFactor, 
             *	      helper.mRotQuatBeforeLastUpdate+rotationChange, 
             *       helper.mLastQuatRotation+rotationChange);
             */
            float smoothingFactor = helper.mRotationSmoothingSteps/helper.mCurrentTotalRotationSmoothingSteps;
            newRot.slerp(smoothingFactor,helper.mRotQuatBeforeLastUpdate+rotationChange,drQuat);
               rot.set(newRot);
         }
         else
         {
            newRot = drQuat;
            rot.set(newRot);
            helper.mRotationResolved = true;
         }

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            osg::Matrix tempRot;
            tempRot.set(drQuat);
            osg::Vec3 tempVec;	
            dtUtil::MatrixUtil::MatrixToHpr(tempVec,tempRot);
            std::ostringstream ss;
               ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " target attitude "
                  << "\"" << tempVec << " au temps " 
                  << helper.mLastRotationUpdatedTime + helper.mRotationSmoothingSteps << "\"";
            mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, 
                  ss.str().c_str());               
         }

		   /****************************************************************************/
         
         if (positionChange.length2()>1e-3 && helper.mTranslationSmoothingSteps < helper.mCurrentTotalTranslationSmoothingSteps)
         {
            pos = helper.mTransBeforeLastUpdate + positionChange;
            float smoothingFactor = helper.mTranslationSmoothingSteps/helper.mCurrentTotalTranslationSmoothingSteps;
            //a bit of smoothing.
            pos = pos + (drPos - pos) * smoothingFactor;

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               std::ostringstream ss;
               ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " has translation "
                  << "\"" << pos << "\"";
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
                  ss.str().c_str());               
            }
         }
         else
         {
            pos = drPos;
         }

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
               ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " target position "
                  << "\"" << drPos << " au temps " 
                  << helper.mLastTranslationUpdatedTime + helper.mTranslationSmoothingSteps << "\"";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
                  ss.str().c_str());               
         }
         
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
               ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " current position "
                  << "\"" << pos << " au temps " 
                  << helper.mLastTranslationUpdatedTime + helper.mTranslationSmoothingSteps << "\"";
            mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, 
                  ss.str().c_str());               
         }

         helper.mTranslationSmoothingSteps += deltaTime;
         helper.mRotationSmoothingSteps += deltaTime;

         if (!helper.IsFlying() && gameActor.GetCollisionGeomType() == &dtCore::Transformable::CollisionGeomType::CUBE)
         {
            ClampToGround(timeSinceTranslationUpdate, xform, gameActor.GetGameActorProxy(), helper);
         }

         helper.mCurrentDeadReckonedTranslation = pos;
         helper.mCurrentDeadReckonedRotation = newRot;

         xform.SetTranslation(pos);
         xform.SetRotation(rot);
         gameActor.SetTransform(xform);

         osg::Matrix mCurrentRotation = xform.GetRotation();
         dtUtil::MatrixUtil::MatrixToHpr(helper.mCurrentAttitudeVector,mCurrentRotation);
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " has attitude "
               << "\"" << helper.mCurrentAttitudeVector << " au temps" 
               << helper.mLastRotationUpdatedTime + helper.mRotationSmoothingSteps << "\"";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
                  ss.str().c_str());               
         }
      } 
      else
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Entity does not need to be Dead Reckoned");
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::CalculateTotalSmoothingSteps(DeadReckoningHelper& helper, const dtCore::Transform& xform)
   {
      if (helper.GetMaxRotationSmoothingSteps()<helper.mAverageTimeBetweenRotationUpdates)
			helper.mCurrentTotalRotationSmoothingSteps = helper.GetMaxRotationSmoothingSteps();
	  else helper.mCurrentTotalRotationSmoothingSteps = helper.mAverageTimeBetweenRotationUpdates;       

	  if (helper.mAngularVelocityVector.length2() * (helper.mCurrentTotalRotationSmoothingSteps*helper.mCurrentTotalRotationSmoothingSteps) < 0.1*((helper.mLastQuatRotation-helper.mCurrentDeadReckonedRotation).length2()))
		helper.mCurrentTotalRotationSmoothingSteps = 1.0;
      
	  if (helper.GetMaxTranslationSmoothingSteps()<helper.mAverageTimeBetweenTranslationUpdates)
			helper.mCurrentTotalTranslationSmoothingSteps = helper.GetMaxTranslationSmoothingSteps();
	  else helper.mCurrentTotalTranslationSmoothingSteps = helper.mAverageTimeBetweenTranslationUpdates;      
      //If the player could not possible get to the new position in 10 seconds
      //based on the magnitude of it's velocity vector, then just warp the entity in 1 second.
      if (helper.mVelocityVector.length2() * (helper.mCurrentTotalTranslationSmoothingSteps*helper.mCurrentTotalTranslationSmoothingSteps) < (helper.mLastTranslation - xform.GetTranslation()).length2() )
         helper.mCurrentTotalTranslationSmoothingSteps = 1.0;
      
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::TickRemote(const dtGame::TickMessage& tickMessage)
   {
      if(!mIsector.valid())
         mIsector = new dtCore::Isector;

      //Setup the iSector to use the player position only once so that get transform is not called
      //for every single actor to be clamped.
      if(GetEyePointActor() != NULL)
      {
         dtCore::Transform xform;
         GetEyePointActor()->GetTransform(xform);
         mIsector->SetEyePoint(xform.GetTranslation());
         mIsector->SetUseEyePoint(true);

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            osg::Vec3& debugPos = xform.GetTranslation();

            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Setting the eye point to the position %f, %f, %f.",
               debugPos.x(), debugPos.y(), debugPos.z());
         }
      }
      else
      {
         mIsector->SetUseEyePoint(false);

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Setting the isector to not use an eye point.");
         }
      }

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<DeadReckoningHelper> >::iterator i = mRegisteredActors.begin();
         i != mRegisteredActors.end(); ++i)
      {

         dtGame::GameActorProxy& gameActorProxy = *GetGameManager()->FindGameActorById(i->first);
         dtGame::GameActor& gameActor = gameActorProxy.GetGameActor();
         DeadReckoningHelper& helper = *i->second;

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Dead Reckoning actor named \"%s\" with ID \"%s\" and type \"%s.%s.\"",
               gameActor.GetName().c_str(), gameActor.GetUniqueId().ToString().c_str(),
               gameActorProxy.GetActorType().GetCategory().c_str(),
               gameActorProxy.GetActorType().GetName().c_str());
         }

         dtCore::Transform xform;
         gameActor.GetTransform(xform);

         if (helper.IsUpdated())
         {
            //Pretend we were updated on the last tick so we have time delta to work with
            //when calculating movement.
            if (helper.mTranslationUpdated)
            {
            	//helper.SetLastTranslationUpdatedTime(tickMessage.GetSimulationTime() - tickMessage.GetDeltaSimTime());
            	helper.SetLastTranslationUpdatedTime(helper.mLastTimeTag);
            	helper.mTranslationSmoothingSteps = tickMessage.GetDeltaSimTime();
            }
            
            if (helper.mRotationUpdated)
            {
            	//helper.SetLastRotationUpdatedTime(tickMessage.() - tickMessage.GetDeltaSimTime());
            	helper.SetLastRotationUpdatedTime(helper.mLastTimeTag);
            	helper.mRotationSmoothingSteps = tickMessage.GetDeltaSimTime();
            	helper.mRotationResolved = false;
            }
         }

         bool forceClamp = false;

         double timeSinceTranslationUpdate = helper.mTranslationSmoothingSteps;
         double timeSinceRotationUpdate = helper.mRotationSmoothingSteps;
         mTimeUntilForceClamp -= tickMessage.GetDeltaSimTime();

         if (mTimeUntilForceClamp <= 0.0f)
         {
            mTimeUntilForceClamp = ForceClampTime;
            forceClamp = true;

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Forcing a ground clamp.");
            }
         }

         //make sure it's greater than 0 in case of time being set.
         dtUtil::Clamp(timeSinceTranslationUpdate, 0.0, timeSinceTranslationUpdate);
         dtUtil::Clamp(timeSinceRotationUpdate, 0.0, timeSinceRotationUpdate);

         if (helper.GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::NONE)
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Dead Reckoning Algorithm set to NONE, doing nothing.");
            }
         }
         else if (helper.GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::STATIC)
         {
            if (helper.IsUpdated() || forceClamp)
            {
               DRStatic(helper, timeSinceTranslationUpdate, timeSinceRotationUpdate, gameActor, xform);
            }
         }
         else
         {
            DRVelocityAcceleration(helper, tickMessage.GetDeltaSimTime(), timeSinceTranslationUpdate, timeSinceRotationUpdate, forceClamp, gameActor, xform);
         }

         if (helper.GetNodeCollector() != NULL)
         {
            std::list<dtCore::RefPtr<DeadReckoningHelper::DeadReckoningDOF> >::iterator iterDOF = helper.mDeadReckonDOFS.begin();
            
            for(iterDOF = helper.mDeadReckonDOFS.begin();iterDOF != helper.mDeadReckonDOFS.end(); ++iterDOF)
            {
               (*iterDOF)->mUpdate = false;
            }

            iterDOF = helper.mDeadReckonDOFS.begin();
            while(iterDOF != helper.mDeadReckonDOFS.end())
            {
               DeadReckoningHelper::DeadReckoningDOF *currentDOF = (*iterDOF).get();
               if(currentDOF->mPrev == NULL && !currentDOF->mUpdate)
               {
                  currentDOF->mCurrentTime += tickMessage.GetDeltaSimTime();
                  currentDOF->mUpdate = true;

                  // there is something in the chain
                  if(currentDOF->mNext != NULL)
                  {
                     osg::Vec3 NewDistance = (currentDOF->mNext->mStartLocation  - currentDOF->mStartLocation);
                     
                     for (int i = 0; i < 3; ++i)
                     {
                        while (NewDistance[i] > osg::PI)
                           NewDistance[i] -= 2 * osg::PI;
                        while (NewDistance[i] < -osg::PI)
                           NewDistance[i] += 2 * osg::PI;
                     }
                     
                     osg::Vec3 RateOverTime = (NewDistance * currentDOF->mCurrentTime);
                     
                     osg::Vec3 result(currentDOF->mStartLocation[0] + RateOverTime[0],
                                     currentDOF->mStartLocation[1] + RateOverTime[1],
                                     currentDOF->mStartLocation[2] + RateOverTime[2]);
                     
                     for (int i = 0; i < 3; ++i)
                     {
                        while (result[i] > 2 * osg::PI)
                           result[i] -= 2 * osg::PI;
                           
                        while (result[i] < 0)
                           result[i] += 2 * osg::PI;
                     }
                     
                     osgSim::DOFTransform* dofTransform = helper.mDOFDeadReckoning->GetDOFByName(currentDOF->mName);
                     if (dofTransform != NULL)
                        dofTransform->updateCurrentHPR(result);
                  }
                  else
                  {
                     osg::Vec3 result( currentDOF->mStartLocation[0] + (currentDOF->mCurrentTime * currentDOF->mRateOverTime[0]),
                                       currentDOF->mStartLocation[1] + (currentDOF->mCurrentTime * currentDOF->mRateOverTime[1]),
                                       currentDOF->mStartLocation[2] + (currentDOF->mCurrentTime * currentDOF->mRateOverTime[2]));
                     osgSim::DOFTransform* dofTransform = helper.mDOFDeadReckoning->GetDOFByName(currentDOF->mName);
                     if (dofTransform != NULL)
                        dofTransform->updateCurrentHPR(result);
                  }

                  // Get rid of middle man, would take out if u want full movement
                  // between multiple steps
                  if(currentDOF->mNext != NULL)
                  {
                     bool changedList = false;
                     while((*iterDOF)->mNext->mNext != NULL)
                     {
                        helper.RemoveDRDOF(*(*iterDOF)->mNext);
                        // start over at the beginning of the DOF list.  Hence the use of the Update flag.
                        iterDOF = helper.mDeadReckonDOFS.begin();
                        changedList = true;
                     }
                     if (changedList)
                        continue;
                  }

                  // One second has passed, and this has more in its chain
                  if(currentDOF->mNext != NULL && currentDOF->mCurrentTime >= 1)
                  {
                     currentDOF->mNext->mStartLocation = helper.mDOFDeadReckoning->GetDOFByName(currentDOF->mName)->getCurrentHPR();
                     currentDOF->mNext->mCurrentTime = 0;

                     // Get rid of the rotation
                     helper.RemoveDRDOF(*currentDOF);
                     // start over at the beginning of the DOF list.  Hence the use of the Update flag.
                     iterDOF = helper.mDeadReckonDOFS.begin();
                     continue;
                  }                  
               }
               ++iterDOF;
            }
         }

         //clear the updated flag.
         helper.ClearUpdated();
      }
   }
}
