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
#include <dtCore/resourceactorproperty.h>
#include <dtCore/particlesystem.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/map.h>
#include <dtCore/functor.h>
#include <dtGame/gamemanager.h>
#include <dtGame/message.h>
#include <dtGame/basemessages.h>

using dtCore::RefPtr;

////////////////////////////////////////////////////////
FireHoseActor::FireHoseActor()
: mParticleSystem(new dtCore::ParticleSystem)
{
   mItemUseSnd->SetLooping(true);
   mItemIndex = 1;
}

FireHoseActor::~FireHoseActor()
{

}

void FireHoseActor::BuildPropertyMap()
{
   GameItemActor::BuildPropertyMap();

   AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::PARTICLE_SYSTEM,
      "StreamFile", "StreamFile",
      dtUtil::MakeFunctor(&FireHoseActor::SetStreamFilename, this),
      "Sets the stream file particle system"));
}

void FireHoseActor::BuildInvokables()
{
   GameItemActor::BuildInvokables();
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
