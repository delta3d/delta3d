/* -*-c++-*-
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
#ifndef DELTA_TRANSFORMABLE_ACTOR_PROXY
#define DELTA_TRANSFORMABLE_ACTOR_PROXY

#include <osg/Vec3>
#include <osg/Matrix>

#include <dtDAL/actorproxy.h>
#include <dtDAL/export.h>
#include <dtCore/transformable.h>

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
         TransformableActorProxy()
         { 
            SetClassName("dtCore::Transformable");             
            mCollisionType = &dtCore::Transformable::CollisionGeomType::NONE;
            mCollisionRadius = mCollisionLength = 0.0f;
            mCollisionBoxDims = osg::Vec3(0,0,0);
         }

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

         /**
          * Called when the SetScale function is called
          * @param oldValue The previous value
          * @param newValue The new value
          */
         virtual void OnScale(const osg::Vec3 &oldValue, const osg::Vec3 &newValue) { }

         /**
          * Called when the SetRotation function is called
          * @param oldValue The previous value
          * @param newValue The new value
          */
         virtual void OnRotation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue) { }

         /**
          * Called when the SetScale function is called
          * @param oldValue The previous value
          * @param newValue The new value
          */
         virtual void OnTranslation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue) { }


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
          * Sets the type of collision geometry to use for this object.
          * @param type Enumeration depicting the type of collision to use.
          */
         void SetCollisionType(dtCore::Transformable::CollisionGeomType &type);

         /**
          * Gets the current collision geometry type.
          * @return An enumeration of the type of geometry.
          */
         dtCore::Transformable::CollisionGeomType &GetCollisionType();

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
         virtual ~TransformableActorProxy() {}

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

         osg::Vec3 mHPR;

      private:
         osg::Vec3 mCollisionBoxDims;
         float mCollisionRadius;
         float mCollisionLength;
         dtCore::Transformable::CollisionGeomType *mCollisionType;
   };
}

#endif
