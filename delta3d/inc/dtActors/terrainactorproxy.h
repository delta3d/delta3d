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
#ifndef DELTA_TERRAIN_ACTOR_PROXY
#define DELTA_TERRAIN_ACTOR_PROXY

#include "dtDAL/plugin_export.h"
#include "dtDAL/transformableactorproxy.h"
#include <dtCore/terrain.h>
#include <dtDAL/exceptionenum.h>

namespace dtActors 
{
    /**
     * @class TerrainActorProxy
     * @brief This proxy wraps the Terrain Delta3D object.
     */
    class DT_PLUGIN_EXPORT TerrainActorProxy : public dtDAL::TransformableActorProxy 
    {
    public:

        /**
         * Constructor
         */
        TerrainActorProxy() { SetClassName("dtCore::Terrain"); }

        /**
         * Adds the properties that are common to Delta3D terrains.
         */
        virtual void BuildPropertyMap();

        /**
         * Sets the geo origin
         * @param origin The origin to Set
         */
        void SetGeoOrigin(const osg::Vec3 &origin);

        /**
         * Gets the geo origin
         * @return The current origin
         */
        osg::Vec3 GetGeoOrigin();

        /**
         * Sets the cache path
         * @param path The path to Set
        */
        void SetCachePath(const std::string &path);

        /**
         * Gets the cache path
         * @param The current cache path
         */
        std::string GetCachePath();

        /**
         * Adds a DTED terrain to the object
         * @param fileName The name of the DTED file
         */
        void AddDTEDPath(std::string &fileName);

        /**
         * Removes a DTED terrain from the object
         * @param fileName The name of the DTED file
         */
        void RemoveDTEDPath(std::string &fileName);

    protected:

        /**
         * Creates the actor proxy
         */
        virtual void CreateActor();

        /**
         * Destructor
         */
        virtual ~TerrainActorProxy() { }

    };
}

#endif

