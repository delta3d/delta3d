/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef DELTA_TEST_APP_SURFACE_VESSEL_ACTOR_COMPONENT_H
#define DELTA_TEST_APP_SURFACE_VESSEL_ACTOR_COMPONENT_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtActors/dynamicparticlesystemactor.h>
#include <dtCore/particlesystem.h>
#include <dtCore/refptr.h>
#include <dtGame/actorcomponent.h>
#include <dtCore/gameevent.h>
#include <dtPhysics/buoyancyaction.h>
#include <osg/Vec3>


namespace dtGame
{
   class GameEventMessage;
}


namespace dtExample
{
   class WaterActorHeightQuery;
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class SurfaceVesselActorComponent : public dtGame::ActorComponent
   {
   public:
      typedef dtGame::ActorComponent BaseClass;

      static const ACType TYPE;

      static const dtUtil::RefString CLASS_NAME;

      SurfaceVesselActorComponent(const ACType& type = TYPE);

      DT_DECLARE_ACCESSOR(bool, SprayEnabled);
      DT_DECLARE_ACCESSOR(float, SprayUpdateFrequency);
      DT_DECLARE_ACCESSOR(std::string, SprayShaderGroup);
      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, SprayFrontResource);
      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, SpraySideLeftResource);
      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, SpraySideRightResource);
      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, SprayBackResource);

      DT_DECLARE_ACCESSOR(osg::Vec3, SprayFrontOffset);
      DT_DECLARE_ACCESSOR(osg::Vec3, SprayRightOffset);
      DT_DECLARE_ACCESSOR(osg::Vec3, SprayLeftOffset);
      DT_DECLARE_ACCESSOR(osg::Vec3, SprayBackOffset);

      DT_DECLARE_ACCESSOR(float, SprayVelocityMin);
      DT_DECLARE_ACCESSOR(float, SprayVelocityMax);
      DT_DECLARE_ACCESSOR(dtCore::RefPtr<dtCore::GameEvent>, UpdateFromControllerEvent);

      float GetVelocityRatio(float velocity) const;

      void UpdateSpray(float simTimeDelta);

      /*virtual*/ void OnTickLocal(const dtGame::TickMessage& tickMessage);
      /*virtual*/ void OnTickRemote(const dtGame::TickMessage& tickMessage);
      virtual void OnMapLoaded(const dtGame::MapMessage& mapMessage);
      virtual void OnGameEvent(const dtGame::GameEventMessage& gameEvent);

      /// Adds the properties associated with this actor
      /*virtual*/ void BuildPropertyMap();
      /*virtual*/ void OnRemovedFromWorld();

      /*virtual*/ void OnEnteredWorld();

   protected:

      /// Destructor
      virtual ~SurfaceVesselActorComponent();

      void CreateSprayEffects();

      void BindShaderToParticleSystem(dtCore::ParticleSystem& particles);
      void BindShaderToNode(osg::Node& node);

      typedef dtActors::DynamicParticleSystem DynamicParticles;
      typedef dtActors::DynamicParticleSystemActor DynamicParticlesActor;
      dtCore::RefPtr<DynamicParticlesActor> CreateDynamicParticleSystemActor(const dtCore::ResourceDescriptor& rd,
         const std::string& actorName);
      DynamicParticles* GetDynamicParticles(DynamicParticlesActor* actor);

      void InterpolateParticleSystem(DynamicParticles& particles, float ratio);

   private:

      float mLastSprayRatio;
      float mSprayUpdateTimer;
      osg::Vec3 mLastPos;

      dtCore::RefPtr<DynamicParticlesActor> mSprayFrontActor;
      dtCore::RefPtr<DynamicParticlesActor> mSpraySideRightActor;
      dtCore::RefPtr<DynamicParticlesActor> mSpraySideLeftActor;
      dtCore::RefPtr<DynamicParticlesActor> mSprayBackActor;

      dtCore::RefPtr<DynamicParticles> mSprayFront;
      dtCore::RefPtr<DynamicParticles> mSpraySideRight;
      dtCore::RefPtr<DynamicParticles> mSpraySideLeft;
      dtCore::RefPtr<DynamicParticles> mSprayBack;

      dtCore::ObserverPtr<dtCore::Transformable> mOwnerDrawable;
      dtCore::ObserverPtr<dtCore::Transformable> mWhomToFollow;
      dtCore::RefPtr<dtPhysics::BuoyancyAction> mBuoyancyAction;
      WaterActorHeightQuery* mWaterHeightQuery;
   };
}

#endif
