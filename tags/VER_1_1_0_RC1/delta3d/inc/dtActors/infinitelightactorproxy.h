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
 * @author Matthew W. Campbell
*/
#ifndef __InfiniteLightActorProxy__h
#define __InfiniteLightActorProxy__h

#include <dtCore/infinitelight.h>
#include "dtDAL/actorproxy.h"
#include "dtDAL/plugin_export.h"
#include "dtActors/baselightactorproxy.h"

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
         * @see ActorProxy::IsPlaceable()
         */
        virtual bool IsPlaceable() const { return false; }

        /**
         * Registers this actor's properties.
         */
        virtual void BuildPropertyMap();

        /**
         * Sets the direction the infinite light is pointing.
         * @param dir The direction to set.
         */
        void SetDirection(const osg::Vec3 &dir) 
        {
            dtCore::InfiniteLight *l = dynamic_cast<dtCore::InfiniteLight *>(mActor.get());
            if (l == NULL) 
            {
                EXCEPT(dtDAL::ExceptionEnum::InvalidActorException,
                    "Actor should be type dtCore::InfiniteLight.");
            }

            l->SetAzimuthElevation(dir[2],dir[0]);
        }

        /**
         * Gets the direction the infinite light is pointing.
         * @return The current direction
         */
        osg::Vec3 GetDirection() const 
        {
            const dtCore::InfiniteLight *l = dynamic_cast<const dtCore::InfiniteLight *>(mActor.get());
            if (l == NULL) 
            {
                EXCEPT(dtDAL::ExceptionEnum::InvalidActorException,
                    "Actor should be type dtCore::InfiniteLight.");
            }

            float h,p;
            l->GetAzimuthElevation(h,p);
            return osg::Vec3(p, 0.0f, h);
        }

    protected:

        /**
         * Initializes the actor
         */
        virtual void CreateActor();

        /**
         * Destructor
         */
        virtual ~InfiniteLightActorProxy() { }
    };

}

#endif
