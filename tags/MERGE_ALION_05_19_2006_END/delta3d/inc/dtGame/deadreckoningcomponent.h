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
#ifndef DELTA_DEAD_RECKONING_COMPONENT
#define DELTA_DEAD_RECKONING_COMPONENT

#include <string>
#include <map>

#include <dtCore/refptr.h>
#include <dtGame/export.h>
#include <dtGame/gmcomponent.h>

namespace dtUtil
{
   class Log;
}

namespace dtCore
{
   class Isector;
}

namespace dtGame
{
   class Message;
   class TickMessage;
   class GameActorProxy;

   class DeadReckoningComponent;
   
   class DT_GAME_EXPORT DeadReckoningAlgorithm : public dtUtil::Enumeration
   {
      DECLARE_ENUM(DeadReckoningAlgorithm);
      public:
         static DeadReckoningAlgorithm NONE;
         static DeadReckoningAlgorithm STATIC;
         static DeadReckoningAlgorithm VELOCITY_ONLY;
         static DeadReckoningAlgorithm VELOCITY_AND_ACCELERATION;
      private:
         DeadReckoningAlgorithm(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };
      
   class DT_GAME_EXPORT DeadReckoningHelper : public dtCore::Base
   {
      public:
   
         DeadReckoningHelper();
 
         bool IsUpdated() const { return mUpdated; }
         void ClearUpdated() { mUpdated = false; }
 
         /**
          * @return true if no ground following should be performed on this actor.
          */
         bool IsFlying() const { return mFlying; }
   
         /**
          * True means no ground following should be performed on this actor.  False
          * it will follow the ground as it moves.
          * @param newFlying the new value to set.
          */
          void SetFlying(bool newFlying);
    
    
         /**
          * Sets the entity's minimum Dead Reckoning Algorithm.
          *
          * @param newAlgorithm the new algorithm enum value.
          */
         void SetDeadReckoningAlgorithm(DeadReckoningAlgorithm& newAlgorithm);
   
         /**
          * @return the current minimum Dead Reckoning Algorithm.
          */
         DeadReckoningAlgorithm& GetDeadReckoningAlgorithm() const { return *mMinDRAlgorithm; }
       
         /**
          * Sets this entity's last known translation.  This should
          * only be set for remote actors.
          *
          * @param vec the new last position.
          */
         void SetLastKnownTranslation(const osg::Vec3 &vec);

         /**
          * @return the last known position for this if it's a remote entity.
          */
         const osg::Vec3& GetLastKnownTranslation() const { return mLastTranslation; }
   
         /**
          * Sets this entity's last known rotation.  This should
          * only be set for remote actors.
          * @param vec the new last rotation as yaw, pitch, roll.
          */
         void SetLastKnownRotation(const osg::Vec3 &vec);
   
         /**
          * @return the last known rotation for this if it's a remote entity as yaw, pitch, roll.
          */
         const osg::Vec3& GetLastKnownRotation() const { return mLastRotation; }
   
         /**
          * Sets this entity's DIS/RPR-FOM velocity vector.
          * @param vec the velocity vector to copy
          */
         void SetVelocityVector(const osg::Vec3 &vec);
   
         /**
          * Retrieves this entity's DIS/RPR-FOM velocity vector.
          * @return the velocity vector
          */
         const osg::Vec3& GetVelocityVector() const { return mVelocityVector; }
         
         /**
          * Sets this entity's DIS/RPR-FOM acceleration vector.
          * @param accelerationVector the acceleration vector to copy
          */
         void SetAccelerationVector(const osg::Vec3 &vec);
   
         /**
          * Retrieves this entity's DIS/RPR-FOM acceleration vector.
          * @return the acceleration vector
          */
         const osg::Vec3& GetAccelerationVector() const { return mAccelerationVector; }
         
         /**
         * Sets this entity's DIS/RPR-FOM angular velocity vector.
         * @param angularVelocityVector the angular velocity vector to copy
         */
         void SetAngularVelocityVector(const osg::Vec3 &vec);
   
         /**
          * Retrieves this entity's DIS/RPR-FOM angular velocity vector.
          * @return the angular velocity vector
          */
         const osg::Vec3& GetAngularVelocityVector() const { return mAngularVelocityVector; }
      protected:
         ~DeadReckoningHelper() {}
      private:
         friend class DeadReckoningComponent;
         
         bool mUpdated;
         bool mFlying;
         
         ///the simulation time this was last updated.
         double mLastUpdatedTime;
         ///number of smoothing steps in seconds left from the last update.
         float mSmoothingSteps;
         
         ///Last known position of this actor.
         osg::Vec3 mLastTranslation;
         ///The Dead-Reckoned position prior to the last update.
         osg::Vec3 mTransBeforeLastUpdate;
         
         ///last known orientation.
         osg::Vec3 mLastRotation;
         osg::Matrix mLastRotationMatrix;
         osg::Quat mLastQuatRotation;
         ///The Dead-Reckoned rotation prior to the last update.
         osg::Quat mRotQuatBeforeLastUpdate;
   
         /// The velocity vector.
         osg::Vec3 mVelocityVector;
         
         /// The acceleration vector.
         osg::Vec3 mAccelerationVector;
         
         ///The angular velocity vector.
         osg::Vec3 mAngularVelocityVector;
         
         DeadReckoningAlgorithm* mMinDRAlgorithm;
         
      
         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         DeadReckoningHelper(const DeadReckoningHelper&) {}
         DeadReckoningHelper& operator=(const DeadReckoningHelper&) {return *this;}
         
   };
   
   class DT_GAME_EXPORT DeadReckoningComponent : public dtGame::GMComponent
   {
      public:
         DeadReckoningComponent(const std::string& name = "");
         
         /**
          * handles a sent a message
          * @see dtGame::GMComponent#SendMessage
          * @param The message
          */
         virtual void SendMessage(const dtGame::Message& message);

         /**
          * handles a processed a message
          * @see dtGame::GMComponent#ProcessMessage
          * @param The message
          */
         virtual void ProcessMessage(const dtGame::Message& message);
         
         /**
          * Registers an actor with this component.  To simplify coding in the actor, specifically when it comes
          * to setting properties on the helper, the actor should create it's own helper and pass it in when registering.
          * @param toRegister the actor to register.
          * @param helper the preconfigured helper object to use.
          * @throws dtUtil::Exception if this actor is already registered with the component.
          */
         void RegisterActor(dtGame::GameActorProxy& toRegister, DeadReckoningHelper& helper) throw(dtUtil::Exception);

         /**
          * Registers an actor with this component.  To simplify coding in the actor, specifically when it comes
          * to setting properties on the helper, the actor should create it's own helper and pass it in when registering.
          * @param toRegister the actor to register.
          * @param helper the preconfigured helper object to use.
          */
         void UnregisterActor(dtGame::GameActorProxy& toRegister);
         
         /**
          * @return true if the given actor is registered with this component.
          */
         bool IsRegisteredActor(dtGame::GameActorProxy& gameActorProxy);

         ///@return the terrain actor using the given name.  If it has not yet been queried, the query will run when this is called.
         dtDAL::ActorProxy* GetTerrainActor();

         ///@return the name used when querying for the terrain actor.  
         const std::string& GetTerrainActorName() const { return mTerrainActorName; }
         
         /**
          * Changes the name of the actor used when querying the Game Manager for the terrain actor.
          * The terrain is used for ground following. Calling this will cause this component to requery the terrain actor.  
          * The terrain actor will only be queried once, so make sure to add the terrain actor to the Game Manager before the next tick
          * after setting this.
          * @param newTerrainActorName the new name.
          */
         void SetTerrainActorName(const std::string& newTerrainActorName);
         
      protected:
         virtual ~DeadReckoningComponent();
      private:
         ///number of seconds between forcing vehicles to ground clamp.
         static const float ForceClampTime;
         
         dtUtil::Log* mLogger;
         std::string mTerrainActorName;
         dtCore::RefPtr<dtGame::GameActorProxy> mPlayerActor;
         dtCore::RefPtr<dtDAL::ActorProxy> mTerrainActor;
         dtCore::RefPtr<dtCore::Isector> mIsector;
         bool mTerrainQueried; //< this bool is set to true if the actor attempts to find the terrain.  This supports never finding it.
         
         float mTimeUntilForceClamp;
         
         std::map<dtCore::RefPtr<dtGame::GameActorProxy>, dtCore::RefPtr<DeadReckoningHelper> > mRegisteredActors;
         
         void TickRemote(const dtGame::TickMessage& tickMessage);
         void ClampToGround(float timeSinceUpdate, osg::Vec3& position, osg::Matrix& rotation, dtCore::Transform& xform, dtGame::GameActorProxy& gameActorProxy);
         double GetTerrainZIntersectionPoint(dtCore::DeltaDrawable& terrainActor, const osg::Vec3& point);
         
         
   };
   
}

#endif 
