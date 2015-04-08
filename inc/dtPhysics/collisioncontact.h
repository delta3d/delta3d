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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#ifndef COLLISIONCONTACT_H_
#define COLLISIONCONTACT_H_

#include <dtCore/refptr.h>
#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/physicsobject.h>

namespace dtPhysics
{
   class DT_PHYSICS_EXPORT CollisionContact
   {
   public:
      CollisionContact();
      dtCore::RefPtr<PhysicsObject> mObject1, mObject2; //!< objects in the collision
      VectorType mPosition; //!< The contact position
      VectorType mNormal; //!< The contact normal
      Real mDistance; //!< The distance between closest points. Negative distance indicates interpenetrations
      Real mImpulse; //!< The impulse magnitude used to resolve the constraints on the bodies along the normal.
      VectorType mImpulseLateral1; //!< Impulse vector applied to the body based on friction in the direction of travel along the second body.
      VectorType mImpulseLateral2; //!< Impulse vector applied to the body based on friction in the direction normal to both the contact normal and the laterl1 vector
   };
}
#endif /* COLLISIONCONTACT_H_ */
