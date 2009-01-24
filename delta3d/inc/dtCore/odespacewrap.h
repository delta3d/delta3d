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

#ifndef odespacewrap_h__
#define odespacewrap_h__

#include <dtCore/export.h>
#include <dtCore/refptr.h>
#include <dtUtil/functor.h>

#include <osg/Referenced>
#include <osg/Vec3>
#include <ode/common.h>
#include <ode/collision_space.h>

namespace dtCore
{
   class Transformable;
   class ODEWorldWrap;

   /** Used to wrap up the functionality provided by the ODE Space.  Typically
    * not referenced directly by end users.  Contains most of the collision
    * detection functionality.
    */
   class DT_CORE_EXPORT ODESpaceWrap : public osg::Referenced
   {
   public:

      /** Default constructor.  Uses the supplied parameter to get the world ID
       * in order to create collision contact joints.
       * @param worldWrapper : Collision contact joints will be created in this
       * world.
       */
      ODESpaceWrap(ODEWorldWrap* worldWrapper);

      /** Register a collidable object with the collision detection system.
       * @param collidable : valid collidable object to add to the system
       */
      void RegisterCollidable(Transformable* collidable);

      /** Unregister a collidable object with the collision detection system.
       * @param collidable : valid collidable object to remove from the system.
       */
      void UnRegisterCollidable(Transformable* collidable);


      /** Check the system for collision detections.  Will use the default
       * near collision detection method unless a user created one is supplied.
       * @see SetUserCollisionCallback()
       */
      void Collide();

      /** Performs cleanup functionality after Collide() has taken place.
       */
      void PostCollide();

      /** Supply a user-defined near collision callback to replace the internal one.
       *  This is used to do a rough-cut filter of geoms that could be colliding.
       */
      void SetUserCollisionCallback(dNearCallback* func, void* data=NULL);

      dNearCallback* GetUserCollisionCallback() const;
      void* GetUserCollisionData();
      const void* GetUserCollisionData() const;

      ///The user data associated with "collision" messages
      struct DT_CORE_EXPORT CollisionData
      {
         Transformable* mBodies[2]; ///<The bodies colliding
         osg::Vec3 mLocation; ///<The collision location
         osg::Vec3 mNormal; ///<The collision normal
         float mDepth; ///<The penetration depth
      };

      typedef dtUtil::Functor<void, TYPELIST_1(const CollisionData&)> CollisionCBFunc;

      /** Supply a callback function to be used by the default collision function.
       * The func will be called when two geom's are colliding and supplied with
       * the collision data.
       * @param func A functor to be called when two geoms are actually colliding
       * @see CollisionCBFunc
       */
      void SetDefaultCollisionCBFunc(const CollisionCBFunc& func);

      ///Get the ODE contact join group ID
      dJointGroupID GetContactJoinGroupID() const;

      ///Get the ODE space ID
      dSpaceID GetSpaceID() const;

   protected:
      virtual ~ODESpaceWrap();

   private:
      ///ODE collision callback
      static void DefaultNearCallback(void* data, dGeomID o1, dGeomID o2);

      dSpaceID mSpaceID;  ///< the current collision space ID
      dNearCallback* mUserNearCallback;   ///<The user-supplied collision callback func
      void* mUserNearCallbackData; ///< pointer to user-supplied data

      dJointGroupID mContactJointGroupID; ///<The group that contains all contact joints

      ///Where to get the WorldID from to create collision contact joints.
      dtCore::RefPtr<dtCore::ODEWorldWrap> mWorldWrapper;

      ///A functor to call should two geoms actually collide (could be NULL)
      CollisionCBFunc mCollisionCBFunc;
   };
}

#endif // odespacewrap_h__
