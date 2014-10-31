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

#ifndef DELTA_PARTICLE_UTIL
#define DELTA_PARTICLE_UTIL

/////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
/////////////////////////////////////////////////////////////////////////////
#include <dtCore/export.h>
#include <osg/Array>
#include <dtCore/particlesystem.h>



/////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
/////////////////////////////////////////////////////////////////////////////
namespace osg
{
   class Geode;
   class MatrixTransform;
};

namespace osgParticle
{
   class ModularEmitter;
   class Particle;
   class ParticleSystem;
}



namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   enum ParticlePropertyEnum
   {
      PS_EMIT_RATE,
      PS_EMIT_SPEED,
      PS_PARTICLE_LIFE,
      PS_PARTICLE_COLOR_RANGE,
      PS_PARTICLE_SIZE,
      PS_ALL_PROPERTIES
   };



   /////////////////////////////////////////////////////////////////////////////
   // TYPE DECLARATIONS
   /////////////////////////////////////////////////////////////////////////////
   typedef dtCore::ParticleSystem::LayerList PSLayerList;
   typedef std::set<ParticlePropertyEnum> ParticlePropertySet;



   /////////////////////////////////////////////////////////////////////////////
   // PARTICLE UTILS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT ParticleUtils : public osg::Referenced
   {
      public:
         ParticleUtils();

         /**
          * Access a single particle layer by index from the specified Delta3D particle system.
          * @param ps Delta3D particle system from which a particle layer is being accessed.
          * @param index Layer index.
          * @return Particle layer; NULL if not found.
          */
         dtCore::ParticleLayer* GetLayer( dtCore::ParticleSystem& ps, unsigned index = 0 ) const;
         const dtCore::ParticleLayer* GetLayer( const dtCore::ParticleSystem& ps,
            unsigned index = 0 ) const;

         /**
          * Access the emitter from a specified Delta3D particle system layer.
          * @param ps Delta3D particle system that contains layers.
          * @param layerName Name of the layer to be accessed.
          * @return Emitter of a particle layer; NULL if not found.
          */
         osgParticle::ModularEmitter* GetEmitter(
            dtCore::ParticleSystem& ps, const std::string& layerName ) const;
         const osgParticle::ModularEmitter* GetEmitter(
            const dtCore::ParticleSystem& ps, const std::string& layerName ) const;

         void SetEmitterRateRange( dtCore::ParticleLayer& layer, const osg::Vec2& rateMinMax );
         osg::Vec2 GetEmitterRateRange( const dtCore::ParticleLayer& layer ) const;

         void SetEmitterSpeedRange( dtCore::ParticleLayer& layer, const osg::Vec2& speedMinMax );
         osg::Vec2 GetEmitterSpeedRange( const dtCore::ParticleLayer& layer ) const;

         /**
          * Set the area the particle system covers.
          * NOTE: This method assumes the emitter's placer to be an OSG
          * Multi Segment Placer with 4 segments (5 vertices).
          *
          * @param layer Delta3D Particle System Layer that contains references to
          *        the imporatant components of a single OSG particle system.
          * @param area Width-Height value pair
          */
         void SetEmitterArea( dtCore::ParticleLayer& layer, const osg::Vec2& area );
         //osg::Vec2 GetEmitterArea( const dtCore::ParticleLayer& layer ) const;

         void SetParticleSizeRange( osgParticle::Particle& particle, const osg::Vec2& sizeMinMax );
         osg::Vec2 GetParticleSizeRange( const osgParticle::Particle& particle ) const;

         void SetParticleColorRange( osgParticle::Particle& particle, const osg::Vec3& colorStart, const osg::Vec3& colorEnd );
         void SetParticleColorRange( osgParticle::Particle& particle, const osg::Vec4& colorStart, const osg::Vec4& colorEnd );
         void GetParticleColorRange( const osgParticle::Particle& particle, osg::Vec4& outColorStart, osg::Vec4& outColorEnd ) const;

         void SetDefaultParticle( dtCore::ParticleLayer& layer, const osgParticle::Particle& particle );
         void GetDefaultParticle( dtCore::ParticleLayer& layer, osgParticle::Particle& outParticle );
         void GetDefaultParticle( const dtCore::ParticleLayer& layer, osgParticle::Particle& outParticle ) const;

      protected:
         virtual ~ParticleUtils();
   };



   /////////////////////////////////////////////////////////////////////////////
   // PARTICLE SYSTEM SETTINGS
   /////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT ParticleSystemSettings : public osg::Referenced
   {
      public:
         ParticleSystemSettings();
      
         float mLifeTime;
         osg::Vec2 mRangeLife;
         osg::Vec2 mRangeRate;
         osg::Vec2 mRangeSize;
         osg::Vec2 mRangeSpeed;
         osg::Vec4 mRangeColorMin;
         osg::Vec4 mRangeColorMax;

      protected:
         virtual ~ParticleSystemSettings();
   };



   /////////////////////////////////////////////////////////////////////////////
   // INTERPOLATOR PARTICLE UTILS CODE (2-Dimensional Particle Interpolation)
   /////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT InterpolatorParticleUtils : public ParticleUtils
   {
      public:
         typedef ParticleUtils BaseClass;

         InterpolatorParticleUtils();

         void SetDefaultRanges( const dtCore::ParticleLayer& layer );

         void GetDefaultSettingsFromLayer( const dtCore::ParticleLayer& layer, ParticleSystemSettings& outSettings );

         ParticleSystemSettings& GetStartSettings();
         const ParticleSystemSettings& GetStartSettings() const;

         ParticleSystemSettings& GetEndSettings();
         const ParticleSystemSettings& GetEndSettings() const;

         void GetInterpolatedColorRange( float interpolateRatio, osg::Vec4& outRangeMin, osg::Vec4& outRangeMax );
         void GetInterpolatedColorStart( float interpolateRatio, osg::Vec4& outColor );
         void GetInterpolatedColorEnd( float interpolateRatio, osg::Vec4& outColor );

         float GetInterpolatedLifeRange( float interpolateRatio );
         osg::Vec2 GetInterpolatedSizeRange( float interpolateRatio );
         osg::Vec2 GetInterpolatedSpeedRange( float interpolateRatio );
         osg::Vec2 GetInterpolatedRateRange( float interpolateRatio );

         bool Interpolate( dtCore::ParticleLayer& layer, ParticlePropertyEnum prop, float interpolationRatio );
         int Interpolate( dtCore::ParticleSystem& particles, ParticlePropertyEnum prop, float interpolationRatio );

         void InterpolateAllProperties( dtCore::ParticleSystem& particles, float interpolationRatio );
         void InterpolateAllProperties( dtCore::ParticleLayer& layer, float interpolationRatio );

      protected:
         virtual ~InterpolatorParticleUtils();

      private:
         dtCore::RefPtr<ParticleSystemSettings> mStart;
         dtCore::RefPtr<ParticleSystemSettings> mEnd;
   };



   /////////////////////////////////////////////////////////////////////////////
   // PARTICLE LAYER INTERPOLATOR CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT ParticleLayerInterpolator : public InterpolatorParticleUtils
   {
      public:
         typedef InterpolatorParticleUtils BaseClass;

         ParticleLayerInterpolator( dtCore::ParticleLayer& layer );

         void SetLayer( dtCore::ParticleLayer& layer );
         dtCore::ParticleLayer& GetLayer();
         const dtCore::ParticleLayer& GetLayer() const;

         osgParticle::Particle& GetLayerDefaultParticle();
         const osgParticle::Particle& GetLayerDefaultParticle() const;

         void SetEmitterRateRange( const osg::Vec2& rateRangeMin, const osg::Vec2& rateRangeMax );
         void GetEmitterRateRange( osg::Vec2& outRateRangeMin, osg::Vec2& outRateRangeMax ) const;

         void SetEmitterSpeedRange( const osg::Vec2& speedRangeMin, const osg::Vec2& speedRangeMax );
         void GetEmitterSpeedRange( osg::Vec2& outSpeedRangeMin, osg::Vec2& outSpeedRangeMax ) const;

         void SetParticleSizeRange( const osg::Vec2& sizeRangeMin, const osg::Vec2& sizeRangeMax );
         void GetParticleSizeRange( osg::Vec2& outSizeRangeMin, osg::Vec2& outSizeRangeMax ) const;

         void SetParticleColorRangeStart( const osg::Vec4& colorStart, const osg::Vec4& colorEnd );
         void SetParticleColorRangeEnd( const osg::Vec4& colorStart, const osg::Vec4& colorEnd );

         void GetParticleColorRangeStart( osg::Vec4& outColorStart, osg::Vec4& outColorEnd ) const;
         void GetParticleColorRangeEnd( osg::Vec4& outColorStart, osg::Vec4& outColorEnd ) const;

         void Update( float simTimeDelta );

         /**
          * Restore the particle layer back to its original end settings.
          */
         void Reset();
         
         void Interpolate( ParticlePropertyEnum prop, float targetInterpolationRatio );

         void InterpolateOverTime( ParticlePropertyEnum prop, float time, float targetInterpolationRatio );
   
         void InterpolateAllProperties(float interpolationRatio);

         float GetParticlePropertyInterpolation( ParticlePropertyEnum prop ) const;

      protected:
         virtual ~ParticleLayerInterpolator();

      private:
         dtCore::ParticleLayer* mLayer;
         osgParticle::Particle mDefaultParticle; // Particle as set for the current layer.
         dtCore::RefPtr<ParticleSystemSettings> mDefaultSettings;

         class InterpParams : public osg::Referenced
         {
            public:
               InterpParams();

               void Reset();

               float mTime;
               float mTimeCurrent;
               float mInterpEnd;
               float mInterpStart;
               float mInterpCurrent;

            protected:
               virtual ~InterpParams();
         };

         // This is an interpolation tracker for each particle system property.
         typedef std::map<ParticlePropertyEnum, dtCore::RefPtr<InterpParams> > PropertyInterpolateMap;
         PropertyInterpolateMap mInterpMap;
   };

}

#endif
