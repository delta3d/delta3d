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
#ifndef DELTA_DTED_TERRAIN_ACTOR_PROXY
#define DELTA_DTED_TERRAIN_ACTOR_PROXY

#include "dtDAL/plugin_export.h"
#include "dtActors/terrainactorproxy.h"
#include "dtDAL/exceptionenum.h"
#include "soarx/dtsoarx.h"

namespace dtActors 
{
    /**
     * @class DTEDTerrainActorProxy
     * @brief This proxy wraps DTED terrain.
     */
    class DT_PLUGIN_EXPORT DTEDTerrainActorProxy : public dtActors::TerrainActorProxy 
    {
    public:

        /**
         * Constructor
         */
        DTEDTerrainActorProxy() { SetClassName("dtSOARX::SOARXTerrain"); }

        /**
         * Adds the properties that are common to DTED terrain.
         */
        virtual void BuildPropertyMap();

        /**
         * We can place DTED terrain in the scene.
         */
        virtual bool IsPlaceable() const { return true; }

    protected:

        /**
         * Create a SoarX rendered terrain actor.
         */
        virtual void CreateActor();

        /**
         * Destructor
         */
        virtual ~DTEDTerrainActorProxy() {}

    };
}

#endif
