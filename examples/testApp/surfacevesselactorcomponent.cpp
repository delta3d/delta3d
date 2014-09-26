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
#include <dtCore/enginepropertytypes.h>
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

   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_ENABLED("Spray Enabled");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_FRONT_FILE("Spray Front File");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_SIDE_LEFT_FILE("Spray Side Left File");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_SIDE_RIGHT_FILE("Spray Side Right File");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_BACK_FILE("Spray Back File");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_FRONT_OFFSET("Spray Front Offset");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_SIDE_OFFSET_LEFT("Spray Side Offset Left");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_SIDE_OFFSET_RIGHT("Spray Side Offset Right");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_BACK_OFFSET("Spray Back Offset");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_VELOCITY_MIN("Spray Velocity Min");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_VELOCITY_MAX("Spray Velocity Max");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_UPDATE_FREQUENCY("Spray Update Frequency");
   const dtUtil::RefString SurfaceVesselActorComponent::PROPERTY_SPRAY_SHADER_GROUP("Spray Shader Group");

   //////////////////////////////////////////////////////////
   SurfaceVesselActorComponent::SurfaceVesselActorComponent(const ACType& type)
      : BaseClass(type)
      , mSprayEnabled(false)
      , mLastSprayRatio(0.0f)
      , mSprayVelocityMin(1.0f)
      , mSprayVelocityMax(8.0f)
      , mSprayUpdateTimer(0.0f)
      , mSprayUpdateFrequency(3.0f)
   {
      SetClassName(SurfaceVesselActorComponent::CLASS_NAME);
   }

   //////////////////////////////////////////////////////////
   SurfaceVesselActorComponent::~SurfaceVesselActorComponent()
   {
      SetSprayEnabled(false);
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      using namespace dtCore;
      using namespace dtUtil;

      RefString GROUP("SurfaceVessel");

      AddProperty(new BooleanActorProperty(
         PROPERTY_SPRAY_ENABLED,
         PROPERTY_SPRAY_ENABLED,
         BooleanActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSprayEnabled),
         BooleanActorProperty::GetFuncType(this, &SurfaceVesselActorComponent::IsSprayEnabled),
         RefString("Turns the Spray particle system on or off"),
         GROUP));

      AddProperty(new FloatActorProperty(
         PROPERTY_SPRAY_UPDATE_FREQUENCY,
         PROPERTY_SPRAY_UPDATE_FREQUENCY,
         FloatActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSprayUpdateFrequency),
         FloatActorProperty::GetFuncType(this, &SurfaceVesselActorComponent::GetSprayUpdateFrequency),
         RefString("Set the number of seconds between updates to the particle system effect interpolation"),
         GROUP));

      AddProperty(new ResourceActorProperty(*this,
         DataType::PARTICLE_SYSTEM,
         PROPERTY_SPRAY_FRONT_FILE,
         PROPERTY_SPRAY_FRONT_FILE,
         ResourceActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSprayFrontFile),
         RefString("Loads the particle system for the water spray effect on the front of the vessel"),
         GROUP));

      AddProperty(new ResourceActorProperty(*this,
         DataType::PARTICLE_SYSTEM,
         PROPERTY_SPRAY_SIDE_LEFT_FILE,
         PROPERTY_SPRAY_SIDE_LEFT_FILE,
         ResourceActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSpraySideLeftFile),
         RefString("Loads the particle system for the water spray effect on the left side of the vessel"),
         GROUP));

      AddProperty(new ResourceActorProperty(*this,
         DataType::PARTICLE_SYSTEM,
         PROPERTY_SPRAY_SIDE_RIGHT_FILE,
         PROPERTY_SPRAY_SIDE_RIGHT_FILE,
         ResourceActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSpraySideRightFile),
         RefString("Loads the particle system for the water spray effect on the right side of the vessel"),
         GROUP));

      AddProperty(new ResourceActorProperty(*this,
         DataType::PARTICLE_SYSTEM,
         PROPERTY_SPRAY_BACK_FILE,
         PROPERTY_SPRAY_BACK_FILE,
         ResourceActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSprayBackFile),
         RefString("Loads the particle system for the water spray effect on the back of the vessel"),
         GROUP));

      AddProperty(new Vec3ActorProperty(
         PROPERTY_SPRAY_FRONT_OFFSET,
         PROPERTY_SPRAY_FRONT_OFFSET, 
         Vec3ActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSprayFrontOffset),
         Vec3ActorProperty::GetFuncType(this, &SurfaceVesselActorComponent::GetSprayFrontOffset),
         RefString("The local offset on the right side to apply the water spray particle effect on the front of the vessel."),
         GROUP));

      AddProperty(new Vec3ActorProperty(
         PROPERTY_SPRAY_SIDE_OFFSET_RIGHT,
         PROPERTY_SPRAY_SIDE_OFFSET_RIGHT, 
         Vec3ActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSpraySideOffsetRight),
         Vec3ActorProperty::GetFuncType(this, &SurfaceVesselActorComponent::GetSpraySideOffsetRight),
         RefString("The local offset on the right side to apply the water spray particle effect on the side of the vessel."),
         GROUP));

      AddProperty(new Vec3ActorProperty(
         PROPERTY_SPRAY_SIDE_OFFSET_LEFT,
         PROPERTY_SPRAY_SIDE_OFFSET_LEFT, 
         Vec3ActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSpraySideOffsetLeft),
         Vec3ActorProperty::GetFuncType(this, &SurfaceVesselActorComponent::GetSpraySideOffsetLeft),
         RefString("The local offset on the left side to apply the water spray particle effect on the side of the vessel."),
         GROUP));

      AddProperty(new Vec3ActorProperty(
         PROPERTY_SPRAY_BACK_OFFSET,
         PROPERTY_SPRAY_BACK_OFFSET, 
         Vec3ActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSprayBackOffset),
         Vec3ActorProperty::GetFuncType(this, &SurfaceVesselActorComponent::GetSprayBackOffset),
         RefString("The local offset on the back side to apply the water spray particle effect on the back of the vessel."),
         GROUP));

      AddProperty(new FloatActorProperty(
         PROPERTY_SPRAY_VELOCITY_MIN,
         PROPERTY_SPRAY_VELOCITY_MIN, 
         FloatActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSprayVelocityMin),
         FloatActorProperty::GetFuncType(this, &SurfaceVesselActorComponent::GetSprayVelocityMin),
         RefString("The speed at which to start the water spray particle effect."),
         GROUP));

      AddProperty(new FloatActorProperty(
         PROPERTY_SPRAY_VELOCITY_MAX,
         PROPERTY_SPRAY_VELOCITY_MAX, 
         FloatActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSprayVelocityMax),
         FloatActorProperty::GetFuncType(this, &SurfaceVesselActorComponent::GetSprayVelocityMax),
         RefString("The speed at which to clamp the water spray particle systems' maximum effect."),
         GROUP));

      AddProperty(new StringActorProperty(
         PROPERTY_SPRAY_SHADER_GROUP,
         PROPERTY_SPRAY_SHADER_GROUP, 
         StringActorProperty::SetFuncType(this, &SurfaceVesselActorComponent::SetSprayShaderGroup),
         StringActorProperty::GetFuncType(this, &SurfaceVesselActorComponent::GetSprayShaderGroup),
         RefString("Shader to apply to the particle systems."),
         GROUP));
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();

      // Full initialization of this component may require
      // the map to be fully loaded.
      RegisterForMapLoaded();
   }



   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::OnMapLoaded(const dtGame::MapMessage& mapMessage)
   {
      BaseClass::OnMapLoaded(mapMessage);

      CreateSprayEffects();

      dtGame::GameActorProxy* owner = NULL;
      GetOwner(owner);

      dtCore::Transformable* drawable = NULL;
      owner->GetDrawable(drawable);
      mOwnerDrawable = drawable;

      RegisterForTick();

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

         transform.SetTranslation(mSpraySideOffsetLeft);
         mSpraySideLeft->SetTransform(transform, dtCore::Transformable::REL_CS);

         BindShaderToParticleSystem(*mSpraySideLeft);
      }
 
      if(mSpraySideRight.valid())
      {
         // Attach the particles to the parent
         drawable->GetOSGNode()->asGroup()->addChild(mSpraySideRight->GetOSGNode());

         // Offset the particles 
         dtCore::Transform transform;

         transform.SetTranslation(mSpraySideOffsetRight);
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
      dtCore::ShaderGroup* sg = sm.FindShaderGroupPrototype(mParticleShaderGroup);
      if(sg)
      {
         dtCore::ShaderProgram* sp = sg->GetDefaultShader();

         if(sp != NULL)
         {
            sm.AssignShaderFromPrototype(*sp, node);
         }
         else
         {
            LOG_ERROR("Unable to find particle system shader group: \"" + mParticleShaderGroup + "\" ");
         }
      }
      else
      {
         LOG_ERROR("Unable to find shader group for particle system manager.");
      }
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSprayUpdateFrequency(float frequency)
   {
      mSprayUpdateFrequency = frequency;
   }

   //////////////////////////////////////////////////////////
   float SurfaceVesselActorComponent::GetSprayUpdateFrequency() const
   {
      return mSprayUpdateFrequency;
   }

   //////////////////////////////////////////////////////////
   osg::Vec3 SurfaceVesselActorComponent::GetSprayFrontOffset() const
   {
      return mSprayFrontOffset;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSprayFrontOffset( const osg::Vec3& vec )
   {
      mSprayFrontOffset = vec;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSpraySideOffsetLeft( const osg::Vec3& vec )
   {
      mSpraySideOffsetLeft = vec;
   }

   //////////////////////////////////////////////////////////
   osg::Vec3 SurfaceVesselActorComponent::GetSpraySideOffsetLeft() const
   {
      return mSpraySideOffsetLeft;
   }

   //////////////////////////////////////////////////////////
   osg::Vec3 SurfaceVesselActorComponent::GetSpraySideOffsetRight() const
   {
      return mSpraySideOffsetRight;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSpraySideOffsetRight( const osg::Vec3& vec )
   {
      mSpraySideOffsetRight = vec;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSprayBackOffset( const osg::Vec3& vec )
   {
      mSprayBackOffset = vec;
   }

   //////////////////////////////////////////////////////////
   osg::Vec3 SurfaceVesselActorComponent::GetSprayBackOffset() const
   {
      return mSprayBackOffset;
   }

   //////////////////////////////////////////////////////////
   dtCore::RefPtr<SurfaceVesselActorComponent::DynamicParticlesActor>
      SurfaceVesselActorComponent::CreatDynamicParticleSystemActor(
      const std::string& filename, const std::string& actorName)
   {
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
         drawable->LoadFile(filename);

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
   void SurfaceVesselActorComponent::SetSprayFrontFile(const std::string& fileName)
   {
      mFileSprayFront = fileName;
   }
   
   //////////////////////////////////////////////////////////
   const std::string& SurfaceVesselActorComponent::GetSprayFrontFile() const
   {
      return mFileSprayFront;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSpraySideLeftFile(const std::string& fileName)
   {
      mFileSpraySideLeft = fileName;
   }
   
   //////////////////////////////////////////////////////////
   const std::string& SurfaceVesselActorComponent::GetSpraySideLeftFile() const
   {
      return mFileSpraySideLeft;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSpraySideRightFile(const std::string& fileName)
   {
      mFileSpraySideRight = fileName;
   }
   
   //////////////////////////////////////////////////////////
   const std::string& SurfaceVesselActorComponent::GetSpraySideRightFile() const
   {
      return mFileSpraySideRight;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSprayBackFile(const std::string& fileName)
   {
      mFileSprayBack = fileName;
   }
   
   //////////////////////////////////////////////////////////
   const std::string& SurfaceVesselActorComponent::GetSprayBackFile() const
   {
      return mFileSprayBack;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::CreateSprayEffects()
   {
      if(!mSprayFront.valid())
      {
         mSprayFrontActor = CreatDynamicParticleSystemActor(mFileSprayFront, "SprayFrontRight");
         mSprayFront = GetDynamicParticles(mSprayFrontActor.get());
      }

      if(!mSpraySideLeft.valid())
      {
         mSpraySideLeftActor = CreatDynamicParticleSystemActor(mFileSpraySideLeft, "SpraySideLeft");
         mSpraySideLeft = GetDynamicParticles(mSpraySideLeftActor.get());
      }

      if(!mSpraySideRight.valid())
      {
         mSpraySideRightActor = CreatDynamicParticleSystemActor(mFileSpraySideRight, "SpraySideRight");
         mSpraySideRight = GetDynamicParticles(mSpraySideRightActor.get());
      }

      if(!mSprayBack.valid())
      {
         mSprayBackActor = CreatDynamicParticleSystemActor(mFileSprayBack, "SprayBack");
         mSprayBack = GetDynamicParticles(mSprayBackActor.get());
      }
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSprayEnabled( bool enable )
   {
      mSprayEnabled = enable;

      if(mSprayFront.valid())
      {
         mSprayFront->SetEnabled(enable);
      }

      if(mSpraySideLeft.valid())
      {
         mSpraySideLeft->SetEnabled(enable);
      }

      if(mSpraySideRight.valid())
      {
         mSpraySideRight->SetEnabled(enable);
      }

      if(mSprayBack.valid())
      {
         mSprayBack->SetEnabled(enable);
      }
   }

   //////////////////////////////////////////////////////////
   bool SurfaceVesselActorComponent::IsSprayEnabled()
   {
      return mSprayEnabled;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSprayShaderGroup(const std::string& shaderGroup)
   {
      mParticleShaderGroup = shaderGroup;
   }
   
   //////////////////////////////////////////////////////////
   const std::string& SurfaceVesselActorComponent::GetSprayShaderGroup() const
   {
      return mParticleShaderGroup;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSprayVelocityMin(float minVelocity)
   {
      mSprayVelocityMin = minVelocity;
   }

   //////////////////////////////////////////////////////////
   float SurfaceVesselActorComponent::GetSprayVelocityMin() const
   {
      return mSprayVelocityMin;
   }

   //////////////////////////////////////////////////////////
   void SurfaceVesselActorComponent::SetSprayVelocityMax(float maxVelocity)
   {
      mSprayVelocityMax = maxVelocity;
   }
   
   //////////////////////////////////////////////////////////
   float SurfaceVesselActorComponent::GetSprayVelocityMax() const
   {
      return mSprayVelocityMax;
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
      particles.InterpolateAllLayers(dtCore::ParticlePropertyEnum::PS_PARTICLE_SIZE, ratio);
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

