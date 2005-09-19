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
#ifndef DELTA_CLOUDPLANE_ACTOR_PROXY
#define DELTA_CLOUDPLANE_ACTOR_PROXY

#include "dtDAL/plugin_export.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/exception.h"
#include "dtActors/enveffectactorproxy.h"

namespace dtActors 
{
    /**
     * @class CloudPlaneActorProxy
     * @brief This proxy wraps Delta3D cloud planes.
     */
    class DT_PLUGIN_EXPORT CloudPlaneActorProxy : public dtActors::EnvEffectActorProxy 
    {
    public:

        /**
         * Constructor
         */
        CloudPlaneActorProxy() { SetClassName("dtCore::CloudPlane"); }

        /**
         * Adds the properties that are common to all cloud plane objects.
         */
        virtual void BuildPropertyMap();

    protected:

        /**
         * Creates the cloud plane actor.
         */
        virtual void CreateActor();

        /**
         * Destructor
         */
        virtual ~CloudPlaneActorProxy() { }
    };
}

#endif
