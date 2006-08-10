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
 * @author William E. Johnson II
 */
#include <dtGame/export.h>
#include <dtGame/gameactor.h>
#include <dtDAL/environmentactor.h>
#include <dtCore/environment.h>

#ifndef DELTA_ENVIRONMENT_ACTOR
#define DELTA_ENVIRONMENT_ACTOR

namespace dtGame
{
   class DT_GAME_EXPORT EnvironmentActor : public GameActor, public dtDAL::EnvironmentActor
   {
      public:

         /// Constructor
         EnvironmentActor(GameActorProxy &proxy);

         /**
          * Enables a cloud plane in this environment
          * @todo Refactor this to not use hardcoded numbers
          * @param enable True to turn on, false to turn off
          */
         virtual void EnableCloudPlane(bool enable) { }

         /**
          * Returns if a cloud plane is enabled
          */
         virtual bool IsCloudPlaneEnabled() const { return false; }

      protected:

         /// Destructor
         virtual ~EnvironmentActor();

   };

   class DT_GAME_EXPORT EnvironmentActorProxy : public GameActorProxy
   {
      public:

         /// Constructor
         EnvironmentActorProxy();

         /// Creates the actor this proxy encapsulates
         virtual void CreateActor() = 0;

         /// Called when a proxy is added to the game manager
         virtual void OnEnteredWorld() = 0;

         /// Called to build the properties associated with a proxy
         virtual void BuildPropertyMap() = 0;

         /// Called to build the invokables associated with a proxy
         virtual void BuildInvokables() = 0;

      protected:

         /// Destructor
         virtual ~EnvironmentActorProxy();

      private:
   };
}
#endif
