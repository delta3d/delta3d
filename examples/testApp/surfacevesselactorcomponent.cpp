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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "surfacevesselactorcomponent.h"
#include <dtActors/particlesystemactorproxy.h>
#include <dtActors/engineactorregistry.h>
#include <dtCore/shadermanager.h>
#include <dtCore/particlesystem.h>
#include <dtCore/transform.h>
#include <dtCore/project.h>
#include <dtCore/propertymacros.h>
#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtUtil/mathdefines.h>
#include <dtActors/dynamicparticlesystemactor.h>

#include <osg/MatrixTransform>
#include <osg/Geode>



namespace dtExample
{
   //////////////////////////////////////////////////////////
   // CLASS CODE
   //////////////////////////////////////////////////////////
   const dtGame::ActorComponent::ACType SurfaceVesselActorComponent::TYPE(new dtCore::ActorType("SurfaceVessel", "ActorComponents", "", dtGame::ActorComponent::BaseActorComponentType));
   
   const dtUtil::RefString SurfaceVesselActorComponent::CLASS_NAME("dtExample.SurfaceVesselActorComponent");

   //////////////////////////////////////////////////////////
   SurfaceVesselActorComponent::SurfaceVesselActorComponent(const ACType& type)
      : BaseClass(type)
      , mSprayEnabled(false)
      , mSprayUpdateFrequency(3.0f)
      , mSprayVelocityMin(1.0f)
      , mSprayVelocityMax(8.0f)
      , mLastSprayRatio()
      , mSprayUpdateTimer(0.0f)
   {
      SetClassName(SurfaceVesselActorComponent::CLASS_NAME);
   }

   //////////////////////////////////////////////////////////
   SurfaceVesselActorComponent::~SurfaceVesselActorComponent()
   {
      SetSprayEnabled(false);
   }

   DT_IMPLEMENT_ACCESSOR_GETTER(SurfaceVesselActorComponent, bool, SprayEnabled);
   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent, float, SprayUpdateFrequency);
   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent, std::string, SprayShaderGroup);
   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent, dtCore::ResourceDescriptor, SprayFrontResource);
   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent, dtCore::ResourceDescriptor, SpraySideLeftResource);
   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent, dtCore::ResourceDescriptor, SpraySideRightResource);
   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent, dtCore::ResourceDescriptor, SprayBackResource);

   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent, osg::Vec3, SprayFrontOffset);
   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent, osg::Vec3, SprayRightOffset);
   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent, osg::Vec3, SprayLeftOffset);
   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent, osg::Vec3, SprayBackOffset);

   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent,float, SprayVelocityMin);
   DT_IMPLEMENT_ACCESSOR(SurfaceVesselActorComponent,float, SprayVelocityMax);

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      const dtUtil::RefString GROUP("SurfaceVessel");
      typedef dtCore::PropertyRegHelper<SurfaceVesselActorComponent&, SurfaceVesselActorComponent> RegHelperType;
      RegHelperType propReg(*this, this, GROUP);

      DT_REGISTER_PROPERTY(SprayEnabled, "Turns the Spray particle system on or off", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(SprayUpdateFrequency, "Set the number of seconds between updates to the particle system effect interpolation", RegHelperType, propReg);
      DT_REGISTER_RESOURCE_PROPERTY(dtCore::DataType::PARTICLE_SYSTEM, SprayFrontResource, "Spray Resource Front", "Loads the particle system for the water spray effect on the front of the vessel", RegHelperType, propReg);
      DT_REGISTER_RESOURCE_PROPERTY(dtCore::DataType::PARTICLE_SYSTEM, SpraySideLeftResource,"Spray Resource Left", "Loads the particle system for the water spray effect on the left side of the vessel", RegHelperType, propReg);
      DT_REGISTER_RESOURCE_PROPERTY(dtCore::DataType::PARTICLE_SYSTEM, SpraySideRightResource,"Spray Resource Right", "Loads the particle system for the water spray effect on the right side of the vessel", RegHelperType, propReg);
      DT_REGISTER_RESOURCE_PROPERTY(dtCore::DataType::PARTICLE_SYSTEM, SprayBackResource, "Spray Resource Back", "Loads the particle system for the water spray effect on the back of the vessel", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(SprayFrontOffset, "The local offset on the right side to apply the water spray particle effect on the front of the vessel.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(SprayLeftOffset, "The local offset on the right side to apply the water spray particle effect on the left of the vessel.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(SprayRightOffset, "The local offset on the right side to apply the water spray particle effect on the right of the vessel.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(SprayBackOffset, "The local offset on the right side to apply the water spray particle effect on the back of the vessel.", RegHelperType, propReg);

      DT_REGISTER_PROPERTY(SprayVelocityMin, "The speed at which to start the water spray particle effect.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(SprayVelocityMax, "The speed at which to clamp the water spray particle systems' maximum effect.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(SprayShaderGroup, "Shader to apply to the particle systems.", RegHelperType, propReg);
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();

      RegisterForTick();

      dtGame::GameActorProxy* owner;
      GetOwner(owner);
      owner->RegisterForMessages(dtGame::MessageType::INFO_MAP_CHANGE_LOAD_END, dtUtil::MakeFunctor(&SurfaceVesselActorComponent::OnMapLoaded, this));
      owner->RegisterForMessages(dtGame::MessageType::INFO_MAPS_OPENED, dtUtil::MakeFunctor(&SurfaceVesselActorComponent::OnMapLoaded, this));
   }



   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::OnMapLoaded(const dtGame::MapMessage& mapMessage)
   {
      CreateSprayEffects();

      dtGame::GameActorProxy* owner = NULL;
      GetOwner(owner);

      dtCore::Transformable* drawable = NULL;
      owner->GetDrawable(drawable);
      mOwnerDrawable = drawable;

      if(mSprayFront.valid())
      {
         // Attach the particles to the parent
         drawable->GetOSGNode()->asGroup()->addChild(mSprayFront->GetOSGNode());

         // Offset the particles 
         dtCore::Transform transform;

         transform.SetTranslation(mSprayFrontOffset);
         mSprayFront->SetTransform(transform, dtCore::Transformable::REL_CS);  

         BindShaderToParticleSystem(*mSprayFront);        
      }
 
      if(mSpraySideLeft.valid())
      {
         // Attach the particles to the parent
         drawable->GetOSGNode()->asGroup()->addChild(mSpraySideLeft->GetOSGNode());

         // Offset the particles 
         dtCore::Transform transform;

         transform.SetTranslation(mSprayLeftOffset);
         mSpraySideLeft->SetTransform(transform, dtCore::Transformable::REL_CS);

         BindShaderToParticleSystem(*mSpraySideLeft);
      }
 
      if(mSpraySideRight.valid())
      {
         // Attach the particles to the parent
         drawable->GetOSGNode()->asGroup()->addChild(mSpraySideRight->GetOSGNode());

         // Offset the particles 
         dtCore::Transform transform;

         transform.SetTranslation(mSprayRightOffset);
         mSpraySideRight->SetTransform(transform, dtCore::Transformable::REL_CS);

         BindShaderToParticleSystem(*mSpraySideRight);
      }

      if(mSprayBack.valid())
      {
         // Attach the particles to the parent
         drawable->GetOSGNode()->asGroup()->addChild(mSprayBack->GetOSGNode());

         // Offset the particles 
         dtCore::Transform transform;

         transform.SetTranslation(mSprayBackOffset);
         mSprayBack->SetTransform(transform, dtCore::Transformable::REL_CS);

         BindShaderToParticleSystem(*mSprayBack);
      }

      // Ensure the particle systems have the current enabled state.
      SetSprayEnabled(mSprayEnabled);
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::BindShaderToParticleSystem(dtCore::ParticleSystem& particles)
   {
      dtCore::ParticleSystem::LayerList& layers = particles.GetAllLayers();
      dtCore::ParticleSystem::LayerList::iterator iter = layers.begin();
      for( ; iter != layers.end(); ++iter )
      {
         //osgParticle::ParticleSystem* ref = &iter->GetParticleSystem();
         dtCore::ParticleLayer& pLayer = *iter;
         BindShaderToNode(pLayer.GetGeode());
      }
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::BindShaderToNode(osg::Node& node)
   {
      dtCore::ShaderManager& sm = dtCore::ShaderManager::GetInstance();
      dtCore::ShaderGroup* sg = sm.FindShaderGroupPrototype(mSprayShaderGroup);
      if(sg)
      {
         dtCore::ShaderProgram* sp = sg->GetDefaultShader();

         if(sp != NULL)
         {
            sm.AssignShaderFromPrototype(*sp, node);
         }
         else
         {
            LOG_ERROR("Unable to find particle system shader group: \"" + mSprayShaderGroup + "\" ");
         }
      }
      else
      {
         LOG_ERROR("Unable to find shader group for particle system manager.");
      }
   }


   //////////////////////////////////////////////////////////
   dtCore::RefPtr<SurfaceVesselActorComponent::DynamicParticlesActor>
      SurfaceVesselActorComponent::CreatDynamicParticleSystemActor(
      const dtCore::ResourceDescriptor& rd, const std::string& actorName)
   {
      std::string fileName;
      try
      {
         fileName = dtCore::Project::GetInstance().GetResourcePath(rd);
      }
      catch(const dtUtil::Exception& ex)
      {
         ex.LogException();
         return NULL;
      }

      dtGame::GameActorProxy* owner = NULL;
      GetOwner(owner);
      if (owner == NULL)
      {
         return NULL;
      }

      dtCore::RefPtr<DynamicParticlesActor> actor;
      
      dtGame::GameManager* gm = owner->GetGameManager();
      if(gm != NULL)
      {
         // Create the actor.
         DynamicParticles* drawable = NULL;
         gm->CreateActor(*dtActors::EngineActorRegistry::DYNAMIC_PARTICLE_SYSTEM_ACTOR_TYPE, actor);

         actor->GetDrawable(drawable);
         drawable->SetName(actorName);
         drawable->LoadFile(fileName);

         // Set default settings.
         typedef DynamicParticles::InterpolatorArray InterpolatorArray;
         InterpolatorArray interpArray;
         drawable->GetAllInterpolators(interpArray);

         InterpolatorArray::iterator curInterp = interpArray.begin();
         InterpolatorArray::iterator endInterpArray = interpArray.end();
         for( ; curInterp != endInterpArray; ++curInterp)
         {         
            dtCore::ParticleSystemSettings& settings = (*curInterp)->GetStartSettings();
            settings.mRangeRate *= 0.0f;
            settings.mRangeSpeed *= 0.0f;
         }

         gm->AddActor(*actor, false, false);
      }

      return actor;
   }

   //////////////////////////////////////////////////////////
   SurfaceVesselActorComponent::DynamicParticles* SurfaceVesselActorComponent::GetDynamicParticles(DynamicParticlesActor* actor)
   {
      DynamicParticles* drawable = NULL;
      if(actor != NULL)
      {
         actor->GetDrawable(drawable);
      }
      return drawable;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::CreateSprayEffects()
   {
      if(!mSprayFront.valid())
      {
         mSprayFrontActor = CreatDynamicParticleSystemActor(mSprayFrontResource, "SprayFrontRight");
         mSprayFront = GetDynamicParticles(mSprayFrontActor.get());
      }

      if(!mSpraySideLeft.valid())
      {
         mSpraySideLeftActor = CreatDynamicParticleSystemActor(mSpraySideLeftResource, "SpraySideLeft");
         mSpraySideLeft = GetDynamicParticles(mSpraySideLeftActor.get());
      }

      if(!mSpraySideRight.valid())
      {
         mSpraySideRightActor = CreatDynamicParticleSystemActor(mSpraySideRightResource, "SpraySideRight");
         mSpraySideRight = GetDynamicParticles(mSpraySideRightActor.get());
      }

      if(!mSprayBack.valid())
      {
         mSprayBackActor = CreatDynamicParticleSystemActor(mSprayBackResource, "SprayBack");
         mSprayBack = GetDynamicParticles(mSprayBackActor.get());
      }
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSprayEnabled( bool enable )
   {
      mSprayEnabled = enable;
      if(mSprayFront.valid())
      {
         mSprayFront->SetEnabled(mSprayEnabled);
      }

      if(mSpraySideLeft.valid())
      {
         mSpraySideLeft->SetEnabled(mSprayEnabled);
      }

      if(mSpraySideRight.valid())
      {
         mSpraySideRight->SetEnabled(mSprayEnabled);
      }

      if(mSprayBack.valid())
      {
         mSprayBack->SetEnabled(mSprayEnabled);
      }
   }


   //////////////////////////////////////////////////////////
   float SurfaceVesselActorComponent::GetVelocityRatio(float velocity) const
   {
      float ratio = 0.0f;

      if(mSprayVelocityMax != 0.0f)
      {
         if(velocity >= mSprayVelocityMin && mSprayVelocityMax > mSprayVelocityMin)
         {
            ratio = (velocity - mSprayVelocityMin) / (mSprayVelocityMax - mSprayVelocityMin);
         }
      }

      dtUtil::Clamp(ratio, 0.0f, 1.0f);
      
      return ratio;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::UpdateSpray(float simTimeDelta)
   {
      // Determine the displacement of the object.
      dtCore::Transform xform;
      mOwnerDrawable->GetTransform(xform);
      osg::Vec3 pos;
      xform.GetTranslation(pos);

      // --- Get the displacement from last tick.
      pos.z() = 0.0f; // The spray effect is not affected by up/down motion.
      osg::Vec3 dif(mLastPos);
      mLastPos = pos;
      dif = pos - dif;

      // Get the velocity and its amount of spray effect.
      float velocity = dif.length2() > 0.0f ? dif.length() : 0.0f;
      float ratio = GetVelocityRatio(velocity * (simTimeDelta!=0.0f ? 1.0f/simTimeDelta : 0.0f));

      mSprayUpdateTimer += simTimeDelta;

      bool allowInterpolation = mSprayUpdateTimer > mSprayUpdateFrequency || dtUtil::Abs(mLastSprayRatio - ratio) > 0.1f;

      if(allowInterpolation)
      {
         // Reset timer and control variables.
         mLastSprayRatio = ratio;
         mSprayUpdateTimer = 0.0f;

         // DEBUG:
         //printf("\n\tUpdating particles (%f / %f = %f)\n\n",
         //   velocity, GetSprayVelocityMax(), ratio);

         // Update the particle systems.
         if(mSprayFront.valid())
         {
            InterpolateParticleSystem(*mSprayFront, ratio);
         }

         if(mSpraySideLeft.valid())
         {
            InterpolateParticleSystem(*mSpraySideLeft, ratio);
         }

         if(mSpraySideRight.valid())
         {
            InterpolateParticleSystem(*mSpraySideRight, ratio);
         }

         if(mSprayBack.valid())
         {
            InterpolateParticleSystem(*mSprayBack, ratio);
         }
      }
   }
   
   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::InterpolateParticleSystem(DynamicParticles& particles, float ratio)
   {
      particles.InterpolateAllLayers(dtCore::ParticlePropertyEnum::PS_EMIT_SPEED, ratio);
      particles.InterpolateAllLayers(dtCore::ParticlePropertyEnum::PS_EMIT_RATE, ratio);
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::OnTickLocal(const dtGame::TickMessage& tickMessage)
   {
      BaseClass::OnTickLocal(tickMessage);

      float simTimeDelta = tickMessage.GetDeltaSimTime();
      UpdateSpray(simTimeDelta);
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::OnTickRemote(const dtGame::TickMessage& tickMessage)
   {
      BaseClass::OnTickRemote(tickMessage);

      float simTimeDelta = tickMessage.GetDeltaSimTime();
      UpdateSpray(simTimeDelta);
   }

}

