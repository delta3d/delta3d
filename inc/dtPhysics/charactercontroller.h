/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2008, Alion Science and Technology, Inc.
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
* Allen Danklefsen
*/

#ifndef CHARACTERCONTROLLER_H_
#define CHARACTERCONTROLLER_H_

#include <dtCore/propertycontainer.h>

#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/geometry.h>

#include <dtCore/propertymacros.h>
namespace dtPhysics
{
   class CharacterControllerImpl;
   class PhysicsObject;

   /**
    * This class implements the front end to the physics engine character controller.  It's not a motion model, but
    * more of a steering class.
    * TODO:  This should be made into an actor.
    */
   class DT_PHYSICS_EXPORT CharacterController : public dtCore::PropertyContainer {
   public:
      CharacterController(Geometry& shape);

      static dtCore::RefPtr<dtCore::ObjectType> TYPE;

      const dtCore::ObjectType& GetObjectType() const override;

      virtual void Init();

      virtual void BuildPropertyMap();

      /// Moves with a given displacement vector
      virtual void Move(const VectorType& displacement);

      /// Starts motion along the walkVelocity vector for the specified item interval.
      virtual void Walk(const VectorType& walkVelocity, Real timeInterval);

      virtual void Jump();

      /// Clears a walk call early
      virtual void WalkClear();

      /// Forces the underlying body to warp to the given position.
      virtual void Warp(const VectorType& worldPos);

      /** Retrieves the position of the underlying body.
       */
      virtual VectorType GetTranslation() const;

      Geometry& GetGeometry();
      const Geometry& GetGeometry() const;

      DT_DECLARE_ACCESSOR(Real, JumpHeight);
      DT_DECLARE_ACCESSOR(Real, JumpSpeed);
      DT_DECLARE_ACCESSOR(Real, FallSpeed);
      DT_DECLARE_ACCESSOR(Real, StepHeight);
      /// Max incline the character can walk up in degrees.  Defaults to 45
      DT_DECLARE_ACCESSOR(Real, MaxInclineAngle);
      DT_DECLARE_ACCESSOR(Real, SkinWidth);
      DT_DECLARE_ACCESSOR(CollisionGroup, CollisionGroup);

   protected:
      virtual ~CharacterController();
   private:
      // Hide  copy constructor and operator =
      CharacterController(const CharacterController&);
      CharacterController& operator=(const CharacterController&);

      CharacterControllerImpl* mImpl;

   };
}
#endif /* CHARACTERCONTROLLER_H_ */
