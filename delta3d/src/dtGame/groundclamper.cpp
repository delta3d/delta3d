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
#include <prefix/dtgameprefix-src.h>
#include <dtCore/batchisector.h>
#include <dtGame/groundclamper.h>
#include <dtGame/gameactor.h>

#include <dtCore/boundingboxvisitor.h>

#include <dtUtil/log.h>

#include <osg/io_utils>
#include <osg/Matrix>

#include <sstream>

namespace dtGame
{
   //////////////////////////////////////////////////////////////////////
   //////////       Ground Clamping Data                /////////////////
   //////////////////////////////////////////////////////////////////////
   GroundClampingData::GroundClampingData():
      mGroundOffset(0.0f), mLastClampedOffset(0.0f), mLastClampedTime(0.0),
      mAdjustRotationToGround(true), mUseModelDimensions(false)
   {
   }

   //////////////////////////////////////////////////////////////////////
   GroundClampingData::~GroundClampingData()
   {
   }

   //////////////////////////////////////////////////////////////////////
   //////////       Ground Clamper                      /////////////////
   //////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(GroundClamper::GroundClampingType);
   GroundClamper::GroundClampingType GroundClamper::GroundClampingType::NONE("NONE");
   GroundClamper::GroundClampingType GroundClamper::GroundClampingType::RANGED("RANGED");
   GroundClamper::GroundClampingType 
      GroundClamper::GroundClampingType::INTERMITTENT_SAVE_OFFSET("INTERMITTENT_SAVE_OFFSET");

   GroundClamper::GroundClamper(): mLogger(dtUtil::Log::GetInstance("groundclamper.cpp")),
      mTripleIsector(new dtCore::BatchIsector), mIsector(new dtCore::BatchIsector),
      mHighResClampRange(0.0f), mHighResClampRange2(0.0f), 
      mLowResClampRange(0.0f), mLowResClampRange2(0.0f),
      mIntermittentGroundClampingTimeDelta(1.0f),
      mIntermittentGroundClampingSmoothingTime(0.25)
   {
      mGroundClampBatch.reserve(32);
   }

   //////////////////////////////////////////////////////////////////////
   GroundClamper::~GroundClamper()
   {
   }

   //////////////////////////////////////////////////////////////////////
   void GroundClamper::SetTerrainActor(dtCore::Transformable* newTerrain)
   {
      mTerrainActor = newTerrain;
   }

   //////////////////////////////////////////////////////////////////////
   void GroundClamper::SetEyePointActor(dtCore::Transformable* newEyePointActor)
   {
      mEyePointActor = newEyePointActor;
   }

   //////////////////////////////////////////////////////////////////////
   dtCore::BatchIsector& GroundClamper::GetGroundClampIsector()
   {
      return *mIsector;
   }
   
   //////////////////////////////////////////////////////////////////////
   void GroundClamper::SetHighResGroundClampingRange(float range)
   {
      mHighResClampRange = range;
      mHighResClampRange2 = range * range;
   }

   //////////////////////////////////////////////////////////////////////
   void GroundClamper::SetLowResGroundClampingRange(float range)
   {
      mLowResClampRange = range;
      mLowResClampRange2 = range * range;
   }
   
   //////////////////////////////////////////////////////////////////////
   void GroundClamper::CalculateAndSetBoundingBox(osg::Vec3& modelDimensions,
         dtGame::GameActorProxy& gameActorProxy, GroundClampingData& data)
   {
      if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
               "Determining Actor dimensions via Bounding Box visitor.");
      }
      osg::Matrix oldMatrix = gameActorProxy.GetGameActor().GetMatrix();
      gameActorProxy.GetGameActor().SetMatrix( osg::Matrix::identity() );

      dtCore::BoundingBoxVisitor bbv;
      gameActorProxy.GetGameActor().GetOSGNode()->accept(bbv);

      gameActorProxy.GetGameActor().SetMatrix(oldMatrix);

      modelDimensions.x() = bbv.mBoundingBox.xMax() - bbv.mBoundingBox.xMin();
      modelDimensions.y() = bbv.mBoundingBox.yMax() - bbv.mBoundingBox.yMin();
      modelDimensions.z() = bbv.mBoundingBox.zMax() - bbv.mBoundingBox.zMin();
      data.SetModelDimensions(modelDimensions);
   }

   //////////////////////////////////////////////////////////////////////
   bool GroundClamper::GetClosestHit(dtCore::BatchIsector::SingleISector& single, float pointz,
            osg::Vec3& hit, osg::Vec3& normal)
   {
      bool found = false;
      float diff = FLT_MAX;
      osg::Vec3 tempHit;
      for (int i = 0; i < single.GetNumberOfHits(); ++i)
      {
         single.GetHitPoint(tempHit, i);
         float newDiff = std::abs(tempHit.z() - pointz);
         if (newDiff < diff)
         {
            diff = newDiff;
            hit = tempHit;
            single.GetHitPointNormal(normal, i);
            found = true;
         }
      }
      return found;
   }

   void GroundClamper::UpdateEyePoint()
   {
      //Setup the iSector to use the player position only once so that get transform is not called
      //for every single actor to be clamped.
      if(GetEyePointActor() != NULL)
      {
         dtCore::Transform xform;
         GetEyePointActor()->GetTransform(xform, dtCore::Transformable::ABS_CS);
         mCurrentEyePointABSPos = xform.GetTranslation();

         if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            osg::Vec3& debugPos = xform.GetTranslation();

            mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                     "Setting the eye point to the position %f, %f, %f.",
               debugPos.x(), debugPos.y(), debugPos.z());
         }
      }
      else if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {         
         mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "The isector will not use an eye point.");
         
         mCurrentEyePointABSPos.set(0.0f, 0.0f, 0.0f);
      }
   }
   
   //////////////////////////////////////////////////////////////////////
   void GroundClamper::ClampToGroundThreePoint(dtCore::Transform& xform,
      dtGame::GameActorProxy& gameActorProxy, GroundClampingData& data)
   {
      if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Using three point ground clamping.");
      }

      osg::Vec3& position = xform.GetTranslation();
      osg::Matrix& rotation = xform.GetRotation();

      osg::Vec3 modelDimensions = data.GetModelDimensions(); 
      if (!data.UseModelDimensions())
      {
         CalculateAndSetBoundingBox(modelDimensions, gameActorProxy, data);
      }
      
      if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
               "Actor dimensions [%f, %f, %f].", 
               modelDimensions[0], modelDimensions[1], modelDimensions[2]);
      }

      //make points for the front center and back corners in relative coordinates.
      osg::Vec3 points[3];
      points[0].set(0.0f, modelDimensions[1] / 2, 0.0f);
      points[1].set(modelDimensions[0] / 2, -(modelDimensions[1] / 2), 0.0f);
      points[2].set(-(modelDimensions[0] / 2), -(modelDimensions[1] / 2), 0.0f);

      //must use the updated matrix in xform for the ground-clamp start position.
      osg::Matrix actorMatrix;
      xform.Get(actorMatrix);

      mTripleIsector->Reset();
      mTripleIsector->SetQueryRoot(mTerrainActor.get());

      for (unsigned i = 0; i < 3; ++i)
      {
         dtCore::BatchIsector::SingleISector& single = mTripleIsector->EnableAndGetISector(i);

         //convert point to absolute space.
         points[i] = points[i] * actorMatrix;
         const osg::Vec3& singlePoint = points[i];
         
         if (osg::isNaN(singlePoint.x()) || osg::isNaN(singlePoint.y()) || osg::isNaN(singlePoint.z()))
         {
            if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_INFO))
            {
               mLogger.LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, 
                     "Intersect point has parts that are NAN, no ground clamping will be performed "
                     "on actor named \"%s\".", gameActorProxy.GetName().c_str());
            } 
            return;
         }
         
         if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
                  "Intersect point %d [%f, %f, %f].", i, singlePoint.x(), singlePoint.y(), singlePoint.z());
         } 
         single.SetSectorAsLineSegment(osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] + 100.0f),
               osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] - 100.0f));
      }
      
      if (mTripleIsector->Update(mCurrentEyePointABSPos, GetEyePointActor() == NULL))
      {
         for (unsigned i = 0; i < 3; ++i)
         {
            dtCore::BatchIsector::SingleISector& single = mTripleIsector->EnableAndGetISector(i);
            osg::Vec3 hp, normal;

            if (GetClosestHit(single, points[i].z(), hp, normal))
            {
               if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  std::ostringstream ss;
                  ss << "Found a hit - old z \"" << points[i].z() << "\" new z \"" << hp.z() << "\".";
                  mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
               }
               points[i] = hp;
            }
            else
            {
               std::ostringstream ss;
               ss << "Found no hit on line segment [" << i <<  "] on points:";
               for (unsigned i = 0; i < 3; ++i)
               {
                  ss << " [" << points[i] << "]";
               }
               
               mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
               
            }
         }
      } 
      else if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Found no hit with eye point [" << mCurrentEyePointABSPos << "] on points:";
         for (unsigned i = 0; i < 3; ++i)
         {
            ss << " [" << points[i] << "]";
         }
         
         mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }
      
      float averageZ = 0;
      for (unsigned i = 0; i < 3; ++i)
      {
         averageZ += points[i].z();
      }
      averageZ /= 3;
      
      //save the offset
      data.SetLastClampedOffset(averageZ - position.z());

      //move the actor position up to the ground.
      position.z() = averageZ;


      osg::Vec3 ab = points[0] - points[2];
      osg::Vec3 ac = points[0] - points[1];

      osg::Vec3 normal = ab ^ ac;
      normal.normalize();

      osg::Vec3 oldNormal ( 0, 0, 1 );

      oldNormal = osg::Matrix::transform3x3(oldNormal, rotation);
      osg::Matrix normalRot;
      normalRot.makeRotate(oldNormal, normal);

      rotation = rotation * normalRot;
   }

   //////////////////////////////////////////////////////////////////////
   void GroundClamper::ClampToGroundIntermittent(double currentTime,
            dtCore::Transform& xform,
            dtGame::GameActorProxy& gameActorProxy, GroundClampingData& data)
   {
      //probably need some smoothing code here.
      if ( (data.GetLastClampedTime() + mIntermittentGroundClampingTimeDelta )<= currentTime)
      {
         data.SetLastClampedTime(currentTime);
         mGroundClampBatch.push_back(std::make_pair(xform, std::make_pair(&gameActorProxy, &data)) );
         if (mGroundClampBatch.size() == 32)
            RunClampBatch();
      }
      else
      {
         if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Using previous offset for actor z \"" << data.GetLastClampedOffset() << "\".";

            mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }

         xform.GetTranslation().z() += data.GetLastClampedOffset();
         gameActorProxy.GetGameActor().SetTransform(xform, dtCore::Transformable::REL_CS);
      }

   }

   //////////////////////////////////////////////////////////////////////
   void GroundClamper::RunClampBatch()
   {      
      if (mGroundClampBatch.empty())
         return;
      
      if (mGroundClampBatch.size() > 32)
      {
         mLogger.LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Attempted to batch %u entities, when 32 is the max.", mGroundClampBatch.size());        
      }
      
      //I don't really like doing this, but there doesn't seem to be an efficient way to 
      //reuse the ones that exist.  This should be revisited. - DID - AD 5/1/2007
      //mIsector->DeleteAllISectors();
      mIsector->Reset();
      mIsector->SetQueryRoot(mTerrainActor.get());

      for (size_t i = 0; i < mGroundClampBatch.size(); ++i)
      {
         dtCore::BatchIsector::SingleISector& single = mIsector->EnableAndGetISector(i);

         if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Using one point ground clamping.");
         }

         dtCore::Transform& xform = mGroundClampBatch[i].first;
         osg::Vec3& singlePoint = xform.GetTranslation();
         
         single.SetSectorAsLineSegment(osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] + 100.0f),
               osg::Vec3(singlePoint[0], singlePoint[1], singlePoint[2] - 100.0f));
      }

      if (mIsector->Update(mCurrentEyePointABSPos, GetEyePointActor() == NULL))
      {
         osg::Vec3 normal;
         osg::Vec3 hp;

         BatchVector::iterator i, iend;
         i = mGroundClampBatch.begin();
         iend = mGroundClampBatch.end();

         unsigned index = 0;
         for (; i != iend; ++i, ++index)
         {
            dtCore::Transform& xform = i->first;
            osg::Matrix& rotation = xform.GetRotation();
            osg::Vec3& singlePoint = xform.GetTranslation();

            dtCore::BatchIsector::SingleISector& single = mIsector->EnableAndGetISector(index);
            if (GetClosestHit(single, singlePoint.z(), hp, normal))
            {
               if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  std::ostringstream ss;
                  ss << "Found a hit - old z " << singlePoint.z() << " new z " << hp.z();
                  mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
               }

               dtGame::GameActorProxy* proxy = i->second.first;
               GroundClampingData* gcData = i->second.second;

               gcData->SetLastClampedOffset(hp.z() - singlePoint.z());

               singlePoint = hp;

               if (gcData->GetAdjustRotationToGround())
               {
                  normal.normalize();

                  osg::Vec3 oldNormal(0, 0, 1);

                  oldNormal = osg::Matrix::transform3x3(oldNormal, rotation);
                  osg::Matrix normalRot;
                  normalRot.makeRotate(oldNormal, normal);

                  rotation = rotation * normalRot;
               }

               proxy->GetGameActor().SetTransform(xform, dtCore::Transformable::REL_CS);
            }
         }
      } 
      else if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Found no hits with batch query.";
         mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }
      mGroundClampBatch.clear();
   }

   //////////////////////////////////////////////////////////////////////
   void GroundClamper::ClampToGround(GroundClamper::GroundClampingType& type, double currentTime,
            dtCore::Transform& xform,
            dtGame::GameActorProxy& gameActorProxy, GroundClampingData& data)
   {
      if (GetTerrainActor() == NULL || type == GroundClamper::GroundClampingType::NONE)
      {
         //No terrain? just set the position and exit
         gameActorProxy.GetGameActor().SetTransform(xform, dtCore::Transformable::REL_CS);
         return;
      }

      if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Ground clamping actor.");
      }

      if (type == GroundClamper::GroundClampingType::RANGED)
      {
         osg::Vec3& position = xform.GetTranslation();
   
         const osg::Vec3 eyePoint = GetLastEyePoint();
         if ((GetEyePointActor() != NULL
                  && GetLowResGroundClampingRange() > 0.0f
                  && (position - eyePoint).length2() > mLowResClampRange2))
         {
            ClampToGroundIntermittent(currentTime, xform, gameActorProxy, data);
         }
         else if (!data.GetAdjustRotationToGround() || 
               (GetEyePointActor() != NULL
                  && GetHighResGroundClampingRange() > 0.0f
                  && (position - eyePoint).length2() > mHighResClampRange2))
         {
            // this should be moved.
            mGroundClampBatch.push_back(std::make_pair(xform, std::make_pair(&gameActorProxy, &data)) );
            if (mGroundClampBatch.size() == 32)
               RunClampBatch();
         }
         else
         {
            ClampToGroundThreePoint(xform, gameActorProxy, data);
            position.z() += data.GetGroundOffset();
            data.SetLastClampedOffset(position.z());
   
            if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               std::ostringstream ss;
               ss << "New ground-clamped actor position \"" << position << "\".";
   
               mLogger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
            }
   
            gameActorProxy.GetGameActor().SetTransform(xform, dtCore::Transformable::REL_CS);
         }
      }
      else if (type == GroundClamper::GroundClampingType::INTERMITTENT_SAVE_OFFSET)
      {
         ClampToGroundIntermittent(currentTime, xform, gameActorProxy, data);
      }
   }
}
