// physical.h: Declaration of the Physical class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_PHYSICAL
#define DELTA_PHYSICAL

#include "transform.h"

#include <osg/Node>
#include <osg/Geode>

#include <sg.h>

//extern "C"
//{
   #include <ode/ode.h>
   #include <ode/collision_trimesh.h>
//}

namespace dtCore
{
   /**
    * A physical object.
    */
   class DT_EXPORT Physical
   {
      public:
      
         /**
          * Constructor.
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
         void SetCenterOfGravity(const sgVec3 centerOfGravity);
         
         /**
          * Retrieves this object's center of gravity.
          *
          * @param dest the vector in which to place the center
          * of gravity
          */
         void GetCenterOfGravity(sgVec3 dest) const;
         
         /**
          * Sets this object's inertia tensor.
          *
          * @param inertiaTensor the new inertia tensor
          */
         void SetInertiaTensor(const sgMat3 inertiaTensor);
         
         /**
          * Retrieves this object's inertia tensor.
          *
          * @param dest the matrix in which to place the inertia
          * tensor
          */
         void GetInertiaTensor(sgMat3 dest) const;
         
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
         void RenderCollisionGeometry( const bool enable = true);

         ///Are we currently rendering the collision geometry?
         bool GetRenderCollisionGeometry(void)const {return mRenderingGeometry;}

      private:
      
         /**
          * The ODE geometry identifier of the geometry transform.
          */
         dGeomID mGeomID;
         
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
         osg::ref_ptr<osg::Geode> mGeomGeod;

         bool mRenderingGeometry;///<if we're rendering the collision geometry
   };
};

#endif // DELTA_PHYSICAL