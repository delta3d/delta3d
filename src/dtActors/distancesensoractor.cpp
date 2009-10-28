/*
* Delta3D Open Source Game and Simulation Engine
* Copyright 2008, Alion Science and Technology
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
* David Guthrie
*/

#include <dtActors/distancesensoractor.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtDAL/enginepropertytypes.h>

#include <dtGame/message.h>
#include <dtGame/messagetype.h>

namespace dtActors
{
   const dtUtil::RefString DistanceSensorActorProxy::PROPERTY_TRIGGER_DISTANCE("Trigger Distance");
   const dtUtil::RefString DistanceSensorActorProxy::PROPERTY_ATTACH_TO_ACTOR("AttachToActor");

   ////////////////////////////////////////////////////
   DistanceSensorActorProxy::DistanceSensorActorProxy()
   {

   }

   ////////////////////////////////////////////////////
   DistanceSensorActorProxy::~DistanceSensorActorProxy()
   {

   }

   ////////////////////////////////////////////////////
   void DistanceSensorActorProxy::BuildInvokables()
   {
      GameActorProxy::BuildInvokables();
   }

   ////////////////////////////////////////////////////
   void DistanceSensorActorProxy::BuildPropertyMap()
   {
      GameActorProxy::BuildPropertyMap();

      DistanceSensorActor* actor;
      GetActor(actor);

      static const dtUtil::RefString GROUP("Distance Sensor");
      static const dtUtil::RefString EMPTY;

      AddProperty(new dtDAL::ActorActorProperty(*this, DistanceSensorActorProxy::PROPERTY_ATTACH_TO_ACTOR,
               DistanceSensorActorProxy::PROPERTY_ATTACH_TO_ACTOR,
               dtDAL::MakeFunctor(*this, &DistanceSensorActorProxy::SetAttachToProxy),
               dtDAL::MakeFunctorRet(*actor, &DistanceSensorActor::GetAttachToActor),
               "dtCore::DeltaDrawable", EMPTY, GROUP));

      AddProperty(new dtDAL::FloatActorProperty(DistanceSensorActorProxy::PROPERTY_TRIGGER_DISTANCE,
               DistanceSensorActorProxy::PROPERTY_TRIGGER_DISTANCE,
               dtDAL::MakeFunctor(*actor, &DistanceSensorActor::SetTriggerDistance),
               dtDAL::MakeFunctorRet(*actor, &DistanceSensorActor::GetTriggerDistance),
               EMPTY, GROUP));
   }

   ////////////////////////////////////////////////////
   void DistanceSensorActorProxy::SetAttachToProxy(dtDAL::ActorProxy* newAttachToProxy)
   {
      SetLinkedActor(DistanceSensorActorProxy::PROPERTY_ATTACH_TO_ACTOR, newAttachToProxy);
      DistanceSensorActor* actor;
      GetActor(actor);
      if (newAttachToProxy == NULL)
      {
         actor->SetAttachToActor(NULL);
      }
      else
      {
         actor->SetAttachToActor(newAttachToProxy->GetActor());
      }
   }
   ////////////////////////////////////////////////////
   void DistanceSensorActorProxy::CreateActor()
   {
      SetActor(*new DistanceSensorActor(*this));
   }

   ////////////////////////////////////////////////////
   DistanceSensorActor::DistanceSensorActor(dtGame::GameActorProxy& parent):
   dtGame::GameActor(parent),
   mTriggerDistance(0.f)
   {
   }

   ////////////////////////////////////////////////////
   DistanceSensorActor::~DistanceSensorActor()
   {
   }

   ////////////////////////////////////////////////////
   void DistanceSensorActor::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();
      if (GetAttachToActor() != NULL)
      {
         if (GetParent() != NULL)
         {
            Emancipate();
         }
         else if (GetSceneParent() != NULL)
         {
            GetSceneParent()->RemoveDrawable(this);
         }

         GetAttachToActor()->AddChild(this);
      }

      if (IsRemote())
      {
         GetGameActorProxy().RegisterForMessages(dtGame::MessageType::TICK_REMOTE, dtGame::GameActorProxy::TICK_REMOTE_INVOKABLE);
      }
      else
      {
         GetGameActorProxy().RegisterForMessages(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
      }
   }

   ////////////////////////////////////////////////////
   void DistanceSensorActor::SetAttachToActor(dtCore::DeltaDrawable* attachTo)
   {
      mAttachToActor = attachTo;
   }

   ////////////////////////////////////////////////////
   dtCore::DeltaDrawable* DistanceSensorActor::GetAttachToActor()
   {
      return mAttachToActor.get();
   }

   ////////////////////////////////////////////////////
   void DistanceSensorActor::SetTriggerDistance(float distance)
   {
      mTriggerDistance = distance;
   }

   ////////////////////////////////////////////////////
   float DistanceSensorActor::GetTriggerDistance() const
   {
      return mTriggerDistance;
   }

   ////////////////////////////////////////////////////
   bool DistanceSensorActor::HasRegistration(const std::string& name) const
   {
      return mSensors.find(name) != mSensors.end();
   }

   ////////////////////////////////////////////////////
   void DistanceSensorActor::RemoveSensorRegistration(const std::string& name)
   {
      mRemoveList.push_back(name);
   }

   ////////////////////////////////////////////////////
   void DistanceSensorActor::OnTickLocal(const dtGame::TickMessage& tickMessage)
   {

      //do all removes here
      while(!mRemoveList.empty())
      {
         mSensors.erase(*mRemoveList.rbegin());
         mRemoveList.pop_back();
      }

      BaseClass::OnTickLocal(tickMessage);

      SensorMap::iterator i = mSensors.begin();
      SensorMap::iterator iend = mSensors.end();
      for (; i != iend; ++i)
      {
         dtAI::SensorBase<float>* sb = i->second.get();
         sb->Evaluate();
      }
   }

   ////////////////////////////////////////////////////
   void DistanceSensorActor::OnTickRemote(const dtGame::TickMessage& tickMessage)
   {
      BaseClass::OnTickRemote(tickMessage);
      //This could potentially cause problems with subclassing this.
      OnTickLocal(tickMessage);
   }

}
