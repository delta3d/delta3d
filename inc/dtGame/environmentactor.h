/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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

#ifndef DELTA_ENVIRONMENT_ACTOR
#define DELTA_ENVIRONMENT_ACTOR

#include <dtGame/export.h>
#include <dtGame/gameactor.h>
#include <dtGame/gameactorproxy.h>
#include <dtCore/environmentactor.h>
#include <dtCore/observerptr.h>

namespace dtGame
{
  /**
   * Interface to the Environment Game Actor.  The dtGame::GameManager uses this
   * class as the Environment Drawable root.  It is expected users will derive and
   * supply their own implementation of a concrete Environment Actor.
   */
   class DT_GAME_EXPORT IEnvGameActor : public dtCore::Transformable, public dtCore::IEnvironmentActor
   {
   public:

      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      /// Constructor
      IEnvGameActor(GameActorProxy& owner);

      GameActorProxy* GetOwner();
      const GameActorProxy* GetOwner() const;
    protected:
      /// Destructor
      virtual ~IEnvGameActor();
      dtCore::ObserverPtr<GameActorProxy> mOwner;
   };

   /**
    * Interface of the Environment Game Actor.  Users should derive
    * a concrete version and supply that to the GameManager.
    * @see dtGame::GameManager::SetEnvironmentActor()
    */
   class DT_GAME_EXPORT IEnvGameActorProxy : public GameActorProxy
   {
   public:
      /// Constructor
      IEnvGameActorProxy();

   protected:
      /// Destructor
      virtual ~IEnvGameActorProxy();

   private:
   };
} // namespace dtGame

#endif // DELTA_ENVIRONMENT_ACTOR
