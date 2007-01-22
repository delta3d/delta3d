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
#ifndef DELTA_DEAD_RECKONING_COMPONENT
#define DELTA_DEAD_RECKONING_COMPONENT

#include <string>
#include <map>

#include <dtCore/refptr.h>
#include <dtGame/export.h>
#include <dtGame/gmcomponent.h>
#include <dtCore/nodecollector.h>

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
         class DT_GAME_EXPORT DeadReckoningDOF : public osg::Referenced
         {
            public:
               DeadReckoningDOF(){}
          
               static const std::string REPRESENATION_POSITION;
               static const std::string REPRESENATION_POSITIONRATE;
               static const std::string REPRESENATION_EXTENSION;
               static const std::string REPRESENATION_EXTENSIONRATE;
               static const std::string REPRESENATION_X;
               static const std::string REPRESENATION_XRATE;
               static const std::string REPRESENATION_Y;
               static const std::string REPRESENATION_YRATE;
               static const std::string REPRESENATION_Z;
               static const std::string REPRESENATION_ZRATE;
               static const std::string REPRESENATION_AZIMUTH;
               static const std::string REPRESENATION_AZIMUTHRATE;
               static const std::string REPRESENATION_ELEVATION;
               static const std::string REPRESENATION_ELEVATIONRATE;
               static const std::string REPRESENATION_ROTATION;
               static const std::string REPRESENATION_ROTATIONRATE;

            protected:
               virtual ~DeadReckoningDOF(){}
            
            public:
               // dof has a name
               std::string mName;

               // rate at which it needs to move in xyz direction
               osg::Vec3 mRateOverTime;

               // the start location of the object
               osg::Vec3 mStartLocation;

               // how long we've been going through the stuff
               float mCurrentTime;

               // has update been called yet.
               bool mUpdate;

               //  pointer to know where it should gos
               DeadReckoningDOF *mNext, *mPrev;
         };

         class DT_GAME_EXPORT UpdateMode : public dtUtil::Enumeration
         {
            DECLARE_ENUM(UpdateMode);
            public:
               static UpdateMode AUTO;
               static UpdateMode CALCULATE_ONLY;
               static UpdateMode CALCULATE_AND_MOVE_ACTOR;
            private:
               UpdateMode(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
         };
   
         DeadReckoningHelper();
 
         bool IsUpdated() const { return mUpdated; }
         void ClearUpdated() { mUpdated = false; mTranslationUpdated= false; mRotationUpdated= false;}
 
         UpdateMode& GetUpdateMode() const { return *mUpdateMode; }
         UpdateMode& GetEffectiveUpdateMode(bool isRemote) const;
         void SetUpdateMode(UpdateMode& newUpdateMode) { mUpdateMode = &newUpdateMode; }
 
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
          * Sets the offset from the ground the actor should be clamped to.
          * This only matters if flying is set to false.
          * @param newOffset the new offset value. 
          */
         void SetGroundOffset(float newOffset);
       
         ///@return The distance from the ground that the actor should be.
         float GetGroundOffset() const { return mGroundOffset; }
       
         ///Sets max amount of time to use when smoothing the translation.
         void SetMaxTranslationSmoothingSteps(float newMax) { mMaxTranslationSmoothingSteps = newMax; }

         ///Sets max amount of time to use when smoothing the rotation.
         void SetMaxRotationSmoothingSteps(float newMax) { mMaxRotationSmoothingSteps = newMax; }
       
         ///@return the max amount of time to use when smoothing the translation.
         float GetMaxTranslationSmoothingSteps() const { return mMaxTranslationSmoothingSteps; }
         ///@return the max amount of time to use when smoothing the rotation.
         float GetMaxRotationSmoothingSteps() const { return mMaxRotationSmoothingSteps; }

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

         /**
          * Retrieves this entity's Dead Reckoning matrix.
          * @return the Dead Reckoning matrix
          */
         const osg::Matrix& GetDeadReckoningMatrix() const { return mDeadReckoningMatrix; }
         
         /**
          * Sets this entity's Dead Reckoning matrix.
          * @param deltaTime the time elapsed since the last measured attitude 
          */
         void SetDeadReckoningMatrix(double deltaTime);
         
         ///@return the total amount of time to use when smoothing the translation for this last update.
         float GetCurrentTotalTranslationSmoothingSteps() const { return mCurrentTotalTranslationSmoothingSteps; }
         ///@return the total amount of time to use when smoothing the rotation for this last update.
         float GetCurrentTotalRotationSmoothingSteps() const { return mCurrentTotalRotationSmoothingSteps; }

         ///@return the last simulation time this helper was updated for translation.
         double GetLastTranslationUpdatedTime() const { return mLastTranslationUpdatedTime; };

         ///@return the last simulation time this helper was updated for rotation.
         double GetLastRotationUpdatedTime() const { return mLastRotationUpdatedTime; };

         ///Sets the last time this helper was updated for translation.  This will also updated the average time between updates.
         void SetLastTranslationUpdatedTime(double newUpdatedTime);

         /// Sets the last time this helper was updated for rotation.  This will also updated the average time between updates.
         void SetLastRotationUpdatedTime(double newUpdatedTime);

         /// @return The node collector for this helper or NULL none has been set.
         dtCore::NodeCollector* GetNodeCollector() { return mDOFDeadReckoning.get(); }
         
         /// Set the dof container to what the entity is using for reference.
         void SetNodeCollector(dtCore::NodeCollector& dofContainerToSet) { mDOFDeadReckoning = &dofContainerToSet; }

         ///@return the rough average amount of time between translation updates.  This is based on values sent to SetLastTranslationUpdatedTime.
         double GetAverageTimeBetweenTranslationUpdates() const { return mAverageTimeBetweenTranslationUpdates; };
         /// Add onto the dof dead reckoning list where the dof should move 
         void AddToDeadReckonDOF(const std::string &DofName, osg::Vec3& position, osg::Vec3& rateOverTime);

         ///@return the rough average amount of time between rotation updates.  This is based on values sent to SetLastRotationUpdatedTime.
         double GetAverageTimeBetweenRotationUpdates() const { return mAverageTimeBetweenRotationUpdates; };
         /// Remove a drDOF from the list at this spot
         void RemoveDRDOF(std::list<dtCore::RefPtr<DeadReckoningDOF> >::iterator &iter);

         /// Remove a drdof by checking against values compared to everything else.
         void RemoveDRDOF(DeadReckoningDOF &obj);

         const osg::Vec3& GetCurrentDeadReckonedTranslation() const { return mCurrentDeadReckonedTranslation; }
         const osg::Vec3& GetCurrentDeadReckonedRotation() const { return mCurrentAttitudeVector; }
 
      protected:
         virtual ~DeadReckoningHelper() {}
         
      private:
         friend class DeadReckoningComponent;

         /// The list of DeadReckoningDOFs, might want to change to has table of list later.
         std::list<dtCore::RefPtr<DeadReckoningDOF> > mDeadReckonDOFS;

         /// The Dead reckoning DOF Container object
         dtCore::RefPtr<dtCore::NodeCollector> mDOFDeadReckoning;
         
         double mLastTimeTag;
         
         ///the simulation time this was last updated.
         double mLastTranslationUpdatedTime;
         double mLastRotationUpdatedTime;

         ///This should be fairly clear.
         float mAverageTimeBetweenTranslationUpdates;
         float mAverageTimeBetweenRotationUpdates;

         ///The maximum amount of time to use when smoothing translation.
         float mMaxTranslationSmoothingSteps;
		 ///The maximum amount of time to use when smoothing rotation.
         float mMaxRotationSmoothingSteps;
                 
         ///the amount of time since this actor started smoothing.
         float mTranslationSmoothingSteps;
         float mRotationSmoothingSteps;
         
           ///the total number of smoothing steps to use when smoothing translation since the last update.
         float mCurrentTotalTranslationSmoothingSteps;
		 ///the total number of smoothing steps to use when smoothing rotation since the last update.
         float mCurrentTotalRotationSmoothingSteps;
         
         ///The distance from the ground that the actor should be.
         float mGroundOffset;
         
         ///Last known position of this actor.
         osg::Vec3 mLastTranslation;

         ///The Dead-Reckoned position prior to the last update.
         osg::Vec3 mTransBeforeLastUpdate;
         // Current Dead Reckoned Position
         osg::Vec3 mCurrentDeadReckonedTranslation;
         
         ///last known orientation (vector)
         osg::Vec3 mLastRotation;
         ///last known orientation (matrix)
         osg::Matrix mLastRotationMatrix;
         ///last known orientation (quaternion)
         osg::Quat mLastQuatRotation;
         ///dead reckoned attitude quaternion prior update
         osg::Quat mRotQuatBeforeLastUpdate;
         ///current dead reckoned attitude quaternion  
         osg::Quat mCurrentDeadReckonedRotation;
         osg::Vec3 mCurrentAttitudeVector;
  
         /// The velocity vector.
         osg::Vec3 mVelocityVector;
         
         /// The acceleration vector.
         osg::Vec3 mAccelerationVector;
         
         ///The angular velocity vector.
         osg::Vec3 mAngularVelocityVector;
         
         // The Dead Reckoning Matrix
         osg::Matrix mDeadReckoningMatrix;
         
         DeadReckoningAlgorithm* mMinDRAlgorithm;
         
         /// The update mode - whether to actually move the actor or to just calculate. 
         UpdateMode* mUpdateMode;
      
         bool mTranslationInitiated;
         bool mRotationInitiated;
         bool mUpdated;
         bool mTranslationUpdated;
         bool mRotationUpdated;
         bool mFlying;
          //if the rotation has been resolved to the last updated version.
         bool mRotationResolved;
         
         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         DeadReckoningHelper(const DeadReckoningHelper&) {}
         DeadReckoningHelper& operator=(const DeadReckoningHelper&) {return *this;}
   };
   
   class DT_GAME_EXPORT DeadReckoningComponent : public dtGame::GMComponent
   {
      public:
         ///The default component name, used when looking it up on the GM. 
         static const std::string DEFAULT_NAME;
      
         DeadReckoningComponent(const std::string& name = DEFAULT_NAME);
         
         /**
          * handles a processed a message
          * @see dtGame::GMComponent#ProcessMessage
          * @param The message
          */
         virtual void ProcessMessage(const dtGame::Message& message);
         
         /**
          * Gets the helper registered for an actor
          * @param proxy The proxy to get the helper for
          * @return A pointer to the helper, or NULL if the proxy is not registered
          */
         const DeadReckoningHelper* GetHelperForProxy(dtGame::GameActorProxy &proxy) const;

         /**
          * Registers an actor with this component.  To simplify coding in the actor, specifically when it comes
          * to setting properties on the helper, the actor should create it's own helper and pass it in when registering.
          * @param toRegister the actor to register.
          * @param helper the preconfigured helper object to use.
          * @throws dtUtil::Exception if this actor is already registered with the component.
          */
         void RegisterActor(dtGame::GameActorProxy& toRegister, DeadReckoningHelper& helper);

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
         dtCore::Transformable* GetTerrainActor() { return mTerrainActor.get(); }

         ///@return the terrain actor using the given name.  If it has not yet been queried, the query will run when this is called.
         const dtCore::Transformable* GetTerrainActor() const { return mTerrainActor.get(); };
         
         ///changes the actor to use for the terrain.
         void SetTerrainActor(dtCore::Transformable* newTerrain);
         
         ///@return the actor to use as an eye point for ground clamping.  This determines which LOD to clamp to. 
         dtCore::Transformable* GetEyePointActor() { return mEyePointActor.get(); };

         ///@return the actor to use as an eye point for ground clamping.  This determines which LOD to clamp to. 
         const dtCore::Transformable* GetEyePointActor() const { return mEyePointActor.get(); };
         
         ///changes the actor to use for the terrain.
         void SetEyePointActor(dtCore::Transformable* newEyePointActor);
         
         /**
          * Sets the maximum distance from the player that three intersection point clamping will be used.  
          * After this, one intersection will be used.
          */
         void SetHighResGroundClampingRange(float range) 
         { 
            mHighResClampRange = range; 
            mHighResClampRange2 = range * range; 
         }

         /**
          * @return the maximum distance from the player that three intersection point clamping will be used.  
          */
         float GetHighResGroundClampingRange() const { return mHighResClampRange; }
         
      protected:
         virtual ~DeadReckoningComponent();

         /**
          * Clamps an actor to the ground.  This doesn't actually move an actor, it just outputs the position and rotation.
          * @param timeSinceUpdate the amount of time since the last actor update.
          * @param xform the current absolute transform of the actor.
          * @param gameActorProxy the actual actor.  This is passed case collision geometry is needed.
          * @param helper the deadreckoning helper for the actor
          */
         void ClampToGround(float timeSinceUpdate, dtCore::Transform& xform, 
            dtGame::GameActorProxy& gameActorProxy, DeadReckoningHelper& helper);
         
         ///Version of clamping that uses three intersection points to calculate the height and the rotation.
         void ClampToGroundThreePoint(float timeSinceUpdate, dtCore::Transform& xform,
            dtGame::GameActorProxy& gameActorProxy);
         
         ///Version of clamping that uses one intersection points and the vertex normal.
         void ClampToGroundOnePoint(float timeSinceUpdate, dtCore::Transform& xform);
         
         dtCore::Isector& GetGroundClampIsector();

         ///number of seconds between forcing vehicles to ground clamp.
         static const float ForceClampTime;
         
         dtUtil::Log* mLogger;
         dtCore::RefPtr<dtCore::Transformable> mEyePointActor;
         dtCore::RefPtr<dtCore::Transformable> mTerrainActor;

         dtCore::RefPtr<dtCore::Isector> mIsector;
         
         float mTimeUntilForceClamp, mHighResClampRange, mHighResClampRange2;
         
         std::map<dtCore::UniqueId, dtCore::RefPtr<DeadReckoningHelper> > mRegisteredActors;
         
         void TickRemote(const dtGame::TickMessage& tickMessage);
                  
         //double GetTerrainZIntersectionPoint(dtCore::DeltaDrawable& terrainActor, const osg::Vec3& point, osg::Vec3& groundNormalOut);
         virtual double GetTerrainZIntersectionPoint(dtCore::DeltaDrawable& terrainActor, const osg::Vec3& point, osg::Vec3& groundNormalOut);

         /**
          * Calculates how long the associated actor's position and rotation should be smoothed into a updated value.
          * The values are assigned to the helper.
          * @param helper the DR helper for the actor.
          * @param xform the actors current absolute transform.
          */
         void CalculateTotalSmoothingSteps(DeadReckoningHelper& helper,  const dtCore::Transform& xform);

         ///perform static dead-reckoning, which means applying the new position directly and ground clamping.  xform will be updated.
         void DRStatic(DeadReckoningHelper& helper, const double timeSinceTranslationUpdate, const double timeSinceRotationUpdate, GameActor& gameActor, dtCore::Transform& xform);
         ///perform velocity + acceleration dead-reckoning.  Acceleration may be ignored.  xform will be updated.
         void DRVelocityAcceleration(DeadReckoningHelper& helper, const float deltaTime, const double timeSinceTranslationUpdate, const double timeSinceRotationUpdate, const bool forceClamp, GameActor& gameActor, dtCore::Transform& xform); 
   };
   
}

#endif 
