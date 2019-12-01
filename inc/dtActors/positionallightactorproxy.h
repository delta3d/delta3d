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

#ifndef _POSITIONAL_ACTOR_PROXY_H_
#define _POSITIONAL_ACTOR_PROXY_H_

#include "dtCore/plugin_export.h"
#include "dtActors/baselightactorproxy.h"
#include "dtCore/transformableactorproxy.h"
#include <dtCore/positionallight.h>
#include <dtCore/exceptionenum.h>

namespace dtActors
{
    /**
     * This proxy wraps the PositionalLight Delta3D object.
     */
    class DT_PLUGIN_EXPORT PositionalLightActorProxy : public BaseLightActorProxy
    {
    public:

        /**
         * Constructor
         */
        PositionalLightActorProxy() { SetClassName("dtCore::PositionLight"); }

        /**
         * Adds the properties for positional lights.
         */
        virtual void BuildPropertyMap();

        /**
         * Positional lights can be placed in a scene.
         */
        virtual bool IsPlaceable() const { return true; }

        /**
        * Determines if the rotation should be shown in the actor properties.
        */
        virtual bool IsRotationPropertyShown() const { return false; }

        /**
         * Gets the billboard icon for lights. Normally this wouldn't be here
         * as it's inherited from BaseLightActorProxy. But since this inheritence
         * chain uses virtual inheritence and MSVC spits a warning, this should
         * quell it.
         * @return The billboard icon common to all lights.
         */
        virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

        /**
         * Gets the current render mode for positional lights.
         * @return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.
         */
        virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
        {
            return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
        }

        /**
         * Sets the attenuation for a light actor.
         * @param atten The attenuation to set.
         */
        void SetAttenuation(const osg::Vec3 &atten)
        {
            dtCore::PositionalLight *pl = dynamic_cast<dtCore::PositionalLight*>(GetDrawable());
            if (!pl)
            {
                throw dtCore::InvalidActorException(
                "Actor should be type dtCore::PositionalLight", __FILE__, __LINE__);
            }

            pl->SetAttenuation(atten[0], atten[1], atten[2]);
        }

        /**
         * Gets the attenuation for a light actor.
         * @return The current attenuation.
         */
        osg::Vec3 GetAttenuation()
        {
            dtCore::PositionalLight *pl = dynamic_cast<dtCore::PositionalLight*>(GetDrawable());
            if (!pl)
            {
                throw dtCore::InvalidActorException(
                "Actor should be type dtCore::PositionalLight", __FILE__, __LINE__);
            }

            float c, l, q;
            pl->GetAttenuation(c, l, q);
            return osg::Vec3(c, l, q);
        }

    protected:

        /**
         * Initializes the actor.
         */
        virtual void CreateDrawable();

        /**
         * Destructor.
         */
        virtual ~PositionalLightActorProxy() { }
    };
}

#endif
