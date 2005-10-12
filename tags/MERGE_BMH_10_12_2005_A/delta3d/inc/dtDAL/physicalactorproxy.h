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
#ifndef _PHYSICAL_ACTOR_PROXY_H_
#define _PHYSICAL_ACTOR_PROXY_H_

#include "dtDAL/plugin_export.h"
#include "dtDAL/transformableactorproxy.h"
#include <dtCore/physical.h>
#include <dtUtil/enumeration.h>

namespace dtDAL
{

    /**
     * This proxy wraps the Physical Delta3D object.
     */
    class DT_DAL_EXPORT PhysicalActorProxy : public dtDAL::TransformableActorProxy
    {
    public:

        /*
         * We need an enumeration to allow the user to set which type
         * of collision geometry to use.  The other properties in this
         * proxy such as radius, length, etc. affect the current type
         * of collision geometry.
         */
        class DT_DAL_EXPORT CollisionGeomType : public dtUtil::Enumeration
        {
            DECLARE_ENUM(CollisionGeomType);
        public:
            static CollisionGeomType NONE;
            static CollisionGeomType SPHERE;
            static CollisionGeomType CYLINDER;
            static CollisionGeomType CUBE;
            static CollisionGeomType RAY;
            static CollisionGeomType MESH;

        private:
            CollisionGeomType(const std::string &name) : dtUtil::Enumeration(name)
            {
                AddInstance(this);
            }
        };

        /**
         * Constructor
         */
        PhysicalActorProxy()
        {
            SetClassName("dtCore::Physical");
            mCollisionType = &CollisionGeomType::NONE;
            mCollisionRadius = mCollisionLength = 0.0f;
            mCollisionBoxDims = osg::Vec3(0,0,0);
        }

        /**
         * Adds the properties that are common to all Delta3D physical objects.
         */
        virtual void BuildPropertyMap();

        /**
         * Sets the mass of an object
         * @param mass the desired mass
         */
        void SetMass(float mass);

        /**
         * Gets the mass of an object
         * @return The current mass
         */
        float GetMass() const;

        /**
         * Enables the rendering of an object's collision geometry
         * @param enable enable rendering
         */
        void SetRenderCollisionGeometry(bool enable);

        /**
         * Determines if an object's collision geometry is rendering
         * @return If geometry is currently rendering
         */
        bool GetRenderCollisionGeometry() const;

        /**
         * Sets the center of gravity of an object
         * @param g the desired center of gravity
         */
        void SetCenterOfGravity(const osg::Vec3 &g);

        /**
         * Gets the center of gravity of an object
         * @return The current center of gravity
         */
        osg::Vec3 GetCenterOfGravity() const;

        /**
         * Sets the type of collision geometry to use for this object.
         * @param type Enumeration depicting the type of collision to use.
         */
        void SetCollisionType(CollisionGeomType &type);

        /**
         * Gets the current collision geometry type.
         * @return An enumeration of the type of geometry.
         */
        CollisionGeomType &GetCollisionType();

        /**
         * Sets the collision radius.  This is used if either sphere or cylinder collision
         * geometry is specified.
         * @param radius The new radius.
         */
        void SetCollisionRadius(float radius);

        /**
         * Gets the current collision radius.
         * @return The collision radius.
         */
        float GetCollisionRadius() const;

        /**
         * Sets the dimension of the bounding box if box collision is used.
         * @param dims The new dimensions of the bounding box. (width,depth,height)
         */
        void SetCollisionBoxDims(const osg::Vec3 &dims);

        /**
         * Gets the bounding box volume.
         * @return The volume defining the bounding box used for collision.
         */
        osg::Vec3 GetCollisionBoxDims() const;

        /**
         * Sets the collision length.  This is used if either cylinder or ray collision
         * geometry is specified.
         * @param length The new length.
         */
        void SetCollisionLength(float length);

        /**
         * Get the current collision length.
         * @return The collision length.
         */
        float GetCollisionLength() const;

    protected:

        /**
         * Destructor
         */
        virtual ~PhysicalActorProxy() {}

        /**
         * Initializes the actor.
         */
        virtual void CreateActor() = 0;

        ///Sets box collision geometry.
        void SetBoxCollision();

        ///Sets sphere collision geometry.
        void SetSphereCollision();

        ///Sets cylinder collision geometry.
        void SetCylinderCollision();

        ///Sets ray collision geometry.
        void SetRayCollision();

        ///Sets mesh collision geometry.
        void SetMeshCollision();

    private:
        osg::Vec3 mCollisionBoxDims;
        float mCollisionRadius;
        float mCollisionLength;
        CollisionGeomType *mCollisionType;
    };
}

#endif
