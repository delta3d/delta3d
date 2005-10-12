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
#ifndef _TRANSFORMABLE_ACTOR_PROXY_H_
#define _TRANSFORMABLE_ACTOR_PROXY_H_

#include <osg/Vec3>
#include <osg/Matrix>

#include "dtDAL/plugin_export.h"
#include "dtDAL/actorproxy.h"

namespace dtDAL
{
    /**
     * This proxy wraps the Transformable Delta3D object.
     */
    class DT_DAL_EXPORT TransformableActorProxy : public virtual dtDAL::ActorProxy
    {
    public:

        /**
         * Constructor
         */
        TransformableActorProxy() { SetClassName("dtCore::Transformable"); }

        /**
         * Adds the properties that are common to all Delta3D transformable objects.
         */
        virtual void BuildPropertyMap();

        /**
         * Transformables can be placed in a scene
         */
        virtual bool IsPlaceable() const { return true; }

        /**
         * Sets the rotation of an object.
         * @param rotation the desired rotation
         */
        void SetRotation(const osg::Vec3 &rotation);

        void SetRotationFromMatrix(const osg::Matrix &rotation);

        /**
         * Gets the rotation of an object
         * @return The current rotation vector
         */
        osg::Vec3 GetRotation();

        /**
         * Sets the translation of an object
         * @param translation the desired translation
         */
        void SetTranslation(const osg::Vec3 &translation);

        /**
         * Gets the translation of an object
         * @return The current translation
         */
        osg::Vec3 GetTranslation();

        /**
         * Sets the scale of an object
         * @param scale the desired scale
         */
        void SetScale(const osg::Vec3 &scale);

        /**
         * Gets the scale of an object
         * @return The current scale vector
         */
        osg::Vec3 GetScale();

    protected:

        /**
         * Destructor
         */
        virtual ~TransformableActorProxy() {}

        /**
         * Initializes the actor.
         */
        virtual void CreateActor() = 0;

        osg::Vec3 mHPR;
    };
}

#endif
