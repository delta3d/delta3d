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
#include <fireFighter/firehoseactor.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtCore/particlesystem.h>
#include <dtDAL/gameeventmanager.h>
#include <dtDAL/map.h>
#include <dtGame/gamemanager.h>
#include <dtGame/message.h>
#include <dtGame/basemessages.h>

using dtCore::RefPtr;

////////////////////////////////////////////////////////
FireHoseActorProxy::FireHoseActorProxy()
{

}

FireHoseActorProxy::~FireHoseActorProxy()
{

}

void FireHoseActorProxy::BuildPropertyMap()
{
   GameItemActorProxy::BuildPropertyMap();

   FireHoseActor &fha = static_cast<FireHoseActor&>(GetGameActor());

   AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::PARTICLE_SYSTEM, 
      "StreamFile", "StreamFile", 
      dtDAL::MakeFunctor(fha, &FireHoseActor::SetStreamFilename), 
      "Sets the stream file particle system"));

   AddProperty(new dtDAL::FloatActorProperty("ValvePosition", "ValvePosition", 
      dtDAL::MakeFunctor(fha, &FireHoseActor::SetValvePosition),
      dtDAL::MakeFunctorRet(fha, &FireHoseActor::GetValvePosition), 
      "Sets the valve position"));
}

void FireHoseActorProxy::BuildInvokables()
{
   GameItemActorProxy::BuildInvokables();
}

FireHoseActor::FireHoseActor(dtGame::GameActorProxy &proxy) :
   GameItemActor(proxy),
   mValvePosition(0.0f), 
   mParticleSystem(new dtCore::ParticleSystem)
{
   mItemUseSnd->SetLooping(true);
   mItemIndex = 1;
}

FireHoseActor::~FireHoseActor()
{

}

void FireHoseActor::Activate(bool enable)
{
   GameItemActor::Activate(enable);

   //const std::string &name = "AcquireFireHose";

   //// No event, peace out
   //if(!IsActivated())
   //   return;

   //dtDAL::GameEvent *event = dtDAL::GameEventManager::GetInstance().FindEvent(name);
   //if(event == NULL)
   //{
   //   throw dtUtil::Exception("Failed to find the game event: " + name, __FILE__, __LINE__);
   //}

   //dtGame::GameManager &mgr = *GetGameActorProxy().GetGameManager();
   //RefPtr<dtGame::Message> msg = 
   //   mgr.GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT);

   //dtGame::GameEventMessage &gem = static_cast<dtGame::GameEventMessage&>(*msg);
   //gem.SetGameEvent(*event);
   //mgr.SendMessage(gem);
}

void FireHoseActor::SetStreamFilename(const std::string &filename)
{
   mParticleSystem->LoadFile(filename);
}

std::string FireHoseActor::GetStreamFilename() const
{
   return mParticleSystem->GetFilename();
}

void FireHoseActor::SetValvePosition(float position)
{
   mValvePosition = position;
}

float FireHoseActor::GetValvePosition() const
{
   return mValvePosition;
}
