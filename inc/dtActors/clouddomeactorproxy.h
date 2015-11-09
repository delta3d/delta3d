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
#ifndef DELTA_CLOUDDOME_ACTOR_PROXY
#define DELTA_CLOUDDOME_ACTOR_PROXY

#include "dtCore/plugin_export.h"
#include "dtCore/actorproxy.h"
#include <dtCore/clouddome.h>
#include "dtActors/enveffectactorproxy.h"

namespace dtActors 
{
    /**
     * @class CloudDomeActorProxy
     * @brief This proxy wraps the CloudDome Delta3D object.
     */

    class DT_PLUGIN_EXPORT CloudDomeActorProxy : public dtActors::EnvEffectActorProxy
    {
    public:

        /**
         * Constructor
         */
        CloudDomeActorProxy() { SetClassName("dtCore::CloudDome"); }

        /**
         * Adds the properties for a cloud dome.
         */
        virtual void BuildPropertyMap();

        /**
         * Sets the cloud color of a cloud dome
         * @param color The color to set
         */
        void SetCloudColor(const osg::Vec3 &color);

        /**
         * Gets the cloud color of a cloud dome
         * @return The current color
         */
        osg::Vec3 GetCloudColor();

    protected:

        /**
         * Creates the cloud dome actor.
         */
        virtual void CreateDrawable();

        /**
         * Destructor
         */
        virtual ~CloudDomeActorProxy() {}

    };
}

#endif
