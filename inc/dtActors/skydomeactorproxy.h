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
#ifndef DELTA_SKYDOME_ACTOR_PROXY
#define DELTA_SKYDOME_ACTOR_PROXY

#include <dtCore/plugin_export.h>
#include <dtCore/actorproxy.h>
#include <dtActors/enveffectactorproxy.h>
#include <dtCore/skydome.h>

namespace dtActors 
{
    /**
     * @class SkyDomeActorProxy
     * @brief This proxy wraps mesh sky domes.
     */
    class DT_PLUGIN_EXPORT SkyDomeActorProxy : public dtActors::EnvEffectActorProxy
    {
    public:

        /**
         * Constructor
         */
        SkyDomeActorProxy() { SetClassName("dtCore::SkyDome"); }

        /**
         * Adds the properties that are common to all sky dome objects.
         */
        virtual void BuildPropertyMap();

        /**
         * Gets the base color
         * @return The current color
         */
        osg::Vec3 GetBaseColor();

        /**
         * Sets the base color
         * @param color The desired color
         */
        void SetBaseColor(const osg::Vec3 &color);

    protected:

        /**
         * Creates the sky dome actor.
         */
        virtual void CreateDrawable();

        /**
         * Destructor
         */
        virtual ~SkyDomeActorProxy() {}

    };
}

#endif
