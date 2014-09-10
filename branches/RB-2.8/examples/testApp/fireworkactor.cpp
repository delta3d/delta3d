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

   const dtUtil::RefString FireworkActor::PROPERTY_COLOR_BEGIN("ColorBegin");
   const dtUtil::RefString FireworkActor::PROPERTY_COLOR_END("ColorEnd");
   const dtUtil::RefString FireworkActor::PROPERTY_DETONATE_TIME_LIMIT("DetonateTimeLimit");



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   FireworkActor::FireworkActor()
      : BaseClass()
      , mDetonateTimeRemaining(0.0f)
      , mDetonateTimeLimit(DEFAULT_DETONATE_TIME_LIMIT)
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
   void FireworkActor::StartDetonateTimer()
   {
      mDetonateTimeRemaining = mDetonateTimeLimit;
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::ApplyColorsToParticleSystem()
   {
      dtCore::ParticleSystem* ps = NULL;
      GetDrawable(ps);

      if (ps != NULL)
      {
         typedef dtCore::ParticleSystem::LayerList ParticleLayerList;

         ParticleLayerList& layers = ps->GetAllLayers();
         ParticleLayerList::iterator curIter = layers.begin();
         ParticleLayerList::iterator endIter = layers.end();

         for ( ; curIter != endIter; ++curIter)
         {
            ApplyColorsToParticleLayer(*curIter);
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
      // since the old ParticleSystemActorProxy does not extend the
      // base class that defines it. Thus this method override may
      // not be called. The user will have to define a particle system
      // file first then set either of the color properties so that
      // the colors will be applied to the particle system.
      BaseClass::OnEnteredWorld();

      // Ensure that the actor is ticked.
      RegisterForMessages(dtGame::MessageType::TICK_LOCAL,
         dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);

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

      static const dtUtil::RefString GROUP_("Firework");

      AddProperty(new dtCore::ColorRgbaActorProperty(
         PROPERTY_COLOR_BEGIN,
         PROPERTY_COLOR_BEGIN,
         dtCore::ColorRgbaActorProperty::SetFuncType(this, &FireworkActor::SetColorBegin),
         dtCore::ColorRgbaActorProperty::GetFuncType(this, &FireworkActor::GetColorBegin),
         "Begin color of the firework particle system.", GROUP_));

      AddProperty(new dtCore::ColorRgbaActorProperty(
         PROPERTY_COLOR_END,
         PROPERTY_COLOR_END,
         dtCore::ColorRgbaActorProperty::SetFuncType(this, &FireworkActor::SetColorEnd),
         dtCore::ColorRgbaActorProperty::GetFuncType(this, &FireworkActor::GetColorEnd),
         "End color of the firework particle system.", GROUP_));

      AddProperty(new dtCore::FloatActorProperty(
         PROPERTY_DETONATE_TIME_LIMIT,
         PROPERTY_DETONATE_TIME_LIMIT,
         dtCore::FloatActorProperty::SetFuncType(this, &FireworkActor::SetDetonateTimeLimit),
         dtCore::FloatActorProperty::GetFuncType(this, &FireworkActor::GetDetonateTimeLimit),
         "Time in seconds until the firework will detonate.", GROUP_));
   }

   /////////////////////////////////////////////////////////////////////////////
   void FireworkActor::OnTickLocal(const dtGame::TickMessage& tickMessage)
   {
      BaseClass::OnTickLocal(tickMessage);

      float timeDelta = tickMessage.GetDeltaSimTime();

      if (mDetonateTimeRemaining > 0.0f)
      {
         mDetonateTimeRemaining -= timeDelta;

         if (mDetonateTimeRemaining <= 0.0f)
         {
            mDetonateTimeRemaining = 0.0f;

            dtCore::ParticleSystem* ps = NULL;
            GetDrawable(ps);
            if (ps != NULL)
            {
               ps->SetEnabled(true);
               ps->ResetTime();
            }
            else
            {
               LOG_WARNING("Firework actor \"" + GetName()
                  + "\" has no valid particle system.");
            }
         }
      }
   }

}
