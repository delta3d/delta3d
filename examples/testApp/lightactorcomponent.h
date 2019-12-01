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

#ifndef DELTA_TEST_LIGHT_ACTOR_COMPONENT_H
#define DELTA_TEST_LIGHT_ACTOR_COMPONENT_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "export.h"
#include <dtCore/refptr.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/shaderparameter.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/basemessages.h>
#include <dtRender/dynamiclight.h>



namespace dtExample
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class TEST_APP_EXPORT ParamVecElement : public dtUtil::Enumeration
   {
      DECLARE_ENUM(ParamVecElement);
      public:

         static const ParamVecElement W;
         static const ParamVecElement X;
         static const ParamVecElement Y;
         static const ParamVecElement Z;

      protected:
         ParamVecElement(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class TEST_APP_EXPORT LightActorComponent : public dtGame::ActorComponent
   {
   public:
      typedef dtGame::ActorComponent BaseClass;

      static const ACType TYPE;

      static const dtUtil::RefString CLASS_NAME;
      static const dtUtil::RefString PROPERTY_LIGHT;
      static const dtUtil::RefString PROPERTY_ATTACH_LIGHT_TO_OWNER;
      static const dtUtil::RefString PROPERTY_MESH_SHADER_PARAMETER_NAME;
      static const dtUtil::RefString PROPERTY_MESH_SHADER_PARAMETER_TYPE;
      static const dtUtil::RefString PROPERTY_MESH_SHADER_PARAMETER_ELEMENT;
      static const dtUtil::RefString PROPERTY_UDPATES_FROM_LIGHT_ENABLED;

      LightActorComponent(const ACType& type = TYPE);

      dtCore::RefPtr<dtRender::DynamicLight> GetOrCreateLight() const;

      void SetUpdatesFromLightEnabled(bool enable);
      bool IsUpdatesFromLightEnabled() const;

      void SetLight(dtRender::DynamicLight* light);
      dtRender::DynamicLight* GetLight() const;
      
      void SetLightActorId(const dtCore::UniqueId& id);
      const dtCore::UniqueId& GetLightActorId() const;

      // Convenience method for acquiring the direct light reference
      // from the specified actor id.
      dtRender::DynamicLight* GetLightActorById(const dtCore::UniqueId& id);

      void SetLightIntensity(float intensity);
      float GetLightIntensity() const;

      bool GetAttachLightToOwner() const;
      void SetAttachLightToOwner(bool b);

      void SetMeshShaderParameterName(const std::string& name);
      const std::string& GetMeshShaderParameterName() const;

      void SetMeshShaderParameterType(const dtCore::ShaderParameter::ParamType& paramType);
      const dtCore::ShaderParameter::ParamType& GetMeshShaderParameterType() const;
      
      // Method to satify the relevant EnumActorProperty mapping.
      dtCore::ShaderParameter::ParamType& GetMeshShaderParameterType_NonConst() const;

      void SetMeshShaderParameterElement(ParamVecElement& paramType);
      const ParamVecElement& GetMeshShaderParameterElement() const;

      // Method to satify the relevant EnumActorProperty mapping.
      ParamVecElement& GetMeshShaderParameterElement_NonConst() const;

      void SetIntensityToShader(float intensity);

      /*virtual*/ void OnTickLocal(const dtGame::TickMessage& tickMessage);

      /*virtual*/ void OnMapLoaded(const dtGame::MapMessage& mapMessage);

      /*virtual*/ void BuildPropertyMap();

      /*virtual*/ void OnEnteredWorld();

      /*virtual*/ void OnAddedToActor(dtCore::BaseActorObject& actor);

      /*virtual*/ void OnRemovedFromActor(dtCore::BaseActorObject& actor);

      /*virtual*/ void OnRemovedFromWorld();


   protected:
      virtual ~LightActorComponent();
      void DeleteLight();

      bool mAttachLightToOwner;
      bool mCreateLight;
      bool mEnableUpdatesFromLight;
      float mLastLightIntensity;
      dtCore::UniqueId mLightId;
      dtCore::RefPtr<dtRender::DynamicLight> mLight;
      dtCore::ObserverPtr<dtCore::ShaderProgram> mShader;
      dtCore::ObserverPtr<dtCore::ShaderParameter> mShaderParameter;
      std::string mShaderParameterName;
      const dtCore::ShaderParameter::ParamType* mShaderParameterType;
      const ParamVecElement* mShaderParameterElement;
   };

}

#endif /* PROPELLEDVEHICLEACTOR_H_ */
