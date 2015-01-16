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
 * Matthew W. Campbell
*/
#ifndef __InfiniteLightActorProxy__h
#define __InfiniteLightActorProxy__h

#include <dtCore/infinitelight.h>
#include <dtCore/actorproxy.h>
#include <dtCore/plugin_export.h>
#include <dtActors/baselightactorproxy.h>

#include <osg/Vec3>

namespace dtActors
{
    /**
     * This proxy wraps the Infinite light Delta3D object.
     */
    class DT_PLUGIN_EXPORT InfiniteLightActorProxy : public BaseLightActorProxy
    {
    public:

        /**
         * Constructor
         */
        InfiniteLightActorProxy() { SetClassName("dtCore::InfiniteLight"); }

        /**
         * Infinite lights cannot have a position in the scene.  The indicates
         * that the actor is a "global" actor.
         * @return False
         * @see BaseActorObject::IsPlaceable()
         */
        virtual bool IsPlaceable() const { return false; }

        /**
        * Determines if the translation should be shown in the actor properties.
        */
        virtual bool IsTranslationPropertyShown() const { return false; }

        /**
         * Registers this actor's properties.
         */
        virtual void BuildPropertyMap();

    protected:

        /**
         * Initializes the actor
         */
        virtual void CreateDrawable();

        /**
         * Destructor
         */
        virtual ~InfiniteLightActorProxy() { }
    };

}

#endif
