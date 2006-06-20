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
 * @author David Guthrie
 */
#include <dtGame/deadreckoningcomponent.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtCore/isector.h>
#include <dtGame/gameactor.h>
#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>

#include "dtGame/exceptionenum.h"

namespace dtGame
{

   IMPLEMENT_ENUM(DeadReckoningAlgorithm);
   DeadReckoningAlgorithm DeadReckoningAlgorithm::NONE("None");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::STATIC("Static");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::VELOCITY_ONLY("Velocity Only");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION("Velocity and Acceleration");


   DeadReckoningHelper::DeadReckoningHelper() :
      mUpdated(false), mFlying(false), mLastUpdatedTime(0.0), mSmoothingSteps(0.0f), mMinDRAlgorithm(&DeadReckoningAlgorithm::NONE) 
   {}

   void DeadReckoningHelper::SetFlying(bool newFlying)
   {
      if (mFlying == newFlying)
         return;
         
      mFlying = newFlying;
      mUpdated = true;
   }

   void DeadReckoningHelper::SetDeadReckoningAlgorithm(DeadReckoningAlgorithm& newAlgorithm)
   {
      if (*mMinDRAlgorithm == newAlgorithm)
         return;
      mMinDRAlgorithm = &newAlgorithm;
      mUpdated = true; 
   }

   void DeadReckoningHelper::SetLastKnownTranslation(const osg::Vec3 &vec) 
   { 
      mLastTranslation = vec; 
      mUpdated = true; 
   }

   void DeadReckoningHelper::SetLastKnownRotation(const osg::Vec3 &vec) 
   { 
      mLastRotation = vec;
      
      dtCore::Transform xform;
      //Get the last rotation as a quaternion and a matrix;
      xform.SetRotation(vec);
      xform.GetRotation(mLastRotationMatrix);
      mLastRotationMatrix.get(mLastQuatRotation); 
      mUpdated = true; 
   }

   void DeadReckoningHelper::SetVelocityVector(const osg::Vec3 &vec)
   {
      mVelocityVector = vec;
      mUpdated = true; 
   }
         
   void DeadReckoningHelper::SetAccelerationVector(const osg::Vec3 &vec)
   {
      mAccelerationVector = vec;
      mUpdated = true; 
   }
   
   void DeadReckoningHelper::SetAngularVelocityVector(const osg::Vec3 &vec)
   {
      mAngularVelocityVector = vec;
      mUpdated = true; 
   }      
   
  
   const float DeadReckoningComponent::ForceClampTime(1.25f);

   DeadReckoningComponent::DeadReckoningComponent(const std::string& name): dtGame::GMComponent(name), 
      mTerrainActorName("Terrain"), mTerrainQueried(false), mTimeUntilForceClamp(ForceClampTime)
   {
      mLogger = &dtUtil::Log::GetInstance("deadreckoningcomponent.cpp");
   }
   
   DeadReckoningComponent::~DeadReckoningComponent()
   {
   }

   void DeadReckoningComponent::ProcessMessage(const dtGame::Message& message)
   {
      if (message.GetMessageType() == dtGame::MessageType::TICK_REMOTE)
      {
         const dtGame::TickMessage& tickMessage = static_cast<const dtGame::TickMessage&>(message);
         TickRemote(tickMessage);
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_PLAYER_ENTERED_WORLD)
      {
         mPlayerActor = GetGameManager()->FindGameActorById(message.GetAboutActorId());
         if (!mPlayerActor.valid())
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, 
               "A player entered world message was received without a valid actor Id.");
         }
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
      {
         dtGame::GameActorProxy* mActor = GetGameManager()->FindGameActorById(message.GetAboutActorId());
         if (mActor != NULL)
            UnregisterActor(*mActor);
      }
   }

   void DeadReckoningComponent::DispatchNetworkMessage(const dtGame::Message& message)
   {
   }
   
   dtDAL::ActorProxy* DeadReckoningComponent::GetTerrainActor()
   {
      if (!mTerrainQueried)
      {
         std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > terrains;
         GetGameManager()->FindActorsByName(GetTerrainActorName(), terrains);
         
         if (terrains.size() == 0)
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, "Unable to find terrain named \"Terrain\", no terrain following will be done.");
            }
         }
         else
         {
            mTerrainActor = terrains[0];
         }
         
         mTerrainQueried = true;
      }
      
      return mTerrainActor.get();
   }
   
   void DeadReckoningComponent::SetTerrainActorName(const std::string& newTerrainActorName)
   {
      if (mTerrainActorName == newTerrainActorName)
         return;
         
      mTerrainActorName = newTerrainActorName;
      mTerrainQueried = false;
      mTerrainActor = NULL;
   }
   
   void DeadReckoningComponent::RegisterActor(dtGame::GameActorProxy& toRegister, DeadReckoningHelper& helper) throw(dtUtil::Exception)
   {
      if (!mRegisteredActors.insert(std::make_pair(&toRegister, &helper)).second)
      {
         EXCEPT(ExceptionEnum::DEAD_RECKONING_EXCEPTION,
            "Actor \"" + toRegister.GetName() + 
            "\" is already registered with a helper in the DeadReckoingComponent with name \"" + 
            GetName() +  ".\"" );  
      }
      else if (helper.IsUpdated())
      {
         dtCore::Transform xform;
         toRegister.GetGameActor().GetTransform(&xform);  
         xform.SetTranslation(helper.GetLastKnownTranslation());             
         xform.SetRotation(helper.GetLastKnownRotation());             
         toRegister.GetGameActor().SetTransform(&xform);
         helper.mTransBeforeLastUpdate = helper.GetLastKnownTranslation();
         helper.mRotQuatBeforeLastUpdate = helper.mLastQuatRotation;
      }
   }

   void DeadReckoningComponent::UnregisterActor(dtGame::GameActorProxy& toRegister)
   {
      std::map<dtCore::RefPtr<dtGame::GameActorProxy>, dtCore::RefPtr<DeadReckoningHelper> >::iterator itor;
      itor = mRegisteredActors.find(&toRegister);
      if (itor != mRegisteredActors.end())
      {
         mRegisteredActors.erase(itor);
      }
   }
   
   bool DeadReckoningComponent::IsRegisteredActor(dtGame::GameActorProxy& gameActorProxy)
   {
      std::map<dtCore::RefPtr<dtGame::GameActorProxy>, dtCore::RefPtr<DeadReckoningHelper> >::iterator itor;
      itor = mRegisteredActors.find(&gameActorProxy);
      return itor != mRegisteredActors.end();
   }

   double DeadReckoningComponent::GetTerrainZIntersectionPoint(dtCore::DeltaDrawable& terrainActor, const osg::Vec3& point)
   {   
      mIsector->Reset();
      
      mIsector->SetStartPosition(osg::Vec3(point[0], point[1], point[2] + 100.0f));
      mIsector->SetEndPosition(osg::Vec3(point[0], point[1], point[2] - 100.0f));
      
      mIsector->SetGeometry(&terrainActor);

      if (mIsector->Update())
      {
         osg::Vec3 hp;
         mIsector->GetHitPoint(hp);
         
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
      return point.z();
   }

   void DeadReckoningComponent::ClampToGround(float timeSinceUpdate, osg::Vec3& position, osg::Matrix& rotation, dtCore::Transform& xform, dtGame::GameActorProxy& gameActorProxy)
   {
      if (GetTerrainActor() == NULL)
         return;

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Ground clamping actor.");
      }
                  
      std::vector<float> dimensions;
      gameActorProxy.GetGameActor().GetCollisionGeomDimensions(dimensions);
      //make points for the front center and back corners in relative coordinates.
      osg::Vec3 point1(0, dimensions[1] / 2, 0.0),
         point2(dimensions[0] / 2, -(dimensions[1] / 2), 0.0),
         point3(-(dimensions[0] / 2), -(dimensions[1] / 2), 0.0);
                                             
      //osg::Matrix mInverse = osg::Matrix::inverse(m);
      dtCore::RefPtr<osg::Vec3Array> vertices = new osg::Vec3Array;
      vertices->push_back(point1);
      vertices->push_back(point2);
      vertices->push_back(point3);
      
      osg::Matrix m;
      xform.Get(m);
      
      //convert points to absolute space.
      point1 = point1 * m;
      point2 = point2 * m;
      point3 = point3 * m;
      
      point1.z() = GetTerrainZIntersectionPoint(*mTerrainActor->GetActor(), point1);
      point2.z() = GetTerrainZIntersectionPoint(*mTerrainActor->GetActor(), point2);
      point3.z() = GetTerrainZIntersectionPoint(*mTerrainActor->GetActor(), point3);
      
      float averageZ = (point1.z() + point2.z() + point3.z()) / 3;
      
      if (averageZ >= position.z())
      {
         //move the actor position up to the ground.
         position.z() = averageZ;
      }
      else 
      {
         //float zVal = position.z() - (9.8f / 2.0f) * (timeSinceUpdate * timeSinceUpdate);
         //fall by the acceleration of gravity.
         //if (zVal < averageZ)
         position.z() = averageZ;
         //else
         //   position.z() = zVal;
      }
      
      osg::Vec3 ab = point1 - point3;
      osg::Vec3 ac = point1 - point2;
      
      osg::Vec3 normal = ab ^ ac;
      normal.normalize();
      
      osg::Vec3 oldNormal ( 0, 0, 1 );
      
      osg::Matrix normalRot;
      normalRot.makeRotate(oldNormal, normal);

      xform.SetRotation(rotation);
      osg::Vec3 rotV;
      xform.GetRotation(rotV);
      //clear pitch and roll.
      rotV[1] = 0.0f;
      rotV[2] = 0.0f;
      xform.SetRotation(rotV);
      xform.GetRotation(rotation);
      
      rotation = normalRot * rotation;
   }

   void DeadReckoningComponent::TickRemote(const dtGame::TickMessage& tickMessage)
   {            

      if (mIsector == NULL)
         mIsector = new dtCore::Isector;
      
      //Setup the iSector to use the player position only once so that get transform is not called
      //for every single actor to be clamped.
      if (mPlayerActor.valid())
      {
         dtCore::Transform xform;
         mPlayerActor->GetGameActor().GetTransform(&xform);
         mIsector->SetEyePoint(xform.GetTranslation());
         mIsector->SetUseEyePoint(true);
         
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            osg::Vec3& debugPos = xform.GetTranslation();
            
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Setting the eye point to the player position %f, %f, %f.",
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

      for (std::map<dtCore::RefPtr<dtGame::GameActorProxy>, dtCore::RefPtr<DeadReckoningHelper> >::iterator i = mRegisteredActors.begin();
         i != mRegisteredActors.end(); ++i)
      {

         dtGame::GameActorProxy& gameActorProxy = const_cast<dtGame::GameActorProxy&> (*i->first);
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
         gameActor.GetTransform(&xform);
         osg::Vec3& pos = xform.GetTranslation();
         osg::Matrix& rot = xform.GetRotation();

         if (helper.IsUpdated())
         {
            //Pretend we were updated on the last tick so we have time delta to work with
            //when calculating movement.
            helper.mLastUpdatedTime = tickMessage.GetSimulationTime() - tickMessage.GetDeltaSimTime(); 
            helper.mTransBeforeLastUpdate = pos;
            rot.get(helper.mRotQuatBeforeLastUpdate);
            helper.mSmoothingSteps = 0.1;
         }

         bool forceClamp = false;
   
         double timeSinceUpdate = tickMessage.GetSimulationTime() - helper.mLastUpdatedTime;
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
         dtUtil::Clamp(timeSinceUpdate, 0.0, timeSinceUpdate);

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
               xform.SetTranslation(helper.mLastTranslation);
               xform.SetRotation(helper.mLastRotationMatrix); 
               
               gameActor.SetTransform(&xform);
               helper.mTransBeforeLastUpdate = helper.mLastTranslation;
               helper.mRotQuatBeforeLastUpdate = helper.mLastQuatRotation;
               
               if (!helper.IsFlying() && gameActor.GetCollisionGeomType() == &dtCore::Transformable::CollisionGeomType::CUBE)
               {
                  ClampToGround(timeSinceUpdate, pos, rot, xform, gameActorProxy);
               } 

               xform.SetTranslation(pos);
               xform.SetRotation(rot);
               gameActor.SetTransform(&xform);
            }
         }
         else
         {
            osg::Vec3 unclampedTranslation = pos;
            unclampedTranslation.z() = helper.mLastTranslation.z();

            //avoid the sqrtf by using length2.
            //we went to see if all this dr and ground clamping stuff has to be done.
            if (forceClamp || helper.IsUpdated() || helper.mLastTranslation != unclampedTranslation || 
               (helper.IsFlying() && rot != helper.mLastRotationMatrix) || 
               helper.mVelocityVector.length2() > 1e-2 || 
               (helper.GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION 
                  && helper.mAccelerationVector.length2() > 1e-2))
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Actor passed optimization checks: fully dead-reckoning actor.");
               }
             
               osg::Vec3 positionChange;
               if (helper.GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_ONLY)               
                  positionChange = helper.mVelocityVector * timeSinceUpdate;
               else
                  positionChange = helper.mVelocityVector * timeSinceUpdate + 
                     ((helper.mAccelerationVector * 0.5f) * (timeSinceUpdate * timeSinceUpdate));
   
               osg::Vec3 drPos = helper.mLastTranslation + positionChange;  
   
               osg::Quat newRot;
               
               if (rot != helper.mLastRotationMatrix && helper.mSmoothingSteps < 3.0)
               {
                  float smoothingFactor = helper.mSmoothingSteps/3.0f;           
                  newRot.slerp(smoothingFactor, helper.mRotQuatBeforeLastUpdate, helper.mLastQuatRotation); 
                  rot.set(newRot);
               }
               else
               {
                  rot.set(helper.mLastQuatRotation);
               }

               if (drPos != helper.mLastTranslation && helper.mSmoothingSteps < 10.0)
               {
                  pos = helper.mTransBeforeLastUpdate + positionChange;    
                  float smoothingFactor = helper.mSmoothingSteps/10.0f;           
                  //a bit of smoothing.
                  pos = pos + (drPos - pos) * smoothingFactor;
                  helper.mSmoothingSteps += tickMessage.GetDeltaSimTime();                  
               }
               else
               {
                  pos = drPos;
               }
               
               if (!helper.IsFlying() && gameActor.GetCollisionGeomType() == &dtCore::Transformable::CollisionGeomType::CUBE)
               {
                  ClampToGround(timeSinceUpdate, pos, rot, xform, gameActorProxy);
               } 
               
               xform.SetTranslation(pos);
               xform.SetRotation(rot);
               gameActor.SetTransform(&xform);
            }
            else
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Entity does not need to be Dead Reckoned");
               }
            }
            
         }
         //clear the updated flag.
         helper.ClearUpdated();
      }
   }

}
