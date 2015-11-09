/* -*-c++-*-
 * dtPhysics
 * Copyright 2014, Caper Holdings, LLC
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

#ifndef JOINTDESC_H_
#define JOINTDESC_H_

#include <dtCore/propertycontainer.h>
#include <dtPhysics/physicsexport.h>
#include <dtPhysics/jointtype.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/motordesc.h>
#include <dtUtil/getsetmacros.h>

namespace dtPhysics
{

   /**
    * A property container for configuring joints.
    */
   class DT_PHYSICS_EXPORT JointDesc: public dtCore::PropertyContainer
   {
   public:
      JointDesc();

      static dtCore::RefPtr<dtCore::ObjectType> JOINT_DESC_TYPE;

      const dtCore::ObjectType& GetObjectType() const override;
   public:

      DT_DECLARE_ACCESSOR(std::string, Body1Name)
      DT_DECLARE_ACCESSOR(std::string, Body2Name)
      DT_DECLARE_ACCESSOR(std::string, VisualNodeName)

      DT_DECLARE_ACCESSOR(dtUtil::EnumerationPointer<JointType>, JointType)

      DT_DECLARE_ACCESSOR(dtPhysics::Real, BreakingThreshold)

      DT_DECLARE_ACCESSOR(VectorType, Body1RelativeTranslation)
      DT_DECLARE_ACCESSOR(VectorType, Body1RelativeRotationHPR)
      void GetBody1Frame(TransformType& xform) const;
      void SetBody1Frame(const TransformType& xform);

      DT_DECLARE_ACCESSOR(VectorType, Body2RelativeTranslation)
      DT_DECLARE_ACCESSOR(VectorType, Body2RelativeRotationHPR)
      void GetBody2Frame(TransformType& xform) const;
      void SetBody2Frame(const TransformType& xform);

      DT_DECLARE_ACCESSOR(VectorType, LinearLimitMinimums)
      DT_DECLARE_ACCESSOR(VectorType, LinearLimitMaximums)
      DT_DECLARE_ACCESSOR(VectorType, AngularLimitMinimums)
      DT_DECLARE_ACCESSOR(VectorType, AngularLimitMaximums)

      DT_DECLARE_ACCESSOR(bool, DisableCollisionBetweenBodies)

      DT_DECLARE_ARRAY_ACCESSOR(MotorDescPtr, Motor, Motors)

   protected:
      virtual ~JointDesc();
   };

   typedef dtCore::RefPtr<JointDesc> JointDescPtr;

} /* namespace dtPhysics */

#endif /* JOINTDESC_H_ */
