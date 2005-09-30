/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
*/

#ifndef DELTA_PHYSICAL
#define DELTA_PHYSICAL

// physical.h: Declaration of the Physical class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/transformable.h"
#include "dtUtil/deprecationmgr.h"
#include <osg/Node>
#include <osg/Geode>
#include <osg/Vec3>

//extern "C" {
   #include <ode/ode.h>
   #include <ode/collision_trimesh.h>
//}

#include <osg/NodeVisitor>
#include <osg/Billboard>
#include <osg/MatrixTransform>

namespace dtCore
{
   /**
    * A rigid body object which can behave with realistic physical properties.
    * Typical use would involve supplying the shape to use for the collision
    * detection and the mass of the body.  A Physical instance is not enabled
    * by default and must be enabled by calling EnableDynamics().
    *
    * Since a Physical is derived from DeltaDrawable, it already has a
    * geometry node associated with it.  As such, a collision geometry can be
    * assigned to this body by either supplying the shape properties, or by
    * passing a NULL into the SetCollision*() methods.
    *
    */
   class DT_EXPORT Physical : public Transformable
   {
       DECLARE_MANAGEMENT_LAYER(Physical)

      public:

         ///Used to identify the collision geometry node if RenderCollisionGeometry is
         ///set to true.
         static const std::string COLLISION_GEODE_ID;

         /**
          * Default Constructor.
          */
         Physical();

         /**
          * Destructor.
          */
         virtual ~Physical();

         /**
          * Returns the ODE geometry identifier associated with this
          * object.
          *
          * @return the object's geometry identifier
          */
         dGeomID GetGeomID() const;

         /**
          * Sets the ODE body identifier associated with this object.  Should
          * only be called by dtCore::Scene.
          *
          * @param bodyID the new body identifier
          */
         void SetBodyID(dBodyID bodyID);

         /**
          * Returns the ODE body identifier associated with this
          * object.
          *
          * @return the object's body identifier
          */
         dBodyID GetBodyID() const;

         /**
          * Sets this object's collision geometry to the specified ODE
          * geom.
          *
          * @param geom the new collision geom
          */
         void SetCollisionGeom(dGeomID geom);

         /**
          * Sets this object's collision geometry to a sphere with the
          * specified radius.
          *
          * @param radius the radius of the collision sphere
          */
         void SetCollisionSphere(float radius);

         /**
          * Sets this object's collision geometry to a sphere with
          * radius derived from the specified OpenSceneGraph node.
          *
          * @param node the node from which to obtain the sphere radius
          * (if NULL, attempt to use own node)
          */
         void SetCollisionSphere(osg::Node* node = NULL);

         /**
          * Sets this object's collision geometry to a box with the
          * specified dimensions.
          *
          * @param lx the length of the box in the x direction
          * @param ly the length of the box in the y direction
          * @param lz the length of the box in the z direction
          */
         void SetCollisionBox(float lx, float ly, float lz);

         /**
          * Sets this object's collision geometry to a box with parameters
          * derived from the specified OpenSceneGraph node.
          *
          * @param node the node from which to obtain the box parameters
          * (if NULL, attempt to use own node)
          */
         void SetCollisionBox(osg::Node* node = NULL);

         /**
          * Sets this object's collision geometry to a capped cylinder
          * (oriented along the z axis) with the specified radius and length.
          *
          * @param radius the radius of the cylinder
          * @param length the length of the cylinder
          */
         void SetCollisionCappedCylinder(float radius, float length);

         /**
          * Sets this object's collision geometry to a capped cylinder with
          * parameters derived from the given OpenSceneGraph node.
          *
          * @param node the node from which to obtain the cylinder parameters
          * (if NULL, attempt to use own node)
          */
         void SetCollisionCappedCylinder(osg::Node* node = NULL);

         /**
          * Sets this object's collision geometry to a ray (along the z axis)
          * with the specified length.
          *
          * @param length the length of the ray
          */
         void SetCollisionRay(float length);

         /**
          * Sets this object's collision geometry to a triangle mesh derived
          * from the given OpenSceneGraph node.
          *
          * @param node the node from which to obtain the mesh data
          * (if NULL, attempt to use own node)
          */
         void SetCollisionMesh(osg::Node* node = NULL);

         /**
          * Removes any collision geometry specified for this object.
          */
         void ClearCollisionGeometry();

         /**
          * Enables or disables dynamics for this object.
          *
          * @param enable true to enable dynamics, false to disable
          */
         void EnableDynamics(bool enable = true);

         /**
          * Checks whether or not dynamics are enabled for
          * this object.
          *
          * @return true if dynamics are enabled, false otherwise
          */
         bool DynamicsEnabled() const;

         /**
          * Sets the ODE mass parameters of this object.
          *
          * @param mass a pointer to the mass structure to copy
          */
         void SetMass(const dMass* mass);

         /**
          * Retrieves the ODE mass parameters of this object.
          *
          * @param mass a pointer to the mass structure to fill
          */
         void GetMass(dMass* mass) const;

         /**
          * Sets the mass of this object.
          *
          * @param mass the new mass, in kilograms
          */
         void SetMass(float mass);

         /**
          * Returns the mass of this object.
          *
          * @return the current mass
          */
         float GetMass() const;

         /**
          * Sets this object's center of gravity.
          *
          * @param centerOfGravity the new center of gravity
          */
         void SetCenterOfGravity(const osg::Vec3& centerOfGravity);

          /**
          * Retrieves this object's center of gravity.
          *
          * @param dest the vector in which to place the center
          * of gravity
          */
         void GetCenterOfGravity(osg::Vec3& dest) const;


         /**
          * Sets this object's inertia tensor.
          *
          * @param inertiaTensor the new inertia tensor, uses only the rotation part of the transform matrix
          */
         void SetInertiaTensor(const osg::Matrix& inertiaTensor);

         /**
          * Retrieves this object's inertia tensor.
          *
          * @param dest the matrix in which to place the inertia
          * tensor, uses only rotation part of the transform matrix
          */
         void GetInertiaTensor(osg::Matrix& mat) const;

         /**
          * Updates the state of this object just before a physical
          * simulation step.  Should only be called by dtCore::Scene.
          * The default implementation updates the state of the body
          * to reflect any user-applied transformation.
          */
         virtual void PrePhysicsStepUpdate();

         /**
          * Modifies or cancels the specified contact joint definition
          * according to the relationship between this object and the
          * specified collider.  Should only be called by dtCore::Scene.
          *
          * @param contact the joint definition to modify
          * @param collider the object with which this is colliding
          * @return true to keep the contact joint as modified,
          * false to cancel it
          */
         virtual bool FilterContact(dContact* contact, Physical* collider);

         /**
          * Updates the state of this object just after a physical
          * simulation step.  Should only be called by dtCore::Scene.
          * The default implementation copies the new object
          * position into the user-accessible transformation.
          */
         virtual void PostPhysicsStepUpdate();

         /** Enable or disable the rendering of the collision geometry.
          *  This will draw a purple outline of shape the collision
          *  detection routine is using.
          */
         void RenderCollisionGeometry( const bool enable = true );

         ///Are we currently rendering the collision geometry?
         bool GetRenderCollisionGeometry() const { return mRenderingGeometry; }

         ///Tell this Physical is has been added to the supplied Scene
         virtual void AddedToScene( Scene *scene );

      private:

         /**
          * The ODE geometry identifier of the geometry transform.
          */
         dGeomID mGeomID;

         /**
         * The original geometry set in SetCollisionShape functions.
         * The scale values in our matrix transform will be applied to
         * this value to modify the shape of mGeomID.
         */
         dGeomID mOriginalGeomID;

         /**
          * The ODE triangle mesh data identifier, if any.
          */
         dTriMeshDataID mTriMeshDataID;

         /**
          * The ODE body identifier.
          */
         dBodyID mBodyID;

         /**
          * Whether or not dynamics have been enabled for this object.
          */
         bool mDynamicsEnabled;

         /**
          * The mass, center of gravity, and inertia tensor of the body.
          */
         dMass mMass;

         /**
          * The last geometry transform reported to ODE.
          */
         Transform mGeomTransform;

         /**
          * The heap-allocated array of mesh vertices.
          */
         dVector3* mMeshVertices;

         /**
          * The heap-allocated array of mesh indices.
          */
         int* mMeshIndices;

         /**
          *  Pointer to the collision geometry representation
          */
         RefPtr<osg::Geode> mGeomGeod;

         bool mRenderingGeometry;///<if we're rendering the collision geometry

   };
};

class BoundingBoxVisitor : public osg::NodeVisitor
{
public:

   /**
   * Default Constructor.
   */
   BoundingBoxVisitor()
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
   {}

   /**
   * Visits the specified geode.
   *
   * @param node the geode to visit
   */
   virtual void apply(osg::Geode& node)
   {
      osg::Matrix matrix = osg::computeLocalToWorld(getNodePath());

      for(unsigned int i=0;i<node.getNumDrawables();i++)
      {
         for(unsigned int j=0;j<8;j++)
         {
            mBoundingBox.expandBy( node.getDrawable(i)->getBound().corner(j) * matrix );
         }
      }
   }

   /**
   * The aggregate bounding box.
   */
   osg::BoundingBox mBoundingBox;
};

#endif // DELTA_PHYSICAL
