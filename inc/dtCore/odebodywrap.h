#ifndef odebodywrap_h__
#define odebodywrap_h__

#include <dtCore/export.h>

#include <osg/Referenced>
#include <osg/Vec3>
#include <osg/Matrix>
#include <ode/common.h>
#include <ode/mass.h>


namespace dtCore
{

   class Transform;

   /** Wrapper around an ODE physics body.  To use, call SetBodyID() with
     * a non-zero ID - typically created from dCreateWorld().
     * Body is disabled by default. Call EnableDynamics() to toggle on/off.
     */
   class DT_CORE_EXPORT ODEBodyWrap : public osg::Referenced
   {
   public:
      ODEBodyWrap();

      /**
       * Sets the ODE body identifier associated with this object. Should
       * only be called by dtCore::Scene. If you call this outside the
       * content of Scene be warning that any transformations you have
       * on bodyID will be removed.
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
       * Set the body's position manually.
       *
       * @note: SetBodyID() must be called before this method takes affect.
       */
      void SetPosition(const osg::Vec3& xyz);

      /**
       * Set the body's rotation manually.
       *
       * @note: SetBodyID() must be called before this method takes affect.
       */
      void SetRotation(const osg::Matrix& rot);

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
      void GetInertiaTensor(osg::Matrix& dest) const;

      /**
       * @return the Body's linear velocity
       */
      void SetLinearVelocity(const osg::Vec3& newVelocity);

      /**
       * @return the Body's linear velocity
       */
      osg::Vec3 GetLinearVelocity() const;

      /**
       * @return the Body's angular velocity
       */
      osg::Vec3 GetAngularVelocity() const;

      /**
       * Apply a force at the center of mass in world coordinates.
       *
       * @param force : the world coordinate force to apply
       */
      void ApplyForce(const osg::Vec3& force);

      /**
       * Apply a world coordinate force at a world coordinate.
       * @param force : the world coordinate force to apply
       * @param position : the world relative position to apply the force to
       */
      void ApplyForceAtPos(const osg::Vec3& force,
                           const osg::Vec3& position);

      /**
       * Apply a force at the center of mass in body relative coordinates.
       *
       * @param relForce : the body relative force to apply
       */
      void ApplyRelForce(const osg::Vec3& relForce);

      /**
       * Apply a world coordinate force at a body relative position.
       * @param force : The force relative to the world coordinates
       * @param relPosition : The body relative position to apply the force to
       */
      void ApplyForceAtRelPos(const osg::Vec3& force,
                              const osg::Vec3& relPosition);

      /**
       * Apply a body relative force at a body relative position.
       *
       * @param relForce : The body relative force to apply
       * @param relPosition : The body relative position to apply the force to
       */
      void ApplyRelForceAtRelPos(const osg::Vec3& relForce,
                                 const osg::Vec3& relPosition);

      /**
       * Apply a torque at the center of mass in world coordinates.
       *
       * @param torque: the world coordinate torque to apply
       */
      void ApplyTorque(const osg::Vec3& torque);

      /**
       * Apply a torque at the center of mass in body relative coordinates.
       *
       * @param torque: the body relative torque to apply
       */
      void ApplyRelTorque(const osg::Vec3& torque);

      void GetBodyTransform(dtCore::Transform& xform) const;

      /**
       * If needed, set the ODE Body position and rotation to match the
       * supplied Transform. If a Physical's position or rotation is manually
       * set, then this method should be called to keep the ODE Body
       * in sync.
       *
       * @param newTransform The potentially new position/rotation of the Transformable
       */
      void UpdateBodyTransform(const dtCore::Transform& newTransform);

      /**
       * Reports whether the object will automatically sleep when conditions are met.
       * @return True if the object is configured to automatically sleep.
       */
      bool GetAutoDisableFlag() const;

      /**
       * Set whether an object will automatically sleep when conditions are met.
       * @param auto_disable True to auto-sleep, false to not
       */
      void SetAutoDisableFlag(bool auto_disable);

      /**
       * Returns the value below which this object will "sleep"
       * @return The current sleep threshold
       */
      float GetAutoDisableLinearThreshold() const;

      /**
       * Sets the value below which the object will "sleep"
       * @param threshold The new sleep threshold
       */
      void SetAutoDisableLinearThreshold(float threshold);

      /**
       * Returns the value below which this object will "sleep"
       * @return The current sleep threshold
       */
      float GetAutoDisableAngularThreshold() const;

      /**
       * Sets the value below which the object will "sleep"
       * @param threshold The new sleep threshold
       */
      void SetAutoDisableAngularThreshold(float threshold);

      /**
       * Get the number of simulation steps that the body must be idle (within linear/angular
       * threshold) before being auto-disabled.
       * @return The number of steps to wait before the (idle) body is disabled
       */
      int GetAutoDisableSteps() const;

      /**
       * Set the number of simulation steps that the body must be idle (within linear/angular
       * threshold) before being auto-disabled. Zero removes steps from consideration.
       * @param numSteps The number of steps to wait before the (idle) body is disabled
       */
      void SetAutoDisableSteps(int numSteps);

      /**
       * Get the (simulation) time that the body must be idle (within linear/angular threshold)
       * before being auto-disabled.
       * @return The number of seconds to wait before the (idle) body is disabled
       */
      float GetAutoDisableTime() const;

      /**
       * Set the (simulation) time that the body must be idle (within linear/angular threshold)
       * before being auto-disabled. Zero removes time from consideration.
       * @param numSeconds The number of seconds to wait before the (idle) body is disabled
       */
      void SetAutoDisableTime(float numSeconds);

      /**
       * Returns the linear motion damping value used for this body
       * @return The current linear damping value
       */
      float GetLinearDamping() const;

      /**
       * Sets the linear motion damping value used for this body
       * @param scale The linear damping value to use
       */
      void SetLinearDamping(float scale);

      /**
       * Returns the angular motion damping value used for this body
       * @return The current angular damping value
       */
      float GetAngularDamping() const;

      /**
       * Sets the angular motion damping value used for this body
       * @param scale The angular damping value to use
       */
      void SetAngularDamping(float scale);

   protected:
      virtual ~ODEBodyWrap();

   private:
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

   };
}
#endif // odebodywrap_h__
