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
#include <dtGame/deadreckoninghelper.h>
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
      DeadReckoningHelper::UpdateMode* updateMode = &helper.GetUpdateMode();
      if (*updateMode == DeadReckoningHelper::UpdateMode::AUTO)
      {
         if (toRegister.GetGameActor().IsRemote())
            updateMode = &DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR;
         else
            updateMode = &DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR;
            
      }
      
      if (!mRegisteredActors.insert(std::make_pair(toRegister.GetId(), &helper)).second)
      {
         throw dtUtil::Exception(ExceptionEnum::DEAD_RECKONING_EXCEPTION,
            "Actor \"" + toRegister.GetName() +
            "\" is already registered with a helper in the DeadReckoingComponent with name \"" +
            GetName() +  ".\"" , __FILE__, __LINE__);
      }
      else if (helper.IsUpdated())
      {
         if (helper.GetEffectiveUpdateMode(toRegister.GetGameActor().IsRemote()) 
            == DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR)
         {
            dtCore::Transform xform;
            toRegister.GetGameActor().GetTransform(xform);
            xform.SetTranslation(helper.GetLastKnownTranslation());
            xform.SetRotation(helper.GetLastKnownRotation());
            toRegister.GetGameActor().SetTransform(xform);
            helper.SetTranslationBeforeLastUpdate( helper.GetLastKnownTranslation() );
            helper.SetRotationBeforeLastUpdate( helper.GetLastKnownRotationByQuaternion() );
         }
      }
      
      // If the actor is local, don't move it, and force the 
      // helper to match as if it was just updated.
      if (helper.GetEffectiveUpdateMode(toRegister.GetGameActor().IsRemote()) 
         == DeadReckoningHelper::UpdateMode::CALCULATE_ONLY)
      {
         dtCore::Transform xform;
         toRegister.GetGameActor().GetTransform(xform);

         helper.SetLastKnownTranslation(xform.GetTranslation());
         osg::Vec3 rot;
         xform.GetRotation(rot);
         helper.SetLastKnownRotation(rot);

         helper.SetTranslationBeforeLastUpdate( helper.GetLastKnownTranslation() );
         helper.SetRotationBeforeLastUpdate( helper.GetLastKnownRotationByQuaternion() );
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

      bool forceClamp = false;
      mTimeUntilForceClamp -= tickMessage.GetDeltaSimTime();

      if (mTimeUntilForceClamp <= 0.0f)
      {
         mTimeUntilForceClamp = ForceClampTime;
         forceClamp = true;

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Forcing a ground clamp on all entities.");
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
            if ( helper.IsTranslationUpdated() )
            {
               helper.SetLastTranslationUpdatedTime(tickMessage.GetSimulationTime() - tickMessage.GetDeltaSimTime());
               //helper.SetLastTranslationUpdatedTime(helper.mLastTimeTag);
               helper.SetTranslationSmoothing( 0.0 );
            }

            if ( helper.IsRotationUpdated() )
            {
               helper.SetLastRotationUpdatedTime(tickMessage.GetSimulationTime() - tickMessage.GetDeltaSimTime());
               //helper.SetLastRotationUpdatedTime(helper.mLastTimeTag);
               helper.SetRotationSmoothing( 0.0 );
               helper.SetRotationResolved( false );
            }
         }

         //We want to do this every time.
         helper.SetTranslationSmoothing( helper.GetTranslationSmoothing() + tickMessage.GetDeltaSimTime() );
         helper.SetRotationSmoothing( helper.GetRotationSmoothing() + tickMessage.GetDeltaSimTime() );


         //make sure it's greater than 0 in case of time being set.
         if (helper.GetTranslationSmoothing() < 0.0) 
            helper.SetTranslationSmoothing( 0.0 );
         if (helper.GetRotationSmoothing() < 0.0) 
            helper.SetRotationSmoothing( 0.0 );


         //actual dead reckoning code moved into the helper..
         helper.DoDR(gameActor, xform, mLogger);

         //we could probably group these queries together...
         if (!helper.IsFlying() && gameActor.GetCollisionGeomType() == &dtCore::Transformable::CollisionGeomType::CUBE)
         {
            ClampToGround(helper.GetTranslationSmoothing(), xform, gameActor.GetGameActorProxy(), helper);
         }

         DoArticulation(helper, gameActor, tickMessage);

         //clear the updated flag.
         helper.ClearUpdated();
      }
   }

   void DeadReckoningComponent::DoArticulation(dtGame::DeadReckoningHelper& helper,
                                               const dtGame::GameActor& gameActor,
                                               const dtGame::TickMessage& tickMessage) const
   {
      if( helper.GetNodeCollector() == NULL )
      {
         return;
      }

      dtGame::DeadReckoningHelper::UpdateMode& um = helper.GetEffectiveUpdateMode(gameActor.IsRemote());
      if((um != DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR) )
      {
         return;
      }

      const std::list<dtCore::RefPtr<DeadReckoningHelper::DeadReckoningDOF> > containerDOFs = helper.GetDeadReckoningDOFs();
      std::list<dtCore::RefPtr<DeadReckoningHelper::DeadReckoningDOF> >::const_iterator endDOF = containerDOFs.end();

      std::list<dtCore::RefPtr<DeadReckoningHelper::DeadReckoningDOF> >::const_iterator iterDOF = containerDOFs.begin();
      for(; iterDOF != endDOF; ++iterDOF)
      {
         (*iterDOF)->mUpdate = false;
      }

      iterDOF = containerDOFs.begin();
      while(iterDOF != endDOF)
      {
         DeadReckoningHelper::DeadReckoningDOF *currentDOF = (*iterDOF).get();
         if(currentDOF->mPrev == NULL && !currentDOF->mUpdate)
         {
            currentDOF->mCurrentTime += tickMessage.GetDeltaSimTime();
            currentDOF->mUpdate = true;

            // there is something in the chain
            if(currentDOF->mNext != NULL)
            {
               osgSim::DOFTransform* dofTransform = helper.GetNodeCollector()->GetDOFByName(currentDOF->mName);
               if( dofTransform != NULL )
               {
                  DoArticulationSmooth(*dofTransform, currentDOF->mStartLocation, currentDOF->mNext->mStartLocation, currentDOF->mCurrentTime);
               }
            }
            else
            {
               osgSim::DOFTransform* dofTransform = helper.GetNodeCollector()->GetDOFByName(currentDOF->mName);
               if (dofTransform != NULL)
               {
                  DoArticulationPrediction(*dofTransform, currentDOF->mStartLocation, currentDOF->mRateOverTime, currentDOF->mCurrentTime);
               }
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
                  iterDOF = containerDOFs.begin();
                  changedList = true;
               }
               if (changedList)
                  continue;
            }

            // One second has passed, and this has more in its chain
            if(currentDOF->mNext != NULL && currentDOF->mCurrentTime >= 1)
            {
               osgSim::DOFTransform* ptr = helper.GetNodeCollector()->GetDOFByName(currentDOF->mName);
               if( ptr )
               {
                  currentDOF->mNext->mStartLocation = ptr->getCurrentHPR();
               }
               currentDOF->mNext->mCurrentTime = 0;

               // Get rid of the rotation
               helper.RemoveDRDOF(*currentDOF);
               // start over at the beginning of the DOF list.  Hence the use of the Update flag.
               iterDOF = containerDOFs.begin();
               continue;
            }                  
         }
         ++iterDOF;
      }
   } // end function DoArticulation

   void DeadReckoningComponent::DoArticulationSmooth(osgSim::DOFTransform& dofxform,
                                                     const osg::Vec3& currLocation,
                                                     const osg::Vec3& nextLocation,
                                                     float currentTimeStep) const
   {
      osg::Vec3 NewDistance = (nextLocation  - currLocation);

      for (int i = 0; i < 3; ++i)
      {
         while (NewDistance[i] > osg::PI)
            NewDistance[i] -= 2 * osg::PI;
         while (NewDistance[i] < -osg::PI)
            NewDistance[i] += 2 * osg::PI;
      }

      osg::Vec3 RateOverTime = (NewDistance * currentTimeStep);

      osg::Vec3 result(currLocation[0] + RateOverTime[0],
                       currLocation[1] + RateOverTime[1],
                       currLocation[2] + RateOverTime[2]);

      for (int i = 0; i < 3; ++i)
      {
         while (result[i] > 2 * osg::PI)
            result[i] -= 2 * osg::PI;

         while (result[i] < 0)
            result[i] += 2 * osg::PI;
      }

      dofxform.updateCurrentHPR(result);
   } // end function DoArticulationSmooth

   void DeadReckoningComponent::DoArticulationPrediction(osgSim::DOFTransform& dofxform, const osg::Vec3& currLocation, const osg::Vec3& currentRate, float currentTimeStep) const
   {
      osg::Vec3 result( currLocation[0] + (currentTimeStep * currentRate[0]),
                        currLocation[1] + (currentTimeStep * currentRate[1]),
                        currLocation[2] + (currentTimeStep * currentRate[2]));

      dofxform.updateCurrentHPR(result);
   } // end function DoArticulationPrediction

} // end namespace dtGame
