/* -*-c++-*-
 * Delta3D
 * Copyright 2013, David Guthrie
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
#include <prefix/dtgameprefix.h>
#include <dtGame/cascadingdeleteactorcomponent.h>
#include <dtGame/gameactor.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/basemessages.h>
#include <dtGame/gamemanager.h>

#include <algorithm>

namespace dtGame
{

   ////////////////////////////////////////////////////////////////////////////////
   CascadingDeleteActorComponent::CascadingDeleteActorComponent()
   : BaseClass(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   CascadingDeleteActorComponent::~CascadingDeleteActorComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CascadingDeleteActorComponent::Connect(dtGame::GameActorProxy& deleter, dtCore::BaseActorObject& deletee)
   {
      dtCore::RefPtr<CascadingDeleteActorComponent> cdac;
      deleter.GetComponent(cdac);
      if (!cdac.valid())
      {
         cdac = new CascadingDeleteActorComponent;
         deleter.AddComponent(*cdac);
      }
      cdac->AddActorToDelete(deletee.GetId());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CascadingDeleteActorComponent::Disconnect(dtGame::GameActorProxy& deleter, dtCore::BaseActorObject& deletee)
   {
      dtCore::RefPtr<CascadingDeleteActorComponent> cdac;
      deleter.GetComponent(cdac);
      if (cdac.valid())
      {
         cdac->RemoveActorToDelete(deletee.GetId());
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   void CascadingDeleteActorComponent::OnRemovedFromWorld()
   {
      dtGame::GameActorProxy* actor = NULL;
      GetOwner(actor);
      std::vector<dtCore::UniqueId>::iterator i, iend;
      i = mActorsToDelete.begin();
      iend = mActorsToDelete.end();
      for (; i != iend; ++i)
      {
         actor->GetGameManager()->DeleteActor(*i);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CascadingDeleteActorComponent::AddActorToDelete(const dtCore::UniqueId& toAdd)
   {
      if (!IsActorToDelete(toAdd))
      {
         mActorsToDelete.push_back(toAdd);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CascadingDeleteActorComponent::RemoveActorToDelete(const dtCore::UniqueId& toRemove)
   {
      std::vector<dtCore::UniqueId>::iterator item = std::find(mActorsToDelete.begin(), mActorsToDelete.end(), toRemove);
      if (item != mActorsToDelete.end())
      {
         mActorsToDelete.erase(item);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CascadingDeleteActorComponent::IsActorToDelete(const dtCore::UniqueId& actorId) const
   {
      return std::find(mActorsToDelete.begin(), mActorsToDelete.end(), actorId) != mActorsToDelete.end();
   }

}
