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
#include <osg/Vec3>



namespace dtExample
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class SurfaceVesselActorComponent : public dtGame::ActorComponent
   {
   public:
      typedef dtGame::ActorComponent BaseClass;

      static const ACType TYPE;

      static const dtUtil::RefString CLASS_NAME;
      static const dtUtil::RefString PROPERTY_SPRAY_ENABLED;
      static const dtUtil::RefString PROPERTY_SPRAY_FRONT_FILE;
      static const dtUtil::RefString PROPERTY_SPRAY_SIDE_LEFT_FILE;
      static const dtUtil::RefString PROPERTY_SPRAY_SIDE_RIGHT_FILE;
      static const dtUtil::RefString PROPERTY_SPRAY_BACK_FILE;
      static const dtUtil::RefString PROPERTY_SPRAY_FRONT_OFFSET;
      static const dtUtil::RefString PROPERTY_SPRAY_SIDE_OFFSET_LEFT;
      static const dtUtil::RefString PROPERTY_SPRAY_SIDE_OFFSET_RIGHT;
      static const dtUtil::RefString PROPERTY_SPRAY_BACK_OFFSET;
      static const dtUtil::RefString PROPERTY_SPRAY_VELOCITY_MIN;
      static const dtUtil::RefString PROPERTY_SPRAY_VELOCITY_MAX;
      static const dtUtil::RefString PROPERTY_SPRAY_SHADER_GROUP;

      SurfaceVesselActorComponent(const ACType& type = TYPE);

      void SetSprayEnabled( bool enable );
      bool IsSprayEnabled();

      void SetSprayShaderGroup(const std::string& shaderGroup);
      const std::string& GetSprayShaderGroup() const;

      void SetSprayFrontFile(const std::string& fileName);
      const std::string& GetSprayFrontFile() const;

      void SetSpraySideLeftFile(const std::string& fileName);
      const std::string& GetSpraySideLeftFile() const;

      void SetSpraySideRightFile(const std::string& fileName);
      const std::string& GetSpraySideRightFile() const;

      void SetSprayBackFile(const std::string& fileName);
      const std::string& GetSprayBackFile() const;
    
      void SetSprayFrontOffset(const osg::Vec3& vec);
      osg::Vec3 GetSprayFrontOffset() const;

      void SetSpraySideOffsetRight(const osg::Vec3& vec);
      osg::Vec3 GetSpraySideOffsetRight() const;

      void SetSpraySideOffsetLeft(const osg::Vec3& vec);
      osg::Vec3 GetSpraySideOffsetLeft() const;

      void SetSprayBackOffset(const osg::Vec3& vec);
      osg::Vec3 GetSprayBackOffset() const;

      void SetSprayVelocityMin(float minVelocity);
      float GetSprayVelocityMin() const;

      void SetSprayVelocityMax(float maxVelocity);
      float GetSprayVelocityMax() const;

      float GetVelocityRatio(float velocity) const;

      void UpdateSpray(float simTimeDelta);

      /*virtual*/ void OnTickLocal(const dtGame::TickMessage& tickMessage);
      /*virtual*/ void OnTickRemote(const dtGame::TickMessage& tickMessage);
      /*virtual*/ void OnMapLoaded(const dtGame::MapMessage& mapMessage);

      /// Adds the properties associated with this actor
      /*virtual*/ void BuildPropertyMap();

      /*virtual*/ void OnEnteredWorld();

   protected:

      /// Destructor
      virtual ~SurfaceVesselActorComponent();

      void CreateSprayEffects();

      void BindShaderToParticleSystem(dtCore::ParticleSystem& particles);
      void BindShaderToNode(osg::Node& node);

      typedef dtActors::DynamicParticleSystem DynamicParticles;
      typedef dtActors::DynamicParticleSystemActor DynamicParticlesActor;
      dtCore::RefPtr<DynamicParticlesActor> CreatDynamicParticleSystemActor(const std::string& filename,
         const std::string& actorName);
      DynamicParticles* GetDynamicParticles(DynamicParticlesActor* actor);

   private:
      bool mSprayEnabled;
      float mLastSprayRatio;
      float mSprayVelocityMin;
      float mSprayVelocityMax;
      float mSprayUpdateTimer;

      osg::Vec3 mSprayFrontOffset;
      osg::Vec3 mSpraySideOffsetRight;
      osg::Vec3 mSpraySideOffsetLeft;
      osg::Vec3 mSprayBackOffset;

      osg::Vec3 mLastPos;

      std::string mParticleShaderGroup;
      std::string mFileSprayFront;
      std::string mFileSpraySideLeft;
      std::string mFileSpraySideRight;
      std::string mFileSprayBack;

      dtCore::RefPtr<DynamicParticlesActor> mSprayFrontActor;
      dtCore::RefPtr<DynamicParticlesActor> mSpraySideRightActor;
      dtCore::RefPtr<DynamicParticlesActor> mSpraySideLeftActor;
      dtCore::RefPtr<DynamicParticlesActor> mSprayBackActor;

      dtCore::RefPtr<DynamicParticles> mSprayFront;
      dtCore::RefPtr<DynamicParticles> mSpraySideRight;
      dtCore::RefPtr<DynamicParticles> mSpraySideLeft;
      dtCore::RefPtr<DynamicParticles> mSprayBack;

      dtCore::ObserverPtr<dtCore::Transformable> mOwnerDrawable;
   };
}

#endif
