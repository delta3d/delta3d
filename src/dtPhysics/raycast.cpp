/* -*-c++-*-
 * dtPhysics
 * Copyright 2007-2008, Alion Science and Technology
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
 */

#include <dtPhysics/raycast.h>

namespace dtPhysics
{
   //////////////////////////////////////////////////////////////////////////////
   RayCast::Report::Report()
   : mDistance(0.0f)
   , mHitPos()
   , mHitNormal()
   , mHasHitObject(false)
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   RayCast::RayCast()
   //It hits everything by default.
   : mCollisionGroupFilter(~0)
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void RayCast::SetOrigin(const VectorType& origin)
   {
      mOrigin = origin;
   }

   //////////////////////////////////////////////////////////////////////////////
   const VectorType& RayCast::GetOrigin() const
   {
      return mOrigin;
   }

   //////////////////////////////////////////////////////////////////////////////
   void RayCast::SetDirection(const VectorType& direction)
   {
      mDirection = direction;
   }

   //////////////////////////////////////////////////////////////////////////////
   const VectorType& RayCast::GetDirection() const
   {
      return mDirection;
   }

   //////////////////////////////////////////////////////////////////////////////
   void RayCast::SetCollisionGroupFilter(CollisionGroupFilter filter)
   {
      mCollisionGroupFilter = filter;
   }

   //////////////////////////////////////////////////////////////////////////////
   CollisionGroupFilter RayCast::GetCollisionGroupFilter() const
   {
      return mCollisionGroupFilter;
   }

   //////////////////////////////////////////////////////////////////////////////
   RayCast::~RayCast()
   {
   }
}
