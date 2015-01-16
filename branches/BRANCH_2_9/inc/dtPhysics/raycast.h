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
* Bradley Anderegg
* Allen Danklefsen
*/
#ifndef DELTA_RAY_CAST
#define DELTA_RAY_CAST

#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/physicsobject.h>

#include <dtUtil/functor.h>

namespace dtPhysics
{

   /**
    * An abstraction of a ray to cast into the physics scene to query for collisions.
    */
   class DT_PHYSICS_EXPORT RayCast
   {
      public:

         /**
          * A raycast report stating what the ray collided with.
          */
         struct DT_PHYSICS_EXPORT Report
         {
            Report();

            Real mDistance;
            VectorType mHitPos;
            VectorType mHitNormal;
            bool mHasHitObject;
            dtCore::ObserverPtr<PhysicsObject> mHitObject;
            bool operator<(const Report& other) const { return this->mDistance < other.mDistance; }
         };

         typedef dtUtil::Functor<bool, TYPELIST_1(const Report&)> RayCastCallback;

      public:
         RayCast();
         ~RayCast();

         /// Set the starting world position of the ray as a 3 item vector.
         void SetOrigin(const VectorType&);

         /// @return the starting world position of the ray as a 3 item vector.
         const VectorType& GetOrigin() const;

         /// Set the direction of the ray as a vector from the origin.
         void SetDirection(const VectorType&);

         /// @return the direction of the ray as a vector from the origin.
         const VectorType& GetDirection() const;

         /// Sets the collision group filter of the ray so the engine will be able to filter out certain objects
         void SetCollisionGroupFilter(CollisionGroupFilter cg);

         /// @return the collision group filter of the ray.  It defaults to all flags on.
         CollisionGroupFilter GetCollisionGroupFilter() const;

      private:
         VectorType mOrigin;
         VectorType mDirection;
         CollisionGroupFilter mCollisionGroupFilter;
   };

}//namespace dtPhysics

#endif //DELTA_RAY_CAST
