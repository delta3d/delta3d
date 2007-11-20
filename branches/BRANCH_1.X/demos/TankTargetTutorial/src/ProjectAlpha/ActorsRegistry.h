/*
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
* @author Curtiss Murphy
*/
#ifndef __GM_TUTORIAL_ACTORS_REGISTRY__
#define __GM_TUTORIAL_ACTORS_REGISTRY__

#include "export.h"
#include <dtDAL/actorpluginregistry.h>

class TUTORIAL_TANK_EXPORT ActorsRegistry : public dtDAL::ActorPluginRegistry
{
   public:

      // TUTORIAL - ADD YOUR STATIC REFPTR TO THE ACTOR TYPE HERE
      //static dtCore::RefPtr<dtDAL::ActorType> TANK_ACTOR_TYPE;

      // Constructs our registry.  Creates the actor types easy access when needed.
      ActorsRegistry();

      // Registers actor types with the actor factory in the super class.
      virtual void RegisterActorTypes();
};

#endif
