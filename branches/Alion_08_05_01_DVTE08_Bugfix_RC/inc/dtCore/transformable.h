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
#include <ode/contact.h>
#include <ode/collision_trimesh.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/transform.h>
#include <dtUtil/enumeration.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class MatrixTransform;
   class Geode;
}
/// @endcond

namespace dtCore
{
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
      /**
       * We need an enumeration to allow the user to set which type
       * of collision geometry to use.  The other properties in this
       * proxy such as radius, length, etc. affect the current type
       * of collision geometry.
       */
      class DT_CORE_EXPORT CollisionGeomType : public dtUtil::Enumeration
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

      DECLARE_MANAGEMENT_LAYER(Transformable)

      enum CoordSysEnum
      {
         REL_CS, ///< The Transform coordinate system is relative to the parent
         ABS_CS  ///< The Transform coordinate system is absolute
      } ;

      ///Used to identify the collision geometry node if RenderCollisionGeometry is
      ///set to true.
      static const std::string COLLISION_GEODE_ID;

      Transformable( const std::string& name = "Transformable" );

      typedef osg::MatrixTransform TransformableNode;

      /** Overloaded constructor will use the supplied node instead of
      * creating one internally.
      * @param node : A node this class should use internally
      * @param name : The name of this instance
      */
      Transformable( TransformableNode &node, const std::string &name = "Transformable" );

   protected:

      /**
       * Replaces the scene graph node with a new MatrixTransform.
       * WARNING! This function is a big fat hack to get RepliacntBody
       * to play nice with our API. This will almost certainly be removed
       * in future version, so don't get to comfy using it! ;) Properties
       * set on this Transformable are merged onto the new node, however
       * the old collision geometry will remain.
       *
       * @param matrixTransform The node you wish to squeeze into this
       * Transformable.
       * @pre matrixTransform != NULL
       */
      void ReplaceMatrixNode( TransformableNode* matrixTransform );

      virtual ~Transformable();

   public:

      ///Add a DeltaDrawable child
      virtual bool AddChild( DeltaDrawable* child );
         
      ///Remove a DeltaDrawable child
      virtual void RemoveChild( DeltaDrawable* child );

      /**
       * Sets the Transform to reposition this Transformable. Note that if this
       * function is overriden, then GetMatrix()->getMatrix() may return diffirent
       * values.
       *
       * @param xform The value to set on this Transformable.
       * @param cs The coordinate system of the returned Transform. For absolute,
       * use ABS_CS, and for relative, us REL_CS.
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

      ///Convenience function to return back the internal matrix transform node
      TransformableNode* GetMatrixNode()
      { 
         return mNode.get(); 
      }

      ///Convenience function to return back the internal matrix transform node
      const TransformableNode* GetMatrixNode() const
      { 
         return mNode.get(); 
      }

      /// @returns the matrix for this transformable.  Call this instead of getMatrixNode->getMatrix
      const osg::Matrix& GetMatrix() const;

      /// set the matrix for this transformable.  Call this instead of getMatrixNode->setMatrix
      void SetMatrix(const osg::Matrix& mat);
      
      ///Render method for an object which may not have geometry
      virtual void RenderProxyNode( bool enable = true );

      /// Returns if we are rendering the proxy node
      virtual bool GetIsRenderingProxyNode() const { return mRenderProxyNode; }
      
      /**
       * Gets the world coordinate matrix for the supplied node.
       *
       * @param node The node to start from
       * @param wcMatrix The matrix to fill
       * @return Success
       */
      static bool GetAbsoluteMatrix( const osg::Node* node, osg::Matrix& wcMatrix );

      ///Automatically rescales normals if you scale your objects.
      void SetNormalRescaling( bool enable );

      ///Checks if we are rescaling normals for this object.
      bool GetNormalRescaling() const;

      /**
       * Returns the ODE geometry identifier associated with this
       * object.
       *
       * @return the object's geometry identifier
       */
      dGeomID GetGeomID() const { return mGeomID; }

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
       * RAY      : ( length, start_x, start_y, start_z, dir_x, dir_y, dir_z )
       */
      void GetCollisionGeomDimensions( std::vector<float>& dimensions );

      /**
       * Sets whether or not collisions detection will be performed.
       * Note: This does not handle collisions in any way, the user
       * is still responsible for implementing collision response
       * (or just use Physical). Collision detected is enabled by default
       * if a collision shape has been set.
       *
       * @param enabled true if the Transformable will perform collision detection
       */
      void SetCollisionDetection( bool enabled );

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
      virtual bool FilterContact(dContact* contact, Transformable* collider) { return false; }

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
       * geomertry.
       */
      void RenderCollisionGeometry( bool enable = true );

      /** 
       * Are we currently rendering the collision geometry?
       * @return True if we are rendering collision geometry.
       */
      bool GetRenderCollisionGeometry() const { return mRenderingGeometry; }

      /** 
       * Supply the Scene this Transformable has been added to. Normally this
       * is done inside Scene::RegisterCollidable.
       * @param scene The Scene to which this Transformable is being added to.
       */
      virtual void AddedToScene( Scene* scene );
      
      /** 
       * Set the category bits of this collision geom. Here's the defaults:
       *
       * dtABC::ProximityTrigger  0
       *
       * dtCore::Camera:          1  
       * dtCore::Compass:         2
       * dtCore::InfiniteTerrain: 3
       * dtCore::ISector:         4
       * dtCore::Object:          5
       * dtCore::ParticlSsystem:  6
       * dtCore::Physical:        7
       * dtCore::PointAxis:       8
       * dtCore::PositionalLight: 9
       * dtCore::SpotLight:       10
       * dtCore::Transformable:   11
       * 
       * dtChar::Character:       12
       * dtAudio::Listener:       13
       * dtAudio::Sound:          14
       * dtHLA::Entity:           15
       * dtTerrain::Terrain:      16
       * 
       */
      void SetCollisionCategoryBits( unsigned long bits );
      
      unsigned long GetCollisionCategoryBits() const;
      
      /** 
       * Set the collide bits of this collision geom. If you want this geom to
       * collide with a geom of category bit 00000010 for example, make sure these
       * collide bits contain 00000010. The UNSIGNED_BIT macro in dtCore/macros.h
       * comes in handy here. UNSIGNED_BIT(4) = 00000100
       */
      void SetCollisionCollideBits( unsigned long bits );
      
      unsigned long GetCollisionCollideBits() const;

      ///required by DeltaDrawable
      osg::Node* GetOSGNode();
      const osg::Node* GetOSGNode() const;
            
   protected:
      
      /**
       * The last geometry transform reported to ODE.
       */
      Transform mGeomTransform; 

   private:
      void Ctor();

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

      /**
      * The node passed on GetOSGNode()
      */
      RefPtr<TransformableNode> mNode;

      /**
       * If we're rendering the collision geometry.                                                                    
       */
      bool mRenderingGeometry;

      /**
       * If we're rendering the proxy node
       */
      bool mRenderProxyNode; 

      ///little util to remove any of the rendered collision geometry
      void RemoveRenderedCollisionGeometry();

   };
}
#endif // DELTA_TRANSFORMABLE
