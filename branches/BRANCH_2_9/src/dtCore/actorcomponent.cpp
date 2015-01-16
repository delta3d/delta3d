/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2009, MOVES Institute
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
* @author Choco (forum handle)
* @author David Guthrie
* @author Curtiss Murphy
*/

#include <prefix/dtcoreprefix.h>
#include <dtCore/actorcomponent.h>
#include <dtUtil/log.h>
#include <stdexcept>

namespace dtCore
{
////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////
const dtCore::ActorComponent::ACType dtCore::ActorComponent::BaseActorComponentType(new dtCore::ActorType("Base", "ActorComponents", "A base type so that all actor component types should set as a parent"));



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
ActorComponent::ActorComponent(ActorComponent::ACType type)
   : BaseClass(type.get())
   , mIsInGM(false)
{}


////////////////////////////////////////////////////////////////////////////////
ActorComponent::~ActorComponent()
{}

////////////////////////////////////////////////////////////////////////////////
ActorComponent::ACType ActorComponent::GetType() const
{
   return &GetActorType();
}

////////////////////////////////////////////////////////////////////////////////
BaseActor* ActorComponent::GetOwner() const
{
   return GetParent();
}

////////////////////////////////////////////////////////////////////////////////
void ActorComponent::SetOwner(BaseActor* owner)
{
   SetParent(owner);
}

////////////////////////////////////////////////////////////////////////////
//void ActorComponent::GetOwner(GameActor* ga) const
//{
//   LOG_ERROR("Error, you called the GetOwner template, but expected a type derived from GameActor.  "
//         "This is no longer supported, you need to fix your code.  This will assert in Debug so you can find it more easily.");
//   throw std::bad_cast();
//}

   
/////////////////////////////////////////////////////////////////////////////
void ActorComponent::OnAddedToActor(dtCore::BaseActor& actor)
{
   // pass the component a pointer to its owner
   SetOwner(&actor);

   // The call to Init should eventually move to an actor component library behavior
   // like actors have, but until then, this is the only other place to do it.
   Init(GetActorType());
}

/////////////////////////////////////////////////////////////////////////////
void ActorComponent::OnRemovedFromActor(dtCore::BaseActor& actor)
{
}

//////////////////////////////////////////////////////////////////////////
DT_IMPLEMENT_ACCESSOR(ActorComponent, bool, IsInGM);
}
