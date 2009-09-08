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

#ifndef odeiterator_h__
#define odeiterator_h__

#include <osg/Referenced>
#include <osg/Vec3>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtCore/export.h>
#include <dtCore/odespacewrap.h>
#include <dtCore/odeworldwrap.h>
#include <dtCore/base.h>
#include <dtUtil/deprecationmgr.h>
#include <dtUtil/refstring.h>
#include <vector>
#include <ode/common.h>
#include <ode/collision_space.h>

namespace dtCore
{
   class Transformable;

   /** Used to manage the ODE physics system.  Provides the functionality
    *  to register physical objects, adjust global gravity, iterate the
    *  physics system (both collision detection and body physics), and
    *  provides a default near collision callback.
    *  Typically used by dtCore::Scene, this class is something like the Facade
    *  pattern and manages both collision detection and physical body operations.
    *  This class will generate messages and
    *  will send the messages out via the object supplied in SetMessageSender().
    *
    */
   class DT_CORE_EXPORT ODEController : public osg::Referenced
   {
   public:
      
      ///Two object have collided
      const static dtUtil::RefString MESSAGE_COLLISION;

      ///The physics integrator is about to perform one integration step
      const static dtUtil::RefString MESSAGE_PHYSICS_STEP;


      /** Default constructor.  Will create default ODESpaceWrap and ODEWorldWrap
       * instances used for collision and body functionality.
       * @param msgSender : optional pointer to a class which will be used to send out
       *                the "collision" message.
       * @see SetMessageSender()
       */
      ODEController(dtCore::Base* msgSender = NULL);

      /** Overloaded constructor to supply a custom ODESpaceWrap and ODEWorldWrap.
       * @param spaceWrapper custom ODE space wrapper
       * @param worldWrap custom ODE world wrapper
       * @param msgSender : optional pointer to a class which will be used to send out
       *                the "collision" message.
       */
      ODEController(ODESpaceWrap& spaceWrapper, ODEWorldWrap& worldWrap, dtCore::Base* msgSender = NULL);

      /** Set the class that will act as the sender for the "collision" message.
       * @param msgSender : pointer to a class which will be used to send out
       *                    the "collision" message (can be NULL)
       */
      void SetMessageSender(dtCore::Base* msgSender);

      /** Register a physics object with the system.  When added, the object
       * will be supplied to the underlying physics system for processing.
       * @param collidable A valid collision/physics object
       */
      void RegisterCollidable(Transformable* collidable);

      /** Unregister a physics object with the system.  The object
       * will be removed from the underlying physics system.
       * @param collidable A valid collision/physics object
       */
      void UnRegisterCollidable(Transformable* collidable);

      typedef std::vector<Transformable*> TransformableVector;

      /** Get the container of previously registered collidables.
       * @return The const container of registered collidables
       */
      const TransformableVector& GetRegisteredCollidables() const;

      /** Perform an iteration of the physics system.  Default implementation
       * will perform as many physics steps required to fulfill the supplied
       * time step.  Custom iteration algorithms can be created by deriving from
       * this class and overwriting this method.
       * @param deltaFrameTime The amount of time to simulate in the physics system.
       */
      virtual void Iterate(double deltaFrameTime);

      /**
       * Get the step size of the physics.  The physics will
       * be updated numerous times per frame based on this number.  For example,
       * if the delta frame rate is 33ms and the step size is 2ms, the physics
       * will be updated 16 times.
       *
       * @return the step size in seconds
       * @see SetPhysicsStepSize()
       */
      double GetPhysicsStepSize() const;

      /// @see GetPhysicsStepSize()
      void SetPhysicsStepSize(double stepSize = 0.0);

      ///Set the gravity vector
      void SetGravity(const osg::Vec3& gravity) const;

      ///Get the gravity vector
      osg::Vec3 GetGravity() const;

      ///Get the ODE space ID
      dSpaceID GetSpaceID() const;

      ///Get the ODE world ID
      dWorldID GetWorldID() const;

      ///Supply a user-defined collision callback to replace the internal one
      void SetUserCollisionCallback(dNearCallback* func, void* data=NULL) const;

      dNearCallback* GetUserCollisionCallback() const;
      void* GetUserCollisionData();
      const void* GetUserCollisionData() const;

      ///Get the ODE contact joint group ID
      dJointGroupID GetContactJointGroupID() const;

      ///Get a pointer to the internal ODEWorldWrap instance
      dtCore::ODEWorldWrap* GetWorldWrapper() const;

      ///Get a pointer to the internal ODESpaceWrap instance
      dtCore::ODESpaceWrap* GetSpaceWrapper() const;

      ///DEPRECATED 1/14/2009 in favor of GetContactJointGroupID() (typo: missing t)
      DEPRECATE_FUNC dJointGroupID GetContactJoinGroupID() const
      {
         DEPRECATE("dJointGroupID dtCore::ODEController::GetContactJoinGroupID() const",
                   "dJointGroupID dtCore::ODEController::GetContactJointGroupID() const");

         return this->GetContactJointGroupID();
      }

      /// @return the number of instances of this class that are holding a reference to the static ODE initialization.
      static unsigned GetODERefCount();

   protected:
      virtual ~ODEController();

      ///When two geoms collide, send out a "collide" message
      virtual void DefaultCBFunc(const dtCore::ODESpaceWrap::CollisionData& data);

      ///perform one iteration using the supplied step size
      void Step(double stepSize);

   private:
      void Ctor();


      dtCore::RefPtr<dtCore::ODESpaceWrap> mSpaceWrapper;
      dtCore::RefPtr<dtCore::ODEWorldWrap> mWorldWrapper;

      ///<The time (seconds) for the physics time step.
      ///<(default = 0.0, indicating to use the System deltaFrameTime)
      double mPhysicsStepSize;

      TransformableVector mCollidableContents; ///<The physical contents of the scene

      dtCore::ObserverPtr<dtCore::Base> mMsgSender; ///<only to send out a "collision" message
   };
}
#endif // odeiterator_h__

