/* -*-c++-*-
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
#include <dtDAL/actorpluginregistry.h>
#include "export.h"

class DT_EXAMPLE_EXPORT TestGameActorLibrary : public dtDAL::ActorPluginRegistry
{
   public:

      static dtCore::RefPtr<dtDAL::ActorType> TEST1_GAME_ACTOR_PROXY_TYPE;
      static dtCore::RefPtr<dtDAL::ActorType> TEST2_GAME_ACTOR_PROXY_TYPE;
      static dtCore::RefPtr<dtDAL::ActorType> TEST_PLAYER_GAME_ACTOR_PROXY_TYPE;
      static dtCore::RefPtr<dtDAL::ActorType> TEST_TASK_GAME_ACTOR_PROXY_TYPE;
      static dtCore::RefPtr<dtDAL::ActorType> TEST_COUNTER_TASK_GAME_ACTOR_PROXY_TYPE;
      static dtCore::RefPtr<dtDAL::ActorType> TEST_TANK_GAME_ACTOR_PROXY_TYPE;
      static dtCore::RefPtr<dtDAL::ActorType> TEST_JET_GAME_ACTOR_PROXY_TYPE;
      static dtCore::RefPtr<dtDAL::ActorType> TEST_HELICOPTER_GAME_ACTOR_PROXY_TYPE;
      static dtCore::RefPtr<dtDAL::ActorType> TEST_ENVIRONMENT_GAME_ACTOR_PROXY_TYPE;

      /// Constructor
      TestGameActorLibrary();

      void RegisterActorTypes();
};
