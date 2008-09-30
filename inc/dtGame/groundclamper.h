/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology, BMH Operation.
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
#ifndef DELTA_GROUNDCLAMPER
#define DELTA_GROUNDCLAMPER

#include <vector>

#include <dtGame/export.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtCore/batchisector.h>

#include <osg/Referenced>

#include <osg/Vec3>

namespace dtUtil
{
   class Log;
}

namespace dtGame
{

   class GameActorProxy;
   
   class DT_GAME_EXPORT GroundClampingData
   {
      public:
         GroundClampingData();
         ~GroundClampingData();

         /**
          * Sets the offset from the ground the actor should be clamped to.
          * This only matters if flying is set to false.
          * @param newOffset the new offset value. 
          */
         void SetGroundOffset(float newOffset) { mGroundOffset = newOffset; };

         ///@return The distance from the ground that the actor should be.
         float GetGroundOffset() const { return mGroundOffset; }

         /**
          * @return true if the rotation should be adjusted to match the terrain.
          * @note this only matters if flying is set to false.
          */
         bool GetAdjustRotationToGround() const { return mAdjustRotationToGround; }

         /**
          * Sets if the rotation of the actor should be changed to match the terrain
          * @param newFlying the new value to set.
          */
         void SetAdjustRotationToGround(bool newAdjust) { mAdjustRotationToGround = newAdjust; }

         /// Set this to true to tell the DR comp than the model dimensions are now valid. Defaults to false.
         void SetUseModelDimensions(bool newUse) { mUseModelDimensions = newUse; }
         /// @return true if the model dimensions are now valid.
         bool UseModelDimensions() const { return mUseModelDimensions; }

         /// Sets the model dimensions of this helper.  This will call SetUseModelDimensions(true) internally.
         void SetModelDimensions(const osg::Vec3& newDimensions) { mModelDimensions = newDimensions; }

         /// @return the current assigned dimensions of the model for the actor being dead-reckoned
         const osg::Vec3& GetModelDimensions() const { return mModelDimensions; }

         /// Sets the last known offset from the existing the position to the terrain.
         void SetLastClampedOffset(float newClampedOffset) { mLastClampedOffset = newClampedOffset; }
         
         /// @return the last known offset from the existing the position to the terrain.
         float GetLastClampedOffset() const { return mLastClampedOffset; }
         
         void SetLastClampedTime(double newTime) { mLastClampedTime = newTime; }
         double GetLastClampedTime() const { return mLastClampedTime; }

      private:

         osg::Vec3 mModelDimensions;
         float mGroundOffset;

         float mLastClampedOffset;
         double mLastClampedTime;

         bool mAdjustRotationToGround;
         bool mUseModelDimensions;
   };
   
   /**
    * This is a utility class for doing ground clamping.
    */
   class DT_GAME_EXPORT GroundClamper : public osg::Referenced
   {
      public:

         class DT_GAME_EXPORT GroundClampingType : public dtUtil::Enumeration
         {
            DECLARE_ENUM(GroundClampingType);
            public:
               static GroundClampingType NONE;
               static GroundClampingType RANGED;
               static GroundClampingType INTERMITTENT_SAVE_OFFSET;
            private:
               GroundClampingType(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         GroundClamper();
         virtual ~GroundClamper();

         ///@return the terrain actor using the given name.  If it has not yet been queried, the query will run when this is called.
         dtCore::Transformable* GetTerrainActor() { return mTerrainActor.get(); }

         ///@return the terrain actor using the given name.  If it has not yet been queried, the query will run when this is called.
         const dtCore::Transformable* GetTerrainActor() const { return mTerrainActor.get(); };

         ///changes the actor to use for the terrain.
         void SetTerrainActor(dtCore::Transformable* newTerrain);

         /// Caches the eyepoint from the eye point actor.  This MUST be called each frame before clamping.
         void UpdateEyePoint();
         const osg::Vec3& GetLastEyePoint() const { return mCurrentEyePointABSPos; }

         ///@return the actor to use as an eye point for ground clamping.  This determines which LOD to clamp to. 
         dtCore::Transformable* GetEyePointActor() { return mEyePointActor.get(); };

         ///@return the actor to use as an eye point for ground clamping.  This determines which LOD to clamp to. 
         const dtCore::Transformable* GetEyePointActor() const { return mEyePointActor.get(); };

         ///changes the actor to use for the terrain.
         void SetEyePointActor(dtCore::Transformable* newEyePointActor);

         /**
          * Sets the maximum distance from the player that three intersection point clamping will be used.  
          * After this, one intersection will be used. < 0 means don't do high res.
          */
         void SetHighResGroundClampingRange(float range);

         /**
          * @return the maximum distance from the player that three intersection point clamping will be used.  
          */
         float GetHighResGroundClampingRange() const { return mHighResClampRange; }

         /**
          * Sets the maximum distance from the player that three intersection point clamping will be used.  
          * After this, one intersection will be used.  < 0 means don't do low res.
          */
         void SetLowResGroundClampingRange(float range);

         /**
          * @return the maximum distance from the player that three intersection point clamping will be used.  
          */
         float GetLowResGroundClampingRange() const { return mLowResClampRange; }
         
         /// Sets the delta time between clamping when clamping in intermittent mode.
         void SetIntermittentGroundClampingTimeDelta(float newDelta) 
         { mIntermittentGroundClampingTimeDelta = newDelta; }
         
         /// @return the delta time between clamping when clamping in intermittent mode.
         float GetIntermittentGroundClampingTimeDelta() const 
         { return mIntermittentGroundClampingTimeDelta; }

         /// Sets the smoothing time between offsets when clamping in intermittent mode.
         void SetIntermittentGroundClampingSmoothingTime(float newTime) 
         { mIntermittentGroundClampingSmoothingTime = newTime; }
         
         /// @return the smoothing time between offsets when clamping in intermittent mode.
         float GetIntermittentGroundClampingSmoothingTime() const 
         { return mIntermittentGroundClampingSmoothingTime; }

         /// Calculates the bounding box for the given proxy, stores it in the data object, and populates the Vec3.
         void CalculateAndSetBoundingBox(osg::Vec3& modelDimensions,
               dtGame::GameActorProxy& gameActorProxy, GroundClampingData& data);
         
         /// Gets the ground clamping hit that is closest to the deadreckoned z value.
         bool GetClosestHit(dtCore::BatchIsector::SingleISector& single, float pointz,
                  osg::Vec3& hit, osg::Vec3& normal);

         /**
          * Clamps an actor to the ground.  It will pick, based on the type and eye point 
          * which algorithm to Use.
          * @param type  the ground clamping type to perform.
          * @param currentTime the current simulation time.  Used for intermittent gc.
          * @param xform the current absolute transform of the actor.
          * @param gameActorProxy the actual actor.  This is passed case collision geometry is needed.
          * @param helper the deadreckoning helper for the actor
          */
         void ClampToGround(GroundClampingType& type, double currentTime, dtCore::Transform& xform,
            dtGame::GameActorProxy& gameActorProxy, GroundClampingData& data);

         ///Version of clamping that uses three intersection points to calculate the height and the rotation.
         void ClampToGroundThreePoint(dtCore::Transform& xform,
            dtGame::GameActorProxy& gameActorProxy, GroundClampingData& data);
         
         /**
          * Clamps an actor to the ground by running an intersection query occasionally and saving
          * the offset.
          * @param currentTime the current simulation time. 
          * @param xform the current absolute transform of the actor.
          * @param gameActorProxy the actual actor.  This is passed case collision geometry is needed.
          * @param helper the deadreckoning helper for the actor
          */
         void ClampToGroundIntermittent(double currentTime,
                  dtCore::Transform& xform,
                  dtGame::GameActorProxy& gameActorProxy, GroundClampingData& data);

         /**
          * This should be called manually at the end an group of ground clamping calls.
          * It will go through any remaining ground clamping queries and run them in a batch.
          * This is normally called when the number queued reaches a threshold, but in most cases
          * some are left waiting at the end of a group of calls.
          */
         void RunClampBatch();
         
         dtCore::BatchIsector& GetGroundClampIsector();

      private:
         typedef std::pair<dtGame::GameActorProxy*, GroundClampingData*> ProxyAndData;
         typedef std::vector<std::pair<dtCore::Transform, ProxyAndData> > BatchVector;

         dtUtil::Log& mLogger;
         
         BatchVector mGroundClampBatch;

         dtCore::RefPtr<dtCore::Transformable> mEyePointActor;
         dtCore::RefPtr<dtCore::Transformable> mTerrainActor;

         dtCore::RefPtr<dtCore::BatchIsector> mTripleIsector, mIsector;

         float mHighResClampRange, mHighResClampRange2;
         float mLowResClampRange, mLowResClampRange2;
         float mIntermittentGroundClampingTimeDelta;
         float mIntermittentGroundClampingSmoothingTime;

         osg::Vec3 mCurrentEyePointABSPos;

   };

}

#endif /*GROUNDCLAMPER_H_*/
