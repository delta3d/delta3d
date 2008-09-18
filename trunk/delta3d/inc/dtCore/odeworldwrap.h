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

#include <osg/Referenced>
#include <osg/Vec3>
#include <ode/common.h>

namespace dtCore
{
   class Transformable;

   /** Used as a wrapper around ODE world functionality.  Typically not referenced directly
    *  by end user applications.
    */
   class ODEWorldWrap : public osg::Referenced
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

   protected:
      virtual ~ODEWorldWrap();
   	
   private:
      dWorldID mWorldID;  ///<The generated world ID
      osg::Vec3 mGravity; ///<The current gravity vector
   };
}
#endif // odeworldwrap_h__
