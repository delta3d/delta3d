/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, LLC
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

#ifndef DELTA_DYNAMIC_PARTICLE_SYSTEM_ACTOR
#define DELTA_DYNAMIC_PARTICLE_SYSTEM_ACTOR

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtCore/plugin_export.h>
#include <dtActors/particlesystemactorproxy.h>
#include <dtCore/particlesystem.h>
#include <dtCore/particleutil.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtCore
{
   class ParticleLayer;
   class ParticleSystem;
}

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   //////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT DynamicParticleSystem : public dtCore::ParticleSystem
   {
      public:
         typedef dtCore::ParticleSystem BaseClass;

         DynamicParticleSystem();

         dtCore::ParticleLayerInterpolator* GetInterpolator( const std::string& layerName ); 
         const dtCore::ParticleLayerInterpolator* GetInterpolator( const std::string& layerName ) const;

         dtCore::ParticleLayerInterpolator* GetInterpolator( unsigned index = 0 ); 
         const dtCore::ParticleLayerInterpolator* GetInterpolator( unsigned index = 0 ) const;

         typedef std::vector<dtCore::ParticleLayerInterpolator*> InterpolatorArray;
         typedef std::vector<const dtCore::ParticleLayerInterpolator*> InterpolatorArray_Const;
         void GetAllInterpolators(InterpolatorArray& outArray);
         void GetAllInterpolators(InterpolatorArray_Const& outArray) const;

         dtCore::ParticleLayer* GetParticleLayer( const std::string& layerName );
         const dtCore::ParticleLayer* GetParticleLayer( const std::string& layerName ) const;
         
         /**
          * Sets all properties on all contained particle layers to the specified
          * interpolation, IMMEDIATELY.
          * NOTE: This will completely override all existing timed property interpolators
          * on each layer.
          */
         void SetInterpolation( float interpolationRatio );
         /**
          * NOTE: This method only returns interpolations from the first particle layer
          * in the particle system.
          * This method is NOT accurate when modifying the contained particle layers
          * independantly from each other.
          */
         float GetInterpolation() const;

         /**
          * Interpolate a single property type for all layers.
          * @param prop Property type to be interpolated
          * @param time Seconds over which to interpolate
          * @param interpolation Target interpolation to interpolate to over the specified time
          * @return Number of layers updated
          */
         int InterpolateAllLayers(dtCore::ParticlePropertyEnum prop, float time, float interpolation );

         /**
          * Interpolate a single property type for all layers.
          * @param prop Property type to be interpolated
          * @param interpolation Target interpolation
          * @return Number of layers updated
          */
         int InterpolateAllLayers(dtCore::ParticlePropertyEnum prop, float interpolation );

         /**
          * Interpolate all properties of all layers to the specified interpolation.
          * @param time Seconds over which to interpolate
          * @param interpolation Target interpolation to interpolate to over the specified time
          * @return Number of layers updated
          */
         int InterpolateAllLayers( float time, float interpolation );

         /**
          * Interpolate all properties of all layers to the specified interpolation.
          * @param interpolation Target interpolation
          * @return Number of layers updated
          */
         int InterpolateAllLayers(float interpolation);

         void Reset();

         void Update( float simTimeDelta );

         virtual void Init();

      protected:
         virtual ~DynamicParticleSystem();

         int ResetParticleLayers();
         void ClearParticleLayers();

      private:
         typedef std::map<dtUtil::RefString, dtCore::RefPtr<dtCore::ParticleLayerInterpolator> >
            ParticleLayerInterpMap;

         ParticleLayerInterpMap mLayerInterps;
   };



   //////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   //////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT DynamicParticleSystemActor : public dtActors::ParticleSystemActor
   {
      public:
         typedef dtActors::ParticleSystemActor BaseClass;

         static const dtUtil::RefString CLASS_NAME;
         static const dtUtil::RefString PROPERTY_INITIAL_INTERPOLATION;
         
         DynamicParticleSystemActor();

         void SetEnabled(bool enabled);
         bool IsEnabled() const;

         virtual void CreateDrawable();

         virtual void BuildPropertyMap();

         virtual void OnEnteredWorld();
         virtual void OnRemovedFromWorld();

         virtual void TickLocal(const dtGame::Message& tickMessage);

         /**
          * Convenience method for accessing the actor already cast to its type.
          */
         DynamicParticleSystem& GetDynamicParticleSystem();
         const DynamicParticleSystem& GetDynamicParticleSystem() const;

         // Used in STAGE
         virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

         // STAGE - Draw Billboard or not?
         virtual const dtCore::ActorProxy::RenderMode& GetRenderMode();

      protected:
         virtual ~DynamicParticleSystemActor();

         dtCore::RefPtr<DynamicParticleSystem> mParticleSystem;
   };
}

#endif
