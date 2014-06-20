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
#ifndef DELTA_ENVEFFECT_ACTOR_PROXY
#define DELTA_ENVEFFECT_ACTOR_PROXY

#include "dtCore/plugin_export.h"
#include "dtCore/actorproxy.h"

namespace dtActors 
{

    /**
     * @class EnvEffectActorProxy
     * @brief This proxy wraps environment effects.
     */
    class DT_PLUGIN_EXPORT EnvEffectActorProxy : public dtCore::BaseActorObject 
    {
    public:

        /**
         * Constructor
         */
        EnvEffectActorProxy() { SetClassName("dtCore::EnvEffect"); }

        /**
         * Environment effects are interfaces, therefore, they have no properties.
         * Actor proxies for effects must extend this class to support the individual
         * effect.
         */
        virtual void BuildPropertyMap();

        /**
         * Effects cannot be placed in a scene
         */
        virtual bool IsPlaceable() const { return false; }

    protected:

        /**
         * Subclasses corresponding to concrete environmental effects need to
         * implement this method.
         */
        virtual void CreateDrawable() = 0;

        /**
         * Destructor
         */
        virtual ~EnvEffectActorProxy() { }
    };
}

#endif
