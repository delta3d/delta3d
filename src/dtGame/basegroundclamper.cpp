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
#include <dtDAL/transformableactorproxy.h>
#include <dtGame/basegroundclamper.h>
#include <dtCore/transform.h>
#include <dtUtil/log.h>
#include <osg/io_utils>



namespace dtGame
{
   /////////////////////////////////////////////////////////////////////////////
   // GROUND CLAMPING DATA
   /////////////////////////////////////////////////////////////////////////////
   GroundClampingData::GroundClampingData()
      : mGroundOffset(0.0f)
      , mAdjustRotationToGround(true)
      , mUseModelDimensions(false)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   GroundClampingData::~GroundClampingData()
   {
   }



   /////////////////////////////////////////////////////////////////////////////
   // GROUND CLAMPING TYPE
   /////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(BaseGroundClamper::GroundClampingType);
   BaseGroundClamper::GroundClampingType BaseGroundClamper::GroundClampingType::NONE("NONE");
   BaseGroundClamper::GroundClampingType BaseGroundClamper::GroundClampingType::RANGED("RANGED");
   BaseGroundClamper::GroundClampingType 
      BaseGroundClamper::GroundClampingType::INTERMITTENT_SAVE_OFFSET("INTERMITTENT_SAVE_OFFSET");



   /////////////////////////////////////////////////////////////////////////////
   // BASE GROUND CLAMPER
   /////////////////////////////////////////////////////////////////////////////
   BaseGroundClamper::BaseGroundClamper()
      : mLogger(dtUtil::Log::GetInstance("basegroundclamper.cpp"))
      , mHighResClampRange(0.0f)
      , mHighResClampRange2(0.0f)
      , mLowResClampRange(0.0f)
      , mLowResClampRange2(0.0f)
      , mIntermittentGroundClampingTimeDelta(1.0f)
      , mIntermittentGroundClampingSmoothingTime(0.25)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   BaseGroundClamper::~BaseGroundClamper()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::Transformable* BaseGroundClamper::GetTerrainActor()
   {
      return mTerrainActor.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::Transformable* BaseGroundClamper::GetTerrainActor() const
   {
      return mTerrainActor.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseGroundClamper::SetTerrainActor(dtCore::Transformable* newTerrain)
   {
      mTerrainActor = newTerrain;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseGroundClamper::SetEyePointActor(dtCore::Transformable* newEyePointActor)
   {
      mEyePointActor = newEyePointActor;
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void BaseGroundClamper::SetHighResGroundClampingRange(float range)
   {
      mHighResClampRange = range;
      mHighResClampRange2 = range * range;
   }

   /////////////////////////////////////////////////////////////////////////////
   float BaseGroundClamper::GetHighResGroundClampingRange() const
   {
      return mHighResClampRange;
   }

   /////////////////////////////////////////////////////////////////////////////
   float BaseGroundClamper::GetHighResGroundClampingRange2() const
   {
      return mHighResClampRange2;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseGroundClamper::SetLowResGroundClampingRange(float range)
   {
      mLowResClampRange = range;
      mLowResClampRange2 = range * range;
   }

   /////////////////////////////////////////////////////////////////////////////
   float BaseGroundClamper::GetLowResGroundClampingRange() const
   {
      return mLowResClampRange;
   }

   /////////////////////////////////////////////////////////////////////////////
   float BaseGroundClamper::GetLowResGroundClampingRange2() const
   {
      return mLowResClampRange2;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseGroundClamper::SetIntermittentGroundClampingTimeDelta(float newDelta) 
   {
      mIntermittentGroundClampingTimeDelta = newDelta;
   }

   /////////////////////////////////////////////////////////////////////////////
   float BaseGroundClamper::GetIntermittentGroundClampingTimeDelta() const 
   {
      return mIntermittentGroundClampingTimeDelta;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseGroundClamper::SetIntermittentGroundClampingSmoothingTime(float newTime) 
   {
      mIntermittentGroundClampingSmoothingTime = newTime;
   }

   /////////////////////////////////////////////////////////////////////////////
   float BaseGroundClamper::GetIntermittentGroundClampingSmoothingTime() const 
   {
      return mIntermittentGroundClampingSmoothingTime;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseGroundClamper::UpdateEyePoint()
   {
      //Setup the iSector to use the player position only once so that get transform is not called
      //for every single actor to be clamped.
      if(GetEyePointActor() != NULL)
      {
         dtCore::Transform xform;
         GetEyePointActor()->GetTransform(xform, dtCore::Transformable::ABS_CS);
         xform.GetTranslation(mCurrentEyePointABSPos);

         if (mLogger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            osg::Vec3 debugPos;
            xform.GetTranslation(debugPos);

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

   /////////////////////////////////////////////////////////////////////////////
   const osg::Vec3& BaseGroundClamper::GetLastEyePoint() const
   {
      return mCurrentEyePointABSPos;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::Transformable* BaseGroundClamper::GetEyePointActor()
   {
      return mEyePointActor.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::Transformable* BaseGroundClamper::GetEyePointActor() const
   {
      return mEyePointActor.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtUtil::Log& BaseGroundClamper::GetLogger()
   {
      return mLogger;
   }

}
