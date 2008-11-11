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
 * David Guthrie, Chris Rodgers
 */

#ifndef DELTA_BASEGROUNDCLAMPER
#define DELTA_BASEGROUNDCLAMPER

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtGame/export.h>
#include <dtCore/transformable.h>
#include <osg/Referenced>
#include <osg/Vec3>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtDAL
{
   class TransformableActorProxy;
}

namespace dtUtil
{
   class Log;
}

namespace dtGame
{
   /////////////////////////////////////////////////////////////////////////////
   // CLAMPING DATA CODE
   /////////////////////////////////////////////////////////////////////////////
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
         void SetGroundOffset(float newOffset) { mGroundOffset = newOffset; }

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

         /**
          * Set additional data associated with ground clamping.
          * This is intended for sub-classes of Base Ground Clamper.
          */
         void SetUserData(osg::Referenced* userData) { mUserData = userData; }

         /**
          * Get additional data associated with ground clamping.
          * This is intended for sub-classes of Base Ground Clamper.
          */
         osg::Referenced* GetUserData() { return mUserData.get(); }
         const osg::Referenced* GetUserData() const { return mUserData.get(); }

      private:

         float mGroundOffset;

         bool mAdjustRotationToGround;
         bool mUseModelDimensions;
         osg::Vec3 mModelDimensions;
         dtCore::RefPtr<osg::Referenced> mUserData;
   };
   
   /**
    * This is a utility class for doing ground clamping.
    */
   class DT_GAME_EXPORT BaseGroundClamper : public osg::Referenced
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

         BaseGroundClamper();
         virtual ~BaseGroundClamper();

         ///@return the terrain actor using the given name.  If it has not yet been queried, the query will run when this is called.
         dtCore::Transformable* GetTerrainActor();

         ///@return the terrain actor using the given name.  If it has not yet been queried, the query will run when this is called.
         const dtCore::Transformable* GetTerrainActor() const;

         ///changes the actor to use for the terrain.
         void SetTerrainActor(dtCore::Transformable* newTerrain);

         /// Caches the eyepoint from the eye point actor.  This MUST be called each frame before clamping.
         void UpdateEyePoint();
         const osg::Vec3& GetLastEyePoint() const;

         ///@return the actor to use as an eye point for ground clamping.  This determines which LOD to clamp to. 
         dtCore::Transformable* GetEyePointActor();

         ///@return the actor to use as an eye point for ground clamping.  This determines which LOD to clamp to. 
         const dtCore::Transformable* GetEyePointActor() const;

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
         float GetHighResGroundClampingRange() const;

         /**
          * @return the maximum squared distance from the player that three intersection point clamping will be used.  
          */
         float GetHighResGroundClampingRange2() const;

         /**
          * Sets the maximum distance from the player that three intersection point clamping will be used.  
          * After this, one intersection will be used.  < 0 means don't do low res.
          */
         void SetLowResGroundClampingRange(float range);

         /**
          * @return the maximum distance from the player that three intersection point clamping will be used.  
          */
         float GetLowResGroundClampingRange() const;

         /**
          * @return the maximum squared distance from the player that three intersection point clamping will be used.  
          */
         float GetLowResGroundClampingRange2() const;

         /// Sets the delta time between clamping when clamping in intermittent mode.
         void SetIntermittentGroundClampingTimeDelta(float newDelta);

         /// @return the delta time between clamping when clamping in intermittent mode.
         float GetIntermittentGroundClampingTimeDelta() const;

         /// Sets the smoothing time between offsets when clamping in intermittent mode.
         void SetIntermittentGroundClampingSmoothingTime(float newTime);

         /// @return the smoothing time between offsets when clamping in intermittent mode.
         float GetIntermittentGroundClampingSmoothingTime() const;

         /**
          * Clamps an actor to the ground.  It will pick, based on the type and eye point 
          * which algorithm to Use.
          * @param type Ground clamping type to perform.
          * @param currentTime Current simulation time. Used for intermittent ground clamping.
          * @param xform Current absolute transform of the actor.
          * @param proxy Actor to be clamped and is passed in case collision geometry is needed.
          * @param data Ground Clamping Data containing clamping options.
          * @param transformChanged Flag to help the clamper to determine if it should perform a clamp or not.
          * @param velocity The transformable's instantaneous velocity for the current frame.
          */
         virtual void ClampToGround(GroundClampingType& type, double currentTime, dtCore::Transform& xform,
            dtDAL::TransformableActorProxy& proxy, GroundClampingData& data,
            bool transformChanged = false,
            const osg::Vec3& velocity = osg::Vec3()) = 0;

         /**
          * Override this method to handle any cleanup after ground clamping has been completed.
          */
         virtual void FinishUp() = 0;

      protected:
         dtUtil::Log& GetLogger();

      private:
         dtUtil::Log& mLogger;

         dtCore::RefPtr<dtCore::Transformable> mEyePointActor;
         dtCore::RefPtr<dtCore::Transformable> mTerrainActor;

         float mHighResClampRange;
         float mHighResClampRange2;
         float mLowResClampRange;
         float mLowResClampRange2;
         float mIntermittentGroundClampingTimeDelta;
         float mIntermittentGroundClampingSmoothingTime;

         osg::Vec3 mCurrentEyePointABSPos;

   };

}

#endif /*GROUNDCLAMPER_H_*/
