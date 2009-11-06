/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2009 MOVES Institute
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
* Michael Guerrero
*/

#ifndef TEST_ACTOR_LIBRARY_ACTOR_REGISTRY_H_
#define TEST_ACTOR_LIBRARY_ACTOR_REGISTRY_H_

#include <dtDAL/actorpluginregistry.h>

#include "testexport.h"

////////////////////////////////////////////////////////////////////////////////

/**
 * Class that exports the applicable actor proxies to a library
 */
class TEST_ANIM_EXPORT TestActorLibraryRegistry : public dtDAL::ActorPluginRegistry
{
public:
   
   static dtCore::RefPtr<dtDAL::ActorType> IK_ACTOR_TYPE;

   TestActorLibraryRegistry();

   /// Registers custom actors to make sure the game manager is aware of them
   void RegisterActorTypes();
};

////////////////////////////////////////////////////////////////////////////////



#endif // TEST_ACTOR_LIBRARY_ACTOR_REGISTRY_H_
