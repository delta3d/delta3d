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
 * William E. Johnson II
 */
#include <fireFighter/fireactor.h>
#include <fireFighter/messagetype.h>
#include <fireFighter/hatchactor.h>
#include <fireFighter/messages.h>
#include <fireFighter/gamestate.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>
#include <dtGame/basemessages.h>
#include <dtGame/invokable.h>
#include <dtGame/messagetype.h>
#include <dtGame/gamemanager.h>
#include <dtCore/particlesystem.h>
#include <dtUtil/mathdefines.h>
#include <dtCore/positionallight.h>
#include <dtDAL/gameeventmanager.h>
#include <dtGame/gamemanager.h>
#include <dtGame/basemessages.h>

using dtCore::RefPtr;

/////////////////////////////////////////////////
FireActorProxy::FireActorProxy()
{

}

FireActorProxy::~FireActorProxy()
{

}

void FireActorProxy::BuildPropertyMap()
{
   GameItemActorProxy::BuildPropertyMap();

   FireActor &fa = static_cast<FireActor&>(GetGameActor());

   AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::PARTICLE_SYSTEM, 
      "FlameFileName", "FlameFileName", 
      dtDAL::MakeFunctor(fa, &FireActor::SetFlameFilename),  
      "Sets the flame file name"));

   AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::PARTICLE_SYSTEM,
      "SparkFileName", "SparkFileName", 
      dtDAL::MakeFunctor(fa, &FireActor::SetSparkFilename),  
      "Sets the spark file name"));

   AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::PARTICLE_SYSTEM,
      "SmokeFileName", "SmokeFileName", 
      dtDAL::MakeFunctor(fa, &FireActor::SetSmokeFilename),  
      "Sets the smoke file name"));

   AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::PARTICLE_SYSTEM,
      "SmokeCeilingFileName", "SmokeCeilingFileName", 
      dtDAL::MakeFunctor(fa, &FireActor::SetSmokeCeilingFilename), 
      "Sets the smoke ceiling file name"));

   AddProperty(new dtDAL::FloatActorProperty("Radius", "Radius", 
      dtDAL::MakeFunctor(fa, &FireActor::SetRadius), 
      dtDAL::MakeFunctorRet(fa, &FireActor::GetRadius), 
      "Sets the fire radius"));

   AddProperty(new dtDAL::FloatActorProperty("Intensity", "Intensity", 
      dtDAL::MakeFunctor(fa, &FireActor::SetIntensity), 
      dtDAL::MakeFunctorRet(fa, &FireActor::GetIntensity), 
      "Sets the fire intensity"));

   AddProperty(new dtDAL::Vec3ActorProperty("LightRotation", "LightRotation", 
      dtDAL::MakeFunctor(fa, &FireActor::SetLightRotation), 
      dtDAL::MakeFunctorRet(fa, &FireActor::GetLightRotation), 
      "Sets the light rotation"));

   AddProperty(new dtDAL::Vec3ActorProperty("LightTranslation", "LightTranslation", 
      dtDAL::MakeFunctor(fa, &FireActor::SetLightTranslation), 
      dtDAL::MakeFunctorRet(fa, &FireActor::GetLightTranslation), 
      "Sets the light translation"));
}

void FireActorProxy::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
}

dtDAL::ActorProxyIcon* FireActorProxy::GetBillBoardIcon()
{
   if(!mBillBoardIcon.valid())
   {
      mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::GENERIC);
   }
   return mBillBoardIcon.get();
}

void FireActorProxy::OnEnteredWorld()
{
   FireActor &fa = static_cast<FireActor&>(GetGameActor());

   dtGame::Invokable *playSoundInvoke = new dtGame::Invokable("PlaySound", 
      dtDAL::MakeFunctor(fa, &FireActor::PlayFireSound));

   dtGame::Invokable *stopSoundInvoke = new dtGame::Invokable("StopSounds", 
      dtDAL::MakeFunctor(fa, &FireActor::StopSounds));

   AddInvokable(*playSoundInvoke);
   AddInvokable(*stopSoundInvoke);

   RegisterForMessages(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
   RegisterForMessages(MessageType::ITEM_ACTIVATED,   "PlaySound");
   RegisterForMessages(MessageType::ITEM_DEACTIVATED, "PlaySound");
   RegisterForMessages(MessageType::GAME_STATE_CHANGED, "StopSounds");
}

/////////////////////////////////////////////////
FireActor::FireActor(dtGame::GameActorProxy &proxy) :
   GameItemActor(proxy),
   mFlameSystem(new dtCore::ParticleSystem),
   mSmokeSystem(new dtCore::ParticleSystem),
   mSparkSystem(new dtCore::ParticleSystem),
   mCeilingSystem(new dtCore::ParticleSystem),
   mLight(new dtCore::PositionalLight(5)),
   mRadius(1.0f),
   mIntensity(10.0f)
{
   mItemUseSnd->SetLooping(true);

   AddChild(mFlameSystem.get());
   AddChild(mSmokeSystem.get());
   AddChild(mSparkSystem.get());
   AddChild(mCeilingSystem.get());
   AddChild(mLight.get());
}

FireActor::~FireActor()
{

}

void FireActor::PlayFireSound(const dtGame::Message &msg)
{
   // Check to see if the actor is the door to the fire room
   dtGame::GameActorProxy *proxy = GetGameActorProxy().GetGameManager()->FindGameActorById(msg.GetAboutActorId());
   if(proxy == NULL)
      return; 

   HatchActor *ha = dynamic_cast<HatchActor*>(proxy->GetActor());
   if(ha == NULL)
      return;

   // Hatch item was activated/deactivated, play/stop the sound
   ha->IsActivated() ? mItemUseSnd->Play() : mItemUseSnd->Stop();
}

void FireActor::OnEnteredWorld()
{
   GameItemActor::OnEnteredWorld();

   osg::Vec4 leftWall(0, -1, 0, 0.337), rightWall(-1, 0, 0, 18.271),
             ceiling(0, 0, -1, 2.459), back(1, 0, 0, -5.31),
             side(0, 1, 0, 5.96);

   AddBoundaryPlane(leftWall);
   AddBoundaryPlane(rightWall);
   AddBoundaryPlane(ceiling);
   AddBoundaryPlane(back);
   AddBoundaryPlane(side);
}

void FireActor::SetFlameFilename(const std::string &filename)
{
   mFlameSystem->LoadFile(filename);
}

void FireActor::SetSparkFilename(const std::string &filename)
{
   mSparkSystem->LoadFile(filename);
}

void FireActor::SetSmokeFilename(const std::string &filename)
{
   mSmokeSystem->LoadFile(filename);
}

void FireActor::SetSmokeCeilingFilename(const std::string &filename)
{
   mCeilingSystem->LoadFile(filename);
}

void FireActor::SetRadius(float radius)
{
   mRadius = radius;
}

float FireActor::GetRadius() const
{
   return mRadius;
}

void FireActor::DecreaseIntensity(float intensity)
{
   if(mIntensity != intensity)
   {
      // Force the value to be between 0 and 1
      dtUtil::Clamp(intensity, 0.0f, 1.0f);
      
      mIntensity -= intensity;
      if(mIntensity <= 0.0f)
      {
         // Fire is dead
         if(mItemUseSnd->IsPlaying())
            mItemUseSnd->Stop();

         mIntensity = 0.0f;
         mFlameSystem->SetEnabled(false);
         mSmokeSystem->SetEnabled(false);
         mSparkSystem->SetEnabled(false);
         mCeilingSystem->SetEnabled(false);
         mLight->SetEnabled(false);

         const std::string &name = "ExtinguishFire";

         dtDAL::GameEvent *event = dtDAL::GameEventManager::GetInstance().FindEvent(name);
         if(event == NULL)
         {
            throw dtUtil::Exception("Failed to find the game event: " + name, __FILE__, __LINE__);
         }

         dtGame::GameManager &mgr = *GetGameActorProxy().GetGameManager();
         RefPtr<dtGame::Message> msg = 
            mgr.GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT);

         dtGame::GameEventMessage &gem = static_cast<dtGame::GameEventMessage&>(*msg);
         gem.SetGameEvent(*event);
         mgr.SendMessage(gem);
      }
      else if(mIntensity < 2.0f)
      {
         if(mFlameSystem->GetFilename() != "Particles/flame1_minor.osg")
            mFlameSystem->LoadFile("Particles/flame1_minor.osg");
      }
   }
}

void FireActor::SetIntensity(float intensity)
{
   mIntensity = intensity;
}

float FireActor::GetIntensity() const
{
   return mIntensity;
}

void FireActor::AddBoundaryPlane(const osg::Vec4 plane)
{
   mBoundaryPlanes.push_back(osg::Vec4(plane[0], plane[1], plane[2], plane[3]));;
}

void FireActor::RemoveBoundaryPlane(int index)
{
   mBoundaryPlanes.erase(mBoundaryPlanes.begin() + index);
}

int FireActor::GetBoundaryPlaneCount() const
{
   return mBoundaryPlanes.size();
}

void FireActor::GetBoundaryPlane(osg::Vec4 dest, int index) const
{
   dest.set(mBoundaryPlanes[index][0],mBoundaryPlanes[index][1],mBoundaryPlanes[index][2],mBoundaryPlanes[index][3]);
}

void FireActor::TickLocal(const dtGame::Message &msg)
{
   const dtGame::TickMessage &tickMsg = static_cast<const dtGame::TickMessage&>(msg);

   double delta = tickMsg.GetDeltaSimTime();

   static double randomWalk = 0.5;

   randomWalk += (delta * (((float)rand()/RAND_MAX)*5-2.5));

   if(randomWalk < 0.0)
   {
      randomWalk = 0.0;
   }
   else if(randomWalk > 1.0)
   {
      randomWalk = 1.0;
   }
   const float r = 0.9f * 10.0f;
   const float g = 0.7f * 10.0f;
   const float b = 0.35f * 10.0f;
   mLight->SetDiffuse(osg::Vec4(mIntensity * r * (0.5 + 0.5*randomWalk), mIntensity * g * (0.5 + 0.5*randomWalk), mIntensity * b * (0.5 + 0.5*randomWalk), 1));
}

void FireActor::StopSounds(const dtGame::Message &msg)
{
   const GameStateChangedMessage &gscm = static_cast<const GameStateChangedMessage&>(msg);
   if(gscm.GetNewState() == GameState::STATE_DEBRIEF ||
      gscm.GetNewState() == GameState::STATE_MENU)
   {
      StopItemUseSnd();
   }
}

void FireActor::SetLightRotation(const osg::Vec3 &rotation)
{
   dtCore::Transform xform;
   mLight->GetTransform(xform);
   xform.SetRotation(rotation);
   mLight->SetTransform(xform);
}

osg::Vec3 FireActor::GetLightRotation() const
{
   dtCore::Transform xform;
   mLight->GetTransform(xform);
   osg::Vec3 hpr;
   xform.GetRotation(hpr);
   return hpr;
}

void FireActor::SetLightTranslation(const osg::Vec3 &xyz)
{
   dtCore::Transform xform;
   mLight->GetTransform(xform);
   xform.SetTranslation(xyz);
   mLight->SetTransform(xform);
}

osg::Vec3 FireActor::GetLightTranslation() const
{
   dtCore::Transform xform;
   mLight->GetTransform(xform);
   osg::Vec3 xyz;
   xform.GetTranslation(xyz);
   return xyz;
}
