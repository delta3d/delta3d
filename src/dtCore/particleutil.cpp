/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
 */

/////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
/////////////////////////////////////////////////////////////////////////////
#include <dtCore/particleutil.h>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osgParticle/range>
#include <osgParticle/BoxPlacer>
#include <osgParticle/ModularEmitter>
#include <osgParticle/MultiSegmentPlacer>
#include <osgParticle/Particle>
#include <osgParticle/RadialShooter>
#include <osgParticle/VariableRateCounter>
#include <dtUtil/mathdefines.h>



namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////
   // PARTICLE UTILS CODE
   /////////////////////////////////////////////////////////////////////////////
   ParticleUtils::ParticleUtils()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   ParticleUtils::~ParticleUtils()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::ParticleLayer* ParticleUtils::GetLayer( dtCore::ParticleSystem& ps, unsigned index ) const
   {
      PSLayerList& layers = ps.GetAllLayers();
      if( layers.size() <= size_t(index) )
      {
         return NULL;
      }

      PSLayerList::iterator curLayer = layers.begin();
      size_t limit = layers.size();
      for( size_t i = 0; i < index && i < limit; ++i )
      {
         ++curLayer;
      }

      return &(*curLayer);
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::ParticleLayer* ParticleUtils::GetLayer( const dtCore::ParticleSystem& ps, unsigned index ) const
   {
      const PSLayerList& layers = ps.GetAllLayers();
      if( layers.size() <= size_t(index) )
      {
         return NULL;
      }

      PSLayerList::const_iterator curLayer = layers.begin();
      size_t limit = layers.size();
      for( size_t i = 0; i < index && i < limit; ++i )
      {
         ++curLayer;
      }

      return &(*curLayer);
   }

   /////////////////////////////////////////////////////////////////////////////
   osgParticle::ModularEmitter* ParticleUtils::GetEmitter( dtCore::ParticleSystem& ps, const std::string& layerName ) const
   {
      dtCore::ParticleLayer* layer = ps.GetSingleLayer( layerName );

      return layer != NULL ? &layer->GetModularEmitter() : NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   const osgParticle::ModularEmitter* ParticleUtils::GetEmitter( const dtCore::ParticleSystem& ps, const std::string& layerName ) const
   {
      const dtCore::ParticleLayer* layer = ps.GetSingleLayer( layerName );

      return layer != NULL ? &layer->GetModularEmitter() : NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleUtils::SetEmitterRateRange( dtCore::ParticleLayer& layer, const osg::Vec2& rateMinMax )
   {
      // NOTE: Random Rate Counter (sub-class of Variable Rate Counter)
      //       is assigned to all particle systems within the Delta3D particle editor.
      osgParticle::VariableRateCounter* counter
         = static_cast<osgParticle::VariableRateCounter*>(layer.GetModularEmitter().getCounter());

      counter->setRateRange( rateMinMax.x(), rateMinMax.y() );
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec2 ParticleUtils::GetEmitterRateRange( const dtCore::ParticleLayer& layer ) const
   {
      // NOTE: Random Rate Counter (sub-class of Variable Rate Counter)
      //       is assigned to all particle systems within the Delta3D particle editor.
      const osgParticle::VariableRateCounter* counter
         = static_cast<const osgParticle::VariableRateCounter*>(layer.GetModularEmitter().getCounter());

      const osgParticle::rangef& range = counter->getRateRange();
      return osg::Vec2( range.minimum, range.maximum );
      return osg::Vec2();
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleUtils::SetEmitterSpeedRange( dtCore::ParticleLayer& layer, const osg::Vec2& speedMinMax )
   {
      // NOTE: Assuming Radial Shooter as this is currently the only shooter type in OSG.
      osgParticle::RadialShooter* shooter
         = static_cast<osgParticle::RadialShooter*>(layer.GetModularEmitter().getShooter());

      shooter->setInitialSpeedRange( speedMinMax.x(), speedMinMax.y() );
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec2 ParticleUtils::GetEmitterSpeedRange( const dtCore::ParticleLayer& layer ) const
   {
      // NOTE: Assuming Radial Shooter as this is currently the only shooter type in OSG.
      const osgParticle::RadialShooter* shooter
         = static_cast<const osgParticle::RadialShooter*>(layer.GetModularEmitter().getShooter());

      const osgParticle::rangef& range = shooter->getInitialSpeedRange();
      return osg::Vec2( range.minimum, range.maximum );
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleUtils::SetEmitterArea( dtCore::ParticleLayer& layer, const osg::Vec2& area )
   {
      osgParticle::MultiSegmentPlacer* segPlacer
         = dynamic_cast<osgParticle::MultiSegmentPlacer*>(layer.GetModularEmitter().getPlacer());

      if( segPlacer != NULL && segPlacer->numVertices() == 5 )
      {
         osg::Vec2 halfArea( area * 0.5f );
         segPlacer->setVertex( 0, -halfArea.x(),  halfArea.y(), 0.0f );
         segPlacer->setVertex( 1, -halfArea.x(), -halfArea.y(), 0.0f );
         segPlacer->setVertex( 2,  halfArea.x(), -halfArea.y(), 0.0f );
         segPlacer->setVertex( 3,  halfArea.x(),  halfArea.y(), 0.0f );
         segPlacer->setVertex( 4, segPlacer->getVertex( 0 ) );
      }
      else
      {
         osgParticle::BoxPlacer* boxPlacer
            = dynamic_cast<osgParticle::BoxPlacer*>(layer.GetModularEmitter().getPlacer());

         if( boxPlacer != NULL )
         {
            boxPlacer->setXRange( area.x() * -0.5f, area.x() * 0.5f );
            boxPlacer->setYRange( area.y() * -0.5f, area.y() * 0.5f );
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleUtils::SetParticleSizeRange( osgParticle::Particle& particle, const osg::Vec2& sizeMinMax )
   {
      particle.setSizeRange( osgParticle::rangef( sizeMinMax.x(), sizeMinMax.y() ) );
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec2 ParticleUtils::GetParticleSizeRange( const osgParticle::Particle& particle ) const
   {
      const osgParticle::rangef& sizeRange = particle.getSizeRange();

      return osg::Vec2( sizeRange.minimum, sizeRange.maximum );
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleUtils::SetParticleColorRange( osgParticle::Particle& particle,
      const osg::Vec3& colorStart, const osg::Vec3& colorEnd )
   {
      const osgParticle::rangef& alphaRange = particle.getAlphaRange();
      particle.setColorRange
         (
         osgParticle::rangev4
         ( 
         osg::Vec4( colorStart.x(), colorStart.y(), colorStart.z(), alphaRange.minimum ),
         osg::Vec4( colorEnd.x(), colorEnd.y(), colorEnd.z(), alphaRange.maximum )
         )
         );
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleUtils::SetParticleColorRange( osgParticle::Particle& particle,
      const osg::Vec4& colorStart, const osg::Vec4& colorEnd )
   {
      particle.setColorRange( osgParticle::rangev4( colorStart, colorEnd ) );
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleUtils::GetParticleColorRange( const osgParticle::Particle& particle,
      osg::Vec4& outColorStart, osg::Vec4& outColorEnd ) const
   {
      const osgParticle::rangev4& colorRange = particle.getColorRange();
      outColorStart = colorRange.minimum;
      outColorEnd = colorRange.maximum;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleUtils::SetDefaultParticle( dtCore::ParticleLayer& layer, const osgParticle::Particle& particle )
   {
      layer.GetParticleSystem().setDefaultParticleTemplate( particle );
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleUtils::GetDefaultParticle( dtCore::ParticleLayer& layer, osgParticle::Particle& outParticle )
   {
      outParticle = layer.GetParticleSystem().getDefaultParticleTemplate();
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleUtils::GetDefaultParticle( const dtCore::ParticleLayer& layer, osgParticle::Particle& outParticle ) const
   {
      outParticle = layer.GetParticleSystem().getDefaultParticleTemplate();
   }



   /////////////////////////////////////////////////////////////////////////////
   // PARTICLE SYSTEM SETTINGS
   /////////////////////////////////////////////////////////////////////////////
   ParticleSystemSettings::ParticleSystemSettings()
      : mLifeTime(1.0f)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   ParticleSystemSettings::~ParticleSystemSettings()
   {
   }



   /////////////////////////////////////////////////////////////////////////////
   // INTERPOLATOR PARTICLE UTILS CODE
   /////////////////////////////////////////////////////////////////////////////
   InterpolatorParticleUtils::InterpolatorParticleUtils()
      : mStart(new ParticleSystemSettings)
      , mEnd(new ParticleSystemSettings)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   InterpolatorParticleUtils::~InterpolatorParticleUtils()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void InterpolatorParticleUtils::SetDefaultRanges( const dtCore::ParticleLayer& layer )
   {
      // Get the end range settings.
      GetDefaultSettingsFromLayer( layer, *mEnd );

      // Set the start settings to the same as the end range settings,
      // since individual properties can be interpolated without affecting
      // other properties.
      *mStart = *mEnd;
   }

   /////////////////////////////////////////////////////////////////////////////
   void InterpolatorParticleUtils::GetDefaultSettingsFromLayer( const dtCore::ParticleLayer& layer, ParticleSystemSettings& outSettings )
   {
      // Get the default particle and set values that should be copied
      // back into the actual default particle held in the particle system.
      osgParticle::Particle defaultParticle;
      GetDefaultParticle( layer, defaultParticle );

      // Color Ranges
      GetParticleColorRange( defaultParticle, outSettings.mRangeColorMin, outSettings.mRangeColorMax );

      // Life Ranges
      outSettings.mLifeTime = defaultParticle.getLifeTime();

      // Size Ranges
      outSettings.mRangeSize = GetParticleSizeRange( defaultParticle );

      // Speed Ranges
      outSettings.mRangeSpeed = GetEmitterSpeedRange( layer );

      // Rate Ranges
      outSettings.mRangeRate = GetEmitterRateRange( layer );
   }

   /////////////////////////////////////////////////////////////////////////////
   ParticleSystemSettings& InterpolatorParticleUtils::GetStartSettings()
   {
      return *mStart;
   }

   /////////////////////////////////////////////////////////////////////////////
   const ParticleSystemSettings& InterpolatorParticleUtils::GetStartSettings() const
   {
      return *mStart;
   }

   /////////////////////////////////////////////////////////////////////////////
   ParticleSystemSettings& InterpolatorParticleUtils::GetEndSettings()
   {
      return *mEnd;
   }

   /////////////////////////////////////////////////////////////////////////////
   const ParticleSystemSettings& InterpolatorParticleUtils::GetEndSettings() const
   {
      return *mEnd;
   }

   /////////////////////////////////////////////////////////////////////////////
   void InterpolatorParticleUtils::GetInterpolatedColorRange( float interpolateRatio, osg::Vec4& outRangeMin, osg::Vec4& outRangeMax )
   {
      GetInterpolatedColorStart( interpolateRatio, outRangeMin );
      GetInterpolatedColorEnd( interpolateRatio, outRangeMax );
   }

   /////////////////////////////////////////////////////////////////////////////
   void InterpolatorParticleUtils::GetInterpolatedColorStart( float interpolateRatio, osg::Vec4& outColor )
   {
      outColor = mStart->mRangeColorMin + ((mEnd->mRangeColorMin - mStart->mRangeColorMin) * interpolateRatio);
   }

   /////////////////////////////////////////////////////////////////////////////
   void InterpolatorParticleUtils::GetInterpolatedColorEnd( float interpolateRatio, osg::Vec4& outColor )
   {
      outColor = mStart->mRangeColorMax + ((mEnd->mRangeColorMax - mStart->mRangeColorMax) * interpolateRatio);
   }

   /////////////////////////////////////////////////////////////////////////////
   float InterpolatorParticleUtils::GetInterpolatedLifeRange( float interpolateRatio )
   {
      return mStart->mLifeTime + ((mEnd->mLifeTime - mStart->mLifeTime) * interpolateRatio);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec2 InterpolatorParticleUtils::GetInterpolatedSizeRange( float interpolateRatio )
   {
      return mStart->mRangeSize + ((mEnd->mRangeSize - mStart->mRangeSize) * interpolateRatio);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec2 InterpolatorParticleUtils::GetInterpolatedSpeedRange( float interpolateRatio )
   {
      return mStart->mRangeSpeed + ((mEnd->mRangeSpeed - mStart->mRangeSpeed) * interpolateRatio);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec2 InterpolatorParticleUtils::GetInterpolatedRateRange( float interpolateRatio )
   {
      return mStart->mRangeRate + ((mEnd->mRangeRate - mStart->mRangeRate) * interpolateRatio);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool InterpolatorParticleUtils::Interpolate( dtCore::ParticleLayer& layer, ParticlePropertyEnum prop, float interpolationRatio )
   {
      bool success = true;

      switch( prop )
      {
         case PS_PARTICLE_COLOR_RANGE:
            {
               osgParticle::Particle particle;
               osg::Vec4 colorMin, colorMax;
               GetDefaultParticle( layer, particle );
               GetInterpolatedColorRange( interpolationRatio, colorMin, colorMax );
               SetParticleColorRange( particle, colorMin, colorMax );
               SetDefaultParticle( layer, particle );
            }
            break;

         case PS_PARTICLE_LIFE:
            {
               osgParticle::Particle particle;
               GetDefaultParticle( layer, particle );
               particle.setLifeTime( GetInterpolatedLifeRange( interpolationRatio ) );
               SetDefaultParticle( layer, particle );
            }
            break;

         case PS_PARTICLE_SIZE:
            {
               osgParticle::Particle particle;
               GetDefaultParticle( layer, particle );
               SetParticleSizeRange( particle, GetInterpolatedSizeRange( interpolationRatio ) );
               SetDefaultParticle( layer, particle );
            }
            break;

         case PS_EMIT_RATE:
            SetEmitterRateRange( layer, GetInterpolatedRateRange( interpolationRatio ) );
            break;

         case PS_EMIT_SPEED:
            SetEmitterSpeedRange( layer, GetInterpolatedSpeedRange( interpolationRatio ) );
            break;

         default:
            success = false;
            break;
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   int InterpolatorParticleUtils::Interpolate( dtCore::ParticleSystem& particles,
      ParticlePropertyEnum prop, float interpolationRatio )
   {
      int successes = 0;

      // Got through all layers and interpolate them the same.
      PSLayerList& layers = particles.GetAllLayers();
      PSLayerList::iterator curLayer = layers.begin();
      PSLayerList::iterator endLayerList = layers.end();
      for( ; curLayer != endLayerList; ++curLayer )
      {
         successes += Interpolate( *curLayer, prop, interpolationRatio ) ? 1 : 0;
      }

      return successes;
   }

   /////////////////////////////////////////////////////////////////////////////
   void InterpolatorParticleUtils::InterpolateAllProperties( dtCore::ParticleSystem& particles, float interpolationRatio )
   {
      // Got through all layers and interpolate them the same.
      PSLayerList& layers = particles.GetAllLayers();
      PSLayerList::iterator curLayer = layers.begin();
      PSLayerList::iterator endLayerList = layers.end();
      for( ; curLayer != endLayerList; ++curLayer )
      {
         InterpolateAllProperties( *curLayer, interpolationRatio );
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void InterpolatorParticleUtils::InterpolateAllProperties( dtCore::ParticleLayer& layer, float interpolationRatio )
   {
      osgParticle::Particle defaultParticle;
      GetDefaultParticle( layer, defaultParticle );

         // Size Range
         SetParticleSizeRange( defaultParticle, GetInterpolatedSizeRange( interpolationRatio ) );

         // Life Range
         defaultParticle.setLifeTime( GetInterpolatedLifeRange( interpolationRatio ) );

         // Color Range
         osg::Vec4 colorMin, colorMax;
         GetInterpolatedColorRange( interpolationRatio, colorMin, colorMax );
         SetParticleColorRange( defaultParticle, colorMin, colorMax );

      SetDefaultParticle( layer, defaultParticle );

      // Speed Range
      SetEmitterSpeedRange( layer, GetInterpolatedSpeedRange( interpolationRatio ) );

      // Rate Range
      SetEmitterRateRange( layer, GetInterpolatedRateRange( interpolationRatio ) );
   }



   /////////////////////////////////////////////////////////////////////////////
   // PARTICLE LAYER INTERPOLATOR CODE
   /////////////////////////////////////////////////////////////////////////////
   ParticleLayerInterpolator::ParticleLayerInterpolator( dtCore::ParticleLayer& layer )
      : mLayer(NULL)
      , mDefaultSettings(new ParticleSystemSettings)
   {
      SetLayer( layer );
   }

   /////////////////////////////////////////////////////////////////////////////
   ParticleLayerInterpolator::~ParticleLayerInterpolator()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::SetLayer( dtCore::ParticleLayer& layer )
   {
      mLayer = &layer;
      GetDefaultSettingsFromLayer( layer, *mDefaultSettings );
      GetDefaultParticle( layer, mDefaultParticle );

      SetDefaultRanges( layer );
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::ParticleLayer& ParticleLayerInterpolator::GetLayer()
   {
      return *mLayer;
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::ParticleLayer& ParticleLayerInterpolator::GetLayer() const
   {
      return *mLayer;
   }

   /////////////////////////////////////////////////////////////////////////////
   osgParticle::Particle& ParticleLayerInterpolator::GetLayerDefaultParticle()
   {
      return mDefaultParticle;
   }
   
   /////////////////////////////////////////////////////////////////////////////
   const osgParticle::Particle& ParticleLayerInterpolator::GetLayerDefaultParticle() const
   {
      return mDefaultParticle;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::SetEmitterRateRange(
      const osg::Vec2& rateRangeMin, const osg::Vec2& rateRangeMax )
   {
      GetStartSettings().mRangeRate = rateRangeMin;
      GetEndSettings().mRangeRate = rateRangeMax;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::GetEmitterRateRange(
      osg::Vec2& outRateRangeMin, osg::Vec2& outRateRangeMax ) const
   {
      outRateRangeMin = GetStartSettings().mRangeRate;
      outRateRangeMax = GetEndSettings().mRangeRate;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::SetEmitterSpeedRange(
      const osg::Vec2& speedRangeMin, const osg::Vec2& speedRangeMax )
   {
      GetStartSettings().mRangeSpeed = speedRangeMin;
      GetEndSettings().mRangeSpeed = speedRangeMax;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::GetEmitterSpeedRange(
      osg::Vec2& outSpeedRangeMin, osg::Vec2& outSpeedRangeMax ) const
   {
      outSpeedRangeMin = GetStartSettings().mRangeSpeed;
      outSpeedRangeMax = GetEndSettings().mRangeSpeed;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::SetParticleSizeRange(
      const osg::Vec2& sizeRangeMin, const osg::Vec2& sizeRangeMax )
   {
      GetStartSettings().mRangeSize = sizeRangeMin;
      GetEndSettings().mRangeSize = sizeRangeMax;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::GetParticleSizeRange(
      osg::Vec2& outSizeRangeMin, osg::Vec2& outSizeRangeMax ) const
   {
      outSizeRangeMin = GetStartSettings().mRangeSize;
      outSizeRangeMax = GetEndSettings().mRangeSize;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::SetParticleColorRangeStart(
      const osg::Vec4& colorStart, const osg::Vec4& colorEnd )
   {
      GetStartSettings().mRangeColorMin = colorStart;
      GetEndSettings().mRangeColorMin = colorEnd;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::SetParticleColorRangeEnd(
      const osg::Vec4& colorStart, const osg::Vec4& colorEnd )
   {
      GetStartSettings().mRangeColorMax = colorStart;
      GetEndSettings().mRangeColorMax = colorEnd;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::GetParticleColorRangeStart(
      osg::Vec4& outColorStart, osg::Vec4& outColorEnd ) const
   {
      outColorStart = GetStartSettings().mRangeColorMin;
      outColorEnd = GetEndSettings().mRangeColorMin;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::GetParticleColorRangeEnd(
      osg::Vec4& outColorStart, osg::Vec4& outColorEnd ) const
   {
      outColorStart = GetStartSettings().mRangeColorMax;
      outColorEnd = GetEndSettings().mRangeColorMax;
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::Update( float simTimeDelta )
   {
      PropertyInterpolateMap::iterator curInterp = mInterpMap.begin();
      PropertyInterpolateMap::iterator endInterpMap = mInterpMap.end();

      for( ; curInterp != endInterpMap; ++curInterp )
      {
         InterpParams& params = *(curInterp->second);

         // Increment timer.
         if( params.mTimeCurrent < params.mTime )
         {
            params.mTimeCurrent += simTimeDelta;

            if( params.mTime != 0.0f )
            {
               // Figure the current interpolation.
               float diff = params.mInterpEnd - params.mInterpStart;
               diff *= params.mTimeCurrent / params.mTime;

               params.mInterpCurrent = params.mInterpStart + diff;

               float minValue = params.mInterpStart;
               float maxValue = params.mInterpEnd;
               if (maxValue < minValue)
               {
                  float temp = minValue;
                  minValue = maxValue;
                  maxValue = temp;
               }

               dtUtil::Clamp( params.mInterpCurrent, minValue, maxValue );
               dtUtil::Clamp( params.mInterpCurrent, 0.0f, 1.0f );

               // Interpolate the specified property.
               if( curInterp->first == PS_ALL_PROPERTIES )
               {
                  // This should be the only loop.
                  // Interpolate all the properties at one time.
                  BaseClass::InterpolateAllProperties( *mLayer, params.mInterpCurrent );
                  break;
               }
               else // Individual property
               {
                  BaseClass::Interpolate( *mLayer, curInterp->first, params.mInterpCurrent );
               }
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::Reset()
   {
      // Make sure the end settings are set bake to the default settings.
      GetEndSettings() = *mDefaultSettings;

      // Change the particle system to have the end settings.
      BaseClass::InterpolateAllProperties( *mLayer, 1.0f );

      // Restore the default particle properties.
      SetDefaultParticle( *mLayer, mDefaultParticle );

      // Reset all the interpolation timers.
      PropertyInterpolateMap::iterator curInterp = mInterpMap.begin();
      PropertyInterpolateMap::iterator endInterpMap = mInterpMap.end();
      for( ; curInterp != endInterpMap; ++curInterp )
      {
         curInterp->second->Reset();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::Interpolate( ParticlePropertyEnum prop,
      float targetInterpolationRatio )
   {
      dtUtil::Clamp( targetInterpolationRatio, 0.0f, 1.0f );

      // Interpolate the specified property.
      if( prop == PS_ALL_PROPERTIES )
      {
         // This should be the only loop.
         // Interpolate all the properties at one time.
         BaseClass::InterpolateAllProperties( *mLayer, targetInterpolationRatio );
      }
      else // Individual property
      {
         BaseClass::Interpolate( *mLayer, prop, targetInterpolationRatio );
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::InterpolateOverTime( ParticlePropertyEnum prop,
      float time, float targetInterpolationRatio )
   {
      PropertyInterpolateMap::iterator foundIter = mInterpMap.find( prop );

      bool wasCreated = false;
      dtCore::RefPtr<InterpParams> params;
      if( foundIter != mInterpMap.end() )
      {
         params = foundIter->second.get();
      }
      else
      {
         params = new InterpParams;
         params->mInterpCurrent = 0.0f; // Start at the max interpolation
         wasCreated = true;
      }

      // Set the other interpolation variables.
      params->mInterpStart = params->mInterpCurrent;
      params->mInterpEnd = targetInterpolationRatio;
      params->mTime = time;
      params->mTimeCurrent = 0.0f;

      // If this is for all properties, then remove all other ones.
      if(wasCreated && prop == PS_ALL_PROPERTIES && ! mInterpMap.empty())
      {
         mInterpMap.clear();
      }

      // Add the new entry if this was created.
      if( wasCreated )
      {
         mInterpMap.insert( std::make_pair( prop, params.get() ) );
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::InterpolateAllProperties(float interpolationRatio)
   {
      BaseClass::InterpolateAllProperties(*mLayer, interpolationRatio);
   }

   /////////////////////////////////////////////////////////////////////////////
   float ParticleLayerInterpolator::GetParticlePropertyInterpolation( ParticlePropertyEnum prop ) const
   {
      PropertyInterpolateMap::const_iterator foundIter = mInterpMap.find( prop );
      return foundIter != mInterpMap.end() ? foundIter->second->mInterpCurrent : 0.0f;
   }



   /////////////////////////////////////////////////////////////////////////////
   // PARTICLE LAYER INTERPOLATOR : INTERP PARAMS
   /////////////////////////////////////////////////////////////////////////////
   ParticleLayerInterpolator::InterpParams::InterpParams()
   {
      Reset();
   }

   /////////////////////////////////////////////////////////////////////////////
   ParticleLayerInterpolator::InterpParams::~InterpParams()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void ParticleLayerInterpolator::InterpParams::Reset()
   {
      mTime = 0.0f;
      mTimeCurrent = 0.0f;
      mInterpEnd = 1.0f;
      mInterpStart = 0.0f;
      mInterpCurrent = mInterpEnd;
   }

}
