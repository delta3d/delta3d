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
#include <cmath>
#include <cfloat>

#include <dtGame/deadreckoningcomponent.h>
#include <dtGame/deadreckoninghelper.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>
#include <dtDAL/actortype.h>
#include <dtGame/gameactor.h>
#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/defaultgroundclamper.h>

#include <osgSim/DOFTransform>

namespace dtGame
{
   //////////////////////////////////////////////////////////////////////
   const std::string DeadReckoningComponent::DEFAULT_NAME("Dead Reckoning Component");

   //////////////////////////////////////////////////////////////////////
   DeadReckoningComponent::DeadReckoningComponent(const std::string& name): dtGame::GMComponent(name),
      mGroundClamper(new DefaultGroundClamper), mForceClampInterval(3.0f), mArticSmoothTime(0.5f)
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
            
         dtCore::Transformable* xformActor = mGroundClamper->GetEyePointActor();
         if (xformActor != NULL && message.GetAboutActorId() == xformActor->GetUniqueId())
         {
            mGroundClamper->SetEyePointActor(NULL);
         }
         
         xformActor = mGroundClamper->GetTerrainActor();
         if (xformActor != NULL && message.GetAboutActorId() == xformActor->GetUniqueId())
         {
            mGroundClamper->SetTerrainActor(NULL);
         }
      }
      else if (message.GetMessageType()  == dtGame::MessageType::INFO_MAP_UNLOADED)
      {
         mRegisteredActors.clear();
         mGroundClamper->SetEyePointActor(NULL);
         mGroundClamper->SetTerrainActor(NULL);
      }
   }

   //////////////////////////////////////////////////////////////////////
   dtCore::Transformable* DeadReckoningComponent::GetTerrainActor()
   {
      return mGroundClamper->GetTerrainActor();
   }

   //////////////////////////////////////////////////////////////////////
   const dtCore::Transformable* DeadReckoningComponent::GetTerrainActor() const
   {
      return mGroundClamper->GetTerrainActor();
   }
   
   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::SetTerrainActor(dtCore::Transformable* newTerrain)
   {
      mGroundClamper->SetTerrainActor(newTerrain);
   }

   //////////////////////////////////////////////////////////////////////
   dtCore::Transformable* DeadReckoningComponent::GetEyePointActor()
   {
      return mGroundClamper->GetEyePointActor();
   }

   //////////////////////////////////////////////////////////////////////
   const dtCore::Transformable* DeadReckoningComponent::GetEyePointActor() const
   {
      return mGroundClamper->GetEyePointActor();
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::SetEyePointActor(dtCore::Transformable* newEyePointActor)
   {
      mGroundClamper->SetEyePointActor(newEyePointActor);
   }
   
   //////////////////////////////////////////////////////////////////////
   void DeadReckoningComponent::SetGroundClamper( dtGame::BaseGroundClamper& clamper )
   {
      mGroundClamper = &clamper;
   }

   //////////////////////////////////////////////////////////////////////
   BaseGroundClamper& DeadReckoningComponent::GetGroundClamper()
   {
      return *mGroundClamper;
   }

   //////////////////////////////////////////////////////////////////////
   const BaseGroundClamper& DeadReckoningComponent::GetGroundClamper() const
   {
      return *mGroundClamper;
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

            // DEBUG:
            osg::Vec3 pos;
            xform.GetTranslation( pos );
            std::cout << "\n\tDR setting trans to: " << pos.z() << "\n\n";
         }
      }

      // If the actor is local, don't move it, and force the 
      // helper to match as if it was just updated.
      if (helper.GetEffectiveUpdateMode(toRegister.GetGameActor().IsRemote()) 
         == DeadReckoningHelper::UpdateMode::CALCULATE_ONLY)
      {
         dtCore::Transform xform;
         toRegister.GetGameActor().GetTransform(xform, dtCore::Transformable::REL_CS);

         osg::Vec3 pos;
         xform.GetTranslation(pos);
         helper.SetLastKnownTranslation(pos);
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
      mGroundClamper->UpdateEyePoint();

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<DeadReckoningHelper> >::iterator i = mRegisteredActors.begin();
         i != mRegisteredActors.end(); ++i)
      {

         dtGame::GameActorProxy* gameActorProxy = GetGameManager()->FindGameActorById(i->first);
         if (gameActorProxy == NULL)
            continue;

         dtGame::GameActor& gameActor = gameActorProxy->GetGameActor();
         DeadReckoningHelper& helper = *i->second;

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Dead Reckoning actor named \"%s\" with ID \"%s\" and type \"%s.\"",
               gameActor.GetName().c_str(), gameActor.GetUniqueId().ToString().c_str(),
               gameActorProxy->GetActorType().GetFullName().c_str());
         }

         dtCore::Transform xform;
         //Init the transform with the last deadreckoned position, not
         //the current actual position, because the current actual can be clamped
         xform.SetTranslation(helper.GetCurrentDeadReckonedTranslation());
         xform.SetRotation(helper.GetCurrentDeadReckonedRotation());

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
         BaseGroundClamper::GroundClampingType* groundClampingType = &BaseGroundClamper::GroundClampingType::NONE;
         bool bTransformChanged = helper.DoDR(gameActor, xform, mLogger, groundClampingType);

         //Only actually ground clamp and move remote ones.
         if (helper.GetEffectiveUpdateMode(gameActor.IsRemote()) 
               == DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR)
         {
            bool bForceClamp = ShouldForceClamp(helper, tickMessage.GetDeltaRealTime(), bTransformChanged);
            
            // if the actor moved, it's time to force clamp, or we are using intermittent, 
            // we should clamp.
            // The clamping also applies the transform when the clamping type is none.
            // force clamp is silly when using intermittent because it's already running intermittently.
            if (bTransformChanged || *groundClampingType == BaseGroundClamper::GroundClampingType::INTERMITTENT_SAVE_OFFSET
                  || (bForceClamp && *groundClampingType != BaseGroundClamper::GroundClampingType::NONE))
            {
               //we could probably group these queries together...
               mGroundClamper->ClampToGround(*groundClampingType, tickMessage.GetSimulationTime(),
                        xform, gameActor.GetGameActorProxy(), helper.GetGroundClampingData());

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
      mGroundClamper->FinishUp();
   }

   void DeadReckoningComponent::DoArticulation(dtGame::DeadReckoningHelper& helper,
                                               const dtGame::GameActor& gameActor,
                                               const dtGame::TickMessage& tickMessage) const
   {
      if( helper.GetNodeCollector() == NULL )
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
