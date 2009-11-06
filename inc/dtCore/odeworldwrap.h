/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 *  Erik Johnson
 */

#ifndef odeworldwrap_h__
#define odeworldwrap_h__

#include <dtCore/export.h>
#include <osg/Referenced>
#include <osg/Vec3>
#include <ode/common.h>

namespace dtCore
{
   class Transformable;

   /** Used as a wrapper around ODE world functionality.  Typically not referenced directly
    *  by end user applications.
    */
   class DT_CORE_EXPORT ODEWorldWrap : public osg::Referenced
   {
   public:
      ODEWorldWrap();

      ///Get the ODE world ID
      dWorldID GetWorldID() const;


      /**
        * Perform one step of the physics system of the supplied time.
        * @param stepSize : the number of seconds to iterate the world.
        */
      void Step(const double &stepSize);

      ///Set the gravity vector
      void SetGravity(const osg::Vec3& gravity);

      ///Set the gravity vector
      void SetGravity(float x, float y, float z);

      ///Get the gravity vector
      void GetGravity(osg::Vec3& vec) const;

      ///Get the gravity vector
      osg::Vec3 GetGravity() const;

      ///Get the gravity vector
      void GetGravity(float &x, float &y, float &z) const;

      /** Register a collidable/physical object with the World.
        * @param collidable A valid object to insert into the physics world.
        */
      void RegisterCollidable(Transformable* collidable);

      /** Remove the supplied collidable/physical object from the world.
        * @param collidable A valid object to remove from the physics world.
        */
      void UnRegisterCollidable(Transformable* collidable);

      /** Set the world's default damping parameters.  Bodies will use the world's
        * parameters by default.  Default scale is zero, meaning no
        * damping will take place.
        * @param linearScale the damping scale factor to apply to linear velocities
        * @param angularScale The angular scale factor to apply to angular velocities
        */
      void SetDamping(float linearScale, float angularScale);

      /** Get the world's linear damping scale value.
        * @return The linear velocity damping scale value
        */
      float GetLinearDamping() const;

      /** Get the world's angular damping scale value.
        * @return The angular velocity damping scale value
        */
      float GetAngularDamping() const;

      /** Set the world's linear velocity damping threshold.  If an object's
        * linear velocity is above this value, the linear damping scale
        * will be applied.  Default is 0.01
        * @param linearThreshold The linear velocity threshold above which, the
        *        body will be dampened.  Use zero to disable damping.
        * @see SetDamping()
        */
      void SetLinearDampingThreshold(float linearThreshold);

      /** Get the world's linear velocity damping threshold.
        * @return The linear velocity damping threshold.
        */
      float GetLinearDampingThreshold() const;

      /** Set the world's angular velocity damping threshold.  If an object's
        * angular velocity is above this value, the angular damping scale
        * will be applied.  Default is 0.01
        * @param angularThreshold The angular velocity threshold above which, the
        *        body will be dampened.  Use zero to disable damping.
        * @see SetDamping()
        */
      void SetAngularDampingThreshold(float angularThreshold);

      /** Get the world's angular velocity damping threshold.
        * @return The angular velocity damping threshold.
        */
      float GetAngularDampingThreshold() const;

   protected:
      virtual ~ODEWorldWrap();

   private:
      dWorldID mWorldID;  ///<The generated world ID
      osg::Vec3 mGravity; ///<The current gravity vector
   };
}
#endif // odeworldwrap_h__
