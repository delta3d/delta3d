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

#include <dtPhysics/motordesc.h>
#include <dtCore/propertymacros.h>

namespace dtPhysics
{

   MotorDesc::MotorDesc()
   : mTargetVelocity()
   , mMaxForce()
   , mIsAngular(true)
   , mAxis(&dtCore::AxisEnum::AXIS_X)
   {
      typedef dtCore::PropertyRegHelper<MotorDesc> RegHelper;
      static dtUtil::RefString GROUP("Motor");
      RegHelper regHelper(*this, this, GROUP);
      DT_REGISTER_PROPERTY(TargetVelocity, "The target velocity of the motor in m/s or rad/sec.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY(MaxForce, "The max force (translational) / torque (angular) of the motor.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY(IsAngular, "True if the motor is angular, or false for linear/translational.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY(Axis, "The axis, 0, 1, 2 for x, y, z that this motor uses.  If the joint only allows a motor on one axis, this property is ignored.", RegHelper, regHelper);

      InitDefaults();
   }

   dtCore::RefPtr<dtCore::ObjectType> MotorDesc::MOTOR_DESC_TYPE(new dtCore::ObjectType("MotorDesc", "dtPhysics"));

   /*override*/ const dtCore::ObjectType& MotorDesc::GetObjectType() const { return *MOTOR_DESC_TYPE; }

   MotorDesc::~MotorDesc()
   {
   }

} /* namespace dtPhysics */
