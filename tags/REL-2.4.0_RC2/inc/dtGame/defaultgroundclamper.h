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
#ifndef DELTA_DEFAULTGROUNDCLAMPER
#define DELTA_DEFAULTGROUNDCLAMPER

#include <vector>

#include <dtGame/export.h>
#include <dtGame/basegroundclamper.h>

#include <dtCore/transform.h>
#include <dtCore/batchisector.h>

#include <osg/Referenced>

#include <osg/Vec3>

namespace dtUtil
{
   class Log;
}

namespace dtDAL
{
   class TransformableActorProxy;
}

namespace dtGame
{  
   /**
    * This is a utility class for doing ground clamping.
    */
   class DT_GAME_EXPORT DefaultGroundClamper : public dtGame::BaseGroundClamper
   {
   public:

         /**
          * Inner class for adding extra data to the Ground Clamping Data
          * assigned to each clamped object.
          */
         class DT_GAME_EXPORT RuntimeData : public osg::Referenced
         {
            public:

               RuntimeData()
                  : mLastClampedOffset(0.0f)
                  , mLastClampedTime(0.0)
               {
               }

               /**
                * Sets the last known offset from the existing
                * the position to the terrain.
                */
               void SetLastClampedOffset(float newClampedOffset)
               {
                  mLastClampedOffset = newClampedOffset;
               }

               /**
                * @return the last known offset from the existing
                * the position to the terrain.
                */
               float GetLastClampedOffset() const
               {
                  return mLastClampedOffset;
               }

               /**
                * Sets the last known time the object was flagged to be clamped.
                * @param simTime Simulation time in seconds.
                */
               void SetLastClampedTime(double newTime)
               {
                  mLastClampedTime = newTime;
               }

               /**
                * Gets the last known time the object was flagged to be clamped.
                * @return Simulation time in seconds.
                */
               double GetLastClampedTime() const
               {
                  return mLastClampedTime;
               }

               /**
                * Set the last matrix that was used to orient the object
                * during clamping.
                */
               void SetLastClampedRotation(const osg::Matrix& rotation)
               {
                  mLastClampRotation = rotation;
               }

               /**
                * Get the last matrix that was used to orient the object
                * during clamping.
                */
               const osg::Matrix& GetLastClampedRotation()
               {
                  return mLastClampRotation;
               }

            protected:
               virtual ~RuntimeData()
               {
               }

            private:

               float mLastClampedOffset;
               double mLastClampedTime;
               osg::Matrix mLastClampRotation;
         };

         DefaultGroundClamper();
         virtual ~DefaultGroundClamper();

         /**
          * Determine if the Ground Clamper or any sub-class has a valid surface to clamp to.
          */
         virtual bool HasValidSurface() const;

         /**
          * @return Total objects waiting to be clamped in method RunClampBatch.
          */
         unsigned GetClampBatchSize() const;

         /**
          * Calculates the bounding box for the given proxy, stores it in the data object, and populates the Vec3.
          * @param modelDimensions Capture the calculated box dimensions which is also set on data.
          * @param proxy Actor that should have its bounding box calculated.
          * @param data Set of values to be updated based on the ground clamp operation.
          */
         virtual void CalculateAndSetBoundingBox(osg::Vec3& modelDimensions,
            dtDAL::TransformableActorProxy& proxy, GroundClampingData& data);
         
         /**
          * Gets the ground clamping hit that is closest to the Z value.
          * @param proxy Actor proxy that is to be clamped. This parameter may help
          *              sub-classes of Ground Clamper make better determinations.
          * @param data Ground Clamping Data associated with the proxy.
          * @param single Isector containing points to be checked.
          * @param pointZ Z value to compare against the Isector points.
          * @param outHit Point to capture the values of the point that is the closest match.
          * @param outNormal Normal to capture values of the matched hit point's normal.
          * @return TRUE if a hit point was detected, otherwise FALSE if there are no points
          *         contained in the specified Isector.
          */
         virtual bool GetClosestHit(const dtDAL::TransformableActorProxy& proxy,
            GroundClampingData& data,
            dtCore::BatchIsector::SingleISector& single, float pointZ,
            osg::Vec3& outHit, osg::Vec3& outNormal);

         /**
          * Calculate an improvised surface point and normal closest to the Z value.
          * This method is called if GetClosestHit returns FALSE.
          * @param proxy Actor proxy that is to be clamped. This parameter may help
          *              sub-classes of Ground Clamper make better determinations.
          * @param data Ground Clamping Data associated with the proxy.
          * @param pointZ Z value to compare against the Isector points.
          * @param outHit Point to capture the values of the point that is the closest match.
          * @param outNormal Normal to capture values of the matched hit point's normal.
          * @return TRUE if a hit point was calculated, otherwise FALSE if it could not be calculated.
          */
         virtual bool GetMissingHit(const dtDAL::TransformableActorProxy& proxy,
            GroundClampingData& data, 
            float pointZ, osg::Vec3& outHit, osg::Vec3& outNormal);

         /**
          * Get the surface points of the specified actor based on its model dimensions.
          * @param proxy Actor proxy to have its bounding box calculated.
          * @param data Ground Clamping Data containing model bounding box values to be updated.
          * @param outPoints Container to capture 3 detection points calculated by the actor's bounding box.
          */
         void GetActorDetectionPoints(dtDAL::TransformableActorProxy& proxy,
            GroundClampingData& data, osg::Vec3 outPoints[3]);

         /**
          * Calculate the surface points from the specified location and detection points.
          * @param proxy Actor that owns the specified transform.
          * @param data Ground Clamping Data associated with the proxy.
          * @param xform Transform that has the location and rotation in world space.
          * @param inOutPoints IN: detection points in world space.
          *                    OUT: surface points.
          */
         virtual void GetSurfacePoints( const dtDAL::TransformableActorProxy& proxy,
            GroundClampingData& data, const dtCore::Transform& xform,
            osg::Vec3 inOutPoints[3]);

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
         virtual void ClampToGround(GroundClampingType& type, double currentTime,
            dtCore::Transform& xform, dtDAL::TransformableActorProxy& proxy,
            GroundClampingData& data, bool transformChanged = false,
            const osg::Vec3& velocity = osg::Vec3());

         /**
          * Override this method to make special clamp type determinations on the type of object.
          * This method allows a sub-class clamper to change the clamp type for optimization
          * based on factors such as velocity, transformation change and special-case objects.
          * @param suggestedClampType Clamp type sent into ClampToGround for the specified object.
          * @param proxy Actor to be clamped and is passed in case collision geometry is needed.
          * @param data Ground Clamping Data containing clamping options and runtime data.
          * @param tranformChanged Flag to make a better determination for clamping.
          * @param velocity Instantaneous velocity of the object for the current frame.
          * @return suggestedClampType is returned for the default behavior.
          */
         virtual GroundClampingType& GetBestClampType(GroundClampingType& suggestedClampType,
            const dtDAL::TransformableActorProxy& proxy, const GroundClampingData& data,
            bool transformChanged, const osg::Vec3& velocity) const;

         /**
          * Override method that subsequently calls RunClampBatch once the
          * ground clamping procedures have been completed.
          */
         virtual void FinishUp();

      protected:
         
         /**
          * Modify the specified transform to be oriented to the specified
          * surface points.
          * @param xform Transform to be modified.
          * @param rotation Matrix with original rotation. It will be modified.
          * @param location Position in the world.
          * @param points Surface points used to calculate the oriention.
          */
         void OrientTransformToSurfacePoints(dtCore::Transform& xform,
            osg::Matrix& rotation, const osg::Vec3& location,
            const osg::Vec3 points[3]);

         /**
          * Modify the specified transform to be oriented to the specified
          * surface points.
          * @param xform Transform to be modified.
          * @param rotation Matrix with original rotation. It will be modified.
          * @param location Position in the world.
          * @param normal Surface normal used to set the final orientation.
          */
         void OrientTransform(dtCore::Transform& xform,
            osg::Matrix& rotation, const osg::Vec3& location,
            const osg::Vec3& normal);

         /**
          * Method to handle any surface point modification after the surface points have been
          * detected. Override this method to extend or change the point modification procedures.
          * This is handy for modifying points so that the terrain appears to be soft, like sand,
          * mud, or water.
          * @param proxy Actor proxy that is being clamped. This is passed in case a sub-class of
          *              ground clamper needs to make any determinations based on the actor.
          * @param data Ground clamping data that may have some data fields modified. This may
          *             be used to maintain inertial data per clamp point or rotation velocity.
          * @param inOutPoints IN: Surface points that MAY be modified.
          *                    OUT: Points modified to their final positions.
          */
         virtual void FinalizeSurfacePoints(dtDAL::TransformableActorProxy& proxy,
            GroundClampingData& data, osg::Vec3 inOutPoints[3]);

         /**
          * Version of clamping that uses three intersection points to calculate the height and the rotation.
          * @param xform the current absolute transform of the actor.
          * @param proxy the actual actor.  This is passed case collision geometry is needed.
          * @param data Ground Clamping Data containing clamping options.
          * @param runtimeData Set of values to be updated based on the ground clamp operation.
          */
         void ClampToGroundThreePoint(dtCore::Transform& xform,
            dtDAL::TransformableActorProxy& proxy, GroundClampingData& data,
            RuntimeData& runtimeData);
         
         /**
          * Clamps an actor to the ground by running an intersection query occasionally and saving
          * the offset.
          * @param currentTime the current simulation time. 
          * @param xform the current absolute transform of the actor.
          * @param proxy the actual actor.  This is passed case collision geometry is needed.
          * @param data Ground Clamping Data containing clamping options.
          * @param runtimeData Set of values to be updated based on the ground clamp operation.
          */
         void ClampToGroundIntermittent(double currentTime,
                  dtCore::Transform& xform, dtDAL::TransformableActorProxy& proxy,
                  GroundClampingData& data, RuntimeData& runtimeData);

         /**
          * This should be called manually at the end an group of ground clamping calls.
          * It will go through any remaining ground clamping queries and run them in a batch.
          * This is normally called when the number queued reaches a threshold, but in most cases
          * some are left waiting at the end of a group of calls.
          */
         void RunClampBatch();

         /**
          * Get or create a Runtime Data for the specified Ground Clamp Data.
          * Any existing data that is NOT a Runtime Data instance will be replaced
          * with a new Runtime Data and an error will be logged.
          * Override this method if a sub-class of Ground Clamper uses an extended
          * type of RuntimeData.
          * @data Ground Clamping Data to have a Runtime Data reference accessed;
          *       also created if one does not exist on the specified data.
          */
         virtual RuntimeData& GetOrCreateRuntimeData(dtGame::GroundClampingData& data);

         dtCore::BatchIsector& GetGroundClampIsector();

      private:

         typedef std::pair<dtDAL::TransformableActorProxy*, GroundClampingData*> ProxyAndData;
         typedef std::vector<std::pair<dtCore::Transform, ProxyAndData> > BatchVector;
         
         BatchVector mGroundClampBatch;

         dtCore::RefPtr<dtCore::BatchIsector> mTripleIsector;
         dtCore::RefPtr<dtCore::BatchIsector> mIsector;
   };

}

#endif /*GROUNDCLAMPER_H_*/
