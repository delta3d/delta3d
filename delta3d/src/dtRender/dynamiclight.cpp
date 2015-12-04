/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2014, Caper Holdings, LLC
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
*
* Bradley Anderegg
*/

#include <dtRender/dynamiclight.h>
#include <dtRender/scenemanager.h>
#include <dtRender/lightscene.h>

#include <dtCore/enginepropertytypes.h>
#include <dtGame/gameactor.h>
#include <dtGame/gamemanager.h>
#include <dtCore/functor.h> // deprecated

namespace dtRender
{      

      OpenThreads::Atomic DynamicLight::mLightCounter(1U);

      //////////////////////////////////////////////////////////////////////////
      IMPLEMENT_ENUM(DynamicLight::LightType)
      DynamicLight::LightType DynamicLight::LightType::OMNI_DIRECTIONAL("OMNI_DIRECTIONAL");
      DynamicLight::LightType DynamicLight::LightType::SPOT_LIGHT("SPOT_LIGHT");


      //////////////////////////////////////////////////////////
      DynamicLight::DynamicLight()
         : mAttenuation(osg::Vec3(0.1f, 0.005f, 0.00002f))
         , mLightColor(osg::Vec3(1.0f, 1.0f, 1.0f))
         , mFlicker(false)
         , mFlickerScale(0.0f)
         , mDeleteAfterMaxTime(false)
         , mMaxTime(0.0f)
         , mFadeOut(false)
         , mFadeOutTime(1.0f)
         , mRadius(100.0f)
         , mDeleteOnTargetIsNull(true)
         , mLightType(&DynamicLight::LightType::OMNI_DIRECTIONAL)
         , mDeleteMe(false)
         , mLightPosition(0.0f, 0.0f, 0.0f)
         , mIntensity(1.0f)
         , mIntensityMod(1.0f)
         , mId(++mLightCounter)
      {
         SetName("DynamicLight");
         SetClassName("dtRender::DynamicLight");
      }

      //////////////////////////////////////////////////////////
      DynamicLight::DynamicLight(LightType& lt)
         : mAttenuation(osg::Vec3(0.1f, 0.005f, 0.00002f))
         , mLightColor(osg::Vec3(1.0f, 1.0f, 1.0f))
         , mFlicker(false)
         , mFlickerScale(0.0f)
         , mDeleteAfterMaxTime(false)
         , mMaxTime(0.0f)
         , mFadeOut(false)
         , mFadeOutTime(1.0f)
         , mRadius(100.0f)
         , mDeleteOnTargetIsNull(true)
         , mLightType(&lt)
         , mDeleteMe(false)
         , mLightPosition(0.0f, 0.0f, 0.0f)
         , mIntensity(1.0f)
         , mIntensityMod(1.0f)
         , mId(++mLightCounter)
      {
         SetName("DynamicLight");
         SetClassName("dtRender::DynamicLight");
      }


      //////////////////////////////////////////////////////////
      DynamicLight::~DynamicLight()
      {

      }

      //////////////////////////////////////////////////////////
      void DynamicLight::CreateDrawable()
      {
         dtCore::RefPtr<dtGame::GameActor> ga = new dtGame::GameActor(*this);
         SetDrawable(*ga);
         SetTarget(*GetDrawable<dtCore::Transformable>());
      }

      //////////////////////////////////////////////////////////
      void DynamicLight::BuildPropertyMap()
      {
         dtGame::GameActorProxy::BuildPropertyMap();

         static const dtUtil::RefString LIGHT_GROUP("Dynamic Light");

         static const dtUtil::RefString PROPERTY_ATTENUATION("Attenuation");
         static const dtUtil::RefString PROPERTY_ATTENUATION_DESC("Controls how far the light is visible. The vec3 represents constant(X), linear(Y), quadratic(Z) attenuations");
         AddProperty(new dtCore::Vec3ActorProperty(PROPERTY_ATTENUATION, PROPERTY_ATTENUATION,
            dtCore::Vec3ActorProperty::SetFuncType(this, &DynamicLight::SetAttenuation),
            dtCore::Vec3ActorProperty::GetFuncType(this, &DynamicLight::GetAttenuation),
            PROPERTY_ATTENUATION_DESC, LIGHT_GROUP));

         static const dtUtil::RefString PROPERTY_LIGHTCOLOR("Light Color");
         static const dtUtil::RefString PROPERTY_LIGHTCOLOR_DESC("The base color of this light");
         AddProperty(new dtCore::Vec3ActorProperty(PROPERTY_LIGHTCOLOR, PROPERTY_LIGHTCOLOR,
            dtCore::Vec3ActorProperty::SetFuncType(this, &DynamicLight::SetLightColor),
            dtCore::Vec3ActorProperty::GetFuncType(this, &DynamicLight::GetLightColor),
            PROPERTY_LIGHTCOLOR_DESC, LIGHT_GROUP));

         static const dtUtil::RefString PROPERTY_INTENSITY("Intensity");
         static const dtUtil::RefString PROPERTY_INTENSITY_DESC("How long the light should take to fade out (in seconds). This occurs AFTER Max Time. 0 means no fade out.");
         AddProperty(new dtCore::FloatActorProperty(PROPERTY_INTENSITY, PROPERTY_INTENSITY, 
            dtCore::FloatActorProperty::SetFuncType(this, &DynamicLight::SetIntensity),
            dtCore::FloatActorProperty::GetFuncType(this, &DynamicLight::GetIntensity),
            PROPERTY_INTENSITY_DESC, LIGHT_GROUP));

         static const dtUtil::RefString PROPERTY_FLICKERSCALE("Flicker Scale");
         static const dtUtil::RefString PROPERTY_FLICKERSCALE_DESC("Indicates if the light should flicker and how much. (value is the max range of variation - ex 0.1 to 0.4). 0.0 means no flicker.");
         AddProperty(new dtCore::FloatActorProperty(PROPERTY_FLICKERSCALE, PROPERTY_FLICKERSCALE, 
            dtCore::FloatActorProperty::SetFuncType(this, &DynamicLight::SetFlickerScale),
            dtCore::FloatActorProperty::GetFuncType(this, &DynamicLight::GetFlickerScale),
            PROPERTY_FLICKERSCALE_DESC, LIGHT_GROUP));

         static const dtUtil::RefString PROPERTY_MAXIMUMTIME("Maximum Time");
         static const dtUtil::RefString PROPERTY_MAXIMUMTIME_DESC("The maximum time (in seconds) before this light will begin to fade out (see Fade Out Time). Use 0 to specify no maximum time");
         AddProperty(new dtCore::FloatActorProperty(PROPERTY_MAXIMUMTIME, PROPERTY_MAXIMUMTIME, 
            dtCore::FloatActorProperty::SetFuncType(this, &DynamicLight::SetMaxTime),
            dtCore::FloatActorProperty::GetFuncType(this, &DynamicLight::GetMaxTime),
            PROPERTY_MAXIMUMTIME_DESC, LIGHT_GROUP));

         static const dtUtil::RefString PROPERTY_FADEOUTTIME("Fade Out Time");
         static const dtUtil::RefString PROPERTY_FADEOUTTIME_DESC("How long the light should take to fade out (in seconds). This occurs AFTER Max Time. 0 means no fade out.");
         AddProperty(new dtCore::FloatActorProperty(PROPERTY_FADEOUTTIME, PROPERTY_FADEOUTTIME, 
            dtCore::FloatActorProperty::SetFuncType(this, &DynamicLight::SetFadeOutTime),
            dtCore::FloatActorProperty::GetFuncType(this, &DynamicLight::GetFadeOutTime),
            PROPERTY_FADEOUTTIME_DESC, LIGHT_GROUP));

         static const dtUtil::RefString PROPERTY_RADIUS("Radius");
         static const dtUtil::RefString PROPERTY_RADIUS_DESC("The distance the light should affect (currently used only for light priority, not for rendering) ");
         AddProperty(new dtCore::FloatActorProperty(PROPERTY_RADIUS, PROPERTY_RADIUS, 
            dtCore::FloatActorProperty::SetFuncType(this, &DynamicLight::SetRadius),
            dtCore::FloatActorProperty::GetFuncType(this, &DynamicLight::GetRadius),
            PROPERTY_RADIUS_DESC, LIGHT_GROUP));

         static const dtUtil::RefString PROPERTY_DELETEONNULL("Delete When Target Is Null");
         static const dtUtil::RefString PROPERTY_DELETEONNULL_DESC("Indicates to delete the light if the target ever becomes NULL.");
         AddProperty(new dtCore::BooleanActorProperty(PROPERTY_DELETEONNULL, PROPERTY_DELETEONNULL,
            dtCore::BooleanActorProperty::SetFuncType(this, &DynamicLight::SetDeleteOnTargetIsNull),
            dtCore::BooleanActorProperty::GetFuncType(this, &DynamicLight::GetDeleteOnTargetIsNull),
            PROPERTY_DELETEONNULL_DESC, LIGHT_GROUP));

      }

      dtCore::Transformable* DynamicLight::GetTarget()
      {
         return mTarget.get();
      }

      const dtCore::Transformable* DynamicLight::GetTarget() const
      {
         return mTarget.get();
      }

      void DynamicLight::SetTarget( dtCore::Transformable& t)
      {
         mTarget = &t;
      }

      DT_IMPLEMENT_ACCESSOR_GETTER(DynamicLight, float, FadeOutTime)
      DT_IMPLEMENT_ACCESSOR_GETTER(DynamicLight, float, FlickerScale)
      DT_IMPLEMENT_ACCESSOR_GETTER(DynamicLight, float, MaxTime)

      void DynamicLight::SetFadeOutTime(float f)
      {
         mFadeOutTime = f;

         if(mFadeOutTime > 0.0)
         {
            mFadeOut = true;
         }
         else
         {
            mFadeOut = false;
         }
      }

      void DynamicLight::SetFlickerScale(float f)
      {
         mFlickerScale = f;

         if(mFlickerScale > 0.0)
         {
            mFlicker = true;
         }
         else
         {
            mFlicker = false;
         }
      }

      void DynamicLight::SetMaxTime(float f)
      {
         mMaxTime = f;

         if(mMaxTime > 0.0)
         {
            mDeleteAfterMaxTime = true;
         }
         else
         {
            mDeleteAfterMaxTime = false;
         }
      }

      void DynamicLight::OnEnteredWorld()
      {
         dtGame::GameManager* gm = GetGameManager();
         if(gm != NULL)
         {
            dtGame::IEnvGameActorProxy* env = gm->GetEnvironmentActor();
            SceneManagerActor* smp = dynamic_cast<SceneManagerActor*>(env);
            if(smp != NULL)
            {
               SceneManager* sm = dynamic_cast<SceneManager*>(smp->GetDrawable());

               LightScene* ls = dynamic_cast<LightScene*>(sm->FindSceneByType(*LightScene::LIGHT_SCENE));
               if(ls != NULL)
               {
                  ls->AddDynamicLight(this);
               }
            }
         }
      }

      void DynamicLight::OnRemovedFromWorld()
      {
         dtGame::GameManager* gm = GetGameManager();
         if (gm != NULL)
         {
            dtGame::IEnvGameActorProxy* env = gm->GetEnvironmentActor();
            SceneManagerActor* smp = dynamic_cast<SceneManagerActor*>(env);
            if (smp != NULL)
            {
               SceneManager* sm = dynamic_cast<SceneManager*>(smp->GetDrawable());

               LightScene* ls = dynamic_cast<LightScene*>(sm->FindSceneByType(*LightScene::LIGHT_SCENE));
               if (ls != NULL)
               {
                  ls->RemoveDynamicLight(GetLightId());
               }
            }
         }
      }

      //////////////////////////////////////////////////////////
      // Proxy code
      //////////////////////////////////////////////////////////
      SpotLight::SpotLight( )
         : DynamicLight(LightType::SPOT_LIGHT)
         , mUseAbsoluteDirection(false)
         , mSpotExponent(0.5f)
         , mSpotCosCutoff(0.75f)
         , mDirection(0.0f, 1.0f, 0.0f)
         , mCurrentDirection(0.0f, 1.0f, 0.0f)
      {
         SetName("SpotLight");      
         SetClassName("dtRender::SpotLight");
      }

      //////////////////////////////////////////////////////////
      SpotLight::~SpotLight()
      {

      }

      void SpotLight::CreateDrawable()
      {
         SetDrawable(*new dtGame::GameActor(*this));
         SetTarget(*GetDrawable<dtCore::Transformable>());
      }

      //////////////////////////////////////////////////////////
      void SpotLight::BuildPropertyMap()
      {
         DynamicLight::BuildPropertyMap();

         static const dtUtil::RefString PROPERTY_USE_ABSOLUTE_DIRECTION("UseAbsoluteDirection");
         AddProperty(new dtCore::BooleanActorProperty(PROPERTY_USE_ABSOLUTE_DIRECTION, PROPERTY_USE_ABSOLUTE_DIRECTION,
            dtCore::BooleanActorProperty::SetFuncType(this, &SpotLight::SetUseAbsoluteDirection),
            dtCore::BooleanActorProperty::GetFuncType(this, &SpotLight::GetUseAbsoluteDirection),
            "use this flag if this light is attached to a transformable but you do not want to accumulate its parents rotation", "SpotLight"));

         static const dtUtil::RefString PROPERTY_SPOT_EXPONENT("SpotExponent");
         AddProperty(new dtCore::FloatActorProperty(PROPERTY_SPOT_EXPONENT, PROPERTY_SPOT_EXPONENT, 
            dtCore::FloatActorProperty::SetFuncType(this, &SpotLight::SetSpotExponent),
            dtCore::FloatActorProperty::GetFuncType(this, &SpotLight::GetSpotExponent),
            "SpotExponent is the spot rate of decay and controls how the lights intensity decays from the center of the cone it its borders. The larger the value the faster de decay, with zero meaning constant light within the light cone.",
            "SpotLight"));

         static const dtUtil::RefString PROPERTY_SPOT_COS_CUTOFF("SpotCosCutoff");
         AddProperty(new dtCore::FloatActorProperty(PROPERTY_SPOT_COS_CUTOFF, PROPERTY_SPOT_COS_CUTOFF, 
            dtCore::FloatActorProperty::SetFuncType(this, &SpotLight::SetSpotCosCutoff),
            dtCore::FloatActorProperty::GetFuncType(this, &SpotLight::GetSpotCosCutoff),
            "The cosine of the angle between the light to vertex vector and the spot direction must be larger than spotCosCutoff",
            "SpotLight"));


         static const dtUtil::RefString PROPERTY_SPOTDIRECTION("SpotDirection");
         AddProperty(new dtCore::Vec3ActorProperty(PROPERTY_SPOTDIRECTION, PROPERTY_SPOTDIRECTION,
            dtCore::Vec3ActorProperty::SetFuncType(this, &SpotLight::SetDirection),
            dtCore::Vec3ActorProperty::GetFuncType(this, &SpotLight::GetDirection),
            "The direction of the light", "SpotLight"));

      }


}
