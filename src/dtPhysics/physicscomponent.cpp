/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * @author Brad Anderegg
 * @author Allen Danklefsen
 * @author David Guthrie
 */
#include <dtPhysics/physicscomponent.h>

#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/physicsmaterials.h>
#include <dtPhysics/physicsactorregistry.h>
#include <dtPhysics/geometry.h>
#include <dtGame/basemessages.h>
#include <dtGame/actorupdatemessage.h>
#include <dtCore/uniqueid.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtABC/application.h>
#include <dtCore/enginepropertytypes.h>
#include <dtGame/messagetype.h>
#include <dtGame/environmentactor.h>
#include <algorithm>
// gets rid of the global PF = getInstance define.
#ifdef PF
#undef PF
#endif

namespace dtPhysics
{
   const dtCore::RefPtr<dtCore::SystemComponentType> PhysicsComponent::TYPE(new dtCore::SystemComponentType("PhysicsComponent","GMComponents",
         "Maintains the physics worlds and the updating of them."));

   const std::string PhysicsComponent::DEFAULT_NAME(TYPE->GetName());

   /////////////////////////////////////////////////////////////////////////////
   PhysicsComponent::PhysicsComponent(dtCore::SystemComponentType& type)
   : GMComponent(type)
   , mStepInBackground(false)
   , mSteppingEnabled(true)
   , mImpl(NULL)
   , mClearOnMapchange(true)
   , mOverrodeStepInBackground(false)
   {
      // Impl...
   }

   /////////////////////////////////////////////////////////////////////////////
   PhysicsComponent::PhysicsComponent(dtPhysics::PhysicsWorld& world, bool debugPhysics,
         dtCore::SystemComponentType& type)
   : GMComponent(type)
   , mStepInBackground(false)
   , mSteppingEnabled(true)
   , mImpl(&world)
   , mClearOnMapchange(true)
   , mOverrodeStepInBackground(false)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   PhysicsComponent::~PhysicsComponent(void)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   class PhysicsComponentRemoveFunc
   {
   public:
      PhysicsComponentRemoveFunc(const dtCore::UniqueId& id):
         mId(id)
   {}

      bool operator()(dtCore::RefPtr<PhysicsActComp>& actComp)
      {
         dtGame::GameActorProxy* act = NULL;
         actComp->GetOwner(act);
         if (act != NULL && act->GetId() == mId)
         {
            actComp->CleanUp();
            return true;
         }
         return false;
      }

   private:
      const dtCore::UniqueId& mId;
   };

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsComponent::ProcessMessage(const dtGame::Message& message)
   {
      if (message.GetMessageType() == dtGame::MessageType::TICK_END_OF_FRAME)
      {
         // Begin physics step after all regular simulation has done and all cleanup has finished
         const dtGame::TickMessage& tm = static_cast<const dtGame::TickMessage&>(message);
         BeginUpdate(tm);
      }
      else if (message.GetMessageType() == dtGame::MessageType::SYSTEM_POST_FRAME)
      {
         const dtGame::TickMessage& tm = static_cast<const dtGame::TickMessage&>(message);
         WaitUntilUpdateCompletes(tm.GetDeltaSimTime());
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
      {
         mRegisteredActorComps.erase(
               std::remove_if(mRegisteredActorComps.begin(), mRegisteredActorComps.end(),
                     PhysicsComponentRemoveFunc(message.GetAboutActorId())),
                     mRegisteredActorComps.end());
      }
      else if(message.GetMessageType() == dtGame::MessageType::INFO_MAP_UNLOAD_BEGIN)
      {
         if (mClearOnMapchange)
         {
            ClearAll();
            VertexData::ClearAllCachedData();
         }
         //mControllerManager->purgeControllers();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsComponent::ClearAll()
   {
      if(GetGameManager() != NULL)
      {
         mRegisteredActorComps.clear();
      }

      if (mDebDraw.valid())
      {
         mDebDraw->Emancipate();
         mDebDraw = NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsComponent::SetNextDebugDrawMode()
   {

      if (!mDebDraw.valid())
      {
         mDebDraw = new dtPhysics::DebugDrawable;
         GetGameManager()->GetScene().AddChild(mDebDraw.get());
         mDebDraw->SetActive(false);
      }

      dtCore::ActorProxy* EnvActor = GetGameManager()->GetEnvironmentActor();
      if (EnvActor == NULL)
      {
         bool enable = !mDebDraw->GetActive();
         mDebDraw->SetActive(enable);
         dtPhysics::PhysicsWorld::GetInstance().SetDebugDrawEnabled(enable);
      }
      else
      {
         dtCore::Camera* cam = GetGameManager()->GetApplication().GetCamera();

         static osg::Vec4 oldClearColor;

         if (EnvActor->GetDrawable()->GetActive())
         {
            cam->GetClearColor(oldClearColor);
         }

         bool physicsDrawActive = !GetPhysicsWorld().GetDebugDrawEnabled() ||
               !EnvActor->GetDrawable()->GetActive();
         bool worldActive = dtPhysics::PhysicsWorld::GetInstance().GetDebugDrawEnabled();

         dtPhysics::PhysicsWorld::GetInstance().SetDebugDrawEnabled(physicsDrawActive);
         mDebDraw->SetActive(physicsDrawActive);
         EnvActor->GetDrawable()->SetActive(worldActive);

         if (!worldActive)
         {
            cam->SetClearColor(0.0, 0.25f, 0.8f, 1.0f);
         }
         else
         {
            cam->SetClearColor(oldClearColor);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   PhysicsWorld& PhysicsComponent::GetPhysicsWorld()
   {
      return *mImpl;
   }

   /////////////////////////////////////////////////////////////////////////////
   const PhysicsWorld& PhysicsComponent::GetPhysicsWorld() const
   {
      return *mImpl;
   }

   /////////////////////////////////////////////////////////////////////////////
   const PhysicsActComp* PhysicsComponent::GetActorComp(const std::string& name) const
   {
      PhysicsActCompVector::const_iterator iter = mRegisteredActorComps.begin();
      for(; iter != mRegisteredActorComps.end(); ++iter)
      {
         if((*iter)->GetName() == name)
            return (*iter).get();
      }

      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   PhysicsActComp* PhysicsComponent::GetActorComp(const std::string& name)
   {
      PhysicsActCompVector::iterator iter = mRegisteredActorComps.begin();
      for(; iter != mRegisteredActorComps.end(); ++iter)
      {
         if((*iter)->GetName() == name)
            return (*iter).get();
      }

      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsComponent::RegisterActorComp(PhysicsActComp& helper)
   {
      mRegisteredActorComps.push_back(&helper);
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsComponent::UnregisterActorComp(PhysicsActComp& toRemove)
   {
      PhysicsActCompVector::iterator iter = mRegisteredActorComps.begin();
      for(; iter != mRegisteredActorComps.end(); ++iter)
      {
         if((*iter) == &toRemove)
         {
            (*iter)->CleanUp();
            mRegisteredActorComps.erase(iter);
            return;
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsComponent::IsActorCompRegistered(const PhysicsActComp& pActorComp)
   {
      PhysicsActCompVector::const_iterator iter = mRegisteredActorComps.begin();
      for(; iter != mRegisteredActorComps.end(); ++iter)
      {
         if((*iter) == &pActorComp)
         {
            return true;
         }
      }
      return false;
   }

   /// @see PhysicsWorld::SetGroupCollision
   void PhysicsComponent::SetGroupCollision(CollisionGroup one, CollisionGroup two, bool enabled)
   {
      GetPhysicsWorld().SetGroupCollision(one, two, enabled);
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsComponent::OnAddedToGM()
   {
      ClearAll();
      if (!mOverrodeStepInBackground)
      {
         std::string enableStepInBackground = GetGameManager()->GetConfiguration().
               GetConfigPropertyValue("dtPhysics.EnableStepPhysicsInBackground", "false");
         if (dtUtil::ToType<bool>(enableStepInBackground))
         {
            SetStepInBackground(true);
         }
         else
         {
            SetStepInBackground(false);
         }
         // Calling SetStepInBackGround sets this variable to true, but it should only
         // remain true if the method was called by external code since it is meant to keep
         // the default or value in the config file from preventing code from setting the value.
         mOverrodeStepInBackground = false;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsComponent::OnRemovedToGM()
   {
      ClearAll();
   }


   /////////////////////////////////////////////////////////////////////////////
   void PhysicsComponent::SetStepInBackground(bool value)
   {
      mStepInBackground = value;
      mOverrodeStepInBackground = true;
   }

   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR_GETTER(PhysicsComponent, bool, StepInBackground);

   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(PhysicsComponent, bool, SteppingEnabled);

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsComponent::BeginUpdate(const dtGame::TickMessage& tm)
   {
      if (mSteppingEnabled)
      {
         if (mDebDraw.valid())
         {
            dtCore::Transform xform;
            GetGameManager()->GetApplication().GetCamera()->GetTransform(xform);
            mDebDraw->SetReferencePosition(xform.GetTranslation());
         }

         std::for_each(mRegisteredActorComps.begin(), mRegisteredActorComps.end(), [&](PhysicsActCompPtr& pac)
               {
            pac->PrePhysicsUpdate(tm.GetDeltaSimTime());
               });

         if (mStepInBackground)
         {
            mImpl->StartBackgroundUpdateStep(tm.GetDeltaSimTime());
         }
         else
         {
            mImpl->UpdateStep(tm.GetDeltaSimTime());

            std::for_each(mRegisteredActorComps.begin(), mRegisteredActorComps.end(), [&](PhysicsActCompPtr& pac)
                  {
               pac->PostPhysicsUpdate(tm.GetDeltaSimTime());
                  });
         }
      }
      else
      {
         dtPhysics::PhysicsWorld::GetInstance().ClearContacts();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsComponent::UpdateStep(float dt)
   {
      if (mDebDraw.valid())
      {
         dtCore::Transform xform;
         GetGameManager()->GetApplication().GetCamera()->GetTransform(xform);
         mDebDraw->SetReferencePosition(xform.GetTranslation());
      }

      std::for_each(mRegisteredActorComps.begin(), mRegisteredActorComps.end(), [&](PhysicsActCompPtr& pac)
            {
         pac->PrePhysicsUpdate(dt);
            });

      mImpl->UpdateStep(dt);

      std::for_each(mRegisteredActorComps.begin(), mRegisteredActorComps.end(), [&](PhysicsActCompPtr& pac)
            {
         pac->PostPhysicsUpdate(dt);
            });
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsComponent::WaitUntilUpdateCompletes(float dt)
   {
      if (mSteppingEnabled && mStepInBackground)
      {
         mImpl->WaitForUpdateStepToComplete();

         std::for_each(mRegisteredActorComps.begin(), mRegisteredActorComps.end(), [&](PhysicsActCompPtr& pac)
               {
            pac->PostPhysicsUpdate(dt);
               });
      }
   }

}
