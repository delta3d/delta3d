/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * Curtiss Murphy
 */
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/shaderparamoscillator.h>
#include <osg/Uniform>
#include <osg/StateSet>
#include <dtUtil/mathdefines.h>
#include <dtCore/system.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ShaderParamOscillator::OscillationType)
   const ShaderParamOscillator::OscillationType ShaderParamOscillator::OscillationType::UP("Up");
   const ShaderParamOscillator::OscillationType ShaderParamOscillator::OscillationType::DOWN("Down");
   const ShaderParamOscillator::OscillationType ShaderParamOscillator::OscillationType::UPANDDOWN("UpAndDown");
   const ShaderParamOscillator::OscillationType ShaderParamOscillator::OscillationType::DOWNANDUP("DownAndUp");

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamOscillator::ShaderParamOscillator(const std::string &name) : 
      ShaderParameter(name), 
      mValue(0.0),
      mOffset(0.0),
      mRangeMin(1.0),
      mRangeMax(1.0),
      mCurrentRange(1.0), // init not really needed since calced before used
      mCycleTimeMin(1.0),
      mCycleTimeMax(1.0),
      mCurrentCycleTime(1.0), // init not really needed since calced before used
      mUseRealTime(true),
      mOscillationType(&ShaderParamOscillator::OscillationType::UP),
      mCycleDirection(1.0)
   {
      SetShared(false); // we don't want to oscillating parameters by default
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamOscillator::~ShaderParamOscillator()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamOscillator::AttachToRenderState(osg::StateSet &stateSet)
   {
      osg::Uniform *floatUniform = NULL;

      if (IsShared())
      {
         floatUniform = GetUniformParam();
      }

      // Create a new one if unshared or if shared but not set yet
      if (floatUniform == NULL)
      {
         floatUniform = new osg::Uniform(osg::Uniform::FLOAT,GetName());
         SetUniformParam(*floatUniform);

         // force an update
         Update();

         // register for PreFrame
         AddSender(&dtCore::System::GetInstance());
      }

      stateSet.addUniform(floatUniform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamOscillator::Update()
   {
      // Update doesn't actually update the shader.  Instead, it resets the ranges and such 
      // so that PreFrame can do it's thing.

      if (!IsDirty() || GetUniformParam() == NULL)
         return;

      // RANGE
      // negative ranges are not allowed and are reset to 1.
      if (mRangeMin < 0.0) mRangeMin = 1.0;
      // max range must be >= min
      if (mRangeMax < mRangeMin) mRangeMax = mRangeMin;
      mCurrentRange = dtUtil::RandFloat(mRangeMin, mRangeMax);

      // CYCLE TIMER
      // negative ranges are not allowed and are reset to 1.
      if (mCycleTimeMin < 0.0) mCycleTimeMin = 1.0;
      // max range must be >= min
      if (mCycleTimeMax < mCycleTimeMin) mCycleTimeMax = mCycleTimeMin;
      mCurrentCycleTime = dtUtil::RandFloat(mCycleTimeMin, mCycleTimeMax);

      // Restart mValue based on oscillation type.
      if (mOscillationType == &OscillationType::UP || mOscillationType == &OscillationType::UPANDDOWN)
      {
         mValue = mOffset;
         mCycleDirection = 1.0;
      }
      else 
      {
         mValue = mCurrentRange + mOffset;
         mCycleDirection = -1.0;
      }

      GetUniformParam()->set(mValue);

      SetDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParameter *ShaderParamOscillator::Clone()
   {
      ShaderParamOscillator *newParam;

      // Shared params are shared at the pointer level, exactly the same. Non shared are new instances
      if (IsShared())
         newParam = this;
      else
      {
         newParam = new ShaderParamOscillator(GetName());

         newParam->SetDirty(true); // force a recompute of range and stuff.
         newParam->mOffset = mOffset;
         newParam->mRangeMin = mRangeMin;
         newParam->mRangeMax = mRangeMax;
         newParam->mCycleTimeMin = mCycleTimeMin;
         newParam->mCycleTimeMax = mCycleTimeMax;
         newParam->mUseRealTime = mUseRealTime;
         newParam->mOscillationType = mOscillationType;
         // Note - you don't copy the current values since they are gonna change on update
         // anyway.
      }

      return newParam;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamOscillator::OnMessage(MessageData *data)
   {
      if (data->message == "preframe")
      {
         // timeChange[0] is sim time, [1] is real
         double* timeChange = (double*)data->userData;
         if (mUseRealTime)
            DoShaderUpdate(timeChange[1]);
         else
            DoShaderUpdate(timeChange[0]);

      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamOscillator::DoShaderUpdate(float timeDelta)
   {
      float timePercent = timeDelta/mCurrentCycleTime; 
      float cycleDelta = timePercent*mCurrentRange;

      // UP
      if (mOscillationType == &OscillationType::UP)
      {
         mValue += cycleDelta;
         if (mValue > mOffset + mCurrentRange)
         {
            float remainder = mValue - (mOffset + mCurrentRange);
            mValue = mOffset + remainder;
         }
      }
      // DOWN
      else if (mOscillationType == &OscillationType::DOWN)
      {
         mValue -= cycleDelta;
         if (mValue < mOffset)
         {
            float remainder = mOffset - mValue;
            mValue = mOffset + mCurrentRange - remainder;
         }
      }
      // UPANDDOWN
      else //if (mOscillationType == &OscillationType::UPANDDOWN || mOscillationType == &OscillationType::DOWNANDUP)
      {
         mValue += mCycleDirection * cycleDelta;
         if (mValue < mOffset) // was going down and need to turn around
         {
            float remainder = mOffset - mValue;
            mCycleDirection = 1.0;
            mValue = mOffset + remainder;
         } 
         else if (mValue > mOffset + mCurrentRange) // was going up.  Turn around
         {
            float remainder = mValue - (mOffset + mCurrentRange);
            mCycleDirection = -1.0;
            mValue = mOffset + mCurrentRange - remainder;
         }
      }

      GetUniformParam()->set(mValue);
   }
}
