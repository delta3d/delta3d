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
{}


////////////////////////////////////////////////////////////////////////////////
ActorComponent::~ActorComponent()
{}

////////////////////////////////////////////////////////////////////////////////
ActorComponent::ACType ActorComponent::GetType() const
{
   return &GetActorType();
}

   
/////////////////////////////////////////////////////////////////////////////
void ActorComponent::OnAddedToActor(dtCore::BaseActor& actor)
{
   // pass the component a pointer to its owner
   SetParent(&actor);

   // The call to Init should eventually move to an actor component library behavior
   // like actors have, but until then, this is the only other place to do it.
   Init(GetActorType());

   // Determine if OnEnteredWorld needs to be called if adding
   // this component to an actor that is already in the world.
   if (actor.IsInGM())
   {
      OnEnteredWorld();
   }
}

/////////////////////////////////////////////////////////////////////////////
void ActorComponent::OnRemovedFromActor(dtCore::BaseActor& actor)
{
   // Determine if OnRemovedFromWorld needs to be called if removing
   // this component from an actor that is already in the world.
   if (actor.IsInGM())
   {
      OnRemovedFromWorld();
   }
}

/////////////////////////////////////////////////////////////////////////////
void ActorComponent::OnEnteredWorld()
{
   // TEMP:
   // Map properties to the root level actor.
   // WARNING! Property names have to be unique.
   AddPropertiesToRootActor();
}

/////////////////////////////////////////////////////////////////////////////
void ActorComponent::OnRemovedFromWorld()
{
   // TEMP:
   // Remove properties that were mapped to the root level actor.
   // WARNING! Property names have to be unique.
   RemovePropertiesFromRootActor();
}

/////////////////////////////////////////////////////////////////////////////
void ActorComponent::AddPropertiesToRootActor()
{
   // TEMP: Temporary section for mapping the component properties as
   // direct properties on the root actor.
   dtCore::BaseActor* rootActor = GetParent();

   if (rootActor != NULL)
   {
      while (rootActor->GetParent() != NULL)
      {
         rootActor = rootActor->GetParent();
      }

      typedef dtCore::PropertyContainer::PropertyVector PropList;
      PropList props;
      GetPropertyList(props);

      dtCore::ActorProperty* curProp = NULL;
      PropList::iterator curIter = props.begin();
      PropList::iterator endIter = props.end();
      for (; curIter != endIter; ++curIter)
      {
         curProp = *curIter;
         rootActor->AddProperty(curProp);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
void ActorComponent::RemovePropertiesFromRootActor()
{
   // TEMP: Temporary section for removing component properties
   // that may have been mapped directly to the root actor.
   dtCore::BaseActor* rootActor = GetParent();

   if (rootActor != NULL)
   {
      while (rootActor->GetParent() != NULL)
      {
         rootActor = rootActor->GetParent();
      }

      typedef dtCore::PropertyContainer::PropertyVector PropList;
      PropList props;
      GetPropertyList(props);

      dtCore::ActorProperty* curProp = NULL;
      PropList::iterator curIter = props.begin();
      PropList::iterator endIter = props.end();
      for (; curIter != endIter; ++curIter)
      {
         curProp = *curIter;
         rootActor->RemoveProperty(curProp);
      }
   }
}

}
