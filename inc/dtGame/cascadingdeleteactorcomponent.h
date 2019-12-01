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

#ifndef CASCADINGDELETEACTORCOMPONENT_H_
#define CASCADINGDELETEACTORCOMPONENT_H_

#include <dtGame/export.h>
#include <dtGame/actorcomponent.h>
#include <dtCore/uniqueid.h>

namespace dtCore
{
   class BaseActorObject;
}

namespace dtGame
{
   class GameActorProxy;

   /*
    * A simple actor component that allows other actors to be deleted as a result
    * of the owner being deleted.
    */
   class DT_GAME_EXPORT CascadingDeleteActorComponent: public dtGame::ActorComponent
   {
   public:
      // set the type of the actor component
      static const dtGame::ActorComponent::ACType TYPE;
      typedef dtGame::ActorComponent BaseClass;

      CascadingDeleteActorComponent();
      virtual ~CascadingDeleteActorComponent();

      // This static function will add this actor component to the delter if need be and add the deletee to its list to delete when it is deleted.
      static void Connect(dtGame::GameActorProxy& deleter, dtCore::BaseActorObject& deletee);
      // This static function will remove the deletee from the list in the cascading delete actor component.
      static void Disconnect(dtGame::GameActorProxy& deleter, dtCore::BaseActorObject& deletee);

      virtual void OnRemovedFromWorld();

      void AddActorToDelete(const dtCore::UniqueId& toAdd);
      void RemoveActorToDelete(const dtCore::UniqueId& toRemove);
      bool IsActorToDelete(const dtCore::UniqueId& actorId) const;

   private:
      std::vector<dtCore::UniqueId> mActorsToDelete;
   };
}
#endif /* CASCADINGDELETEACTORCOMPONENT_H_ */
