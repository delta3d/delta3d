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
#ifndef DELTA_DEAD_RECKONING_HELPER
#define DELTA_DEAD_RECKONING_HELPER

#include <string>
#include <vector>

#include <dtCore/refptr.h>
#include <dtGame/export.h>
#include <dtUtil/nodecollector.h>

#include <dtCore/base.h>
#include <dtCore/transform.h>
#include <dtGame/basegroundclamper.h>
#include <dtGame/actorcomponent.h>

namespace dtDAL
{
   class ActorProperty;
}

namespace dtUtil
{
   class Log;
}

namespace dtGame
{
   class DeadReckoningComponent;
   class GameActor;

   class DeadReckoningHelperImpl;

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


   class DT_GAME_EXPORT DeadReckoningHelper : public dtGame::ActorComponent
   {
      public:
         static const float DEFAULT_MAX_SMOOTHING_TIME_ROT;
         static const float DEFAULT_MAX_SMOOTHING_TIME_POS;

         // The type of the actor component - use to look it up.
         static const ActorComponent::ACType TYPE;

         class DT_GAME_EXPORT DeadReckoningDOF : public osg::Referenced
         {
            public:
               DeadReckoningDOF()
               : mCurrentTime(0.0f),
               mUpdate(false),
               mNext(NULL),
               mPrev(NULL)
               {}

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
               std::string mMetricName;

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

         // base methods for actor components.
         virtual void OnAddedToActor(dtGame::GameActor& actor);
         virtual void OnRemovedFromActor(dtGame::GameActor& actor);

         /** add actor component properties to game actor for configuring in STAGE */
         virtual void BuildPropertyMap();


         /**
          * This function is responsible for manipulating the internal data types to do the actual
          * dead reckoning.  To implement another dead reckoning algorithm, overload this function.
          * @param gameActor the actor to DR
          * @param xform the resulting position of the actor after DR.
          * @param pLogger The Dead Reckoning Components instance of logger
          * @param gcType The type of ground clamping to do.  Will be NONE if it is not set.
          * @return Return true if you think you changed the Transform, false if you did not.
          */
         virtual bool DoDR(GameActor& gameActor, dtCore::Transform& xform,
                  dtUtil::Log* pLogger, BaseGroundClamper::GroundClampingType*& gcType);

         /**
          * Calculates how long the associated actor's position and rotation should be smoothed into a updated value.
          * The values are assigned to the helper.
          * @param helper the DR helper for the actor.
          * @param xform the actors current absolute transform.
          */
         void CalculateSmoothingTimes(const dtCore::Transform& xform);


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
         float GetGroundOffset() const { return mGroundClampingData.GetGroundOffset(); }

         /**
          * @return true if the rotation should be adjusted to match the terrain.
          * @note this only matters if flying is set to false.
          */
         bool GetAdjustRotationToGround() const { return mGroundClampingData.GetAdjustRotationToGround(); }

         /**
          * Sets if the rotation of the actor should be changed to match the terrain
          * @param newFlying the new value to set.
          */
         void SetAdjustRotationToGround(bool newAdjust);

         /// Set this to true to tell the DR comp than the model dimensions are now valid. Defaults to false.
         void SetUseModelDimensions(bool newUse) { mGroundClampingData.SetUseModelDimensions(newUse); }
         /// @return true if the model dimensions are now valid.
         bool UseModelDimensions() const { return mGroundClampingData.UseModelDimensions(); }

         /// Sets the model dimensions of this helper.  This will call SetUseModelDimensions(true) internally.
         void SetModelDimensions(const osg::Vec3& newDimensions);

         /// @return the current assigned dimensions of the model for the actor being dead-reckoned
         const osg::Vec3& GetModelDimensions() { return mGroundClampingData.GetModelDimensions(); }

         ///Sets max amount of time to use when smoothing the translation.
         void SetMaxTranslationSmoothingTime(float newMax) { mMaxTranslationSmoothingTime = newMax; }

         ///@return the max amount of time to use when smoothing the translation.
         float GetMaxTranslationSmoothingTime() const { return mMaxTranslationSmoothingTime; }

         ///Sets max amount of time to use when smoothing the rotation.
         void SetMaxRotationSmoothingTime(float newMax) { mMaxRotationSmoothingTime = newMax; }


         ///@return the max amount of time to use when smoothing the rotation.
         float GetMaxRotationSmoothingTime() const { return mMaxRotationSmoothingTime; }

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
         void SetLastKnownVelocity(const osg::Vec3 &vec);

         /**
          * Retrieves this entity's DIS/RPR-FOM velocity vector.
          * @return the velocity vector
          */
         const osg::Vec3& GetLastKnownVelocity() const { return mLastVelocity; }

         /**
          * Sets this entity's DIS/RPR-FOM acceleration vector.
          * @param accelerationVector the acceleration vector to copy
          */
         void SetLastKnownAcceleration(const osg::Vec3 &vec);

         /**
          * Retrieves this entity's DIS/RPR-FOM acceleration vector.
          * @return the acceleration vector
          */
         const osg::Vec3& GetLastKnownAcceleration() const { return mAccelerationVector; }

         /**
          * Sets this entity's DIS/RPR-FOM angular velocity vector.
          * @param angularVelocityVector the angular velocity vector to copy
          */
         void SetLastKnownAngularVelocity(const osg::Vec3 &vec);

         /**
          * Retrieves this entity's DIS/RPR-FOM angular velocity vector.
          * @return the angular velocity vector
          */
         const osg::Vec3& GetLastKnownAngularVelocity() const { return mAngularVelocityVector; }

         ///@return the total amount of time to use when smoothing the translation for this last update.
         float GetTranslationEndSmoothingTime() const { return mTranslationEndSmoothingTime; }
         ///@return the total amount of time to use when smoothing the rotation for this last update.
         float GetRotationEndSmoothingTime() const { return mRotationEndSmoothingTime; }

         ///@return the last simulation time this helper was updated for translation.
         double GetLastTranslationUpdatedTime() const { return mLastTranslationUpdatedTime; };

         ///@return the last simulation time this helper was updated for rotation.
         double GetLastRotationUpdatedTime() const { return mLastRotationUpdatedTime; };

         ///Sets the last time this helper was updated for translation.  This will also updated the average time between updates.
         void SetLastTranslationUpdatedTime(double newUpdatedTime);

         /// Sets the last time this helper was updated for rotation.  This will also updated the average time between updates.
         void SetLastRotationUpdatedTime(double newUpdatedTime);

         /// @return The node collector for this helper or NULL none has been set.
         dtUtil::NodeCollector* GetNodeCollector() { return mDOFDeadReckoning.get(); }

         /// @return The node collector for this helper or NULL none has been set.
         const dtUtil::NodeCollector* GetNodeCollector() const { return mDOFDeadReckoning.get(); }

         /// Set the dof container to what the entity is using for reference.
         void SetNodeCollector(dtUtil::NodeCollector& dofContainerToSet) { mDOFDeadReckoning = &dofContainerToSet; }

         ///@return the rough average amount of time between translation updates.  This is based on values sent to SetLastTranslationUpdatedTime.
         double GetAverageTimeBetweenTranslationUpdates() const { return mAverageTimeBetweenTranslationUpdates; };
         /// Add onto the dof dead reckoning list where the dof should move
         void AddToDeadReckonDOF(const std::string &dofName, const osg::Vec3& position,
            const osg::Vec3& rateOverTime, const std::string& metricName = "");

         ///@return the rough average amount of time between rotation updates.  This is based on values sent to SetLastRotationUpdatedTime.
         double GetAverageTimeBetweenRotationUpdates() const { return mAverageTimeBetweenRotationUpdates; };
         /// Remove a drDOF from the list at this spot
         void RemoveDRDOF(std::list<dtCore::RefPtr<DeadReckoningDOF> >::iterator &iter);

         void RemoveAllDRDOFByName(const std::string& removeName);

         /// Remove a drdof by checking against values compared to everything else.
         void RemoveDRDOF(DeadReckoningDOF &obj);

         const osg::Vec3& GetCurrentDeadReckonedTranslation() const { return mCurrentDeadReckonedTranslation; }
         const osg::Vec3& GetCurrentDeadReckonedRotation() const { return mCurrentAttitudeVector; }

         const std::list<dtCore::RefPtr<DeadReckoningDOF> >& GetDeadReckoningDOFs() const { return mDeadReckonDOFS; }

         void SetTranslationBeforeLastUpdate(const osg::Vec3& trans) { mTransBeforeLastUpdate = trans; }
         void SetRotationBeforeLastUpdate(const osg::Quat& rot) { mRotQuatBeforeLastUpdate = rot; }

         const osg::Quat& GetLastKnownRotationByQuaternion() const { return mLastQuatRotation; }

         bool IsTranslationUpdated() const { return mTranslationUpdated; }
         bool IsRotationUpdated() const { return mRotationUpdated; }

         //void SetTranslationCurrentSmoothingTime(float smoothing) { mTranslationCurrentSmoothingTime=smoothing; }
         //float GetTranslationCurrentSmoothingTime() const { return mTranslationCurrentSmoothingTime; }
         void SetTranslationElapsedTimeSinceUpdate(float value);// { mTranslationElapsedTimeSinceUpdate = value; }
         float GetTranslationElapsedTimeSinceUpdate() const { return mTranslationElapsedTimeSinceUpdate; }

         void SetRotationElapsedTimeSinceUpdate(float value) { mRotationElapsedTimeSinceUpdate = value; }
         float GetRotationElapsedTimeSinceUpdate() const { return mRotationElapsedTimeSinceUpdate; }

         void SetRotationResolved(bool resolved) { mRotationResolved=resolved; }

         /**
          * Computes the change in rotation based on the angular velocity. This is used by DeadReckonTheRotation().
          * @param deltaTime the time elapsed since the last measured attitude
          * @param result the resulting matrix.
          */
         void ComputeRotationChangeWithAngularVelocity(double deltaTime, osg::Matrix& result);

         GroundClampingData& GetGroundClampingData() { return mGroundClampingData; }
         const GroundClampingData& GetGroundClampingData() const { return mGroundClampingData; }

         /**
          * When this is true, the DR algorithm uses a Cartmull-Rom Cubic Spline to interpret 
          * between points. It accounts for vel and accel. If false, it uses the older, straight 
          * forward linear blend of pos and vel.  This has no effect on rotation. Defaults to true.
          * @return true if currently using the cubic spline blend. False if using simple linear blend. 
          */
         bool GetUseCubicSplineTransBlend() const { return mUseCubicSplineTransBlend; }

         /**
         * When set to true, the DR algorithm uses a Cartmull-Rom Cubic Spline to interpret 
         * between points. It accounts for vel and accel. If set to false, it uses the older, straight 
         * forward linear blend of pos and vel.  This has no effect on rotation. Defaults to true.
         * @param newValue (default) True to use cartmull-rom splines. False for simple linear blend.
         */
         void SetUseCubicSplineTransBlend(bool newValue) { mUseCubicSplineTransBlend = newValue; }

         /**
          * @return true if maintaining constant smoothing time, false (default) if using avg update time.
          */
         bool GetAlwaysUseMaxSmoothingTime() const { return mAlwaysUseMaxSmoothingTime; }

         /**
         * When this is true, the DR algorithm will maintain a constant smoothing time 
         * by using the MaxSmoothingTime. Rotation & Translation each have their own 
         * max value. Keeping a constant smoothing time will significantly reduce anomalies
         * when sending updates at a rate close to the visual framerate.
         * If false (the default), it will use the avg update time if that is smaller than the max smoothing time.
         * This works better in the general case because the smoothing time should be about the same as the update.
         * @param newValue true if maintaining constant smoothing time, false (default) if using avg update time.
         */
         void SetAlwaysUseMaxSmoothingTime(bool newValue) { mAlwaysUseMaxSmoothingTime = newValue; }


      protected:
         virtual ~DeadReckoningHelper();// {}

         ///perform static dead-reckoning, which means applying the new position directly and ground clamping.  xform will be updated.
         void DRStatic(GameActor& gameActor, dtCore::Transform& xform, dtUtil::Log* pLogger);

         /**
          * perform velocity + acceleration dead-reckoning.  Acceleration may be ignored.  xform will be updated.
          * @return returns true if it thinks it made a change, false otherwise.
          */
         bool DRVelocityAcceleration(GameActor& gameActor, dtCore::Transform& xform, dtUtil::Log* pLogger);

         /*
          * Simple dumps out a log that we have started dead reckoning with lots of information.  Pulled out
          * to help make DRVelocityAcceleration() a bit easier to read.
          */
         void LogDeadReckonStarted(osg::Vec3& unclampedTranslation, osg::Matrix& rot, dtUtil::Log* pLogger);

         /**
          * Computes the new rotation for the object.  This method handles VELOCITY_ONLY and
          * VELOCITY_AND_ACCELERATION, but not static. This is called DRVelocityAcceleration().
          */
         void DeadReckonTheRotation(dtCore::Transform &xform);

         /**
          * Computes the new position for the object.  This method handles VELOCITY_ONLY and
          * VELOCITY_AND_ACCELERATION, but not static. This is called DRVelocityAcceleration()
          */
         void DeadReckonThePosition( osg::Vec3& pos, dtUtil::Log* pLogger, GameActor &gameActor );

         /// Used by DeadReckonThePosition if we are using splines NEW WAY
         void DeadReckonThePositionUsingSplines(osg::Vec3& pos, dtUtil::Log* pLogger, GameActor &gameActor);
         /// Used by DeadReckonThePosition for straight blend. OLD WAY
         void DeadReckonThePositionUsingLinearBlend(osg::Vec3& pos, dtUtil::Log* pLogger, GameActor &gameActor);

      private:
         DeadReckoningHelperImpl* mDRImpl;

         /// The list of DeadReckoningDOFs, might want to change to has table of list later.
         std::list<dtCore::RefPtr<DeadReckoningDOF> > mDeadReckonDOFS;

         /// The Dead reckoning DOF Container object
         dtCore::RefPtr<dtUtil::NodeCollector> mDOFDeadReckoning;

         GroundClampingData mGroundClampingData;

         ///the simulation time this was last updated.
         double mLastTranslationUpdatedTime;
         double mLastRotationUpdatedTime;

         ///This should be fairly clear.
         float mAverageTimeBetweenTranslationUpdates;
         float mAverageTimeBetweenRotationUpdates;

         ///The maximum amount of time to use when smoothing translation.
         float mMaxTranslationSmoothingTime;
         ///The maximum amount of time to use when smoothing rotation.
         float mMaxRotationSmoothingTime;
         /// True means we maintain constant smoothing time for blending. Uses Max Rot or Trans as appropriate
         bool mAlwaysUseMaxSmoothingTime;

         ///the amount of time since this actor started smoothing.
         float mTranslationElapsedTimeSinceUpdate;
         float mRotationElapsedTimeSinceUpdate;

         ///the end amount of time to use when smoothing the translation.  At this point, the blend should be finished.
         float mTranslationEndSmoothingTime;
         ///the end amount of time to use when smoothing the rotation.  At this point, the blend should be finished.
         float mRotationEndSmoothingTime;

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
         osg::Vec3 mLastVelocity;
         osg::Vec3 mVelocityBeforeLastUpdate; /// The velocity we were using just before we got an update

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
         // if the rotation has been resolved to the last updated version.
         bool mRotationResolved;
         bool mUseCubicSplineTransBlend; // true is NEW WAY (default) - should we use simple linear or cubic spline blend?

         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         //DeadReckoningHelper(const DeadReckoningHelper&) {}
         //DeadReckoningHelper& operator=(const DeadReckoningHelper&) {return *this;}
   };

}

#endif
