/* -*-c++-*-
 * dtPhysics
 * Copyright 2015, Caper Holdings, LLC
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

#ifndef DTPHYSICS_MOTORDESC_H_
#define DTPHYSICS_MOTORDESC_H_

#include <dtCore/propertycontainer.h>
#include <dtCore/axisenum.h>
#include <dtPhysics/physicsexport.h>
#include <dtUtil/getsetmacros.h>

namespace dtPhysics
{

   /**
    * Defines a single axis motor, rotational or translational.
    * A motor must refer to a joint description, and that joint description must allow a motor on that axis.
    */
   class DT_PHYSICS_EXPORT MotorDesc: public dtCore::PropertyContainer
   {
   public:
      static dtCore::RefPtr<dtCore::ObjectType> MOTOR_DESC_TYPE;

      MotorDesc();
      const dtCore::ObjectType& GetObjectType() const override;

      DT_DECLARE_ACCESSOR_INLINE(float, TargetVelocity)
      DT_DECLARE_ACCESSOR_INLINE(float, MaxForce)
      DT_DECLARE_ACCESSOR_INLINE(bool,  IsAngular)
      DT_DECLARE_ACCESSOR_INLINE(dtUtil::EnumerationPointer<dtCore::AxisEnum>,  Axis)

   protected:
      virtual ~MotorDesc();
   };

   typedef dtCore::RefPtr<MotorDesc> MotorDescPtr;

} /* namespace dtPhysics */

#endif /* DTPHYSICS_MOTORDESC_H_ */
