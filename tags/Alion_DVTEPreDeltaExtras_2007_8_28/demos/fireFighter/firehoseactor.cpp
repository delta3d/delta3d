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
}

void FireHoseActorProxy::BuildInvokables()
{
   GameItemActorProxy::BuildInvokables();
}

FireHoseActor::FireHoseActor(dtGame::GameActorProxy &proxy) :
   GameItemActor(proxy),
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
}

void FireHoseActor::SetStreamFilename(const std::string &filename)
{
   mParticleSystem->LoadFile(filename);
}

std::string FireHoseActor::GetStreamFilename() const
{
   return mParticleSystem->GetFilename();
}
