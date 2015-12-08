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
#include "fireworkactor.h"
#include "lightactorcomponent.h"
#include <dtCore/colorrgbaactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>



namespace dtExample
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   const float FireworkActor::DEFAULT_DETONATE_TIME_LIMIT = 10.0f;
   const dtUtil::RefString FireworkActor::DEFAULT_LAYER_NAME_FLARE("Flare");
   const dtUtil::RefString FireworkActor::DEFAULT_LAYER_NAME_SPARKS("Sparks");

   const dtUtil::RefString FireworkActor::PROPERTY_COLOR_BEGIN("ColorBegin");
   const dtUtil::RefString FireworkActor::PROPERTY_COLOR_END("ColorEnd");
   const dtUtil::RefString FireworkActor::PROPERTY_LAYER_NAME_FLARE("Layer Name Flare");
   const dtUtil::RefString FireworkActor::PROPERTY_LAYER_NAME_SPARKS("Layer Name Sparks");
   const dtUtil::RefString FireworkActor::PROPERTY_DETONATE_TIME_LIMIT("DetonateTimeLimit");
   const dtUtil::RefString FireworkActor::PROPERTY_EVENT_TO_LAUNCH("Event To Launch");
   const dtUtil::RefString FireworkActor::PROPERTY_EVENT_TO_DETONATE("Event To Detonate");



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   FireworkActor::FireworkActor()
      : BaseClass()
      , mSparkLifeTime(0.0f)
      , mSparkLifeTimeRemaining(0.0f)
      , mDetonateTimeRemaining(0.0f)
      , mDetonateTimeLimit(DEFAULT_DETONATE_TIME_LIMIT)
      , mLayerNameFlare(DEFAULT_LAYER_NAME_FLARE)
      , mLayerNameSparks(DEFAULT_LAYER_NAME_SPARKS)
   {
      mColorBegin.set(1.0f, 1.0f, 1.0f, 1.0f);
      mColorEnd.set(1.0f, 1.0f, 1.0f, 1.0f);
   }

   /////////////////////////////////////////////////////////////////////////////
   FireworkActor::~FireworkActor()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::SetColorBegin(const osg::Vec4& color)
   {
      if (mColorBegin != color)
      {
         mColorBegin = color;

         ApplyColorsToParticleSystem();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec4 FireworkActor::GetColorBegin() const
   {
      return mColorBegin;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::SetColorEnd(const osg::Vec4& color)
   {
      if (mColorEnd != color)
      {
         mColorEnd = color;

         ApplyColorsToParticleSystem();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec4 FireworkActor::GetColorEnd() const
   {
      return mColorEnd;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::SetDetonateTimeLimit(float seconds)
   {
      mDetonateTimeLimit = seconds;
   }

   /////////////////////////////////////////////////////////////////////////////
   float FireworkActor::GetDetonateTimeLimit() const
   {
      return mDetonateTimeLimit;
   }

   /////////////////////////////////////////////////////////////////////////////
   float FireworkActor::GetDetonateTimeRemaining() const
   {
      return mDetonateTimeRemaining;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::ParticleLayer* FireworkActor::GetParticleLayer(
      dtCore::ParticleSystem& ps, const std::string& layerName) const
   {
      dtCore::ParticleLayer* layer = NULL;

      typedef dtCore::ParticleSystem::LayerList ParticleLayerList;

      dtCore::ParticleLayer* curLayer = NULL;
      ParticleLayerList& layers = ps.GetAllLayers();
      ParticleLayerList::iterator curIter = layers.begin();
      ParticleLayerList::iterator endIter = layers.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curLayer = &(*curIter);
         if (curLayer->GetLayerName() == layerName)
         {
            layer = curLayer;
            break;
         }
      }

      return layer;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::ParticleLayer* FireworkActor::GetParticleLayer(const std::string& layerName)
   {
      dtCore::ParticleSystem* ps = NULL;
      GetDrawable(ps);

      return ps == NULL ? NULL : GetParticleLayer(*ps, layerName);
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::SetLayerNameFlare(const std::string& name)
   {
      mLayerNameFlare = name;
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& FireworkActor::GetLayerNameFlare() const
   {
      return mLayerNameFlare;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::SetLayerNameSparks(const std::string& name)
   {
      mLayerNameSparks = name;
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& FireworkActor::GetLayerNameSparks() const
   {
      return mLayerNameSparks;
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec4 FireworkActor::GetCurrentColor(dtCore::ParticleSystem& ps) const
   {
      osg::Vec4 color;

      bool flarePhase = mDetonateTimeRemaining > 0.0f;
      const std::string* layerName = mDetonateTimeRemaining > 0.0f
         ? &mLayerNameFlare : &mLayerNameSparks;

      dtCore::ParticleLayer* layer = GetParticleLayer(ps, *layerName);

      if (layer != NULL)
      {
         osgParticle::ParticleSystem& osgPS = layer->GetParticleSystem();
         osgParticle::Particle& p = osgPS.getDefaultParticleTemplate();
         
         float timeRemaining = 0.0f;
         float timeSpan = 0.0f;

         if (flarePhase)
         {
            timeRemaining = mDetonateTimeRemaining;
            timeSpan = mDetonateTimeLimit;
         }
         else // spark phase
         {
            timeRemaining = mSparkLifeTimeRemaining;
            timeSpan = mSparkLifeTime;
         }

         float ratio = 0.0;
         if (timeSpan != 0.0f)
         {
            ratio = timeRemaining / timeSpan;
         }

         osgParticle::rangev4 colorRange = p.getColorRange();
         color = (colorRange.maximum - colorRange.minimum) * ratio + colorRange.minimum;
      }

      return color;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::SetEventToLaunch(dtCore::GameEvent* gameEvent)
   {
      mEventToLaunch = gameEvent;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::GameEvent* FireworkActor::GetEventToLaunch() const
   {
      return mEventToLaunch.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::SetEventToDetonate(dtCore::GameEvent* gameEvent)
   {
      mEventToDetonate = gameEvent;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::GameEvent* FireworkActor::GetEventToDetonate() const
   {
      return mEventToDetonate.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::OnLaunch()
   {
      dtCore::ParticleSystem* ps = NULL;
      GetDrawable(ps);

      if (ps == NULL)
      {
         LOG_ERROR("Could not launch firework \"" + GetName()
            + "\" because its particle system could not be accessed.");
      }
      else
      {
         OnLaunch(*ps);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::OnDetonate()
   {
      dtCore::ParticleSystem* ps = NULL;
      GetDrawable(ps);

      if (ps == NULL)
      {
         LOG_ERROR("Could not detonate firework \"" + GetName()
            + "\" because its particle system could not be accessed.");
      }
      else
      {
         OnDetonate(*ps);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::StartDetonateTimer()
   {
      mDetonateTimeRemaining = mDetonateTimeLimit;

      ResetLight();
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::ApplyColorsToParticleSystem()
   {
      dtCore::ParticleSystem* ps = NULL;
      GetDrawable(ps);

      if (ps != NULL)
      {
         typedef dtCore::ParticleSystem::LayerList ParticleLayerList;

         dtCore::ParticleLayer* curLayer = NULL;
         ParticleLayerList& layers = ps->GetAllLayers();
         ParticleLayerList::iterator curIter = layers.begin();
         ParticleLayerList::iterator endIter = layers.end();

         for ( ; curIter != endIter; ++curIter)
         {
            curLayer = &(*curIter);

            // Do not set color on the flare layer.
            // Colors are intended for the sparks
            // from the detonation.
            if (curLayer->GetLayerName() != mLayerNameFlare)
            {
               ApplyColorsToParticleLayer(*curLayer);
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::ApplyColorsToParticleLayer(dtCore::ParticleLayer& layer)
   {
      osgParticle::ParticleSystem& osgPS = layer.GetParticleSystem();

      osgParticle::Particle& p = osgPS.getDefaultParticleTemplate();

      osgParticle::rangev4 colorRange = p.getColorRange();
      osg::Vec4 beginColor(mColorBegin);
      osg::Vec4 endColor(mColorEnd);

      // Keep the original alpha values as designed for the particle file.
      beginColor.w() = colorRange.minimum.a();
      endColor.w() = colorRange.maximum.a();

      colorRange.set(beginColor, endColor);

      p.setColorRange(colorRange);
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::OnEnteredWorld()
   {
      // NOTE: For now the BaseClass does not define OnEnteredWorld
      // since the old ParticleSystemActor does not extend the
      // base class that defines it. Thus this method override may
      // not be called. The user will have to define a particle system
      // file first then set either of the color properties so that
      // the colors will be applied to the particle system.
      BaseClass::OnEnteredWorld();

      // Ensure that the actor is ticked.
      RegisterForMessages(dtGame::MessageType::TICK_LOCAL,
         dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
      RegisterForMessages(dtGame::MessageType::INFO_GAME_EVENT,
            dtUtil::MakeFunctor(&FireworkActor::ProcessGameEvent, this));

      // Ensure the colors are set to the particle system
      // in case the default values have not changed.
      ApplyColorsToParticleSystem();

      // Prevent the particle system from showing until the
      // detonation time limit has been reached.
      if (mDetonateTimeRemaining > 0.0f)
      {
         dtCore::ParticleSystem* ps = NULL;
         GetDrawable(ps);
         if (ps != NULL)
         {
            ps->SetEnabled(false);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      using namespace dtCore;
      using namespace dtUtil;

      static const dtUtil::RefString GROUP("Firework");

      AddProperty(new dtCore::ColorRgbaActorProperty(
         PROPERTY_COLOR_BEGIN,
         PROPERTY_COLOR_BEGIN,
         ColorRgbaActorProperty::SetFuncType(this, &FireworkActor::SetColorBegin),
         ColorRgbaActorProperty::GetFuncType(this, &FireworkActor::GetColorBegin),
         RefString("Begin color of the firework particle system."),
         GROUP));

      AddProperty(new dtCore::ColorRgbaActorProperty(
         PROPERTY_COLOR_END,
         PROPERTY_COLOR_END,
         ColorRgbaActorProperty::SetFuncType(this, &FireworkActor::SetColorEnd),
         ColorRgbaActorProperty::GetFuncType(this, &FireworkActor::GetColorEnd),
         RefString("End color of the firework particle system."),
         GROUP));

      AddProperty(new dtCore::FloatActorProperty(
         PROPERTY_DETONATE_TIME_LIMIT,
         PROPERTY_DETONATE_TIME_LIMIT,
         FloatActorProperty::SetFuncType(this, &FireworkActor::SetDetonateTimeLimit),
         FloatActorProperty::GetFuncType(this, &FireworkActor::GetDetonateTimeLimit),
         RefString("Time in seconds until the firework will detonate."),
         GROUP));

      AddProperty(new dtCore::StringActorProperty(
         DEFAULT_LAYER_NAME_FLARE,
         DEFAULT_LAYER_NAME_FLARE,
         StringActorProperty::SetFuncType(this, &FireworkActor::SetLayerNameFlare),
         StringActorProperty::GetFuncType(this, &FireworkActor::GetLayerNameFlare),
         RefString("Name of the layer in the particle system that contains the flare effect."),
         GROUP));

      AddProperty(new dtCore::StringActorProperty(
         DEFAULT_LAYER_NAME_SPARKS,
         DEFAULT_LAYER_NAME_SPARKS,
         StringActorProperty::SetFuncType(this, &FireworkActor::SetLayerNameSparks),
         StringActorProperty::GetFuncType(this, &FireworkActor::GetLayerNameSparks),
         RefString("Name of the layer in the particle system that contains the sparks effect."),
         GROUP));

      // GAME EVENT PROPERTIES
      AddProperty(new GameEventActorProperty(*this,
         PROPERTY_EVENT_TO_LAUNCH,
         PROPERTY_EVENT_TO_LAUNCH,
         GameEventActorProperty::SetFuncType(this, &FireworkActor::SetEventToLaunch),
         GameEventActorProperty::GetFuncType(this, &FireworkActor::GetEventToLaunch),
         RefString("Event to launch the firework, starting its detonate timer, light and particle effects."),
         GROUP));

      AddProperty(new GameEventActorProperty(*this,
         PROPERTY_EVENT_TO_DETONATE,
         PROPERTY_EVENT_TO_DETONATE,
         GameEventActorProperty::SetFuncType(this, &FireworkActor::SetEventToDetonate),
         GameEventActorProperty::GetFuncType(this, &FireworkActor::GetEventToDetonate),
         RefString("Event to cause the firework to detonate regardless of the detonate timer."),
         GROUP));
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::BuildActorComponents()
   {
      BaseClass::BuildActorComponents();

      dtCore::RefPtr<LightActorComponent> comp = new LightActorComponent;
      AddComponent(*comp);
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::OnTickLocal(const dtGame::TickMessage& tickMessage)
   {
      BaseClass::OnTickLocal(tickMessage);

      float timeDelta = tickMessage.GetDeltaSimTime();

      dtCore::ParticleSystem* ps = NULL;
      GetDrawable(ps);

      // Update the detonate timer.
      if (mDetonateTimeRemaining > 0.0f)
      {
         mDetonateTimeRemaining -= timeDelta;

         if (mDetonateTimeRemaining <= 0.0f)
         {
            mDetonateTimeRemaining = 0.0f;

            if (ps == NULL)
            {
               LOGN_WARNING("fireworkactor.cpp", "Firework actor \"" + GetName()
                  + "\" has no valid particle system.");
            }
            else
            {
               OnDetonate(*ps);
            }
         }
      }

      // Update the spark fade timer.
      if (mSparkLifeTimeRemaining > 0.0f)
      {
         mSparkLifeTimeRemaining -= timeDelta;

         if (mSparkLifeTimeRemaining <= 0.0f)
         {
            mSparkLifeTimeRemaining = 0.0f;

            if (ps == NULL)
            {
               // Do nothing. Error reported at time of detonation.
            }
            else
            {
               OnCleared(*ps);
            }
         }
      }

      if (ps != NULL)
      {
         UpdateLight(*ps);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::OnLaunch(dtCore::ParticleSystem& ps)
   {
      ps.SetEnabled(true);
      ps.ResetTime();

      dtCore::ParticleLayer* flareLayer = GetParticleLayer(ps, mLayerNameFlare);
      dtCore::ParticleLayer* sparkLayer = GetParticleLayer(ps, mLayerNameSparks);
      
      if (flareLayer != NULL)
      {
         flareLayer->GetModularEmitter().setEnabled(true);
      }

      if (sparkLayer != NULL)
      {
         sparkLayer->GetModularEmitter().setEnabled(false);
      }

      ResetLight();
      
      StartDetonateTimer();
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::OnDetonate(dtCore::ParticleSystem& ps)
   {
      dtCore::ParticleLayer* flareLayer = GetParticleLayer(ps, mLayerNameFlare);
      dtCore::ParticleLayer* sparkLayer = GetParticleLayer(ps, mLayerNameSparks);
      
      if (flareLayer != NULL)
      {
         flareLayer->GetModularEmitter().setEnabled(false);
      }

      if (sparkLayer != NULL)
      {
         sparkLayer->GetModularEmitter().setEnabled(true);
         osgParticle::ParticleSystem& osgPS = sparkLayer->GetParticleSystem();
         osgParticle::Particle& p = osgPS.getDefaultParticleTemplate();

         mSparkLifeTime = p.getLifeTime();
         mSparkLifeTimeRemaining = mSparkLifeTime;
         mDetonateTimeRemaining = 0.0f;
      }


      //turn light on
      LightActorComponent* lac = NULL;
      GetComponent(lac);

      dtRender::DynamicLight* light = lac->GetLight();
      if (light != NULL)
      {
         light->SetIntensity(1.0f);
      }

      //FadeLight(mSparkLifeTime);
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::OnCleared(dtCore::ParticleSystem& ps)
   {
      ps.SetEnabled(false);

      LightActorComponent* lac = NULL;
      GetComponent(lac);

      dtRender::DynamicLight* light = lac->GetLight();
      if (light != NULL)
      {
         light->SetFadeOut(false);
         light->SetIntensity(0.0f);
         light->RemoveFromLightScene();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::UpdateLight(dtCore::ParticleSystem& ps)
   {
      LightActorComponent* lac = NULL;
      GetComponent(lac);

      dtRender::DynamicLight* light = lac->GetLight();

      // Help the light component get the light reference if the
      // light reference is currently missing.
      if (light == NULL)
      {
         light = lac->GetLightActorById(lac->GetLightActorId());
         lac->SetLight(light);
      }

      if (light != NULL)
      {
         osg::Vec4 color = GetCurrentColor(ps);
         osg::Vec3 color3(color.x(), color.y(), color.z());
         light->SetLightColor(color3);
      }
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::ResetLight()
   {
      LightActorComponent* lac = NULL;
      GetComponent(lac);

      dtRender::DynamicLight* light = lac->GetLight();
      if (light != NULL)
      {
         light->SetFadeOut(false);
         
         if (!light->IsInLightScene())
         {
            light->AddToLightScene();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::FadeLight(float fadeTime)
   {
      LightActorComponent* lac = NULL;
      GetComponent(lac);

      dtRender::DynamicLight* light = lac->GetLight();
      if (light != NULL)
      {
         light->SetFadeOut(true);
         light->SetFadeOutTime(fadeTime);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::ProcessGameEvent(const dtGame::GameEventMessage& gameEventMessage)
   {
      const dtCore::GameEvent* gameEvent = gameEventMessage.GetGameEvent();

      if (gameEvent == mEventToLaunch.get())
      {
         OnLaunch();
      }
      else if (gameEvent == mEventToDetonate.get())
      {
         OnDetonate();
      }
   }

}
