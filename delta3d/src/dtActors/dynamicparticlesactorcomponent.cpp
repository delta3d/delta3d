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
#include <dtActors/dynamicparticlesactorcomponent.h>
#include <dtActors/dynamicparticlesystemactor.h>
#include <dtActors/particlesystemactorproxy.h>
#include <dtActors/engineactorregistry.h>
#include <dtCore/arrayactorpropertycomplex.h>
#include <dtCore/shadermanager.h>
#include <dtCore/transform.h>
#include <dtCore/project.h>
#include <dtCore/propertycontaineractorproperty.h>
#include <dtCore/propertymacros.h>
#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/nodecollector.h>
#include <osg/MatrixTransform>
#include <osg/Geode>



namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   const dtCore::RefPtr<dtCore::ObjectType>
      DynamicParticlesPropertyContainer::DYNAMIC_PARTICLES_PC_TYPE(
         new dtCore::ObjectType("DynamicParticlesPropertyContainer",
            "dtActors", "Properties for creating and managing dynamic particle systems."));



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DynamicParticlesPropertyContainer::DynamicParticlesPropertyContainer()
      : BaseClass()
      , mEnabled(false)
      , mUpdateFrequency(1.0f)
      , mOwnerSpeedMin(1.0f)
      , mOwnerSpeedMax(10.0f)
      , mLastInterpRatio(0.0f)
      , mRelativeToParent(false)
      , mAttachDirectly(false)
      , mUpdateTimer(0.0f)
   {
      const dtUtil::RefString GROUPNAME("DynamicParticlesPropertyContainer");
      typedef dtCore::PropertyRegHelper<DynamicParticlesPropertyContainer&, DynamicParticlesPropertyContainer> PropertyRegType;
      PropertyRegType propertyRegHelper(*this, this, GROUPNAME);

      DT_REGISTER_PROPERTY(
         Name,
         "Name to identify the particle system and its use.",
         PropertyRegType, propertyRegHelper);
      
      DT_REGISTER_RESOURCE_PROPERTY(
         dtCore::DataType::PARTICLE_SYSTEM, ParticleFile,
         "ParticleFile", "Loads the particle system file.",
         PropertyRegType, propertyRegHelper);

      DT_REGISTER_PROPERTY(
         Enabled,
         "Turns the particle system on or off",
         PropertyRegType, propertyRegHelper);

      DT_REGISTER_PROPERTY(
         UpdateFrequency,
         "Set the seconds between updates for the particle system effect interpolation",
         PropertyRegType, propertyRegHelper);

      DT_REGISTER_PROPERTY(OwnerSpeedMin,
         "The speed at which to start the particle system effect.",
         PropertyRegType, propertyRegHelper);

      DT_REGISTER_PROPERTY(OwnerSpeedMax,
         "The speed at which to clamp the particle system's maximum effect.",
         PropertyRegType, propertyRegHelper);

      DT_REGISTER_PROPERTY(RelativeToParent,
         "Flag to cause the particle system to emit particles within parent local space.",
         PropertyRegType, propertyRegHelper);

      DT_REGISTER_PROPERTY(AttachNodeName,
         "Name of a node in the parent actor's drawable to which to attach the particle system.",
         PropertyRegType, propertyRegHelper);

      DT_REGISTER_PROPERTY(AttachDirectly,
         "Determines if the particles should attach to the parent drawable or simply be moved with it every tick.",
         PropertyRegType, propertyRegHelper);

      DT_REGISTER_PROPERTY(ShaderGroup,
         "Shader to apply to the particle system.",
         PropertyRegType, propertyRegHelper);

      DT_REGISTER_PROPERTY(Offset,
         "Translation offset relative to the particle system's attach point.",
         PropertyRegType, propertyRegHelper);

      DT_REGISTER_PROPERTY(OffsetRotation,
         "Rotation offset (HPR in degrees) relative to the particle system's attach point.",
         PropertyRegType, propertyRegHelper);

      InitDefaults();
   }

   /////////////////////////////////////////////////////////////////////////////
   DynamicParticlesPropertyContainer::~DynamicParticlesPropertyContainer()
   {
      ClearParticleSystem();
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::ObjectType& DynamicParticlesPropertyContainer::GetObjectType() const
   {
      return *DYNAMIC_PARTICLES_PC_TYPE;
   }

   /////////////////////////////////////////////////////////////////////////////
   DynamicParticles* DynamicParticlesPropertyContainer::GetDynamicParticles() const
   {
      return mParticleSystem.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   DynamicParticlesActor* DynamicParticlesPropertyContainer::GetDynamicParticlesActor() const
   {
      return mParticleSystemActor.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(DynamicParticlesPropertyContainer, std::string, Name);
   DT_IMPLEMENT_ACCESSOR(DynamicParticlesPropertyContainer, dtCore::ResourceDescriptor, ParticleFile);
   DT_IMPLEMENT_ACCESSOR(DynamicParticlesPropertyContainer, float, UpdateFrequency);
   DT_IMPLEMENT_ACCESSOR(DynamicParticlesPropertyContainer, float, OwnerSpeedMin);
   DT_IMPLEMENT_ACCESSOR(DynamicParticlesPropertyContainer, float, OwnerSpeedMax);
   DT_IMPLEMENT_ACCESSOR(DynamicParticlesPropertyContainer, bool, RelativeToParent);
   DT_IMPLEMENT_ACCESSOR(DynamicParticlesPropertyContainer, bool, AttachDirectly);
   DT_IMPLEMENT_ACCESSOR(DynamicParticlesPropertyContainer, std::string, AttachNodeName);
   DT_IMPLEMENT_ACCESSOR(DynamicParticlesPropertyContainer, std::string, ShaderGroup);
   DT_IMPLEMENT_ACCESSOR(DynamicParticlesPropertyContainer, osg::Vec3, Offset);
   DT_IMPLEMENT_ACCESSOR(DynamicParticlesPropertyContainer, osg::Vec3, OffsetRotation);

   DT_IMPLEMENT_ACCESSOR_GETTER(DynamicParticlesPropertyContainer, bool, Enabled);

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesPropertyContainer::SetEnabled(bool enable)
   {
      mEnabled = enable;
      if (mParticleSystem.valid())
      {
         mParticleSystem->SetEnabled(mEnabled);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesPropertyContainer::Setup(dtGame::GameActorProxy& ownerActor, osg::Group* attachNode)
   {
      dtCore::Transformable* drawable = nullptr;
      ownerActor.GetDrawable(drawable);
      mOwner = &ownerActor;

      CreateParticleSystem();

      if (mOwner.valid())
      {
         mOwner->GetDrawable(drawable);
      }

      if (drawable != nullptr && mParticleSystem.valid())
      {
         mAttachNode = attachNode;

         if (mAttachDirectly || mAttachNode.valid())
         {
            // Attach the particles to the parent drawable.
            mParticleSystemActor->SetParentActor(mOwner.get());
         }

         if (mAttachDirectly && mAttachNode.valid())
         {
            osg::Node* node = mParticleSystem->GetOSGNode();

            // Detach the node from current parents which
            // may only be the scene.
            int numParents = node->getNumParents();
            for (int i = 0; i < numParents; ++i)
            {
               node->getParent(i)->removeChild(node);
            }

            // Attach to the new node.
            mAttachNode->addChild(node);
         }

         // Offset the particles' transform.
         dtCore::Transform transform;

         transform.SetTranslation(mOffset);
         transform.SetRotation(mOffsetRotation);
         mParticleSystem->SetTransform(transform, dtCore::Transformable::REL_CS);

         // Assign any specified shader to the particle system.
         BindShaderToParticleSystem(*mParticleSystem);
      }

      // Ensure the particle systems have the current enabled state.
      SetEnabled(mEnabled);
   }

   /////////////////////////////////////////////////////////////////////////////
   float DynamicParticlesPropertyContainer::GetSpeedRatio(float speed) const
   {
      float ratio = 0.0f;

      if (mOwnerSpeedMax != 0.0f)
      {
         if (speed >= mOwnerSpeedMin && mOwnerSpeedMax > mOwnerSpeedMin)
         {
            ratio = (speed - mOwnerSpeedMin) / (mOwnerSpeedMax - mOwnerSpeedMin);
         }
      }

      dtUtil::Clamp(ratio, 0.0f, 1.0f);

      return ratio;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesPropertyContainer::InterpolateParticleSystem(float ratio)
   {
      if (mParticleSystem.valid())
      {
         InterpolateParticleSystem(*mParticleSystem, ratio);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesPropertyContainer::ClearParticleSystem()
   {
      // Remove the particle system drawable from the owner.
      if (mParticleSystem.valid())
      {
         if (mOwner.valid())
         {
            dtCore::DeltaDrawable* drawable = nullptr;
            mOwner->GetDrawable(drawable);
            if (drawable != nullptr)
            {
               drawable->RemoveChild(mParticleSystem.get());
            }
         }

         mParticleSystem = nullptr;
      }

      // Remove the generated actor from the game scene.
      if (mParticleSystemActor.valid())
      {
         dtGame::GameManager* gm = mParticleSystemActor->GetGameManager();

         if (gm != nullptr)
         {
            gm->DeleteActor(*mParticleSystemActor);
         }

         mParticleSystemActor = nullptr;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   dtActors::DynamicParticleSystem* DynamicParticlesPropertyContainer::CreateParticleSystem()
   {
      if (!mParticleSystem.valid() && !mParticleFile.IsEmpty())
      {
         mParticleSystemActor = CreateDynamicParticleSystemActor(mParticleFile, mName, mRelativeToParent);

         if ( ! mParticleSystemActor.valid())
         {
            LOG_ERROR("Could not create particle system actor for file: "
               + mParticleFile.GetResourceIdentifier());
         }
         else
         {
            DynamicParticles* particles = nullptr;
            mParticleSystemActor->GetDrawable(particles);
            mParticleSystem = particles;
         }
      }

      return mParticleSystem.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<DynamicParticlesActor>
      DynamicParticlesPropertyContainer::CreateDynamicParticleSystemActor(
         const dtCore::ResourceDescriptor& rd, const std::string& actorName, bool relativeToParent)
   {
      std::string fileName;
      try
      {
         fileName = dtCore::Project::GetInstance().GetResourcePath(rd);
      }
      catch (const dtUtil::Exception& ex)
      {
         ex.LogException();
         return nullptr;
      }

      if ( ! mOwner.valid())
      {
         return nullptr;
      }

      dtCore::RefPtr<DynamicParticlesActor> actor;

      dtGame::GameManager* gm = mOwner->GetGameManager();
      if (gm != nullptr)
      {
         // Create the actor.
         DynamicParticles* drawable = nullptr;
         gm->CreateActor(*dtActors::EngineActorRegistry::DYNAMIC_PARTICLE_SYSTEM_ACTOR_TYPE, actor);

         actor->GetDrawable(drawable);
         drawable->SetName(actorName);
         drawable->SetParentRelative(relativeToParent);
         drawable->LoadFile(fileName);

         // Set default settings.
         typedef DynamicParticles::InterpolatorArray InterpolatorArray;
         InterpolatorArray interpArray;
         drawable->GetAllInterpolators(interpArray);

         InterpolatorArray::iterator curInterp = interpArray.begin();
         InterpolatorArray::iterator endInterpArray = interpArray.end();
         for (; curInterp != endInterpArray; ++curInterp)
         {
            dtCore::ParticleSystemSettings& settings = (*curInterp)->GetStartSettings();
            settings.mRangeRate *= 0.0f;
            settings.mRangeSpeed *= 0.0f;
         }

         gm->AddActor(*actor, false, false);
      }

      return actor;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesPropertyContainer::InterpolateParticleSystem(DynamicParticles& particles, float ratio)
   {
      particles.InterpolateAllLayers(dtCore::ParticlePropertyEnum::PS_EMIT_SPEED, ratio);
      particles.InterpolateAllLayers(dtCore::ParticlePropertyEnum::PS_EMIT_RATE, ratio);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesPropertyContainer::UpdateParticleSystem(float simTimeDelta, float speed)
   {
      float ratio = GetSpeedRatio(speed * (simTimeDelta != 0.0f ? 1.0f / simTimeDelta : 0.0f));

      mUpdateTimer += simTimeDelta;

      bool allowInterpolation = mUpdateTimer > mUpdateFrequency || dtUtil::Abs(mLastInterpRatio - ratio) > 0.1f;

      if (allowInterpolation)
      {
         // Reset timer and control variables.
         mLastInterpRatio = ratio;
         mUpdateTimer = 0.0f;

         // Update the particle systems.
         if (mParticleSystem.valid())
         {
            InterpolateParticleSystem(*mParticleSystem, ratio);
         }
      }

      // Indirect attach...
      if (! mAttachDirectly && mAttachNode.valid() && mParticleSystem.valid())
      {
         dtCore::Transform xform;
         osg::Matrix mtx;

         dtCore::Transformable::GetAbsoluteMatrix(mAttachNode.get(), mtx);
         xform.Set(mtx);

         osg::Matrix rot;
         dtUtil::MatrixUtil::HprToMatrix(rot, mOffsetRotation);
         osg::Vec3 finalPos = mtx.preMult(mOffset);
         osg::Matrix finalRot = mtx * rot;

         dtCore::Transform relXform;
         xform.SetTranslation(finalPos);
         xform.SetRotation(finalRot);
         mParticleSystem->SetTransform(xform);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesPropertyContainer::BindShaderToParticleSystem(dtCore::ParticleSystem& particles)
   {
      dtCore::ParticleSystem::LayerList& layers = particles.GetAllLayers();
      dtCore::ParticleSystem::LayerList::iterator iter = layers.begin();
      for (; iter != layers.end(); ++iter)
      {
         //osgParticle::ParticleSystem* ref = &iter->GetParticleSystem();
         dtCore::ParticleLayer& pLayer = *iter;
         BindShaderToNode(pLayer.GetGeode());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesPropertyContainer::BindShaderToNode(osg::Node& node)
   {
      // Avoid unnecessary processing.
      if (mShaderGroup.empty())
      {
         return;
      }

      // Find and assign the specified shader group.
      dtCore::ShaderManager& sm = dtCore::ShaderManager::GetInstance();
      dtCore::ShaderGroup* sg = sm.FindShaderGroupPrototype(mShaderGroup);
      if (sg)
      {
         dtCore::ShaderProgram* sp = sg->GetDefaultShader();

         if (sp != nullptr)
         {
            sm.AssignShaderFromPrototype(*sp, node);
         }
         else
         {
            LOG_ERROR("Unable to find particle system shader group: \"" + mShaderGroup + "\" ");
         }
      }
      else
      {
         LOG_ERROR("Unable to find shader group for particle system manager.");
      }
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtGame::ActorComponent::ACType DynamicParticlesActorComponent::TYPE(
      new dtCore::ActorType("DynamicParticlesActorComponent", "ActorComponents", "", dtGame::ActorComponent::BaseActorComponentType));
   
   const dtUtil::RefString DynamicParticlesActorComponent::CLASS_NAME("dtActors.DynamicParticlesActorComponent");

   /////////////////////////////////////////////////////////////////////////////
   DynamicParticlesActorComponent::DynamicParticlesActorComponent(const ACType& type)
      : BaseClass(type)
      , mCurrentIndex(0)
   {
      SetClassName(DynamicParticlesActorComponent::CLASS_NAME);
   }

   /////////////////////////////////////////////////////////////////////////////
   DynamicParticlesActorComponent::~DynamicParticlesActorComponent()
   {
      mParticleDataArray.clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesActorComponent::SetParticleSystemActorEnabled(const std::string& name, bool enabled)
   {
      DynamicParticlesActor* actor = GetParticleSystemActor(name);
      if (actor != nullptr)
      {
         actor->SetEnabled(enabled);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DynamicParticlesActorComponent::IsParticleSyatemActorEnabled(const std::string& name) const
   {
      DynamicParticlesActor* actor = GetParticleSystemActor(name);
      return actor != nullptr ? actor->IsEnabled() : false;
   }

   /////////////////////////////////////////////////////////////////////////////
   DynamicParticlesActor* DynamicParticlesActorComponent::GetParticleSystemActor(const std::string& name) const
   {
      DynamicParticlesActor* actor = nullptr;

      int index = GetParticleDataIndex(name);
      if (index > -1)
      {
         actor = mParticleDataArray[index]->GetDynamicParticlesActor();
      }

      return actor;
   }

   /////////////////////////////////////////////////////////////////////////////
   int DynamicParticlesActorComponent::GetParticleDataIndex(const std::string& name) const
   {
      int result = -1;

      DynamicParticlesDataArray::const_iterator curIter = mParticleDataArray.begin();
      DynamicParticlesDataArray::const_iterator endIter = mParticleDataArray.end();
      for (int i = 0; curIter != endIter; ++curIter, ++i)
      {
         if (curIter->get()->GetName() == name)
         {
            result = i;
            break;
         }
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   DynamicParticlesPropertyContainer*
      DynamicParticlesActorComponent::GetParticleDataByName(const std::string& name)
   {
      int index = GetParticleDataIndex(name);
      return GetParticleData(index);
   }

   /////////////////////////////////////////////////////////////////////////////
   DynamicParticlesPropertyContainer*
      DynamicParticlesActorComponent::GetParticleData(int index)
   {
      if (index >= 0 && size_t(index) < mParticleDataArray.size())
      {
         return mParticleDataArray[index];
      }

      return nullptr;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesActorComponent::SetParticleData(int index, DynamicParticlesPropertyContainer* animProps)
   {
      if (animProps != nullptr && mParticleDataArray.size() > size_t(index) && index >= 0)
      {
         mParticleDataArray[index] = new DynamicParticlesPropertyContainer(*animProps);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesActorComponent::InsertNewParticleData(int index)
   {
      if (index >= 0 && size_t(index) <= mParticleDataArray.size())
      {
         mParticleDataArray.insert(mParticleDataArray.begin() + index, new DynamicParticlesPropertyContainer());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesActorComponent::RemoveParticleData(int index)
   {
      if (index >= 0 && size_t(index) < mParticleDataArray.size())
      {
         mParticleDataArray.erase(mParticleDataArray.begin() + index);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   size_t DynamicParticlesActorComponent::GetNumParticleData() const
   {
      return mParticleDataArray.size();
   }

   int DynamicParticlesActorComponent::GetCurrentParticleDataIndex() const
   {
      return mCurrentIndex;
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString DynamicParticlesActorComponent::PROPERTY_DYNAMIC_PARTICLE_DATA_ARRAY("DynamicParticleDataArray");

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesActorComponent::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      const dtUtil::RefString GROUP("DynamicParticles");
      typedef dtCore::PropertyRegHelper<DynamicParticlesActorComponent&, DynamicParticlesActorComponent> RegHelperType;
      RegHelperType propReg(*this, this, GROUP);

      typedef dtCore::ArrayActorPropertyComplex<dtCore::RefPtr<DynamicParticlesPropertyContainer> > DPPCArrayPropType;
      dtCore::RefPtr<DPPCArrayPropType> arrayProp = new DPPCArrayPropType(
         PROPERTY_DYNAMIC_PARTICLE_DATA_ARRAY,
         PROPERTY_DYNAMIC_PARTICLE_DATA_ARRAY,
         DPPCArrayPropType::SetFuncType(this, &DynamicParticlesActorComponent::SetParticleData),
         DPPCArrayPropType::GetFuncType(this, &DynamicParticlesActorComponent::GetParticleData),
         DPPCArrayPropType::GetSizeFuncType(this, &DynamicParticlesActorComponent::GetNumParticleData),
         DPPCArrayPropType::InsertFuncType(this, &DynamicParticlesActorComponent::InsertNewParticleData),
         DPPCArrayPropType::RemoveFuncType(this, &DynamicParticlesActorComponent::RemoveParticleData),
         "Array of dynamic particle system parameters. Each element defines an individual dynamic particle system to be created and interpolated on the owner drawable.",
         GROUP);

      const dtUtil::RefString PROPERTY_DYNAMIC_PARTICLE_DATA("DynamicParticleData");
      dtCore::RefPtr<dtCore::BasePropertyContainerActorProperty> propContainerProp =
         new dtCore::SimplePropertyContainerActorProperty<DynamicParticlesPropertyContainer>(
         PROPERTY_DYNAMIC_PARTICLE_DATA,
         PROPERTY_DYNAMIC_PARTICLE_DATA,
         dtCore::SimplePropertyContainerActorProperty<DynamicParticlesPropertyContainer>::SetFuncType(arrayProp.get(), &DPPCArrayPropType::SetCurrentValue),
         dtCore::SimplePropertyContainerActorProperty<DynamicParticlesPropertyContainer>::GetFuncType(arrayProp.get(), &DPPCArrayPropType::GetCurrentValue),
         "Contains data for creating and controlling a dynamic particle system.",
         GROUP);

      arrayProp->SetArrayProperty(*propContainerProp);
      AddProperty(arrayProp);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesActorComponent::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();

      dtGame::GameActorProxy* owner = 0;
      GetOwner(owner);
      if (owner != nullptr)
      {
         mOwnerDrawable = dynamic_cast<dtCore::Transformable*>(owner->GetDrawable());
      }

      RegisterForTick();

      dtCore::RefPtr<dtUtil::NodeCollector> nc;

      // Setup all the particle systems.
      DynamicParticlesPropertyContainer* curData = nullptr;
      DynamicParticlesDataArray::iterator curIter = mParticleDataArray.begin();
      DynamicParticlesDataArray::iterator endIter = mParticleDataArray.end();
      for (int i = 0; curIter != endIter; ++curIter, ++i)
      {
         curData = curIter->get();

         const std::string& attachNodeName = curData->GetAttachNodeName();
         osg::Group* attachNode = nullptr;

         // Attempt to acquire an attach node if a name was specified.
         if ( ! attachNodeName.empty())
         {
            // Determine if a node collector needs to be created for the first time.
            if ( ! nc.valid() && mOwnerDrawable.valid())
            {
               nc = new dtUtil::NodeCollector(mOwnerDrawable->GetOSGNode(), dtUtil::NodeCollector::AllNodeTypes);
            }

            // Get any specified attach node if possible.
            if (nc.valid())
            {
               attachNode = nc->GetGroup(attachNodeName);
            }
         }

         curData->Setup(*owner, attachNode);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesActorComponent::OnRemovedFromActor(dtCore::BaseActorObject& actor)
   {
      // Clear all the particle systems.
      DynamicParticlesDataArray::const_iterator curIter = mParticleDataArray.begin();
      DynamicParticlesDataArray::const_iterator endIter = mParticleDataArray.end();
      for (int i = 0; curIter != endIter; ++curIter, ++i)
      {
         curIter->get()->ClearParticleSystem();
      }

      BaseClass::OnRemovedFromActor(actor);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DynamicParticlesActorComponent::UpdateParticleSystems(float simTimeDelta)
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
      float speed = dif.length2() > 0.0f ? dif.length() : 0.0f;

      // Setup all the particle systems.
      DynamicParticlesDataArray::const_iterator curIter = mParticleDataArray.begin();
      DynamicParticlesDataArray::const_iterator endIter = mParticleDataArray.end();
      for (int i = 0; curIter != endIter; ++curIter, ++i)
      {
         curIter->get()->UpdateParticleSystem(simTimeDelta, speed);
      }
   }
   
   //////////////////////////////////////////////////////////
   void DynamicParticlesActorComponent::DisableAllParticleSystems()
   {
      DynamicParticlesDataArray::const_iterator curIter = mParticleDataArray.begin();
      DynamicParticlesDataArray::const_iterator endIter = mParticleDataArray.end();
      for (int i = 0; curIter != endIter; ++curIter, ++i)
      {
         curIter->get()->SetEnabled(false);
      }
   }

   //////////////////////////////////////////////////////////
   void DynamicParticlesActorComponent::OnTickLocal(const dtGame::TickMessage& tickMessage)
   {
      BaseClass::OnTickLocal(tickMessage);

      float simTimeDelta = tickMessage.GetDeltaSimTime();
      UpdateParticleSystems(simTimeDelta);
   }

   //////////////////////////////////////////////////////////
   void DynamicParticlesActorComponent::OnTickRemote(const dtGame::TickMessage& tickMessage)
   {
      BaseClass::OnTickRemote(tickMessage);

      float simTimeDelta = tickMessage.GetDeltaSimTime();
      UpdateParticleSystems(simTimeDelta);
   }

}

