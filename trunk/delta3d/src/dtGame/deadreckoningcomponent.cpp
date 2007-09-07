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
#include <dtCore/batchisector.h>
#include <dtCore/boundingboxvisitor.h>
#include <dtDAL/actortype.h>
#include <dtGame/gameactor.h>
#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>
#include <dtGame/exceptionenum.h>

#include <osgSim/DOFTransform>

namespace dtGame
{
   //////////////////////////////////////////////////////////////////////
   const std::string DeadReckoningComponent::DEFAULT_NAME("Dead Reckoning Component");

   //////////////////////////////////////////////////////////////////////
   DeadReckoningComponent::DeadReckoningComponent(const std::string& name): dtGame::GMComponent(name),
      mHighResClampRange(0.0f), mHighResClampRange2(0.0f), mForceClampInterval(3.0f), mArticSmoothTime(0.5f)
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
   dtCore::BatchIsector& DeadReckoningComponent::GetGroundClampIsector()
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
            toRegister.GetGameActor().GetTransform(xform, dtCore::Transformable::REL_CS);
            xform.SetTranslation(helper.GetLastKnownTranslation());
            xform.SetRotation(helper.GetLastKnownRotation());
            toRegister.GetGameActor().SetTransform(xform, dtCore::Transformable::REL_CS);
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
         toRegister.GetGameActor().GetTransform(xform, dtCore::Transformable::REL_CS);

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
   void DeadReckoningComponent::CalculateAndSetBoundingBox(osg::Vec3& modelDimensions,
         dtGame::GameActorProxy& gameActorProxy, DeadReckoningHelper& helper)
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
               "Determining Actor dimensions via Bounding Box visitor.");
      }
      osg::Matrix oldMatrix = gameActorProxy.GetGameActor().GetMatrix();
      gameActorProxy.GetGameActor().SetMatrix( osg::Matrix::identity() );

      dtCore::BoundingBoxVisitor bbv;
      gameActorProxy.GetGameActor().GetOSGNode()->accept(bbv);

      gameActorProxy.GetGameActor().SetMatrix(oldMatrix);

      modelDimensions.x() = bbv.mBoundingBox.xMax() - bbv.mBoundingBox.xMin();
      modelDimensions.y() = bbv.mBoundingBox.yMax() - bbv.mBoundingBox.yMin();
      modelDimensions.z() = bbv.mBoundingBox.zMax() - bbv.mBoundingBox.zMin();
      helper.SetModelDimensions(modelDimensions);      
   }
   
   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::ClampToGroundThreePoint(float timeSinceUpdate, dtCore::Transform& xform,
      dtGame::GameActorProxy& gameActorProxy, DeadReckoningHelper& helper)
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Using three point ground clamping.");
      }

      osg::Vec3& position = xform.GetTranslation();
      osg::Matrix& rotation = xform.GetRotation();

      osg::Vec3 modelDimensions = helper.GetModelDimensions(); 
      if (!helper.UseModelDimensions())
      {
         CalculateAndSetBoundingBox(modelDimensions, gameActorProxy, helper);
      }
      
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
               "Actor dimensions [%f, %f, %f].", 
               modelDimensions[0], modelDimensions[1], modelDimensions[2]);
      }

      //make points for the front center and back corners in relative coordinates.
      osg::Vec3 points[3];
      points[0].set(0.0f, modelDimensions[1] / 2, 0.0f);
      points[1].set(modelDimensions[0] / 2, -(modelDimensions[1] / 2), 0.0f);
      points[2].set(-(modelDimensions[0] / 2), -(modelDimensions[1] / 2), 0.0f);

      const osg::Matrix& m = gameActorProxy.GetGameActor().GetMatrix();

      mTripleIsector->Reset();
      mTripleIsector->SetQueryRoot(mTerrainActor.get());

      for (unsigned i = 0; i < 3; ++i)
      {
         dtCore::BatchIsector::SingleISector& single = mTripleIsector->EnableAndGetISector(i);

         //convert point to absolute space.
         points[i] = points[i] * m;
         const osg::Vec3& singlePoint = points[i];
         
         if (osg::isNaN(singlePoint.x()) || osg::isNaN(singlePoint.y()) || osg::isNaN(singlePoint.z()))
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, 
                     "Intersect point has parts that are NAN, no ground clamping will be performed "
                     "on actor named \"%s\".", gameActorProxy.GetName().c_str());
            } 
            return;
         }
         
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
                  "Intersect point %d [%f, %f, %f].", i, singlePoint.x(), singlePoint.y(), singlePoint.z());
         } 
         single.SetSectorAsLineSegment(osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] + 100.0f),
               osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] - 100.0f));
      }
      
      if (mTripleIsector->Update(mCurrentEyePointABSPos, GetEyePointActor() == NULL))
      {
         for (unsigned i = 0; i < 3; ++i)
         {
            dtCore::BatchIsector::SingleISector& single = mTripleIsector->EnableAndGetISector(i);
            if (single.GetNumberOfHits() > 0)
            {
               osg::Vec3 hp;
               //overwrite the point with the hit point.
               single.GetHitPoint(hp, 0);

               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  std::ostringstream ss;
                  ss << "Found a hit - old z \"" << points[i].z() << "\" new z \"" << hp.z() << "\".";
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
               }
               points[i] = hp;
            }
            else
            {
               std::ostringstream ss;
               ss << "Found no hit on line segment [" << i <<  "] on points:";
               for (unsigned i = 0; i < 3; ++i)
               {
                  ss << " [" << points[i] << "]";
               }
               
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
               
            }
         }
      } 
      else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Found no hit with eye point [" << mCurrentEyePointABSPos << "] on points:";
         for (unsigned i = 0; i < 3; ++i)
         {
            ss << " [" << points[i] << "]";
         }
         
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }
      
      float averageZ = 0;
      for (unsigned i = 0; i < 3; ++i)
      {
         averageZ += points[i].z();
      }
      averageZ /= 3;
      
      //move the actor position up to the ground.
      position.z() = averageZ;
      
      osg::Vec3 ab = points[0] - points[2];
      osg::Vec3 ac = points[0] - points[1];

      osg::Vec3 normal = ab ^ ac;
      normal.normalize();

      osg::Vec3 oldNormal ( 0, 0, 1 );

      oldNormal = osg::Matrix::transform3x3(oldNormal, rotation);
      osg::Matrix normalRot;
      normalRot.makeRotate(oldNormal, normal);

      rotation = rotation * normalRot;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::RunClampBatch()
   {      
      if (mGroundClampBatch.empty())
         return;
      
      if (mGroundClampBatch.size() > 32)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Attempted to batch %u entities, when 32 is the max.", mGroundClampBatch.size());        
      }
      
      //I don't really like doing this, but there doesn't seem to be an efficient way to 
      //reuse the ones that exist.  This should be revisited. - DID - AD 5/1/2007
      //mIsector->DeleteAllISectors();
      mIsector->Reset();
      mIsector->SetQueryRoot(mTerrainActor.get());

      for (unsigned i = 0; i < mGroundClampBatch.size(); ++i)
      {
         dtCore::BatchIsector::SingleISector& single = mIsector->EnableAndGetISector(i);
         
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Using one point ground clamping.");
         }
         
         dtCore::Transform& xform = mGroundClampBatch[i].first;
         osg::Vec3& singlePoint = xform.GetTranslation();
         
         single.SetSectorAsLineSegment(osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] + 100.0f),
               osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] - 100.0f));
      }
      
      if (mIsector->Update(mCurrentEyePointABSPos, GetEyePointActor() == NULL))
      {
         osg::Vec3 normal;
         osg::Vec3 hp;
         for (unsigned i = 0; i < mGroundClampBatch.size(); ++i)
         {
            dtCore::Transform& xform = mGroundClampBatch[i].first;
            osg::Matrix& rotation = xform.GetRotation();
            osg::Vec3& singlePoint = xform.GetTranslation();

            dtCore::BatchIsector::SingleISector& single = mIsector->EnableAndGetISector(i);
            if (single.GetNumberOfHits() > 0)
            {
               single.GetHitPoint(hp, 0);
               single.GetHitPointNormal(normal, 0);
                  
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  std::ostringstream ss;
                  ss << "Found a hit - old z " << singlePoint.z() << " new z " << hp.z();
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
               }
               
               singlePoint = hp;
   
               normal.normalize();
   
               osg::Vec3 oldNormal(0, 0, 1);
   
               oldNormal = osg::Matrix::transform3x3(oldNormal, rotation);
               osg::Matrix normalRot;
               normalRot.makeRotate(oldNormal, normal);
   
               rotation = rotation * normalRot;

               mGroundClampBatch[i].second->GetGameActor().SetTransform(xform, dtCore::Transformable::REL_CS);
            }
         }
      } 
      else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Found no hits with batch query.";
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }
      mGroundClampBatch.clear();
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::ClampToGround(float timeSinceUpdate, dtCore::Transform& xform,
      dtGame::GameActorProxy& gameActorProxy, DeadReckoningHelper& helper)
   {
      if (GetTerrainActor() == NULL)
      {
         //No terrain? just set the position and exit
         gameActorProxy.GetGameActor().SetTransform(xform, dtCore::Transformable::REL_CS);
         return;
      }

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Ground clamping actor.");
      }

      osg::Vec3& position = xform.GetTranslation();

      const osg::Vec3 vec = GetLastEyePoint();
      if (GetEyePointActor() != NULL 
            && GetHighResGroundClampingRange() > 0.0f
            && (position - vec).length2() > mHighResClampRange2)
      {
         // this should be moved.
         mGroundClampBatch.reserve(32);
         mGroundClampBatch.push_back(std::make_pair(xform, &gameActorProxy));
         if (mGroundClampBatch.size() == 32)
            RunClampBatch();
      }
      else
      {
         ClampToGroundThreePoint(timeSinceUpdate, xform, gameActorProxy, helper);
         position.z() += helper.GetGroundOffset();
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "New ground-clamped actor position [" << position << "].";
            
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }
         
         gameActorProxy.GetGameActor().SetTransform(xform, dtCore::Transformable::REL_CS);
      }
      
   }

   bool DeadReckoningComponent::ShouldForceClamp(DeadReckoningHelper& helper, float deltaRealTime, bool bTransformChanged)
   {
      bool bForceClamp = false;
      if (mForceClampInterval > 0)
      {
         float newForceClampInterval = helper.GetTimeUntilForceClamp() - deltaRealTime;
         if (bTransformChanged)
         {   
            newForceClampInterval = mForceClampInterval;
         }
         else
         {  
            bForceClamp = newForceClampInterval < 0;

            // reset the force clamp time if the transformed, which means it will be clamped anyway
            // or the timer has elapsed
            if (bForceClamp)
               newForceClampInterval = mForceClampInterval;
         }
         
         helper.SetTimeUntilForceClamp(newForceClampInterval);
      }
      return bForceClamp;
   }
   
   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::TickRemote(const dtGame::TickMessage& tickMessage)
   {
      if(!mIsector.valid())
         mIsector = new dtCore::BatchIsector;
      
      if(!mTripleIsector.valid())
         mTripleIsector = new dtCore::BatchIsector;

      //Setup the iSector to use the player position only once so that get transform is not called
      //for every single actor to be clamped.
      if(GetEyePointActor() != NULL)
      {
         dtCore::Transform xform;
         GetEyePointActor()->GetTransform(xform, dtCore::Transformable::ABS_CS);
         mCurrentEyePointABSPos = xform.GetTranslation();

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            osg::Vec3& debugPos = xform.GetTranslation();

            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Setting the eye point to the position %f, %f, %f.",
               debugPos.x(), debugPos.y(), debugPos.z());
         }
      }
      else if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {         
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Setting the isector to not use an eye point.");
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
         gameActor.GetTransform(xform, dtCore::Transformable::REL_CS);

         if (helper.IsUpdated())
         {
            //Pretend we were updated on the last tick so we have time delta to work with
            //when calculating movement.
            if ( helper.IsTranslationUpdated() )
            {
               helper.SetLastTranslationUpdatedTime(tickMessage.GetSimulationTime() - tickMessage.GetDeltaSimTime());
               //helper.SetLastTranslationUpdatedTime(helper.mLastTimeTag);
               helper.SetTranslationCurrentSmoothingTime( 0.0 );
            }

            if ( helper.IsRotationUpdated() )
            {
               helper.SetLastRotationUpdatedTime(tickMessage.GetSimulationTime() - tickMessage.GetDeltaSimTime());
               //helper.SetLastRotationUpdatedTime(helper.mLastTimeTag);
               helper.SetRotationCurrentSmoothingTime( 0.0 );
               helper.SetRotationResolved( false );
            }
         }

         //We want to do this every time.
         helper.SetTranslationCurrentSmoothingTime( helper.GetTranslationCurrentSmoothingTime() + tickMessage.GetDeltaSimTime() );
         helper.SetRotationCurrentSmoothingTime( helper.GetRotationCurrentSmoothingTime() + tickMessage.GetDeltaSimTime() );


         //make sure it's greater than 0 in case of time being set.
         if (helper.GetTranslationCurrentSmoothingTime() < 0.0) 
            helper.SetTranslationCurrentSmoothingTime( 0.0 );
         if (helper.GetRotationCurrentSmoothingTime() < 0.0) 
            helper.SetRotationCurrentSmoothingTime( 0.0 );

         //actual dead reckoning code moved into the helper..
         bool bShouldGroundClamp = false;
         bool bTransformChanged = helper.DoDR(gameActor, xform, mLogger, bShouldGroundClamp);

         //Only actually ground clamp and move remote ones.
         if (helper.GetEffectiveUpdateMode(gameActor.IsRemote()) 
               == DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR)
         {
            bool bForceClamp = ShouldForceClamp(helper, tickMessage.GetDeltaRealTime(), bTransformChanged);
            
            if (bTransformChanged || (bForceClamp && bShouldGroundClamp))
            {
               //we could probably group these queries together...
               if (bShouldGroundClamp)
               {
                  ClampToGround(helper.GetTranslationCurrentSmoothingTime(), xform, gameActor.GetGameActorProxy(), helper);
               }
               else
               {
                  gameActor.SetTransform(xform, dtCore::Transformable::REL_CS);
               }
               
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  std::ostringstream ss;
                  ss << "Actor " << gameActor.GetUniqueId() << " - " << gameActor.GetName() << " has attitude "
                     << "\"" << helper.GetCurrentDeadReckonedRotation() << "\" and position \"" << helper.GetCurrentDeadReckonedTranslation() << "\" at time " 
                     << helper.GetLastRotationUpdatedTime() +  helper.GetRotationCurrentSmoothingTime() << "";
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
                        ss.str().c_str());               
               }
            }
         }

         DoArticulation(helper, gameActor, tickMessage);

         //clear the updated flag.
         helper.ClearUpdated();
      }
      
      //Make sure the last of them ran.
      RunClampBatch();
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

      const std::list<dtCore::RefPtr<DeadReckoningHelper::DeadReckoningDOF> >& containerDOFs = helper.GetDeadReckoningDOFs();
      std::list<dtCore::RefPtr<DeadReckoningHelper::DeadReckoningDOF> >::const_iterator endDOF = containerDOFs.end();

      std::list<dtCore::RefPtr<DeadReckoningHelper::DeadReckoningDOF> >::const_iterator iterDOF = containerDOFs.begin();
      for(; iterDOF != endDOF; ++iterDOF)
      {
         (*iterDOF)->mUpdate = false;
      }

      // Loop through all the DR stops and remove all those that precede the
      // second to last stop; smoothing should only occur between 2 stops,
      // so the latest stops should be used because they contain the latest data.
      std::vector<DeadReckoningHelper::DeadReckoningDOF*> deletableDRDOFs;
      iterDOF = containerDOFs.begin();
      while(iterDOF != endDOF)
      {
         // an element is a middle man if there are 3 in the chain (ie current->next->next != NULL).
         if((*iterDOF)->mNext != NULL && (*iterDOF)->mNext->mNext != NULL)
         {
            // delete the current
            deletableDRDOFs.push_back(iterDOF->get());
         }
         ++iterDOF; // goto next element
      }

      // Now delete all stops that are unneeded.
      unsigned limit = deletableDRDOFs.size();
      for( unsigned i = 0; i < limit; ++i )
      {
         helper.RemoveDRDOF(*deletableDRDOFs[i]);
      }
      deletableDRDOFs.clear();


      iterDOF = containerDOFs.begin();
      while(iterDOF != endDOF)
      {
         DeadReckoningHelper::DeadReckoningDOF *currentDOF = (*iterDOF).get();

         // Only process the first DR stop in the chain so that subsequent 
         // stops will be used as blending targets.
         if(currentDOF->mPrev == NULL && !currentDOF->mUpdate)
         {
            currentDOF->mCurrentTime += tickMessage.GetDeltaSimTime();
            currentDOF->mUpdate = true;

            // Smooth time has completed, and this has more in its chain
            if( currentDOF->mNext != NULL && currentDOF->mCurrentTime >= mArticSmoothTime )
            {
               osgSim::DOFTransform* ptr = helper.GetNodeCollector()->GetDOFTransform(currentDOF->mName);
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


            // there is something in the chain
            if(currentDOF->mNext != NULL)
            {
               osgSim::DOFTransform* dofTransform = helper.GetNodeCollector()->GetDOFTransform(currentDOF->mName);
               if( dofTransform != NULL )
               {
                  DoArticulationSmooth(*dofTransform, currentDOF->mStartLocation,
                     currentDOF->mNext->mStartLocation, currentDOF->mCurrentTime/mArticSmoothTime);
                  // NOTE: The division by mArticSmoothTime counter balances the mArticSmoothTime check
                  // in the previous code block that attempts to remove the first DR stop. If the time
                  // was not magnified by the reciprocal of mArticSmoothTime, then the smoothing would
                  // never have a full transition over the time set for smoothing because it would stop
                  // prematurely at mArticSmoothTime (which could be less or more than 1.0).
                  //
                  // In other words, the reciprocal counter-scales the smoothing time down to 1.0.
                  // 0.0 to 1.0 is the range of transition between any two stops, no matter what the
                  // specified smooth time is.
                  //
                  // Essentially, the time step is scaled in relation to the smooth time to find its
                  // ratio of transition.
                  //
                  // IE. smoothTime = 2, timeStep = 0.5.
                  // timeStep/smoothTime = 0.25.
                  // 0.5 is 25% of 2, thus timeStep causes a 25% transition from the first stop to the end stop.
               }
            }
            else
            {
               osgSim::DOFTransform* dofTransform = helper.GetNodeCollector()->GetDOFTransform(currentDOF->mName);
               if (dofTransform != NULL)
               {
                  DoArticulationPrediction(*dofTransform, currentDOF->mStartLocation,
                     currentDOF->mRateOverTime, currentDOF->mCurrentTime);
               }
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
      osg::Vec3 NewDistance = (nextLocation - currLocation);

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

   void DeadReckoningComponent::SetArticulationSmoothTime( float smoothTime )
   {
      mArticSmoothTime = smoothTime <= 0.0f ? 1.0f : smoothTime;
   }

} // end namespace dtGame
