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
#include <dtUtil/refstring.h>
#include <dtUtil/getsetmacros.h>
#include <dtUtil/deprecationmgr.h>

#include <dtCore/base.h>
#include <dtCore/transform.h>
#include <dtGame/basegroundclamper.h>
#include <dtGame/actorcomponent.h>
#include <dtCore/motioninterface.h>

namespace dtCore
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


   ///////////////////////////////////////////////////////////////////////////
   class DT_GAME_EXPORT DeadReckoningAlgorithm : public dtUtil::Enumeration
   {
      DECLARE_ENUM(DeadReckoningAlgorithm);
      public:
         static DeadReckoningAlgorithm NONE;
         static DeadReckoningAlgorithm STATIC;
         static DeadReckoningAlgorithm VELOCITY_ONLY;
         static DeadReckoningAlgorithm VELOCITY_AND_ACCELERATION;
      private:
         DeadReckoningAlgorithm(const std::string& name);
   };


   ///////////////////////////////////////////////////////////////////////////
   /** 
    * Add this component to your actor for dead reckoning (DR). DR is typically used in
    * networked applications to reduce publish rate. You dead reckon your actors locally 
    * and then make some determination when to publish (see the DRPublishingActComp). Then,
    * on the remote side, it receives those updates and dead reckons between the update positions.
    * There is a good example of how to use this in the SimulationCore SVN repository in BaseEntity.
    * 
    * Note - this class and its behavior are described in extensive detail in an article in 
    * Game Engine Gems 2 (Mar '11) entitled, 'Believable Dead Reckoning for Networked Games' 
    */
   class DT_GAME_EXPORT DeadReckoningActorComponent : public dtGame::ActorComponent , public dtCore::MotionInterface
   {
      public:
         DT_DECLARE_VIRTUAL_REF_INTERFACE_OVERRIDE_INLINE

         static const float DEFAULT_MAX_SMOOTHING_TIME_ROT;
         static const float DEFAULT_MAX_SMOOTHING_TIME_POS;

         // The type of the actor component - use to look it up.
         static const ActorComponent::ACType TYPE;

         ///////////////////////////////////////////////////////////////////////////
         // Property names.
         static const dtUtil::RefString PROPERTY_LAST_KNOWN_TRANSLATION;
         static const dtUtil::RefString PROPERTY_LAST_KNOWN_ROTATION;
         static const dtUtil::RefString PROPERTY_VELOCITY_VECTOR;
         static const dtUtil::RefString PROPERTY_ACCELERATION_VECTOR;
         static const dtUtil::RefString PROPERTY_ANGULAR_VELOCITY_VECTOR;
      private:
         static const dtUtil::RefString PROPERTY_FLYING;
      public:
         static const dtUtil::RefString PROPERTY_GROUND_CLAMP_TYPE;
         static const dtUtil::RefString PROPERTY_DEAD_RECKONING_ALGORITHM;
         static const dtUtil::RefString PROPERTY_GROUND_OFFSET;

         ///////////////////////////////////////////////////////////////////////////
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

         ///////////////////////////////////////////////////////////////////////////
         class DT_GAME_EXPORT UpdateMode : public dtUtil::Enumeration
         {
            DECLARE_ENUM(UpdateMode);
            public:
               static UpdateMode AUTO;
               static UpdateMode CALCULATE_ONLY;
               static UpdateMode CALCULATE_AND_MOVE_ACTOR;
            private:
               UpdateMode(const std::string& name);
         };

         ///////////////////////////////////////////////////////////////////////////
         /** 
          * Utility data class used by the DRhelper. It works with a vec3 that can be acted on
          * as if it were a world coordinate. This allows a subclass of the DRHelper to do 
          * dead reckoning on its own vec3 values (ex an XYZ Scaler that grows/shrinks). 
          * Note - if used as something other than a world coordiante, the values like
          * velocity & acceleration can be assumed to be 1st and 2nd derivatives instead. 
          */
         class DT_GAME_EXPORT DRVec3Util
         {
            public: 
               DRVec3Util();
               ~DRVec3Util();

            public:
               /// @see DeadReckoningActorComponent::SetLastKnownTranslation()
               void SetLastKnownTranslation(const osg::Vec3& vec);
               /// @see DeadReckoningActorComponent::SetLastKnownVelocity()
               void SetLastKnownVelocity(const osg::Vec3& vec);
               /// @see DeadReckoningActorComponent::SetLastTranslationUpdatedTime()
               void SetLastUpdatedTime(double newUpdatedTime);
               /// Used by DeadReckonPosition for straight blend.
               void DeadReckonUsingLinearBlend(osg::Vec3& pos, dtUtil::Log* pLogger, dtCore::Transformable& txable, bool useAcceleration);
               /// Used by DeadReckonPosition if we are using splines -- OLD WAY
               //void DeadReckonUsingSplines(osg::Vec3& pos, dtUtil::Log* pLogger, dtCore::Transformable& txable);
               /// Called when the trans or vel changes to recompute the parametric values used during spline blending.  
               //void RecomputeTransSplineValues(const osg::Vec3& currentAccel);

               /// Computes the new position for the object. Splines or linear, but not static.
               void DeadReckonPosition(osg::Vec3& pos, dtUtil::Log* pLogger,
                  dtCore::Transformable& txable, bool useAcceleration, float curTimeDelta);


               ///the simulation time this was last updated.
               //double mLastTranslationUpdatedTime;
               double mLastUpdatedTime;
               //float mAverageTimeBetweenTranslationUpdates;
               float mAvgTimeBetweenUpdates;

               ///The maximum amount of time to use when smoothing translation.
               //float mMaxTranslationSmoothingTime;
               float mMaxSmoothingTime;

               ///the amount of time since this actor started smoothing.
               //float mTranslationElapsedTimeSinceUpdate;
               float mElapsedTimeSinceUpdate;

               ///the end amount of time to use when smoothing the translation.  At this point, the blend should be finished.
               //float mTranslationEndSmoothingTime;
               float mEndSmoothingTime;

               ///Last known position of this actor.
               //osg::Vec3 mLastTranslation;
               osg::Vec3 mLastValue;

               ///The Dead-Reckoned position prior to the last update.
               //osg::Vec3 mTransBeforeLastUpdate;
               osg::Vec3 mValueBeforeLastUpdate;
               
               // Current Dead Reckoned Position
               //osg::Vec3 mCurrentDeadReckonedTranslation;
               osg::Vec3 mCurrentDeadReckonedValue;

               //osg::Vec3 mLastVelocity;
               osg::Vec3 mLastVelocity;
               //osg::Vec3 mVelocityBeforeLastUpdate; /// The velocity we were using just before we got an update
               osg::Vec3 mVelocityBeforeLastUpdate; /// The velocity we were using just before we got an update
               osg::Vec3 mPreviousInstantVel;
               //osg::Vec3 mAccelerationVector;
               osg::Vec3 mAcceleration;
               //bool mTranslationInitiated;
               bool mInitialized;
               //bool mTranslationUpdated;
               bool mUpdated;
//
//               // The following variables are used to compute the 'cubic spline' that represents the blended position
//               float mPosSplineXA, mPosSplineXB, mPosSplineXC, mPosSplineXD; // x spline pre-compute values
//               float mPosSplineYA, mPosSplineYB, mPosSplineYC, mPosSplineYD; // y spline pre-compute values
//               float mPosSplineZA, mPosSplineZB, mPosSplineZC, mPosSplineZD; // z spline pre-compute values
         };


         ///////////////////////////////////////////////////////////////////////////
         DeadReckoningActorComponent();

         // base methods for actor components.
         void OnAddedToActor(dtCore::BaseActorObject& actor) override;
         void OnRemovedFromActor(dtCore::BaseActorObject& actor) override;

         DT_DECLARE_ACCESSOR(bool, AutoRegisterWithGMComponent);

         void RegisterWithGMComponent();
         void UnregisterWithGMComponent();

         /// Called when the parent actor enters the "world".
         void OnEnteredWorld() override;
         /// Called when the parent actor leaves the "world".
         void OnRemovedFromWorld() override;

         /** add actor component properties to game actor for configuring in STAGE */
         void BuildPropertyMap() override;

         /** 
          * Use these externally to know if something other than pos, rot, accel, etc were changed
          * This is used mainly to decouple the update behavior (ex Flying) from the DRPublishingActComp.
          * @see SetExtraDataUpdated
          */
         bool IsExtraDataUpdated();
         /** 
          * Call this externally to clear the status of updates on non-positional stuff. 
          * Setting this to true outside of this class is dumb
          * This is used mainly to decouple the update behavior (ex Flying) from the DRPublishingActComp.
          * @see IsExtraDataUpdated
          */
         void SetExtraDataUpdated(bool newValue);

         /**
          * This function updates the elapsedTimeSinceUpdate for both the rotation and translation. 
          * @param simTimeDelta the amount of time to increment (sim time elapsed this frame)
          * @param curSimulationTime the simulationTime from the TickMessage (or from asking the GM). 
          */
         virtual void IncrementTimeSinceUpdate(float simTimeDelta, double curSimulationTime);

         /**
          * This function is responsible for manipulating the internal data types to do the actual
          * dead reckoning.  To implement another dead reckoning algorithm, overload this function.
          * @param gameActor the actor to DR
          * @param xform the resulting position of the actor after DR.
          * @param pLogger The Dead Reckoning Components instance of logger
          * @param gcType The type of ground clamping to do.  Will be NONE if it is not set.
          * @return Return true if you think you changed the Transform, false if you did not.
          */
         virtual bool DoDR(dtCore::Transformable& txable, dtCore::Transform& xform,
                  dtUtil::Log* pLogger, BaseGroundClamper::GroundClampRangeType*& gcType);

         /**
          * Calculates how long the associated actor's position and rotation should be smoothed into a updated value.
          * The values are assigned to the helper.
          * @param helper the DR helper for the actor.
          * @param xform the actors current absolute transform.
          */
         virtual void CalculateSmoothingTimes(const dtCore::Transform& xform);


         bool IsUpdated() const { return mUpdated; }
         void ClearUpdated() { mUpdated = false; mTranslation.mUpdated= false; mRotationUpdated= false;}

         UpdateMode& GetUpdateMode() const { return *mUpdateMode; }
         UpdateMode& GetEffectiveUpdateMode(bool isRemote) const;
         void SetUpdateMode(UpdateMode& newUpdateMode) { mUpdateMode = &newUpdateMode; }

         bool IsFlyingDeprecatedProperty();
         /// Deprecated 8/23/10 - replaced by GroundClampType
         DEPRECATE_FUNC bool IsFlying();

         void SetFlyingDeprecatedProperty(bool newFlying); 
         /// Deprecated 8/23/10 - replaced by GroundClampType
         DEPRECATE_FUNC void SetFlying(bool newFlying);

         /// GROUND CLAMP TYPE - enum property - replaces the old Flying property
         DT_DECLARE_ACCESSOR(dtUtil::EnumerationPointer<dtGame::GroundClampTypeEnum>, GroundClampType);
         //void SetGroundClampType(DeadReckoningActorComponent::GroundClampTypeEnum& typeEnum);

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
         virtual void SetMaxTranslationSmoothingTime(float newMax) { mTranslation.mMaxSmoothingTime = newMax; }

         ///@return the max amount of time to use when smoothing the translation.
         float GetMaxTranslationSmoothingTime() const { return mTranslation.mMaxSmoothingTime; }

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
         void SetLastKnownTranslation(const osg::Vec3& vec);

         /**
          * @return the last known position for this if it's a remote entity.
          */
         const osg::Vec3& GetLastKnownTranslation() const { return mTranslation.mLastValue; }

         /**
          * Sets this entity's last known rotation.  This should
          * only be set for remote actors.
          * @param vec the new last rotation as yaw, pitch, roll.
          */
         void SetLastKnownRotation(const osg::Vec3& vec);

         /**
          * @return the last known rotation for this if it's a remote entity as yaw, pitch, roll.
          */
         const osg::Vec3& GetLastKnownRotation() const { return mLastRotation; }

         /**
          * Sets this entity's DIS/RPR-FOM velocity vector.
          * @param vec the velocity vector to copy
          */
         void SetLastKnownVelocity(const osg::Vec3& vec);

         /**
          * Retrieves this entity's DIS/RPR-FOM velocity vector.
          * @return the velocity vector
          */
         const osg::Vec3& GetLastKnownVelocity() const { return mTranslation.mLastVelocity; }
         /** 
          * For moving objects, the DR helper computes the instantaneous velocity each frame. This
          * exposes that value to classes like the DR Component.
          * It does compute an instant velocity for STATIC as well, but keep in mind that this
          * value is really only useful in systems that update the position every frame.
          * @return the instantaneous velocity
          */
         const osg::Vec3& GetCurrentInstantVelocity() const;

         /**
          * This tries to return a reasonable velocity for this frame.
          */
         /*override MotionInterface*/ osg::Vec3 GetVelocity() const override { return GetCurrentInstantVelocity(); }

         /**
          * Sets this entity's DIS/RPR-FOM acceleration vector.
          * @param accelerationVector the acceleration vector to copy
          */
         void SetLastKnownAcceleration(const osg::Vec3& vec);

         /**
          * Retrieves this entity's DIS/RPR-FOM acceleration vector.
          * @return the acceleration vector
          */
         const osg::Vec3& GetLastKnownAcceleration() const { return mTranslation.mAcceleration; }
         /*override MotionInterface*/ osg::Vec3 GetAcceleration() const override { return GetLastKnownAcceleration(); }

         /**
          * Sets this entity's DIS/RPR-FOM angular velocity vector.
          * @param angularVelocityVector the angular velocity vector to copy
          */
         void SetLastKnownAngularVelocity(const osg::Vec3& vec);

         /**
          * Retrieves this entity's DIS/RPR-FOM angular velocity vector.
          * @return the angular velocity vector
          */
         const osg::Vec3& GetLastKnownAngularVelocity() const { return mAngularVelocityVector; }
         /*override MotionInterface*/ osg::Vec3 GetAngularVelocity() const override { return GetLastKnownAngularVelocity(); }

         ///@return the total amount of time to use when smoothing the translation for this last update.
         float GetTranslationEndSmoothingTime() const { return mTranslation.mEndSmoothingTime; }
         ///@return the total amount of time to use when smoothing the rotation for this last update.
         float GetRotationEndSmoothingTime() const { return mRotationEndSmoothingTime; }

         ///@return the last simulation time this helper was updated for translation.
         double GetLastTranslationUpdatedTime() const { return mTranslation.mLastUpdatedTime; };

         ///@return the last simulation time this helper was updated for rotation.
         double GetLastRotationUpdatedTime() const { return mLastRotationUpdatedTime; };

         ///Sets the last time this helper was updated for translation.  This will also updated the average time between updates.
         virtual void SetLastTranslationUpdatedTime(double newUpdatedTime);

         /// Sets the last time this helper was updated for rotation.  This will also updated the average time between updates.
         void SetLastRotationUpdatedTime(double newUpdatedTime);

         /// @return The node collector for this helper or NULL none has been set.
         dtUtil::NodeCollector* GetNodeCollector() { return mDOFDeadReckoning.get(); }

         /// @return The node collector for this helper or NULL none has been set.
         const dtUtil::NodeCollector* GetNodeCollector() const { return mDOFDeadReckoning.get(); }

         /// Set the dof container to what the entity is using for reference.
         void SetNodeCollector(dtUtil::NodeCollector& dofContainerToSet) { mDOFDeadReckoning = &dofContainerToSet; }

         ///@return the rough average amount of time between translation updates.  This is based on values sent to SetLastTranslationUpdatedTime.
         double GetAverageTimeBetweenTranslationUpdates() const { return mTranslation.mAvgTimeBetweenUpdates; };
         /// Add onto the dof dead reckoning list where the dof should move
         void AddToDeadReckonDOF(const std::string& dofName, const osg::Vec3& position,
            const osg::Vec3& rateOverTime, const std::string& metricName = "");

         ///@return the rough average amount of time between rotation updates.  This is based on values sent to SetLastRotationUpdatedTime.
         double GetAverageTimeBetweenRotationUpdates() const { return mAverageTimeBetweenRotationUpdates; };
         /// Remove a drDOF from the list at this spot
         void RemoveDRDOF(std::list<dtCore::RefPtr<DeadReckoningDOF> >::iterator& iter);

         void RemoveAllDRDOFByName(const std::string& removeName);

         /// Remove a drdof by checking against values compared to everything else.
         void RemoveDRDOF(DeadReckoningDOF& obj);

         const osg::Vec3& GetCurrentDeadReckonedTranslation() const { return mTranslation.mCurrentDeadReckonedValue; }
         const osg::Vec3& GetCurrentDeadReckonedRotation() const { return mCurrentAttitudeVector; }

         const std::list<dtCore::RefPtr<DeadReckoningDOF> >& GetDeadReckoningDOFs() const { return mDeadReckonDOFS; }

         void SetTranslationBeforeLastUpdate(const osg::Vec3& trans) { mTranslation.mValueBeforeLastUpdate = trans; }
         void SetRotationBeforeLastUpdate(const osg::Quat& rot) { mRotQuatBeforeLastUpdate = rot; }

         const osg::Quat& GetLastKnownRotationByQuaternion() const { return mLastQuatRotation; }

         bool IsTranslationUpdated() const { return mTranslation.mUpdated; }
         bool IsRotationUpdated() const { return mRotationUpdated; }

         //void SetTranslationCurrentSmoothingTime(float smoothing) { mTranslationCurrentSmoothingTime=smoothing; }
         //float GetTranslationCurrentSmoothingTime() const { return mTranslationCurrentSmoothingTime; }
         virtual void SetTranslationElapsedTimeSinceUpdate(float value);// { mTranslationElapsedTimeSinceUpdate = value; }
         float GetTranslationElapsedTimeSinceUpdate() const { return mTranslation.mElapsedTimeSinceUpdate; }

         void SetRotationElapsedTimeSinceUpdate(float value) { mRotationElapsedTimeSinceUpdate = value; }
         float GetRotationElapsedTimeSinceUpdate() const { return mRotationElapsedTimeSinceUpdate; }

         void SetRotationResolved(bool resolved) { mRotationResolved=resolved; }

         GroundClampingData& GetGroundClampingData() { return mGroundClampingData; }
         const GroundClampingData& GetGroundClampingData() const { return mGroundClampingData; }

         /**
          * @return true if maintaining constant smoothing time, false (default) if using avg update time.
          */
         bool GetUseFixedSmoothingTime() const { return mUseFixedSmoothingTime; }

         /**
         * When this is true, the DR algorithm will maintain a constant smoothing time 
         * by using the FixedSmoothingTime, which is set with SetFixedSmoothingTime(). 
         * This is used when you anticipate you are going to get updates on a very 
         * regular basis and so you want the smoothing to be done at a steady rate. 
         * note - Rotation & Translation share the same fixed smoothing time, unlike min/max. 
         * If false (the default), it will use the avg update time if that is smaller than the max smoothing time.
         * @param newValue true if maintaining constant smoothing time, false (default) if using avg update time.
         */
         void SetUseFixedSmoothingTime(bool newValue) { mUseFixedSmoothingTime = newValue; }

         /// Returns the fixed smoothing time. @See SetUseFixedSmoothingTime for more info
         float GetFixedSmoothingTime() { return mFixedSmoothingTime; }
         /// Sets the fixed smoothing time. @see SetUseFixedSmoothingTime for more info
         void SetFixedSmoothingTime(float newValue) { mFixedSmoothingTime = newValue; }

         /// Makes the SetLastKnownRotation() ignore the pitch and yaw values to help keep some objects upright.
         void SetForceUprightRotation(bool newValue);
         bool GetForceUprightRotation() const;

         /// Supports the following deprecated properties: 'Flying' 
         dtCore::RefPtr<dtCore::ActorProperty> GetDeprecatedProperty(const std::string& name) override;

      protected:
         virtual ~DeadReckoningActorComponent();// {}

         ///perform static dead-reckoning, which means applying the new position directly and ground clamping.  xform will be updated.
         virtual void DRStatic(dtCore::Transformable& txable, dtCore::Transform& xform, dtUtil::Log* pLogger);

         /**
          * perform velocity + acceleration dead-reckoning.  Acceleration may be ignored.  xform will be updated.
          * @return returns true if it thinks it made a change, false otherwise.
          */
         virtual bool DRVelocityAcceleration(dtCore::Transformable& txable, dtCore::Transform& xform, dtUtil::Log* pLogger);

         /*
          * Simple dumps out a log that we have started dead reckoning with lots of information.  Pulled out
          * to help make DRVelocityAcceleration() a bit easier to read.
          */
         void LogDeadReckonStarted(osg::Vec3& unclampedTranslation, osg::Matrix& rot, dtUtil::Log* pLogger);

         /**
          * Computes the new rotation for the object.  This method handles VELOCITY_ONLY and
          * VELOCITY_AND_ACCELERATION, but not static. This is called DRVelocityAcceleration().
          */
         void DeadReckonRotation(dtCore::Transform& xform);


      private:
         /** 
          * A private, internal version of the SetLastKnownRotation. This is needed 
          * because in STAGE and other tools, the rotation is displayed as XYZ.
          * Note - HPR is like ZXY order so we have to rearrange to be PRH order.
          * THERE IS NO REASON YOU SHOULD CALL THIS. It is needed for the property only
          */
         void SetInternalLastKnownRotationInXYZ(const osg::Vec3& vec);

         /** 
          * A private, internal version of the GetLastKnownRotation. This is needed 
          * because in STAGE and other tools, the rotation is displayed as XYZ.
          * Note - HPR is like ZXY order so we have to rearrange to be PRH order.
          * THERE IS NO REASON YOU SHOULD CALL THIS. It is needed for the property only
          */
         osg::Vec3 GetInternalLastKnownRotationInXYZ() const;


         DRVec3Util mTranslation; // Holds all the DR data for the world coordiante (aka Translation)

         /// The list of DeadReckoningDOFs, might want to change to has table of list later.
         std::list<dtCore::RefPtr<DeadReckoningDOF> > mDeadReckonDOFS;

         /// The Dead reckoning DOF Container object
         dtCore::RefPtr<dtUtil::NodeCollector> mDOFDeadReckoning;

         GroundClampingData mGroundClampingData;

         ///the simulation time this was last updated.
         double mLastRotationUpdatedTime;
         float mAverageTimeBetweenRotationUpdates;

         ///The maximum amount of time to use when smoothing rotation.
         float mMaxRotationSmoothingTime;
         /// True means we maintain constant smoothing time for blending. Uses Max Rot or Trans as appropriate
         bool mUseFixedSmoothingTime;
         float mFixedSmoothingTime; /// The smoothing time for rot & trans if mUseFixedSmoothingTime is true

         ///the amount of time since this actor started smoothing.
         float mRotationElapsedTimeSinceUpdate;

         ///the end amount of time to use when smoothing the rotation.  At this point, the blend should be finished.
         float mRotationEndSmoothingTime;

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

         ///The angular velocity vector.
         osg::Vec3 mAngularVelocityVector;

         // The Dead Reckoning Matrix
         osg::Matrix mDeadReckoningMatrix;

         DeadReckoningAlgorithm* mMinDRAlgorithm;

         /// The update mode - whether to actually move the actor or to just calculate.
         UpdateMode* mUpdateMode;

         bool mRotationInitiated;
         bool mUpdated;
         bool mRotationUpdated;
         //bool mFlying; // Deprecated now - use GroundClampType instead
         // if the rotation has been resolved to the last updated version.
         bool mRotationResolved;
         bool mExtraDataUpdated; // set to true when an important non-positional related property is changed. 

         bool mForceUprightRotation; // Used to keep characters (et al) from wierd leaning over, regardless of the source data

         float mCurTimeDelta; // Tracks how long this process step is for. Used to compute instant vel.

         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         //DeadReckoningActorComponent(const DeadReckoningActorComponent&) {}
         //DeadReckoningActorComponent& operator=(const DeadReckoningActorComponent&) {return *this;}
   };

}

#endif
