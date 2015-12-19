/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * David Guthrie
 * Bradley Anderegg
 * Allen Danklefsen
 */
#ifndef DELTA_PHYSICS_OBJECTS
#define DELTA_PHYSICS_OBJECTS

#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/mechanicstype.h>
#include <dtPhysics/primitivetype.h>
#include <dtPhysics/geometry.h>

#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtCore/actorproperty.h>
#include <dtCore/propertycontainer.h>
#include <dtCore/motioninterface.h>
#include <dtCore/resourcedescriptor.h>
#include <dtCore/uniqueid.h>
#include <dtUtil/getsetmacros.h>
#include <dtUtil/deprecationmgr.h>
#include <osg/BoundingBox>

class palRevoluteLink;
class palGenericLink;
class palRigidLink;
class palLink;
class palMotor;

namespace dtPhysics
{
   ///////////////////////////
   // forward Declarations
   class BaseBodyWrapper;
   class BodyWrapper;
   class GenericBodyWrapper;
   class JointDesc;
   class MotorDesc;

   struct PhysicsObjectDataMembers;
   /////////////////////////////////////////////////////////////////////////////
   // Class:
   // Notes:
   /////////////////////////////////////////////////////////////////////////////
   class DT_PHYSICS_EXPORT PhysicsObject: public dtCore::PropertyContainer, virtual public dtCore::VelocityInterface, virtual public dtCore::AngularVelocityInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_OVERRIDE_INLINE

   protected:
      PhysicsObject();//name will be generated
      PhysicsObject(const std::string& name);
   public:
      /**
       * The constructor is hidden so that the object can be initialized and the defaults generated before it is used.
       * Call this instead.
       */
      static dtCore::RefPtr<PhysicsObject> CreateNew(const std::string& name = std::string("Default"))
      {
         dtCore::RefPtr<PhysicsObject> obj = new PhysicsObject(name);
         obj->BuildPropertyMap();
         obj->InitDefaults();
         return obj;
      }

      /**
       * This create func exists so a functor can call it with no parameters.
       */
      static dtCore::RefPtr<PhysicsObject> CreateNewDefName() { return CreateNew(); }

      static dtCore::RefPtr<dtCore::ObjectType> TYPE;

      const dtCore::ObjectType& GetObjectType() const override;
   public:

      //PhysicsObject(const PhysicsObject&);
      //const PhysicsObject& operator=(const PhysicsObject&);

      //////////////////////////////////////////////////////
      //
      /**
       * Creates/initializes this physics object based on it's settings.  The currently set primitive type will
       * be used a its single geometry.  For primitive geometries, the Extents property will be used for the shape
       * unless a node is passed in and the extents has any values that are less than 1, which is the default.
       * In that case, the code will attempt to use the node to figure out a reasonable set of extents to use.
       *
       * @param nodeToLoad The node to use for generating geometry. NULL means to ignore it.
       * @param adjustOriginOffsetForGeometry if true, then the origin offset will be adjusted to account for a center
       *                                      of geometry that isn't at the origin.
       *                                      This value isn't used for convex hulls and triangle meshes.
       * @param cachingKey key to use for caching the geometry generated from the node.  This saves the cost of generating it
       *                   but not the memory, at least at the time of writing this.  It currently only helps for
       *                   convex hulls, but it will eventually be extended to support more types.
       *                   CreateFromGeometry
       * @note pass in null for non-mesh initialization.
       * @note You may call this repeatedly to re-initialize the object. It will first cleanup, then re-create everything
       */
      virtual bool Create(const osg::Node* nodeToLoad = NULL,
               bool adjustOriginOffsetForGeometry = false, const std::string& cachingKey = VertexData::NO_CACHE_KEY);

      /// Just call Create(...)
      DEPRECATE_FUNC virtual bool CreateFromProperties(const osg::Node* nodeToLoad = NULL,
               bool adjustOriginOffsetForGeometry = false, const std::string& cachingKey = VertexData::NO_CACHE_KEY)
      { return Create(nodeToLoad, false, cachingKey); }

      /**
       * Initializes this PhysicsObject with an already created Geometry object.
       * If you want some help creating the geometry, in the right position using the origin offset,
       * see GetOriginOffsetInWorldSpace
       * @see GetOriginOffsetInWorldSpace
       */
      bool CreateFromGeometry(Geometry& geometry);

      /**
       * Initializes this PhysicsObject with an already created vector of geometry objects..
       * If you want some help creating the geometry, in the right position using the origin offset,
       * see GetOriginOffsetInWorldSpace
       * @see GetOriginOffsetInWorldSpace
       */
      bool CreateFromGeometry(std::vector<GeometryPtr> geometry);

      /// Creates this physics object giving it a pre-set and configured body.
      void CreateWithBody(GenericBodyWrapper& body);

      /**
       * Deletes the internal data structures created in one of the Create methods.
       * It is NOT called from the constructor, so don't expect it to be
       */
      virtual void CleanUp();

      /**
       * Set the world position of this physics object/body
       * This may not work if the body is static.
       */
      virtual void SetTransform(const TransformType&);

      /**
       * Get the world position of this physics object/body
       * @param interpolated Get the position iterpolated for the visual.  The GetTransformAsVisual always does this, but it also
       *        takes into account the visual to simulated position.  If you are doing physics calculations, the default of false is what you want.
       *        If you are planning on setting or comparing something with something in a visual, pass true, but GetTransformAsVisual is more likely what you want.
       *        In System::FixedTimeStep == true this usually won't matter unless the fixed time step and physics step are not clean multiples of each other,
       *        or if you are doing anything in an action update, where you have to to impulses and the interpolated position isn't up to date.
       */
      virtual void GetTransform(TransformType&, bool interpolated = false) const;

      /**
       * Set the transform of the body, but offset it using the SetVisualToBodyTransform transform.
       * This is a handy method for converting back and forth between an visual and physics representation
       * with a different origin.
       */
      virtual void SetTransformAsVisual(const TransformType&);

      /**
       * Get the transform of the body, but offset it using the SetVisualToBodyTransform transform.
       * This is a handy method for converting back and forth between an visual and physics representation
       * with a different origin.
       */
      virtual void GetTransformAsVisual(TransformType&) const;

      /**
       * Sets a transform that will take a position for a visual representation and move it to origin
       * corresponding origin of physics representation
       */
      virtual void SetVisualToBodyTransform(const TransformType&);

      /**
       * Gets the assigned transform that will take a position for a visual representation and move it to origin
       * corresponding origin of physics representation
       */
      virtual void GetVisualToBodyTransform(TransformType&) const;

      void SetTranslation(const VectorType& v);
      VectorType GetTranslation() const;

      void SetRotation(const VectorType& v);
      VectorType GetRotation() const;

      const std::string& GetName() const;
      void SetName(const std::string& s);

      /// Adds a force in global space
      void AddForce(const VectorType& forceToAdd);
      /// Adds a force in local space
      void AddLocalForce(const VectorType& forceToAdd);
      /// Adds a force in global space at a global position
      void AddForceAtPosition(const VectorType& globalPosition, const VectorType& globalForce);
      /// Adds a force in local space at a local position.
      void AddLocalForceAtPosition(const VectorType& localPosition, const VectorType& localForce);
      void ApplyImpulse(const VectorType& impulseAmount);
      void ApplyLocalImpulse(const VectorType& impulseAmount);
      void ApplyImpulseAtPosition(const VectorType& position, const VectorType& impulse);
      void ApplyLocalImpulseAtPosition(const VectorType& position, const VectorType& impulse);
      void ApplyAngularImpulse(const VectorType& angularImpulse);
      void AddTorque(const VectorType& torqueToAdd);
      void AddLocalTorque(const VectorType& torqueToAdd);
      void SetLinearVelocity(const VectorType& velocity);
      void SetAngularVelocity(const VectorType& velocity_rad);

      VectorType GetLinearVelocity() const;
      osg::Vec3 GetVelocity() const override { return GetLinearVelocity(); }
      VectorType GetAngularVelocity() const override;

      /// Gets the velocity of a point relative to the body based on the linear and angular velocity.
      VectorType GetLinearVelocityAtLocalPoint(const VectorType& relPos) const;

      void AddGeometry(Geometry& geometry);
      Geometry* GetGeometry(unsigned idx);
      void RemoveGeometry(Geometry& geometry);
      unsigned GetNumGeometries() const;

      void SetGravityEnabled(bool enable);
      bool IsGravityEnabled() const;

      /// Sets if this body and whatever collides with it responds to collisions.
      void SetCollisionResponseEnabled(bool enabled);
      /// @return true if this body and whatever collides with it responds to collisions.
      bool IsCollisionResponseEnabled() const;
      bool GetCollisionResponseEnabled() const { return IsCollisionResponseEnabled(); }

      BodyWrapper* GetBodyWrapper();
      GenericBodyWrapper* GetGenericBodyWrapper();

      /**
       * Sets a pointer to the user data.  The helper will set this to itself when using the GM
       * Because user data objects tend to be referenced back to the owner, and hence would be a circular
       * reference, it is stored in an observer pointer.
       * This means that this cannot be the only reference to whatever is stored here.
       */
      void SetUserData(osg::Referenced* data);
      /// @return a pointer to the user data.  The helper will set this to itself when using the GM
      const osg::Referenced* GetUserData() const;
      /// @return a pointer to the user data.  The helper will set this to itself when using the GM
      osg::Referenced* GetUserData();

      template<typename T>
      T* GetUserData() { return dynamic_cast<T*>(GetUserData()); }

      template<typename T>
      const T* GetUserData() const { return dynamic_cast<const T*>(GetUserData()); }

      //////////////////////////////////////////////////////
      // property functions
      void SetMass(Real);
      /**
       * The Moment of inertia changes linearly with the mass, assuming the shape stays the same.
       * This function will allows one to change the mass of a body that is either initialized already
       * or has a user-assigned moment of inertia and scale it to match the new mass.
       */
      void SetMassAndScaleMomentOfInertia(Real);
      Real GetMass() const;

      /**
       * The X,Y, and Z extents.  For objects that need fewer than 3, only the relevant axes are read.
       * Spheres just use [0].  Cylinders use x and z for width and height.
       */
      void SetExtents(const VectorType& v);
      VectorType GetExtents() const;

      float GetSkinThickness() const;
      void SetSkinThickness(float);

      void SetMechanicsType(MechanicsType& m);
      MechanicsType& GetMechanicsType() const;

      void SetPrimitiveType(PrimitiveType& p);
      PrimitiveType& GetPrimitiveType() const;

      /**
       * Collision groups are numbers.  By default, bodies in different groups collide with each other, but
       * that can be disabled by calling PhysicsWorld::SetGroupCollision.
       * @return The collision group for the object.
       */
      CollisionGroup GetCollisionGroup() const;
      void SetCollisionGroup(CollisionGroup group);

      /// offset of geometry from the origin, used to offset the geometry from center of mass.  Must be set before calling a Create method.
      void SetOriginOffset(const VectorType& v);
      /// @return offset of geometry from the origin, used to offset the geometry from center of mass.  Must be set before calling a Create method.
      VectorType GetOriginOffset() const;

      /// Gets the origin offset from this physics object transformed into world space based on the physics object's transform.
      void GetOriginOffsetInWorldSpace(TransformType& worldOffset) const;

      /// Sets the diagonals of the moment of inertia tensor, often called iXX iYY and iZZ.
      void SetMomentOfInertia(const VectorType& v);
      /// @return the diagonals of the moment of inertia tensor, often called iXX iYY and iZZ.
      VectorType GetMomentOfInertia() const;

      /**
       * Set to true if you want all collision callbacks for this object.
       * If you only want collision callbacks for this object with another object, you
       * need to set that on the Physics World.
       */
      void SetNotifyCollisions(bool);

      /// @return true if this object wants notifications when it collides with another object.
      bool GetNotifyCollisions() const;

      /**
       * Changes the material for this object.  This will only work if it has already been created with one of the Create functions.
       * Note this behavior has changed from earlier versions.  To preconfigure this, you set the unique id of the dtPhysics::MaterialActor.
       */
      void SetMaterial(Material*);
      Material* GetMaterial();

      /**
       * This sets the unique id of the material actor to use for the material info.  This is used for the toolset to configure this physics object in the editor.
       * It will have to lookup the actor if you call this,
       * so it will only work if it has been registered with an actor component.  If you don't want to register it, lookup the material yourself,
       * then call SetMaterial.
       * @return the material for this object.
       */
      DT_DECLARE_ACCESSOR_GET_SET(dtCore::UniqueId, MaterialId);

      /**
       * Convenience method for assigning the material instance by a name.
       * @param matName Name of the material to reference.
       * @return TRUE if a material is found that matches the specified name.
       */
      bool SetMaterialByName(const std::string& matName);
   
      /**
       * Convenience method for assigning the material instance by an index.
       * @param index Id of the material to reference.
       * @return TRUE if a material is found that matches the specified index.
       */
      bool SetMaterialByIndex(MaterialIndex index);

      /**
       * Sets the material by looking the material actor by the given unique id.
       * This requires the physics object to be registered with a PhysicsActComp.
       */
      bool SetMaterialById(const dtCore::UniqueId& id);

      /**
       * This mesh resource will be loaded to set the physics data for a triangle mesh or convex.
       */
      DT_DECLARE_ACCESSOR_GET_SET(dtCore::ResourceDescriptor, MeshResource);

      /**
       * This mesh resource or code supplied osg node will be scaled to this size.
       */
      DT_DECLARE_ACCESSOR_GET_SET(VectorType, MeshScale);

      //////////////////////////////////////////////////////
      // Build our property functions
      virtual void BuildPropertyMap();

      /// forces the automatic activate and deactivate state.  The physics engine normally controls this.
      void SetActive(bool active);
      /// @return true if this object is active.  this is handy for optimizing updating drawing models.
      bool IsActive() const;

      /// @return the linear velocity threshold under which the object will auto-deactivate.
      Real GetActivationLinearVelocityThreshold() const;
      /// changes the linear velocity threshold under which the object will auto-deactivate.
      void SetActivationLinearVelocityThreshold(Real);

      /// @return the angular velocity threshold under which the object will auto-deactivate
      Real GetActivationAngularVelocityThreshold() const;
      /// Sets the angular velocity threshold under which the object will auto-deactivate
      void SetActivationAngularVelocityThreshold(Real);

      /// @return the length of time an object must be under all other thresholds before auto-deactivating.
      Real GetActivationTimeThreshold() const;
      /// Sets the length of time an object must be under all other thresholds before auto-deactivating.
      void SetActivationTimeThreshold(Real);

      /// @return artifical linear body damping factor
      Real GetLinearDamping() const;
      /// Sets artifical linear body damping. 0 means off, 1 means pretty much don't move.
      void SetLinearDamping(Real damping);

      /// @return angular damping factor.
      Real GetAngularDamping() const;
      /// Sets artificial angular body damping. 0 means off, 1 means pretty much don't rotate.
      void SetAngularDamping(Real);

      VectorType TransformToWorldSpace(const VectorType& localSpaceVector);

      static void CalculateOriginAndExtentsForNode(PrimitiveType& type, const osg::BoundingBox& bb,
               VectorType& center, VectorType& extents);


      /**
       * \brief Creates a revolute joint.  The current position of the parent is used to setup the joint.
       * @return NULL if either physics object is not initialized.
       *
       * @param parent            Parent object It is only the parent because the joint is setup in the reference frame of this object.
       * @param child             Child object
       * @param pivotAnchor       Position in world space of joint center
       * @param pivotAxis         Rotational axis of joint
       *
       * @note the caller is responsible for deleting the link with delete since it is not a referenced class.
       *
       */
      static palRevoluteLink* CreateRevoluteJoint(PhysicsObject& parent, PhysicsObject& child, const VectorType& pivotAnchor, const VectorType& pivotAxis, bool disableCollisionBetweenBodies = true);

      /**
       * \brief Creates a single 6-degree-of-freedom joint.
       * @return NULL if either physics object is not initialized.
       *
       * @param body1             first physics object
       * @param body2             second physics object
       * @param frameA            the transform to go from the joint center to the parent body
       * @param frameB            the transform to go from the joint center to the parent body
       *
       * @note the caller is responsible for deleting the link with delete since it is not a referenced class.
       *
       */
      static palGenericLink* Create6DOFJoint(PhysicsObject& body1, PhysicsObject& body2, const dtCore::Transform& frameA, const dtCore::Transform& frameB, bool disableCollisionBetweenBodies = true);

      /**
       * Creates a fixed joint using the current transforms of the physics objects for relative positions.
       * @return NULL if either physics object is not initialized.
       *
       * @note the caller is responsible for deleting the link with delete since it is not a referenced class.
       */
      static palRigidLink* CreateFixedJoint(PhysicsObject& parent, PhysicsObject& child, bool disableCollisionBetweenBodies = true);

      /**
       * Creates a joint matching the joint description.  The physics objects need to be resolved first.
       */
      static palLink* CreateJoint(PhysicsObject& one, PhysicsObject& two, const JointDesc& desc);

      /**
       * Creates a motor for a joint given the motor description.
       */
      static palMotor* CreateMotor(palLink& joint, const MotorDesc& desc);

   protected:
      ~PhysicsObject();

      void CalculateBoundsAndOrigin(const osg::Node* nodeToLoad, bool calcDimensions, bool adjustOriginOffsetForGeometry);

      // If you don't pass a key here, the resource name itself will be used.
      void GetVertexDataForResource(dtCore::RefPtr<VertexData>& vertDataOut, const std::string& cachingKey);

      /**
       * Assumes all configuration values are set such as computing bounds, and that the loading does not use a resource.
       * then creates a body.
       */
      virtual bool CreateInternal(VertexData* data);

   private:
      // our implementation to the physics engine
      PhysicsObjectDataMembers* mDataMembers;
   };

   typedef dtCore::RefPtr<PhysicsObject> PhysicsObjectPtr;

} // namespace dtPhysics

#endif // DELTA_PHYSICS_OBJECTS
