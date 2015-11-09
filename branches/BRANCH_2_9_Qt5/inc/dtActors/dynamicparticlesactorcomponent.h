/* -*-c++-*-
 * Copyright (C) 2015, Caper Holdings LLC
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
 */

#ifndef DELTA_DYNAMIC_PARTICLES_ACTOR_COMPONENT_H
#define DELTA_DYNAMIC_PARTICLES_ACTOR_COMPONENT_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtCore/plugin_export.h>
#include <dtActors/dynamicparticlesystemactor.h>
#include <dtCore/particlesystem.h>
#include <dtCore/refptr.h>
#include <dtGame/actorcomponent.h>
#include <dtCore/gameevent.h>
#include <osg/Vec3>


namespace dtGame
{
   class GameEventMessage;
}


namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS - for clearer reading.
   /////////////////////////////////////////////////////////////////////////////
   typedef dtActors::DynamicParticleSystem DynamicParticles;
   typedef dtActors::DynamicParticleSystemActor DynamicParticlesActor;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT DynamicParticlesPropertyContainer : public dtCore::PropertyContainer
   {
   public:
      typedef dtCore::PropertyContainer BaseClass;

      static const dtCore::RefPtr<dtCore::ObjectType> DYNAMIC_PARTICLES_PC_TYPE;

      DynamicParticlesPropertyContainer();

      /*override*/ const dtCore::ObjectType& GetObjectType() const;

      DynamicParticles* GetDynamicParticles() const;

      DynamicParticlesActor* GetDynamicParticlesActor() const;

      DT_DECLARE_ACCESSOR(std::string, Name);
      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, ParticleFile);
      DT_DECLARE_ACCESSOR(bool, Enabled);
      DT_DECLARE_ACCESSOR(bool, UpdateInterpolationEnabled);
      DT_DECLARE_ACCESSOR(float, UpdateFrequency);
      DT_DECLARE_ACCESSOR(float, InitialInterpolation);
      DT_DECLARE_ACCESSOR(float, OwnerSpeedMin);
      DT_DECLARE_ACCESSOR(float, OwnerSpeedMax);
      DT_DECLARE_ACCESSOR(bool, RelativeToParent);
      DT_DECLARE_ACCESSOR(bool, AttachDirectly);
      DT_DECLARE_ACCESSOR(std::string, AttachNodeName);
      DT_DECLARE_ACCESSOR(std::string, ShaderGroup);
      DT_DECLARE_ACCESSOR(osg::Vec3, Offset);
      DT_DECLARE_ACCESSOR(osg::Vec3, OffsetRotation);

      float GetSpeedRatio(float speed) const;

      void InterpolateParticleSystem(float ratio);

      void ClearParticleSystem();

      void UpdateParticleSystem(float simTimeDelta, float speed);

      void Setup(dtGame::GameActorProxy& ownerActor, osg::Group* attachNode = nullptr);

   protected:
      virtual ~DynamicParticlesPropertyContainer();

      void BindShaderToParticleSystem(dtCore::ParticleSystem& particles);
      void BindShaderToNode(osg::Node& node);

      dtActors::DynamicParticleSystem* CreateParticleSystem();

      dtCore::RefPtr<DynamicParticlesActor> CreateDynamicParticleSystemActor(
         const dtCore::ResourceDescriptor& rd, const std::string& actorName,
         bool relativeToParent);

      void InterpolateParticleSystem(DynamicParticles& particles, float ratio);

   private:
      float mLastInterpRatio;
      float mUpdateTimer;
      dtCore::RefPtr<DynamicParticleSystemActor> mParticleSystemActor;
      dtCore::RefPtr<DynamicParticleSystem> mParticleSystem;
      dtCore::ObserverPtr<dtGame::GameActorProxy> mOwner;
      dtCore::ObserverPtr<osg::Group> mAttachNode;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT DynamicParticlesActorComponent : public dtGame::ActorComponent
   {
   public:
      typedef dtGame::ActorComponent BaseClass;

      static const ACType TYPE;

      static const dtUtil::RefString CLASS_NAME;

      static const dtUtil::RefString PROPERTY_DYNAMIC_PARTICLE_DATA_ARRAY;

      DynamicParticlesActorComponent(const ACType& type = TYPE);

      void SetParticleSystemActorEnabled(const std::string& name, bool enabled);
      bool IsParticleSyatemActorEnabled(const std::string& name) const;

      DynamicParticlesActor* GetParticleSystemActor(const std::string& name) const;

      int GetParticleDataIndex(const std::string& name) const;

      DynamicParticlesPropertyContainer* GetParticleDataByName(const std::string& name);

      DynamicParticlesPropertyContainer* GetParticleData(int index);

      void SetParticleData(int index, DynamicParticlesPropertyContainer* animProps);

      void InsertNewParticleData(int index);

      void RemoveParticleData(int index);

      size_t GetNumParticleData() const;

      int GetCurrentParticleDataIndex() const;

      void UpdateParticleSystems(float simTimeDelta);

      void DisableAllParticleSystems();

      /*override*/ void OnTickLocal(const dtGame::TickMessage& tickMessage);
      /*override*/ void OnTickRemote(const dtGame::TickMessage& tickMessage);

      /// Adds the properties associated with this actor
      /*override*/ void BuildPropertyMap();

      /*override*/ void OnEnteredWorld();
      /*override*/ void OnRemovedFromWorld();

      /*override*/ void OnRemovedFromActor(dtCore::BaseActorObject& actor);

   protected:
      virtual ~DynamicParticlesActorComponent();

      void ClearParticleData();

   private:
      int mCurrentIndex;
      osg::Vec3 mLastPos;

      typedef std::vector<dtCore::RefPtr<DynamicParticlesPropertyContainer> > DynamicParticlesDataArray;
      DynamicParticlesDataArray mParticleDataArray;

      dtCore::ObserverPtr<dtCore::Transformable> mOwnerDrawable;
   };
}

#endif
