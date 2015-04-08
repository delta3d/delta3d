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
 * William E. Johnson II
 */

#ifndef DELTA_TRANSFORMABLE_ACTOR_PROXY
#define DELTA_TRANSFORMABLE_ACTOR_PROXY

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Node>

#include <dtCore/actorproxy.h>
#include <dtCore/export.h>
#include <dtCore/resourcedescriptor.h>
#include <dtCore/collisiongeometrytypeenum.h>

#include <dtUtil/getsetmacros.h>

namespace dtCore
{
   /**
    * This proxy wraps the Transformable Delta3D object.
    */
   class DT_CORE_EXPORT TransformableActorProxy : public dtCore::BaseActorObject
   {
      public:

         static const dtUtil::RefString PROPERTY_ROTATION;
         static const dtUtil::RefString PROPERTY_TRANSLATION;
         static const dtUtil::RefString PROPERTY_NORMAL_RESCALING;
         static const dtUtil::RefString PROPERTY_RENDER_PROXY_NODE;
         static const dtUtil::RefString PROPERTY_ENABLE_COLLISION;
         static const dtUtil::RefString PROPERTY_COLLISION_TYPE;
         static const dtUtil::RefString PROPERTY_COLLISION_RADIUS;
         static const dtUtil::RefString PROPERTY_COLLISION_LENGTH;
         static const dtUtil::RefString PROPERTY_COLLISION_BOX;
         static const dtUtil::RefString PROPERTY_COLLISION_CATEGORY_MASK;
         static const dtUtil::RefString PROPERTY_COLLISION_COLLIDE_MASK;

         /**
          * Constructor
          */
         TransformableActorProxy();

         /**
          * Adds the properties that are common to all Delta3D transformable objects.
          */
         virtual void BuildPropertyMap();

         /**
          * Transformables can be placed in a scene
          */
         virtual bool IsPlaceable() const { return true; }

         /**
          *  Called when this BaseActorObject has finished loading from a Map and
          *  all Property values have been assigned.  Overwrite for custom
          *  behavior.
          */
         virtual void OnMapLoadEnd();

         /**
         * Determines if the translation should be shown in the actor properties.
         */
         virtual bool IsTranslationPropertyShown() const { return true; }

         /**
         * Determines if the rotation should be shown in the actor properties.
         */
         virtual bool IsRotationPropertyShown() const { return true; }

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
         osg::Vec3 GetRotation() const;

         /**
          * Sets the translation of an object
          * @param translation the desired translation
          */
         void SetTranslation(const osg::Vec3 &translation);

         /**
          * Gets the translation of an object
          * @return The current translation
          */
         osg::Vec3 GetTranslation() const;

         /**
          * Called when the SetRotation function is called.  The rotation will
          * be passed in using x,y,z order (p,r,h)
          * @param oldValue The previous value
          * @param newValue The new value
          */
         virtual void OnRotation(const osg::Vec3& /*oldValue*/, const osg::Vec3& /*newValue*/) { }

         /**
          * Called when the SetTranslation function is called
          * @param oldValue The previous value
          * @param newValue The new value
          */
         virtual void OnTranslation(const osg::Vec3& /*oldValue*/, const osg::Vec3& /*newValue*/) { }


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
         void SetCollisionType(dtCore::CollisionGeomType& type);

         /**
          * Gets the current collision geometry type.
          * @return An enumeration of the type of geometry.
          */
         dtCore::CollisionGeomType& GetCollisionType();

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

         /**
          * Sets the collision category bit mask.
          * @param mask The mask.
          */
         void SetCollisionCategoryMask(unsigned int mask);

         /**
          * Get the collision category bit mask.
          * @return The mask.
          */
         unsigned int GetCollisionCategoryMask() const;

         /**
          * Sets the collision collide bit mask.
          * @param mask The mask.
          */
         void SetCollisionCollideMask(unsigned int mask);

         /**
          * Get the collision collide bit mask.
          * @return The mask.
          */
         unsigned int GetCollisionCollideMask() const;

         /**
          * Get the collision bit mask list.
          * @param The mask names.
          * @param The mask values.
          */
         void GetCollisionMaskList(std::vector<std::string>& names, std::vector<unsigned int>& values) const;

         /**
          * Set this to true if your actors don't use the dt core physics.
          * You need to set this in the constructor or at the beginning of BuildPropertyMap.
          */
         DT_DECLARE_ACCESSOR(bool, HideDTCorePhysicsProps);

         /**
          * An optional StaticMesh Resource which can be used for collision detection
          * geometry. If this is set, this mesh will be used instead of the Node
          * returned by GetOSGNode(). This mesh will not be rendered; only used
          * for collision detection shapes.
          */
         DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, AltCollisionMesh);

      protected:

         /**
          * Destructor
          */
         virtual ~TransformableActorProxy() {}

         ///Sets box collision geometry.
         void SetBoxCollision();

         ///Sets sphere collision geometry.
         void SetSphereCollision();

         ///Sets cylinder collision geometry.
         void SetCapsuleCollision();

         ///Sets ray collision geometry.
         void SetRayCollision();

         ///Sets mesh collision geometry.
         void SetMeshCollision();


      private:

         osg::Vec3 mCollisionBoxDims;
         float mCollisionRadius;
         float mCollisionLength;
         dtCore::CollisionGeomType* mCollisionType;
         osg::ref_ptr<osg::Node> mAltCollisionGeometry;

         osg::Node* LoadAltCollisionMesh(const dtCore::ResourceDescriptor& resource);
   };
}

#endif
