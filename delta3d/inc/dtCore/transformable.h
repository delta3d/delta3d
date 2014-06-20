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
 * ANY WARRANTY; without even the implied warranty lof MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef DELTA_TRANSFORMABLE
#define DELTA_TRANSFORMABLE

#include <ode/common.h>
#include <dtCore/deltadrawable.h>
#include <dtUtil/enumeration.h>
#include <dtCore/collisiongeometrytypeenum.h>

// Must include because it's a typedef
#include <osg/Matrix>
/// @cond DOXYGEN_SHOULD_SKIP_THIS
struct dContact;
namespace osg
{
   class MatrixTransform;
}

/// @endcond

namespace dtCore
{
   class ODEGeomWrap;
   class PointAxis;

   class Transform;
   class TransformableImpl;

   /**
    * The Transformable class is the base class of anything that can move in the
    * virtual world and can be added to the Scene.
    *
    * The default coordinate system of dtCore is +X to the right, +Y forward into
    * the screen, and +Z is up.  Therefore, heading is around the Z axis, pitch
    * is around the X axis, and roll is around the Y axis.  The angles are all
    * right-hand-rule.
    *
    * The Transformable class creates a osg::MatrixTransform node for the
    * protected member mNode.
    */
   class DT_CORE_EXPORT Transformable : public DeltaDrawable
   {
   public:

      DECLARE_MANAGEMENT_LAYER(Transformable)

      enum CoordSysEnum
      {
         REL_CS, ///< The Transform coordinate system is relative to the parent
         ABS_CS  ///< The Transform coordinate system is absolute
      };

      ///Used to identify the collision geometry node if RenderCollisionGeometry is
      ///set to true.
      static const std::string COLLISION_GEODE_ID;

      Transformable(const std::string& name = "Transformable");

      typedef osg::MatrixTransform TransformableNode;

      /**
       * Overloaded constructor will use the supplied node instead of
       * creating one internally.
       * @param node : A node this class should use internally
       * @param name : The name of this instance
       */
      Transformable(TransformableNode& node, const std::string& name = "Transformable");

   protected:
      virtual ~Transformable();

   public:
      /**
       * Add a child to this Transformable.  This will allow the child to be
       * repositioned whenever the parent moves.  An optional offset may be applied to
       * the child.  Any number of children may be added to a parent.
       * The child's position in relation to the parent's will not change (ie: the
       * child will *not* snap to the parent's position) unless the offset is
       * overwritten using SetTransform() on the child.
       *
       * @param *child : The child to add to this Transformable
       * @return : successfully added the child or not
       * @see SetTransform()
       * @see RemoveChild()
       */
      virtual bool AddChild(DeltaDrawable* child);

      /**
       * Remove a child from this Transformable.  This will detach the child from its
       * parent so that its free to be repositioned on its own.
       *
       * @param *child : The child Transformable to be removed
       */
      virtual void RemoveChild(DeltaDrawable* child);

      /**
       * Set position/attitude of this Transformable using the supplied Transform.
       * An optional coordinate system parameter may be supplied to specify whether
       * the Transform is in relation to this Transformable's parent.
       *
       * If the CoordSysEnum is ABS_CS,
       * then the Transformable is positioned assuming
       * absolute world coordinates and the Transformable parent/child relative
       * position is recalculated.
       * If the CoordSysEnum is REL_CS, then the Transformable is positioned relative
       * to it's parent's Transform. (Note - if REL_CS is supplied and the Transformable
       * does not have a parent, the Transform is assumed to be an absolute world
       * coordinate.
       *
       * @param *xform : The new Transform to position this instance
       * @param cs : Optional parameter describing the coordinate system of xform
       *             Defaults to ABS_CS.
       */
      virtual void SetTransform(const Transform& xform, CoordSysEnum cs = ABS_CS);

      /**
       * Get the current Transform of this Transformable.
       *
       * @param xform The value will by assigned to this reference.
       * @param cs The coordinate system of the returned Transform. For absolute,
       * use ABS_CS, and for relative, us REL_CS.
       */
      virtual void GetTransform(Transform& xform, CoordSysEnum cs = ABS_CS) const;

      ///Convenience function for easy type conversion without needing a dynamic cast
      virtual dtCore::Transformable* AsTransformable() { return this; }

      ///Convenience function to return back the internal matrix transform node
      TransformableNode* GetMatrixNode();

      ///Convenience function to return back the internal matrix transform node
      const TransformableNode* GetMatrixNode() const;

      /// @returns the matrix for this transformable.  Call this instead of getMatrixNode->getMatrix
      const osg::Matrix& GetMatrix() const;

      /// set the matrix for this transformable.  Call this instead of getMatrixNode->setMatrix
      void SetMatrix(const osg::Matrix& mat);

      ///Render method for an object which may not have geometry
      virtual void RenderProxyNode(bool enable = true);

      /// Returns if we are rendering the proxy node
      virtual bool GetIsRenderingProxyNode() const;

      /**
       * Gets the world coordinate matrix for the supplied node.
       *
       * @param node The node to start from
       * @param wcMatrix The matrix to fill
       * @return Success
       */
      static bool GetAbsoluteMatrix(const osg::Node* node, osg::Matrix& wcMatrix);

      ///Automatically rescales normals if you scale your objects.
      void SetNormalRescaling(bool enable);

      ///Checks if we are rescaling normals for this object.
      bool GetNormalRescaling() const;

      /**
       * Returns the ODE geometry identifier associated with this
       * object.
       *
       * @return the object's geometry identifier
       */
      dGeomID GetGeomID() const;

      /**
       * Returns the type of collision geometry associated with this
       * object.
       *
       * @return the object's collision geometry type
       */
      CollisionGeomType* GetCollisionGeomType() const;

      /**
       * Returns the dimensions of collision geometry associated with this
       * object. Note: This is not const function since it requires an update
       * to the underlying ODE geometry if we are out of sync with OSG.
       *
       * @param dimensions The dimenstions of the object's collision geometry.
       * What is filled into the vector is dependent on Collision type.
       * CUBE     : ( lx, ly, lz )
       * SPHERE   : ( radius )
       * CYLINDER : ( radius, length )
       * CCYLINDER: ( radius, length )
       * RAY      : ( length, start_x, start_y, start_z, dir_x, dir_y, dir_z )
       */
      void GetCollisionGeomDimensions(std::vector<float>& dimensions);

      /**
       * Sets whether or not collisions detection will be performed.
       * Note: This does not handle collisions in any way, the user
       * is still responsible for implementing collision response
       * (or just use Physical). Collision detected is enabled by default
       * if a collision shape has been set.
       *
       * @param enabled true if the Transformable will perform collision detection
       */
      void SetCollisionDetection(bool enabled);

      /**
       * Gets whether or not collisions with other Transformables
       * will be detected.
       *
       * @return true if the Transformable will perform collision detection
       */
      bool GetCollisionDetection() const;

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
       * (if 0, attempt to use own node)
       */
      void SetCollisionSphere(osg::Node* node = 0);

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
       * (if 0, attempt to use own node)
       */
      void SetCollisionBox(osg::Node* node = 0);

      /**
       * Sets this object's collision geometry to a cylinder
       * (oriented along the z axis) with the specified radius and length.
       *
       * @param radius the radius of the cylinder
       * @param length the length of the cylinder
       */
      void SetCollisionCylinder(float radius, float length);

      /**
       * Sets this object's collision geometry to a cylinder with
       * parameters derived from the given OpenSceneGraph node.
       *
       * @param node the node from which to obtain the cylinder parameters
       * (if 0, attempt to use own node)
       */
      void SetCollisionCylinder(osg::Node* node = NULL);

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
       * (if 0, attempt to use own node)
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
       * (if 0, attempt to use own node)
       */
      void SetCollisionMesh(osg::Node* node = NULL);

      /**
       * Removes any collision geometry specified for this object.
       */
      void ClearCollisionGeometry();

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
       * This is false by default since most things that move should not
       * cause collisions (Lights, Isector's, Camera, etc.)
       *
       * @param contact the joint definition to modify
       * @param collider the object with which this is colliding
       * @return true to keep the contact joint as modified,
       * false to cancel it
       */
      virtual bool FilterContact(dContact* /*contact*/, Transformable* /*collider*/) { return false; }

      /**
       * Updates the state of this object just after a physical
       * simulation step.  Should only be called by dtCore::Scene.
       * The default implementation here does nothing. Physical
       * overrides it and copies the new object position into the
       * user-accessible transformation.
       */
      virtual void PostPhysicsStepUpdate() {}

      /**
       * Enable or disable the rendering of the collision geometry.
       * This will draw a purple outline of shape the collision
       * detection routine is using.
       *
       * @param enable If true, the scene will now render the collision
       * geometry.
       * @param wireFrame Determines whether collision geometry is rendered as
       * wire frame or as solid transparent.
       */
      void RenderCollisionGeometry(bool enable = true, bool wireFrame = false);

      /**
       * Are we currently rendering the collision geometry?
       * @return True if we are rendering collision geometry.
       */
      bool GetRenderCollisionGeometry() const;

      /**
       * This typically gets called from Scene::AddChild().
       *
       * This method perform the standard DeltaDrawable::AddedToScene() functionality
       * then registers this Transformable object with the supplied Scene to create the
       * internal physical properties.
       *
       * If the param scene is 0, this will unregister this Transformable from the
       * previous parent Scene.
       * If this Transformable already has a parent Scene, it will
       * first remove itself from the old Scene (Scene::RemoveDrawable()), then
       * re-register with the new Scene.
       *
       * @param scene The Scene this Transformable has been added to
       */
      virtual void AddedToScene(Scene* scene);

      /**
       * Set the category bits of this collision geom.
       * The defaults are listed in collisioncategorydefaults.h.
       *
       * Set the collide bits of this collision geom. If you want this geom to
       * collide with a geom of category bit 00000010 for example, make sure these
       * collide bits contain 00000010. The UNSIGNED_BIT macro in dtCore/macros.h
       * comes in handy here. UNSIGNED_BIT(4) = 00000100
       */
      void SetCollisionCategoryBits(unsigned long bits);

      unsigned long GetCollisionCategoryBits() const;

      /**
       * Set the collide bits of this collision geom. If you want this geom to
       * collide with a geom of category bit 00000010 for example, make sure these
       * collide bits contain 00000010. The UNSIGNED_BIT macro in dtCore/macros.h
       * comes in handy here. UNSIGNED_BIT(4) = 00000100
       */
      void SetCollisionCollideBits(unsigned long bits);

      unsigned long GetCollisionCollideBits() const;

      ///required by DeltaDrawable
      osg::Node* GetOSGNode();
      const osg::Node* GetOSGNode() const;

      /**
       * Get the const internal ODE Geom wrapper object.
       * @return The ODE Geom collision wrapper
       */
      const ODEGeomWrap* GetGeomWrapper() const;

      /**
       * Get the internal ODE Geom wrapper object.
       * @return The ODE Geom collision wrapper
       */
      ODEGeomWrap* GetGeomWrapper();

   private:
      void Ctor();
      TransformableImpl* mImpl;

      ///little util to remove any of the rendered collision geometry
      void RemoveRenderedCollisionGeometry();
   };

} // namespace dtCore

#endif // DELTA_TRANSFORMABLE
