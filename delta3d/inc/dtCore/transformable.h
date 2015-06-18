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

#include <dtCore/deltadrawable.h>
#include <dtUtil/enumeration.h>

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
       * @param stopNode if the code hits this optional node, it will stop and compute the matrix up to, but not including, the stopNode.
       * @return Success
       */
      static bool GetAbsoluteMatrix(const osg::Node* node, osg::Matrix& wcMatrix, const osg::Node* stopNode = NULL);

      ///Automatically rescales normals if you scale your objects.
      void SetNormalRescaling(bool enable);

      ///Checks if we are rescaling normals for this object.
      bool GetNormalRescaling() const;
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

      ///required by DeltaDrawable
      osg::Node* GetOSGNode();
      const osg::Node* GetOSGNode() const;

   private:
      void Ctor();
      TransformableImpl* mImpl;
   };

} // namespace dtCore

#endif // DELTA_TRANSFORMABLE
