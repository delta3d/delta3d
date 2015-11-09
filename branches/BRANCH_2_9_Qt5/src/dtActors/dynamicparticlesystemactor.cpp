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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtActors/dynamicparticlesystemactor.h>
#include <dtCore/enginepropertytypes.h>
#include <dtCore/actorproxyicon.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtUtil/mathdefines.h>

using namespace dtCore;



namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   //////////////////////////////////////////////////////////////////////////
   DynamicParticleSystem::DynamicParticleSystem()
      : BaseClass()
   {}

   //////////////////////////////////////////////////////////////////////////
   DynamicParticleSystem::~DynamicParticleSystem()
   {
      ClearParticleLayers();
   }

   //////////////////////////////////////////////////////////////////////////
   ParticleLayerInterpolator* DynamicParticleSystem::GetInterpolator( const std::string& layerName )
   {
      ParticleLayerInterpMap::iterator foundIter = mLayerInterps.find( layerName );
      return foundIter == mLayerInterps.end() ? nullptr : foundIter->second.get();
   }

   //////////////////////////////////////////////////////////////////////////
   const ParticleLayerInterpolator* DynamicParticleSystem::GetInterpolator( const std::string& layerName ) const
   {
      ParticleLayerInterpMap::const_iterator foundIter = mLayerInterps.find( layerName );
      return foundIter == mLayerInterps.end() ? nullptr : foundIter->second.get();
   }

   //////////////////////////////////////////////////////////////////////////
   ParticleLayerInterpolator* DynamicParticleSystem::GetInterpolator( unsigned index )
   {
      ParticleLayerInterpolator* interpolator = nullptr;

      PSLayerList& layers = GetAllLayers();
      if( ! layers.empty() && unsigned(layers.size()) > index )
      {
         dtCore::ParticleLayer* layer = nullptr;
         PSLayerList::iterator curLayer = layers.begin();
         PSLayerList::iterator endLayerList = layers.end();
         for( unsigned curIndex = 0; curLayer != endLayerList; ++curLayer, ++curIndex )
         {
            if( index == curIndex )
            {
               layer = &(*curLayer);
               break;
            }
         }

         interpolator = GetInterpolator( layer->GetLayerName() );
      }

      return interpolator;
   }

   //////////////////////////////////////////////////////////////////////////
   const ParticleLayerInterpolator* DynamicParticleSystem::GetInterpolator( unsigned index ) const
   {
      const ParticleLayerInterpolator* interpolator = nullptr;

      const PSLayerList& layers = GetAllLayers();
      if( ! layers.empty() && unsigned(layers.size()) > index )
      {
         const dtCore::ParticleLayer* layer = nullptr;
         PSLayerList::const_iterator curLayer = layers.begin();
         PSLayerList::const_iterator endLayerList = layers.end();
         for( unsigned curIndex = 0; curLayer != endLayerList; ++curLayer, ++curIndex )
         {
            if( index == curIndex )
            {
               layer = &(*curLayer);
               break;
            }
         }

         interpolator = GetInterpolator( layer->GetLayerName() );
      }

      return interpolator;
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystem::GetAllInterpolators(InterpolatorArray& outArray)
   {
      ParticleLayerInterpMap::iterator curInterp = mLayerInterps.begin();
      ParticleLayerInterpMap::iterator endInterpArray = mLayerInterps.end();
      for( ; curInterp != endInterpArray; ++curInterp)
      {
         outArray.push_back(curInterp->second.get());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystem::GetAllInterpolators(InterpolatorArray_Const& outArray) const
   {
      ParticleLayerInterpMap::const_iterator curInterp = mLayerInterps.begin();
      ParticleLayerInterpMap::const_iterator endInterpArray = mLayerInterps.end();
      for( ; curInterp != endInterpArray; ++curInterp)
      {
         outArray.push_back(curInterp->second.get());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ParticleLayer* DynamicParticleSystem::GetParticleLayer( const std::string& layerName )
   {
      ParticleLayerInterpolator* interp = GetInterpolator( layerName );
      return interp == nullptr ? nullptr : &interp->GetLayer();
   }

   //////////////////////////////////////////////////////////////////////////
   const dtCore::ParticleLayer* DynamicParticleSystem::GetParticleLayer( const std::string& layerName ) const
   {
      const ParticleLayerInterpolator* interp = GetInterpolator( layerName );
      return interp == nullptr ? nullptr : &interp->GetLayer();
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystem::SetInterpolation( float interpolationRatio )
   {
      InterpolateAllLayers(interpolationRatio);
   }

   //////////////////////////////////////////////////////////////////////////
   float DynamicParticleSystem::GetInterpolation() const
   {
      // Get the primary interpolator since it may have the best chance
      // of representing the whole particle system's interpolation.
      const ParticleLayerInterpolator* interpolator = GetInterpolator();

      // Access the current interpolation registered with the
      // ALL PROPERTIES particle property.
      return interpolator != nullptr
         ? interpolator->GetParticlePropertyInterpolation( PS_ALL_PROPERTIES ) : 0.0f;
   }

   //////////////////////////////////////////////////////////////////////////
   int DynamicParticleSystem::InterpolateAllLayers( ParticlePropertyEnum prop,
      float time, float targetInterpolationRatio )
   {
      int layers = 0;

      ParticleLayerInterpMap::iterator curInterp = mLayerInterps.begin();
      ParticleLayerInterpMap::iterator endInterpMap = mLayerInterps.end();
      for( ; curInterp != endInterpMap; ++curInterp )
      {
         curInterp->second->InterpolateOverTime( prop, time, targetInterpolationRatio );
         ++layers;
      }

      return layers;
   }

   //////////////////////////////////////////////////////////////////////////
   int DynamicParticleSystem::InterpolateAllLayers(dtCore::ParticlePropertyEnum prop, float interpolation)
   {
      int layers = 0;

      ParticleLayerInterpMap::iterator curInterp = mLayerInterps.begin();
      ParticleLayerInterpMap::iterator endInterpMap = mLayerInterps.end();
      for( ; curInterp != endInterpMap; ++curInterp )
      {
         curInterp->second->Interpolate(prop, interpolation);
         ++layers;
      }

      return layers;
   }

   //////////////////////////////////////////////////////////////////////////
   int DynamicParticleSystem::InterpolateAllLayers( float time, float interpolation )
   {
      return InterpolateAllLayers( PS_ALL_PROPERTIES, time, interpolation );
   }

   //////////////////////////////////////////////////////////////////////////
   int DynamicParticleSystem::InterpolateAllLayers(float targetInterpolationRatio)
   {
      int layers = 0;

      ParticleLayerInterpMap::iterator curInterp = mLayerInterps.begin();
      ParticleLayerInterpMap::iterator endInterpMap = mLayerInterps.end();
      for( ; curInterp != endInterpMap; ++curInterp )
      {
         curInterp->second->InterpolateAllProperties(targetInterpolationRatio);
         ++layers;
      }

      return layers;
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystem::Reset()
   {
      ParticleLayerInterpMap::iterator curInterp = mLayerInterps.begin();
      ParticleLayerInterpMap::iterator endInterpMap = mLayerInterps.end();
      for( ; curInterp != endInterpMap; ++curInterp )
      {
         curInterp->second->Reset();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   int DynamicParticleSystem::ResetParticleLayers()
   {
      // Clear out the mappings to any current layers.
      ClearParticleLayers();

      int successes = 0;

      PSLayerList& layers = GetAllLayers();

      dtCore::RefPtr<ParticleSystemSettings> emptySettings = new ParticleSystemSettings;

      // Maintain a map to all particle layers by their associated particle system's name.
      PSLayerList::iterator curLayer = layers.begin();
      PSLayerList::iterator endLayerList = layers.end();
      for( ; curLayer != endLayerList; ++curLayer )
      {
         dtCore::RefPtr<ParticleLayerInterpolator> newInterp = new ParticleLayerInterpolator(*curLayer);

         // Use empty settings as the start settings for interpolation.
         // The full particle system effect will be used for end settings.
         newInterp->GetStartSettings() = *emptySettings;

         // Map the layers to the name of the associated OSG particle system.
         // This will allow for quick access to a particular layer by name.
         if( mLayerInterps.insert( std::make_pair( curLayer->GetLayerName(),
            newInterp.get()) ).second )
         {
            ++successes;
         }
      }

      Reset();

      return successes;
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystem::ClearParticleLayers()
   {
      mLayerInterps.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystem::Update( float simTimeDelta )
   {
      ParticleLayerInterpMap::iterator curInterp = mLayerInterps.begin();
      ParticleLayerInterpMap::iterator endInterpMap = mLayerInterps.end();
      for( ; curInterp != endInterpMap; ++curInterp )
      {
         curInterp->second->Update( simTimeDelta );
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystem::Init()
   {
      ResetParticleLayers();
   }



   //////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   //////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString DynamicParticleSystemActor::CLASS_NAME("dtCore::DynamicParticleSystem");
   const dtUtil::RefString DynamicParticleSystemActor::PROPERTY_INITIAL_INTERPOLATION("Initial Interpolation");

   //////////////////////////////////////////////////////////////////////////
   DynamicParticleSystemActor::DynamicParticleSystemActor()
      : BaseClass()
   {
      SetClassName(DynamicParticleSystemActor::CLASS_NAME);
   }

   //////////////////////////////////////////////////////////////////////////
   DynamicParticleSystemActor::~DynamicParticleSystemActor()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystemActor::SetEnabled(bool enabled)
   {
      if (mParticleSystem.valid())
      {
         mParticleSystem->SetEnabled(enabled);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool DynamicParticleSystemActor::IsEnabled() const
   {
      return mParticleSystem.valid() ? mParticleSystem->IsEnabled() : false;
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystemActor::CreateDrawable()
   {
      SetDrawable(*new DynamicParticleSystem());
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystemActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      const dtUtil::RefString GROUP("Particle System");

      DynamicParticleSystem* drawable = nullptr;
      GetDrawable(drawable);

      // FLOAT PROPERTIES
      AddProperty(new dtCore::FloatActorProperty(
         PROPERTY_INITIAL_INTERPOLATION.Get(),
         PROPERTY_INITIAL_INTERPOLATION.Get(),
         dtCore::FloatActorProperty::SetFuncType(drawable, &DynamicParticleSystem::SetInterpolation),
         dtCore::FloatActorProperty::GetFuncType(drawable, &DynamicParticleSystem::GetInterpolation),
         dtUtil::RefString("Sets the initial interpolation for the particle system between start and end interpolation settings."),
         GROUP));
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystemActor::TickLocal( const dtGame::Message& tickMessage )
   {
      mParticleSystem->Update(static_cast<const dtGame::TickMessage&>(tickMessage).GetDeltaSimTime());
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystemActor::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();

      DynamicParticleSystem* drawable = nullptr;
      GetDrawable(drawable);

      mParticleSystem = drawable;
      mParticleSystem->Init();

      if( ! IsRemote())
      {
         typedef dtUtil::Functor<void, TYPELIST_1(const dtGame::TickMessage&)> TickFunc;
         TickFunc func(this, &DynamicParticleSystemActor::TickLocal);

         RegisterForMessages<dtGame::TickMessage>
            (dtGame::MessageType::TICK_LOCAL, func);
         //RegisterForMessages(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicParticleSystemActor::OnRemovedFromWorld()
   {
      BaseClass::OnRemovedFromWorld();
   }

   //////////////////////////////////////////////////////////////////////////
   DynamicParticleSystem& DynamicParticleSystemActor::GetDynamicParticleSystem()
   {
      return *mParticleSystem;
   }

   //////////////////////////////////////////////////////////////////////////
   const DynamicParticleSystem& DynamicParticleSystemActor::GetDynamicParticleSystem() const
   {
      return *mParticleSystem;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon* DynamicParticleSystemActor::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         dtCore::ActorProxyIcon::ActorProxyIconConfig config;
         config.mForwardVector = true;
         config.mUpVector = true;
         config.mScale = 0.1;

         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH, config);
      }

      return mBillBoardIcon.get();
   }

   //////////////////////////////////////////////////////////////////////////
   const dtCore::ActorProxy::RenderMode& DynamicParticleSystemActor::GetRenderMode()
   {
      return dtCore::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
   }
}
