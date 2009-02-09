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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <prefix/dtgameprefix-src.h>
#include <dtCore/batchisector.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtGame/defaultgroundclamper.h>
#include <dtUtil/log.h>
#include <dtUtil/boundingshapeutils.h>
#include <osg/io_utils>
#include <osg/Matrix>
#include <cmath>
#include <sstream>



namespace dtGame
{
   /////////////////////////////////////////////////////////////////////////////
   // DEFAULT GROUND CLAMPER
   /////////////////////////////////////////////////////////////////////////////
   DefaultGroundClamper::DefaultGroundClamper()
      : dtGame::BaseGroundClamper()
      , mTripleIsector(new dtCore::BatchIsector)
      , mIsector(new dtCore::BatchIsector)
   {
      mGroundClampBatch.reserve(32);
   }

   /////////////////////////////////////////////////////////////////////////////
   DefaultGroundClamper::~DefaultGroundClamper()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DefaultGroundClamper::HasValidSurface() const
   {
      return GetTerrainActor() != NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned DefaultGroundClamper::GetClampBatchSize() const
   {
      return unsigned(mGroundClampBatch.size());
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::BatchIsector& DefaultGroundClamper::GetGroundClampIsector()
   {
      return *mIsector;
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void DefaultGroundClamper::CalculateAndSetBoundingBox(osg::Vec3& modelDimensions,
         dtDAL::TransformableActorProxy& proxy, GroundClampingData& data)
   {
      if (GetLogger().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         GetLogger().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
               "Determining Actor dimensions via Bounding Box visitor.");
      }

      dtCore::Transformable* actor = NULL;
      proxy.GetActor(actor);

      osg::Matrix oldMatrix = actor->GetMatrix();
      actor->SetMatrix(osg::Matrix::identity());

      dtUtil::BoundingBoxVisitor bbv;
      actor->GetOSGNode()->accept(bbv);

      actor->SetMatrix(oldMatrix);

      modelDimensions.x() = bbv.mBoundingBox.xMax() - bbv.mBoundingBox.xMin();
      modelDimensions.y() = bbv.mBoundingBox.yMax() - bbv.mBoundingBox.yMin();
      modelDimensions.z() = bbv.mBoundingBox.zMax() - bbv.mBoundingBox.zMin();

      data.SetModelDimensions(modelDimensions);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DefaultGroundClamper::GetClosestHit( const dtDAL::TransformableActorProxy& proxy,
      GroundClampingData& data, dtCore::BatchIsector::SingleISector& single, float pointZ,
      osg::Vec3& outHit, osg::Vec3& outNormal)
   {
      bool found = false;
      float diff = FLT_MAX;
      osg::Vec3 tempHit;
      for (int i = 0; i < single.GetNumberOfHits(); ++i)
      {
         single.GetHitPoint(tempHit, i);
         float newDiff = std::abs(tempHit.z() - pointZ);
         if (newDiff < diff)
         {
            diff = newDiff;
            outHit = tempHit;
            single.GetHitPointNormal(outNormal, i);
            found = true;
         }
      }
      return found;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DefaultGroundClamper::GetMissingHit(const dtDAL::TransformableActorProxy& proxy,
      GroundClampingData& data, float pointZ, osg::Vec3& outHit, osg::Vec3& outNormal)
   {
      // This method is a place holder for any sub-class that wants to
      // improvise a new point if one was not detected by an Isector.
      // Thus, this method has no implementation.

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultGroundClamper::GetActorDetectionPoints(dtDAL::TransformableActorProxy& proxy,
      GroundClampingData& data, osg::Vec3 outPoints[3])
   {
      osg::Vec3 modelDimensions = data.GetModelDimensions();

      if(!data.UseModelDimensions())
      {
         CalculateAndSetBoundingBox(modelDimensions, proxy, data);
      }

      if(GetLogger().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         GetLogger().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
            "Actor dimensions [%f, %f, %f].", 
            modelDimensions[0], modelDimensions[1], modelDimensions[2]);
      }

      outPoints[0].set(0.0f, modelDimensions[1] / 2.0, 0.0f);
      outPoints[1].set(modelDimensions[0] / 2.0, -(modelDimensions[1] / 2.0), 0.0f);
      outPoints[2].set(-(modelDimensions[0] / 2.0), -(modelDimensions[1] / 2.0), 0.0f);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultGroundClamper::OrientTransformToSurfacePoints(dtCore::Transform& xform,
      osg::Matrix& rotation, const osg::Vec3& location, const osg::Vec3 points[3])
   {
      osg::Vec3 ab = points[0] - points[2];
      osg::Vec3 ac = points[0] - points[1];

      osg::Vec3 normal = ab ^ ac;
      normal.normalize();

      OrientTransform( xform, rotation, location, normal );
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultGroundClamper::OrientTransform(dtCore::Transform& xform,
      osg::Matrix& rotation, const osg::Vec3& location, const osg::Vec3& normal)
   {
      osg::Vec3 oldNormal(0, 0, 1);

      oldNormal = osg::Matrix::transform3x3(oldNormal, rotation);
      osg::Matrix normalRot;
      normalRot.makeRotate(oldNormal, normal);

      rotation = rotation * normalRot;

      xform.Set(location, rotation);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultGroundClamper::GetSurfacePoints(const dtDAL::TransformableActorProxy& proxy,
      GroundClampingData& data, const dtCore::Transform& xform, osg::Vec3 inOutPoints[3])
   {
      dtUtil::Log& logger = GetLogger();
      bool debugEnabled = logger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG);

      mTripleIsector->Reset();
      mTripleIsector->SetQueryRoot(GetTerrainActor());

      for (unsigned i = 0; i < 3; ++i)
      {
         dtCore::BatchIsector::SingleISector& single = mTripleIsector->EnableAndGetISector(i);

         // The input point should be in world space.
         const osg::Vec3& singlePoint = inOutPoints[i];

         if (osg::isNaN(singlePoint.x()) || osg::isNaN(singlePoint.y()) || osg::isNaN(singlePoint.z()))
         {
            if (logger.IsLevelEnabled(dtUtil::Log::LOG_INFO))
            {
               logger.LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, 
                  "Intersect point has parts that are NAN, no ground clamping will be performed "
                  "on actor named \"%s\".", proxy.GetName().c_str());
            } 
            return;
         }

         if(debugEnabled)
         {
            logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
               "Intersect point %d [%f, %f, %f].", i, singlePoint.x(), singlePoint.y(), singlePoint.z());
         } 
         single.SetSectorAsLineSegment(osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] + 100.0f),
            osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] - 100.0f));
      }

      if (mTripleIsector->Update(GetLastEyePoint(), GetEyePointActor() == NULL))
      {
         for (unsigned i = 0; i < 3; ++i)
         {
            dtCore::BatchIsector::SingleISector& single = mTripleIsector->EnableAndGetISector(i);
            osg::Vec3 hp(inOutPoints[i]), normal;

            if (GetClosestHit( proxy, data, single, inOutPoints[i].z(), hp, normal))
            {
               if(debugEnabled)
               {
                  std::ostringstream ss;
                  ss << "Found a hit - old z \"" << inOutPoints[i].z() << "\" new z \"" << hp.z() << "\".";
                  logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
               }
               inOutPoints[i] = hp;
            }
            else
            {
               // Get an improvised point and normal if possible.
               GetMissingHit(proxy, data, inOutPoints[i].z(), inOutPoints[i], normal);

               std::ostringstream ss;
               ss << "Found no hit on line segment [" << i <<  "] on points:";
               for (unsigned i = 0; i < 3; ++i)
               {
                  ss << " [" << inOutPoints[i] << "]";
               }

               logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());

            }
         }
      } 
      else if(debugEnabled)
      {
         std::ostringstream ss;
         ss << "Found no hit with eye point \"" << GetLastEyePoint() << "\" on points:";
         for (unsigned i = 0; i < 3; ++i)
         {
            ss << " \"" << inOutPoints[i] << "\"";
         }

         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultGroundClamper::FinalizeSurfacePoints(dtDAL::TransformableActorProxy& proxy,
      GroundClampingData& data, osg::Vec3 inOutPoints[3])
   {
      // No implementation for this class but this method is a placeholder
      // for any possible sub-classes needing to make final adjustments.
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultGroundClamper::ClampToGroundThreePoint(dtCore::Transform& xform,
      dtDAL::TransformableActorProxy& proxy, GroundClampingData& data,
      DefaultGroundClamper::RuntimeData& runtimeData)
   {
      dtUtil::Log& logger = GetLogger();
      bool debugEnabled = logger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG);

      if(debugEnabled)
      {
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Using three point ground clamping.");
      }

      osg::Matrix rotation;
      xform.GetRotation(rotation);

      // Get the 3 surface points based on the actors bounding box.
      osg::Vec3 points[3];
      GetActorDetectionPoints(proxy, data, points);

      // Convert points from actor-relative space to world space.
      osg::Matrix transformMatrix;
      xform.Get(transformMatrix);
      points[0] = points[0] * transformMatrix;
      points[1] = points[1] * transformMatrix;
      points[2] = points[2] * transformMatrix;

      GetSurfacePoints(proxy, data, xform, points);

      // Allow any sub-classes to adjust the final clamp points,
      // in case the terrain is not completely rigid; for example mud, water, sand, etc.
      FinalizeSurfacePoints(proxy, data, points);

      float averageZ = 0;
      for(unsigned i = 0; i < 3; ++i)
      {
         averageZ += points[i].z();
      }
      averageZ /= 3;

      osg::Vec3 position;
      xform.GetTranslation(position);

      //save the offset
      runtimeData.SetLastClampedOffset(averageZ - position.z());

      //move the actor position up to the ground.
      position.z() = averageZ;

      if(debugEnabled)
      {
         std::ostringstream ss;
         ss << "Setting z position to average hit position \"" << position
            << "\" with clamp offset of \"" << runtimeData.GetLastClampedOffset() << "\"";

         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }

      OrientTransformToSurfacePoints(xform, rotation, position, points);
      runtimeData.SetLastClampedRotation( rotation );
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultGroundClamper::ClampToGroundIntermittent(double currentTime,
            dtCore::Transform& xform,
            dtDAL::TransformableActorProxy& proxy, GroundClampingData& data,
            DefaultGroundClamper::RuntimeData& runtimeData)
   {
      //probably need some smoothing code here.
      if( (runtimeData.GetLastClampedTime() + GetIntermittentGroundClampingTimeDelta() )<= currentTime)
      {
         runtimeData.SetLastClampedTime(currentTime);
         mGroundClampBatch.push_back(std::make_pair(xform, std::make_pair(&proxy, &data)));
         if(mGroundClampBatch.size() == 32)
         {
            RunClampBatch();
         }
      }
      else
      {
         osg::Vec3 position;
         xform.GetTranslation(position);

         if(GetLogger().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Using previous offset for actor z \"" << runtimeData.GetLastClampedOffset() 
               << "\".  Starting position is \"" << position << "\".";

            GetLogger().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }

         position.z() += runtimeData.GetLastClampedOffset();
         xform.SetTranslation(position);
         static_cast<dtCore::Transformable*>(proxy.GetActor())->SetTransform(xform, dtCore::Transformable::REL_CS);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultGroundClamper::RunClampBatch()
   {
      if (mGroundClampBatch.empty())
      {
         return;
      }

      dtUtil::Log& logger = GetLogger();
      bool debugEnabled = logger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG);

      if(mGroundClampBatch.size() > 32)
      {
         logger.LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, 
            "Attempted to batch %u entities, when 32 is the max.", mGroundClampBatch.size());        
      }

      mIsector->Reset();
      mIsector->SetQueryRoot(GetTerrainActor());

      for (size_t i = 0; i < mGroundClampBatch.size(); ++i)
      {
         dtCore::BatchIsector::SingleISector& single = mIsector->EnableAndGetISector(i);

         if(debugEnabled)
         {
            logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Using one point ground clamping.");
         }

         dtCore::Transform& xform = mGroundClampBatch[i].first;
         osg::Vec3 singlePoint;
         xform.GetTranslation(singlePoint);

         single.SetSectorAsLineSegment(osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] + 100.0f),
               osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] - 100.0f));
      }

      bool ignoreEyePoint = GetEyePointActor() == NULL;
      if (!mIsector->Update(GetLastEyePoint(), ignoreEyePoint))
      {
         if(debugEnabled)
         {
            std::ostringstream ss;
            ss << "Found no hits with batch query.";
            logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }
      }

      // Set the positions even if there are no hits.
      osg::Vec3 normal;
      osg::Vec3 hp;

      BatchVector::iterator i, iend;
      i = mGroundClampBatch.begin();
      iend = mGroundClampBatch.end();

      unsigned index = 0;
      for(; i != iend; ++i, ++index)
      {
         dtCore::Transform& xform = i->first;
         osg::Matrix rotation;
         xform.GetRotation(rotation);
         osg::Vec3 singlePoint;
         xform.GetTranslation(singlePoint);

         dtCore::BatchIsector::SingleISector& single = mIsector->EnableAndGetISector(index);

         dtDAL::TransformableActorProxy* proxy = i->second.first;
         GroundClampingData* gcData = i->second.second;

         // Get the proxy's actor since it has the transform data.
         dtCore::Transformable* actor = NULL;
         proxy->GetActor(actor);

         // Check if user runtime data is valid.
         RuntimeData& runtimeData = GetOrCreateRuntimeData(*gcData);

         // Default the hit point.
         hp.set(singlePoint.x(), singlePoint.y(), 0.0f);

         if(GetClosestHit(*proxy, *gcData, single, singlePoint.z(), hp, normal))
         {
            if(debugEnabled)
            {
               std::ostringstream ss;
               ss << "Found a hit - old z " << singlePoint.z() << " new z " << hp.z();
               logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
            }

            runtimeData.SetLastClampedOffset(hp.z() - singlePoint.z());

            if(gcData->GetAdjustRotationToGround())
            {
               normal.normalize();
               OrientTransform(xform, rotation, hp, normal);
               runtimeData.SetLastClampedRotation(rotation);
            }
            else
            {
               xform.Set(hp, rotation);
            }

            actor->SetTransform(xform, dtCore::Transformable::REL_CS);
         }
         else
         {
            if(GetMissingHit(*proxy, *gcData, singlePoint.z(), hp, normal))
            {
               runtimeData.SetLastClampedOffset(hp.z() - singlePoint.z());

               if(gcData->GetAdjustRotationToGround())
               {
                  normal.normalize();
                  OrientTransform(xform, rotation, hp, normal);
                  runtimeData.SetLastClampedRotation(rotation);
               }
               else
               {
                  xform.Set(hp, rotation);
               }
            }
            else
            {
               runtimeData.SetLastClampedOffset(0);
            }
            actor->SetTransform(xform, dtCore::Transformable::REL_CS);
         }
      }
      mGroundClampBatch.clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultGroundClamper::ClampToGround(DefaultGroundClamper::GroundClampingType& type,
      double currentTime, dtCore::Transform& xform, dtDAL::TransformableActorProxy& proxy,
      GroundClampingData& data, bool transformChanged, const osg::Vec3& velocity)
   {
      // Get the actor that has the transform data.
      dtCore::Transformable* actor = NULL;
      proxy.GetActor(actor);

      // Get or create the Runtime Data and make sure it exists for any subsequent methods
      // that expect it to be in the Ground Clamping Data.
      RuntimeData& runtimeData = GetOrCreateRuntimeData(data);

      // Determine if a different clamp type should be used based on the object and
      // other factors such as transform change or velocity.
      DefaultGroundClamper::GroundClampingType* clampType
         = &GetBestClampType(type, proxy, data, transformChanged, velocity);

      // Avoid any further processing on certain conditions.
      if(!HasValidSurface() || *clampType == DefaultGroundClamper::GroundClampingType::NONE)
      {
         // If no terrain, just set the position and exit.
         actor->SetTransform(xform, dtCore::Transformable::REL_CS);

         return;
      }

      // Use the last clamped rotation only if the transformation has been flagged
      // not to change. Dead Reckoning Helpers set the alter rotation every frame
      // which causes a twitch bug if the following code does not exist.
      if(!transformChanged)
      {
         xform.SetRotation(runtimeData.GetLastClampedRotation());
      }

      dtUtil::Log& logger = GetLogger();
      bool debugEnabled = logger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG);

      if(debugEnabled)
      {
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Ground clamping actor.");
      }

      if(*clampType == DefaultGroundClamper::GroundClampingType::RANGED)
      {
         osg::Vec3 position;
         xform.GetTranslation(position);
 
         const osg::Vec3 eyePoint = GetLastEyePoint();
         float distanceToEyeSqr = (position - eyePoint).length2();
         if((GetEyePointActor() != NULL
                  && GetLowResGroundClampingRange() > 0.0f
                  && distanceToEyeSqr > GetLowResGroundClampingRange2()))
         {
            ClampToGroundIntermittent(currentTime, xform, proxy, data, runtimeData);
         }
         else if( ! data.GetAdjustRotationToGround() || 
               (GetEyePointActor() != NULL
                  && GetHighResGroundClampingRange() > 0.0f
                  && distanceToEyeSqr > GetHighResGroundClampingRange2()))
         {
            // this should be moved.
            mGroundClampBatch.push_back(std::make_pair(xform, std::make_pair(&proxy, &data)));
            if (mGroundClampBatch.size() == 32)
            {
               RunClampBatch();
            }
         }
         else
         {
            ClampToGroundThreePoint(xform, proxy, data, runtimeData);
            //position has changed, so get it again.
            xform.GetTranslation(position);

            if(debugEnabled)
            {
               std::ostringstream ss;
               ss << "New ground-clamped actor position \"" << position << "\".";

               logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
            }

            actor->SetTransform(xform, dtCore::Transformable::REL_CS);

            // Update the runtime data with the last time the object was clamped.
            runtimeData.SetLastClampedTime(currentTime);
         }
      }
      else if(*clampType == DefaultGroundClamper::GroundClampingType::INTERMITTENT_SAVE_OFFSET)
      {
         ClampToGroundIntermittent(currentTime, xform, proxy, data, runtimeData);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   DefaultGroundClamper::GroundClampingType& DefaultGroundClamper::GetBestClampType(
      DefaultGroundClamper::GroundClampingType& suggestedClampType,
      const dtDAL::TransformableActorProxy& proxy, const GroundClampingData& data,
      bool transformChanged, const osg::Vec3& velocity) const
   {
      // The Default Ground Clamper will not make any assumptions in this case.
      // A sub-class should override this method to make any special-case determinations.
      return suggestedClampType;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultGroundClamper::FinishUp()
   {
      // Ground clamping has been completed
      RunClampBatch();
   }

   /////////////////////////////////////////////////////////////////////////////
   DefaultGroundClamper::RuntimeData& DefaultGroundClamper::GetOrCreateRuntimeData(
      dtGame::GroundClampingData& data)
   {
      RuntimeData* runtimeData = dynamic_cast<RuntimeData*>(data.GetUserData());
      if(runtimeData == NULL)
      {
         if(data.GetUserData() != NULL)
         {
            std::ostringstream oss;
            oss << "Ground Clamping Data user data is being replaced by a new Runtime Data.";
            GetLogger().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, oss.str());
         }

         data.SetUserData(new RuntimeData);
         runtimeData = static_cast<RuntimeData*>(data.GetUserData());
      }

      return *runtimeData;
   }
}
