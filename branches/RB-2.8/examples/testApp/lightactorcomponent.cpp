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
#include "lightactorcomponent.h"
#include <dtCore/actoractorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shaderparamfloat.h>
#include <dtCore/shaderparamvec4.h>
#include <dtGame/gameactor.h>
#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtCore/propertymacros.h>



namespace dtExample
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ParamVecElement)
   const ParamVecElement ParamVecElement::W("w");
   const ParamVecElement ParamVecElement::X("x");
   const ParamVecElement ParamVecElement::Y("y");
   const ParamVecElement ParamVecElement::Z("z");



   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   const dtGame::ActorComponent::ACType LightActorComponent::TYPE( new dtCore::ActorType("Light", "ActorComponents", "", dtGame::ActorComponent::BaseActorComponentType));
   const dtUtil::RefString LightActorComponent::CLASS_NAME("dtExample.LightActorComponent");
   const dtUtil::RefString LightActorComponent::PROPERTY_LIGHT("Light");
   const dtUtil::RefString LightActorComponent::PROPERTY_MESH_SHADER_PARAMETER_NAME("Mesh Shader Parameter Name");
   const dtUtil::RefString LightActorComponent::PROPERTY_MESH_SHADER_PARAMETER_TYPE("Mesh Shader Parameter Type");
   const dtUtil::RefString LightActorComponent::PROPERTY_MESH_SHADER_PARAMETER_ELEMENT("Mesh Shader Parameter Element");
   const dtUtil::RefString LightActorComponent::PROPERTY_UDPATES_FROM_LIGHT_ENABLED("Updates From Light Enabled");
   


   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   LightActorComponent::LightActorComponent(const ACType& type)
      : dtGame::ActorComponent(type)
      , mCreateLight(false)
      , mEnableUpdatesFromLight(false)
      , mLastLightIntensity(0.0f)
      , mShaderParameterType(&dtCore::ShaderParameter::ParamType::FLOAT)
      , mShaderParameterElement(&ParamVecElement::X)
   {
      SetClassName(CLASS_NAME);
   }

   /////////////////////////////////////////////////////////////////////////////
   LightActorComponent::~LightActorComponent()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtRender::DynamicLight> LightActorComponent::GetOrCreateLight() const
   {
      dtCore::RefPtr<dtRender::DynamicLight> light = mLight.get();

      if ( ! light.valid())
      {
         light = new dtRender::DynamicLight(
            dtRender::DynamicLight::LightType::OMNI_DIRECTIONAL);
      }

      return light;
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::SetUpdatesFromLightEnabled(bool enabled)
   {
      mEnableUpdatesFromLight = enabled;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool LightActorComponent::IsUpdatesFromLightEnabled() const
   {
      return mEnableUpdatesFromLight;
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::SetLight(dtRender::DynamicLight* light)
   {
      mLight = light;

      // Get the id.
      if (mLight.valid() && mLight->GameActorProxy::GetId().ToString() != mLightId.ToString())
      {
         mLightId = mLight->GameActorProxy::GetId();
      }

      // Ensure the light targets the owner actor.
      if (light != NULL && light->GetInitialOwnership() == dtGame::GameActorProxy::Ownership::PROTOTYPE)
      {
         dtGame::GameActorProxy* actor = NULL;
         GetOwner(actor);

         dtCore::Transformable* drawable = NULL;
         actor->GetDrawable(drawable);

         if (drawable != NULL)
         {
            light->SetTarget(*drawable);
         }
         else
         {
            LOG_WARNING("Could not set target on light since it is not a Transformable, for actor: " + GetName());
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   dtRender::DynamicLight* LightActorComponent::GetLight() const
   {
      return mLight.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::SetLightActorId(const dtCore::UniqueId& id)
   {
      if (mLightId != id)
      {
         mLightId = id;

         SetLight(GetLightActorById(id));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& LightActorComponent::GetLightActorId() const
   {
      return mLightId;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtRender::DynamicLight* LightActorComponent::GetLightActorById(const dtCore::UniqueId& id)
   {
      dtRender::DynamicLight* light = NULL;

      dtGame::GameActorProxy* actor = NULL;
      GetOwner(actor);

      if (actor != NULL)
      {
         dtGame::GameManager* gm = actor->GetGameManager();
         if (gm != NULL)
         {
            gm->FindActorById(id, light);

            if (light == NULL)
            {
               dtCore::RefPtr<dtRender::DynamicLight> ptr;
               gm->FindPrototypeByID(id, ptr);
               light = ptr.get();
            }
         }
      }

      return light;
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::SetLightIntensity(float intensity)
   {
      if (mLight.valid())
      {
         mLight->SetIntensity(intensity);

         SetIntensityToShader(intensity);

         mLastLightIntensity = intensity;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   float LightActorComponent::GetLightIntensity() const
   {
      return mLight.valid() ? mLight->GetIntensity() : 0.0f;
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::SetMeshShaderParameterName(const std::string& name)
   {
      mShaderParameterName = name;
   }
   
   /////////////////////////////////////////////////////////////////////////////
   const std::string& LightActorComponent::GetMeshShaderParameterName() const
   {
      return mShaderParameterName;
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::SetMeshShaderParameterType(
      const dtCore::ShaderParameter::ParamType& paramType)
   {
      mShaderParameterType = &paramType;
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::ShaderParameter::ParamType& LightActorComponent::GetMeshShaderParameterType() const
   {
      return *mShaderParameterType;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::ShaderParameter::ParamType& LightActorComponent::GetMeshShaderParameterType_NonConst() const
   {
      return *const_cast<dtCore::ShaderParameter::ParamType*>(mShaderParameterType);
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::SetMeshShaderParameterElement(ParamVecElement& paramType)
   {
      mShaderParameterElement = &paramType;
   }

   /////////////////////////////////////////////////////////////////////////////
   const ParamVecElement& LightActorComponent::GetMeshShaderParameterElement() const
   {
      return *mShaderParameterElement;
   }

   /////////////////////////////////////////////////////////////////////////////
   ParamVecElement& LightActorComponent::GetMeshShaderParameterElement_NonConst() const
   {
      return *const_cast<ParamVecElement*>(mShaderParameterElement);
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      using namespace dtCore;
      using namespace dtUtil;

      static const dtUtil::RefString GROUP("Light");
      static const dtUtil::RefString EMPTY;

      dtCore::RefPtr<dtCore::ActorIDActorProperty> idProp = new dtCore::ActorIDActorProperty(
         PROPERTY_LIGHT,
         PROPERTY_LIGHT,
         ActorIDActorProperty::SetFuncType(this, &LightActorComponent::SetLightActorId),
         ActorIDActorProperty::GetFuncType(this, &LightActorComponent::GetLightActorId),
         RefString("dtRender::DynamicLight"),
         RefString("Dynamic light actor to which to link and control."),
         GROUP);
      idProp->SetShowPrototypes(true);
      AddProperty(idProp.get());

      AddProperty(new dtCore::StringActorProperty( 
         PROPERTY_MESH_SHADER_PARAMETER_NAME,
         PROPERTY_MESH_SHADER_PARAMETER_NAME,
         StringActorProperty::SetFuncType(this,&LightActorComponent::SetMeshShaderParameterName),
         StringActorProperty::GetFuncType(this,&LightActorComponent::GetMeshShaderParameterName),
         RefString("Name of a parameter that controls the illumination effect on geometry, if available."),
         GROUP));

      // ENUM PROPERTIES
      typedef EnumActorProperty<ShaderParameter::ParamType> ParamTypeEnumProp;
      AddProperty(new ParamTypeEnumProp(
         PROPERTY_MESH_SHADER_PARAMETER_TYPE,
         PROPERTY_MESH_SHADER_PARAMETER_TYPE,
         ParamTypeEnumProp::SetFuncType(this,&LightActorComponent::SetMeshShaderParameterType),
         ParamTypeEnumProp::GetFuncType(this,&LightActorComponent::GetMeshShaderParameterType_NonConst),
         RefString("Shader parameter type of the specified Mesh Shader Parameter."),
         GROUP));

      typedef EnumActorProperty<dtExample::ParamVecElement> ParamElementEnumProp;
      AddProperty(new ParamElementEnumProp(
         PROPERTY_MESH_SHADER_PARAMETER_ELEMENT,
         PROPERTY_MESH_SHADER_PARAMETER_ELEMENT,
         ParamElementEnumProp::SetFuncType(this,&LightActorComponent::SetMeshShaderParameterElement),
         ParamElementEnumProp::GetFuncType(this,&LightActorComponent::GetMeshShaderParameterElement_NonConst),
         RefString("Shader parameter element (X, Y, Z or W) to be used for setting intensity if shader parameter is a vec type."),
         GROUP));

      // BOOLEAN PROPERTIES
      AddProperty(new dtCore::BooleanActorProperty( 
         PROPERTY_UDPATES_FROM_LIGHT_ENABLED,
         PROPERTY_UDPATES_FROM_LIGHT_ENABLED,
         BooleanActorProperty::SetFuncType(this,&LightActorComponent::SetUpdatesFromLightEnabled),
         BooleanActorProperty::GetFuncType(this,&LightActorComponent::IsUpdatesFromLightEnabled),
         RefString("Enables the actor to update its shader by using values from the referenced light actor."),
         GROUP));
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();

      // Light setup should happen after the map has finished loading
      // so that out-of-order light references can be solved.
      RegisterForMapLoaded();
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::OnAddedToActor(dtCore::BaseActorObject& actor)
   {
      SetName(actor.GetName()+"_Light");
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::OnRemovedFromActor(dtCore::BaseActorObject& actor)
   {
      mLight = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::SetIntensityToShader(float intensity)
   {
      if ( ! mShader.valid() && ! mShaderParameterName.empty())
      {
         dtCore::BaseActorObject* actor = NULL;
         GetOwner(actor);

         if (actor != NULL)
         {
            dtCore::DeltaDrawable* drawable = actor->GetDrawable();
            if (drawable != NULL)
            {
               mShader = dtCore::ShaderManager::GetInstance().GetShaderInstanceForDrawable(*drawable);
               mShaderParameter = mShader->FindParameter(mShaderParameterName);
            }
         }
      }

      if (mShaderParameter.valid())
      {
         typedef dtCore::ShaderParameter::ParamType ParamType;

         if (mShaderParameterType == &ParamType::FLOAT)
         {
            dtCore::ShaderParamFloat* param = dynamic_cast<dtCore::ShaderParamFloat*>(mShaderParameter.get());
            if (param != NULL)
            {
               param->SetValue(intensity);
            }
         }
         else if (mShaderParameterType == &ParamType::FLOAT_VEC4)
         {
            dtCore::ShaderParamVec4* param = dynamic_cast<dtCore::ShaderParamVec4*>(mShaderParameter.get());
            osg::Vec4 value = param->GetValue();

            if (mShaderParameterElement == &ParamVecElement::X)
            {
               value.x() = intensity;
            }
            else if (mShaderParameterElement == &ParamVecElement::Y)
            {
               value.y() = intensity;
            }
            else if (mShaderParameterElement == &ParamVecElement::Z)
            {
               value.z() = intensity;
            }
            else if (mShaderParameterElement == &ParamVecElement::W)
            {
               value.w() = intensity;
            }

            if (param != NULL)
            {
               param->SetValue(value);
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void LightActorComponent::OnTickLocal(const dtGame::TickMessage& tickMessage)
   {
      if(!mLight.valid() && !mCreateLight)
      {
         dtRender::DynamicLight* light = GetLightActorById(mLightId);

         if(light != NULL)
         {
            SetLight(light);
         }

      }

      if (mLight.valid())
      {
         float intensity = mLight->GetIntensity();
         bool intesityChange = mLastLightIntensity != intensity;
         mLastLightIntensity = intensity;

         if (intesityChange)
         {
            SetIntensityToShader(intensity);
         }
      }
   }

   void LightActorComponent::OnMapLoaded(const dtGame::MapMessage& mapMessage)
   {
      BaseClass::OnMapLoaded(mapMessage);

      if ( ! mLight.valid())
      {
         // Use a refptr here in case a new light is created.
         dtCore::RefPtr<dtRender::DynamicLight> light = NULL;
         
         /*if (mCreateLight)
         {
            light = GetOrCreateLight();
         }
         else
         {*/
            light = GetLightActorById(mLightId);
         //}

         if ( ! light.valid())
         {
            if (mCreateLight)
            {
               LOG_ERROR("Could not create light for actor: " + GetName());
            }
            else
            {      
               LOG_ERROR("Could not access light \"" + mLightId.ToString()
                  + "\" for actor: " + GetName());
            }
         }
         else
         {
            // Determine if the referenced actor is a prototype.
            if (light.valid() && light->GetInitialOwnership() == dtGame::GameActorProxy::Ownership::PROTOTYPE)
            {
               dtGame::GameActorProxy* actor = NULL;
               GetOwner(actor);

               if (actor->GetInitialOwnership() != dtGame::GameActorProxy::Ownership::PROTOTYPE)
               {
                  dtGame::GameManager* gm = actor->GetGameManager();
                  gm->CreateActorFromPrototype(light->GameActorProxy::GetId(), light);

                  gm->AddActor(*light, false, false);
               }
            }

            // Ensure variables related to the light are updated accordingly.
            SetLight(light);
         }
      }

      // Ensure the initial set intensity is applied.
      if (mLastLightIntensity != 0.0f)
      {
         SetLightIntensity(mLastLightIntensity);
      }
      else if (mLight.valid()) // Use the intensity from the referenced light.
      {
         SetLightIntensity(mLight->GetIntensity());
      }

      if (mEnableUpdatesFromLight)
      {
         RegisterForTick();
      }
   }

}

