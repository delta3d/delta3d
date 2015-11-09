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

#ifndef DELTA_TEST_APP_FIREWORK_ACTOR_H
#define DELTA_TEST_APP_FIREWORK_ACTOR_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtActors/particlesystemactorproxy.h>
#include <dtGame/basemessages.h>
#include <osg/Vec4>



namespace dtExample
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class FireworkActor : public dtActors::ParticleSystemActor
   {
   public:

      typedef dtActors::ParticleSystemActor BaseClass;

      static const float DEFAULT_DETONATE_TIME_LIMIT;
      static const dtUtil::RefString DEFAULT_LAYER_NAME_FLARE;
      static const dtUtil::RefString DEFAULT_LAYER_NAME_SPARKS;

      static const dtUtil::RefString PROPERTY_COLOR_BEGIN;
      static const dtUtil::RefString PROPERTY_COLOR_END;
      static const dtUtil::RefString PROPERTY_LAYER_NAME_FLARE;
      static const dtUtil::RefString PROPERTY_LAYER_NAME_SPARKS;
      static const dtUtil::RefString PROPERTY_DETONATE_TIME_LIMIT;
      static const dtUtil::RefString PROPERTY_EVENT_TO_LAUNCH;
      static const dtUtil::RefString PROPERTY_EVENT_TO_DETONATE;

      FireworkActor();

      void SetColorBegin(const osg::Vec4& color);
      osg::Vec4 GetColorBegin() const;

      void SetColorEnd(const osg::Vec4& color);
      osg::Vec4 GetColorEnd() const;

      void SetDetonateTimeLimit(float seconds);
      float GetDetonateTimeLimit() const;

      float GetDetonateTimeRemaining() const;

      dtCore::ParticleLayer* GetParticleLayer(dtCore::ParticleSystem& ps, const std::string& layerName) const;
      dtCore::ParticleLayer* GetParticleLayer(const std::string& layerName);

      void SetLayerNameFlare(const std::string& name);
      const std::string& GetLayerNameFlare() const;

      void SetLayerNameSparks(const std::string& name);
      const std::string& GetLayerNameSparks() const;

      osg::Vec4 GetCurrentColor(dtCore::ParticleSystem& ps) const;

      void SetEventToLaunch(dtCore::GameEvent* gameEvent);
      dtCore::GameEvent* GetEventToLaunch() const;

      void SetEventToDetonate(dtCore::GameEvent* gameEvent);
      dtCore::GameEvent* GetEventToDetonate() const;

      virtual void OnLaunch();
      virtual void OnDetonate();

      /*virtual*/ void OnEnteredWorld();

      /*virtual*/ void BuildPropertyMap();

      /*virtual*/ void BuildActorComponents();
   
      /*virtual*/ void OnTickLocal(const dtGame::TickMessage& tickMessage);

      void ProcessGameEvent(const dtGame::GameEventMessage& gameEventMessage);

   protected:
      virtual ~FireworkActor();

      void StartDetonateTimer();

      virtual void OnLaunch(dtCore::ParticleSystem& ps);
      virtual void OnDetonate(dtCore::ParticleSystem& ps);
      virtual void OnCleared(dtCore::ParticleSystem& ps);

      void UpdateLight(dtCore::ParticleSystem& ps);
      void ResetLight();
      void FadeLight(float fadeTime);

      void ApplyColorsToParticleSystem();
      void ApplyColorsToParticleLayer(dtCore::ParticleLayer& layer);

      float mSparkLifeTime;
      float mSparkLifeTimeRemaining;
      float mDetonateTimeRemaining;
      float mDetonateTimeLimit;
      osg::Vec4 mColorBegin;
      osg::Vec4 mColorEnd;
      std::string mLayerNameFlare;
      std::string mLayerNameSparks;

      dtCore::RefPtr<dtCore::GameEvent> mEventToLaunch;
      dtCore::RefPtr<dtCore::GameEvent> mEventToDetonate;
   };

}

#endif
