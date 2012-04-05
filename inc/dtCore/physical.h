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

#include <dtCore/transformable.h>

struct dMass;

namespace dtCore
{

   class ODEBodyWrap;

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
   class DT_CORE_EXPORT Physical : public Transformable
   {
      DECLARE_MANAGEMENT_LAYER(Physical)

   public:
      Physical(const std::string& name = "Physical");

      /** Overloaded constructor will use the supplied node instead of
       * creating one internally.
       * @param node : A node this class should use internally
       * @param name : The name of this instance
       */
      Physical( TransformableNode& node, const std::string& name = "Physical" );

   protected:
      virtual ~Physical();

   public:
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
       * Updates the state of this object just after a physical
       * simulation step.  Should only be called by dtCore::Scene.
       * The default implementation copies the new object
       * position into the user-accessible transformation.
       */
      virtual void PostPhysicsStepUpdate();

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
      virtual bool FilterContact(dContact* /*contact*/, Transformable* /*collider*/) { return true; }

      /**
       * Get the const internal ODE physical body wrapper object.
       * @return The body wrapper object
       */
      const ODEBodyWrap* GetBodyWrapper() const {return mBodyWrap.get();}

      /**
       * Get the internal ODE physical body wrapper object.
       * @return The body wrapper object
       */
      ODEBodyWrap* GetBodyWrapper() {return mBodyWrap.get();}

   private:
      void Ctor();

      dtCore::RefPtr<ODEBodyWrap> mBodyWrap;
   };

} // namespace dtCore

#endif // DELTA_PHYSICAL
