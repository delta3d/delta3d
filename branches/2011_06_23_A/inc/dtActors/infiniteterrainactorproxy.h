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
#ifndef DELTA_INFINITETERRAIN_ACTOR_PROXY
#define DELTA_INFINITETERRAIN_ACTOR_PROXY

#include <dtDAL/plugin_export.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtCore/infiniteterrain.h>

namespace dtActors 
{
    /**
     * @class InfiniteTerrainActorProxy
     * @brief This proxy wraps the InfiniteTerrain Delta3D object.
     */
    class DT_PLUGIN_EXPORT InfiniteTerrainActorProxy : public dtDAL::TransformableActorProxy
    {
    public:

        /**
         * Constructor
         */
        InfiniteTerrainActorProxy() { SetClassName("dtCore::InfiniteTerrain"); }

        /**
         * Adds the properties that are common to all Delta3D transformable objects.
         */
        virtual void BuildPropertyMap();

    protected:

        /**
         * Creates an infinite terrain actor.
         */
        virtual void CreateActor();

        /**
         * Destructor
         */
        virtual ~InfiniteTerrainActorProxy() { }

    };
}

#endif
