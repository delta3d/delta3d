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
#ifndef DELTA_SPOTLIGHT_ACTOR_PROXY
#define DELTA_SPOTLIGHT_ACTOR_PROXY

#include "dtCore/plugin_export.h"
#include "dtActors/positionallightactorproxy.h"
#include <dtCore/spotlight.h>

namespace dtActors
{
    /**
     * @class SpotlightActorProxy
     * @brief This proxy wraps the Spotlight Delta3D object.
     */
    class DT_PLUGIN_EXPORT SpotlightActorProxy : public dtActors::PositionalLightActorProxy
    {
    public:

        /**
         * Constructor
         */
        SpotlightActorProxy() { SetClassName("dtCore::SpotLight"); }

        /**
         * Adds the properties that are common to all Delta3D transformable objects.
         */
        virtual void BuildPropertyMap();

        /**
         * Spotlights can be placed in a scene
         */
        virtual bool IsPlaceable() const { return true; }

        /**
        * Determines if the rotation should be shown in the actor properties.
        */
        virtual bool IsRotationPropertyShown() const { return true; }

        /**
        * Gets the billboard icon for lights. Normally this wouldn't be here
        * as it's inherited from BaseLightActorProxy. But since this inheritence
        * chain uses virtual inheritence and MSVC spits a warning, this should
        * quell it.
        * @return The billboard icon common to all lights.
        */
        virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

        /**
         * Gets the current render mode for spotlights.
         * @return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.
         */
        virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode() 
        {
            return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
        }

    protected:

        /**
        * Initializes the actor
        */
        virtual void CreateDrawable();

        /**
         * Destructor
         */
        virtual ~SpotlightActorProxy() { }
    };
}

#endif
