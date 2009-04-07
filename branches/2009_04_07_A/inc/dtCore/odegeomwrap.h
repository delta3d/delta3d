#ifndef odegeomwrap_h__
#define odegeomwrap_h__

#include <dtCore/export.h>
#include <dtCore/refptr.h>
#include <osg/Referenced>
#include <ode/common.h>
#include <ode/collision_trimesh.h>
#include <dtUtil/enumeration.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Node;
   class Geode;
}
/// @endcond

namespace dtCore
{
   class Transform;

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
      static CollisionGeomType CCYLINDER;
      static CollisionGeomType CUBE;
      static CollisionGeomType RAY;
      static CollisionGeomType MESH;

   private:
      CollisionGeomType(const std::string& name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
   };


   /** Class used to wrap up the functionality of the ODE Geom.  Used by Transformable
     * to provide collision detection.
     */
   class DT_CORE_EXPORT ODEGeomWrap : public osg::Referenced
   {
   public:

      ODEGeomWrap();

      /**
       * Returns the ODE geometry identifier associated with this
       * object.
       *
       * @return the object's geometry identifier
       */
      dGeomID GetGeomID() const;

      ///temporary.  Needs to go away.
      dGeomID GetOriginalGeomID() const;

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
       * (if 0, attempt to use own node)
       */
      void SetCollisionBox(osg::Node* node = NULL);

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


      /** Create a renderable shape to match the collision shape.
        * ODEGeomWrap does not maintain ownership of the returned
        * object.
        * @return A renderable geometry node which matches the current
        * collision geometry shape.
        */
      dtCore::RefPtr<osg::Geode> CreateRenderedCollisionGeometry() const;

      /** If needed, set the ODE Geom position and rotation to match the
        * supplied Transform. If a Transformable's position or rotation is manually
        * set, then this method should be called to keep the ODE Geom
        * in sync.
        * @param newTransform The potentially new position/rotation of the Transformable
        */
      void UpdateGeomTransform(const dtCore::Transform& newTransform);

      /** Copy the ODE Geom's current rotation and translation into
        * the supplied Transform.
        * @param xform The Transform to copy the values into.
        */
      void GetGeomTransform(dtCore::Transform& xform) const;

   protected:
      virtual ~ODEGeomWrap();

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
       * The heap-allocated array of mesh vertices.
       */
      dVector3* mMeshVertices;

      /**
       * The heap-allocated array of mesh indices.
       */
      int* mMeshIndices;

   };
}

#endif // odegeomwrap_h__

