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

#include <dtPhysics/jointdesc.h>
#include <dtCore/propertymacros.h>
#include <dtCore/arrayactorpropertycomplex.h>
#include <dtCore/propertycontaineractorproperty.h>
#include <limits>

namespace dtPhysics
{

   JointDesc::JointDesc()
   : mJointType(&JointType::GENERIC_6DOF)
   , mBreakingThreshold(std::numeric_limits<decltype(mBreakingThreshold)>::infinity())
   , mDisableCollisionBetweenBodies(true)
   {
      typedef dtCore::PropertyRegHelper<JointDesc> JointDescRegHelper;
      static dtUtil::RefString GROUP("JointDesc");
      static dtUtil::RefString GROUP_LIMITS("Limits");

      JointDescRegHelper regHelper(*this, this, GROUP);
      DT_REGISTER_PROPERTY(Body1Name, "Name of the first body attached to the joint.", JointDescRegHelper, regHelper);
      DT_REGISTER_PROPERTY(Body2Name, "Name of the second body attached to the joint.", JointDescRegHelper, regHelper);
      DT_REGISTER_PROPERTY(VisualNodeName, "Name of the second transform node this joint should be moving in the visual model.", JointDescRegHelper, regHelper);
      DT_REGISTER_PROPERTY(JointType, "The type of joint this should be.", JointDescRegHelper, regHelper);
      DT_REGISTER_PROPERTY(BreakingThreshold, "The impulse magnitude at which this joint will break", JointDescRegHelper, regHelper);

      DT_REGISTER_PROPERTY(Body1RelativeTranslation, "The relative translation from body 1 to the joint center.", JointDescRegHelper, regHelper);
      DT_REGISTER_PROPERTY(Body1RelativeRotationHPR, "The relative rotation from body 1 to the joint center.", JointDescRegHelper, regHelper);
      DT_REGISTER_PROPERTY(Body2RelativeTranslation, "The relative translation from body 2 to the joint center.", JointDescRegHelper, regHelper);
      DT_REGISTER_PROPERTY(Body2RelativeRotationHPR, "The relative rotation from this body 2 to the joint center.", JointDescRegHelper, regHelper);

      JointDescRegHelper regHelperLimits(*this, this, GROUP_LIMITS);
      DT_REGISTER_PROPERTY(LinearLimitMinimums,  "Linear minimum limits X Y Z.  Min = max for locked. Min > max for free motion.", JointDescRegHelper, regHelperLimits);
      DT_REGISTER_PROPERTY(LinearLimitMaximums,  "Linear maximum limits X Y Z.  Min = max for locked. Min > max for free motion.", JointDescRegHelper, regHelperLimits);
      DT_REGISTER_PROPERTY(AngularLimitMinimums, "Angular minimum limits X Y Z. Min = max for locked. Min > max for free motion.", JointDescRegHelper, regHelperLimits);
      DT_REGISTER_PROPERTY(AngularLimitMaximums, "Angular minimum limits X Y Z. Min = max for locked. Min > max for free motion.", JointDescRegHelper, regHelperLimits);

      DT_REGISTER_PROPERTY(DisableCollisionBetweenBodies, "Body2 angular minimum limits X Y Z. Min = max for locked. Min > max for free motion.", JointDescRegHelper, regHelperLimits);

      typedef dtCore::ArrayActorPropertyComplex<dtCore::RefPtr<MotorDesc> > MotorArrayPropType;
      dtCore::RefPtr<MotorArrayPropType> motorArrayProp =
          new MotorArrayPropType
          ("Motors", "Motors",
                MotorArrayPropType::SetFuncType(this, &JointDesc::SetMotor),
                MotorArrayPropType::GetFuncType(this, &JointDesc::GetMotor),
                MotorArrayPropType::GetSizeFuncType(this, &JointDesc::GetNumMotors),
                MotorArrayPropType::InsertFuncType(this, &JointDesc::InsertMotor),
                MotorArrayPropType::RemoveFuncType(this, &JointDesc::RemoveMotor),
           "Array of Motor Descriptions",
           GROUP
           );

      dtCore::RefPtr<dtCore::BasePropertyContainerActorProperty> singleMotorProp =
           new dtCore::SimplePropertyContainerActorProperty<MotorDesc>("Motor", "Motor",
           dtCore::SimplePropertyContainerActorProperty<MotorDesc>::SetFuncType(motorArrayProp.get(), &MotorArrayPropType::SetCurrentValue),
           dtCore::SimplePropertyContainerActorProperty<MotorDesc>::GetFuncType(motorArrayProp.get(), &MotorArrayPropType::GetCurrentValue),
           "", GROUP);

      motorArrayProp->SetArrayProperty(*singleMotorProp);
      motorArrayProp->SetSendInFullUpdate(false);
      motorArrayProp->SetSendInPartialUpdate(false);

      AddProperty(motorArrayProp.get());

      InitDefaults();
   }

   JointDesc::~JointDesc()
   {
   }

   dtCore::RefPtr<dtCore::ObjectType> JointDesc::JOINT_DESC_TYPE(new dtCore::ObjectType("JointDesc", "dtPhysics"));

   /*override*/ const dtCore::ObjectType& JointDesc::GetObjectType() const { return *JOINT_DESC_TYPE; }


   DT_IMPLEMENT_ACCESSOR(JointDesc, std::string, Body1Name);
   DT_IMPLEMENT_ACCESSOR(JointDesc, std::string, Body2Name);
   DT_IMPLEMENT_ACCESSOR(JointDesc, std::string, VisualNodeName);

   DT_IMPLEMENT_ACCESSOR(JointDesc, dtUtil::EnumerationPointer<JointType>, JointType);
   DT_IMPLEMENT_ACCESSOR(JointDesc, float, BreakingThreshold);

   DT_IMPLEMENT_ACCESSOR(JointDesc, VectorType, Body1RelativeTranslation);
   DT_IMPLEMENT_ACCESSOR(JointDesc, VectorType, Body1RelativeRotationHPR);
   void JointDesc::GetBody1Frame(TransformType& xform) const
   {
      xform.SetTranslation(GetBody1RelativeTranslation());
      xform.SetRotation(GetBody1RelativeRotationHPR());
   }

   void JointDesc::SetBody1Frame(const TransformType& xform)
   {
      xform.GetTranslation(mBody1RelativeTranslation);
      xform.GetRotation(mBody1RelativeRotationHPR);
   }

   DT_IMPLEMENT_ACCESSOR(JointDesc, VectorType, Body2RelativeTranslation);
   DT_IMPLEMENT_ACCESSOR(JointDesc, VectorType, Body2RelativeRotationHPR);
   void JointDesc::GetBody2Frame(TransformType& xform) const
   {
      xform.SetTranslation(GetBody2RelativeTranslation());
      xform.SetRotation(GetBody2RelativeRotationHPR());
   }

   void JointDesc::SetBody2Frame(const TransformType& xform)
   {
      xform.GetTranslation(mBody2RelativeTranslation);
      xform.GetRotation(mBody2RelativeRotationHPR);
   }

   DT_IMPLEMENT_ACCESSOR(JointDesc, VectorType, LinearLimitMinimums);
   DT_IMPLEMENT_ACCESSOR(JointDesc, VectorType, LinearLimitMaximums);
   DT_IMPLEMENT_ACCESSOR(JointDesc, VectorType, AngularLimitMinimums);
   DT_IMPLEMENT_ACCESSOR(JointDesc, VectorType, AngularLimitMaximums);

   DT_IMPLEMENT_ACCESSOR(JointDesc, bool, DisableCollisionBetweenBodies);

   DT_IMPLEMENT_ARRAY_ACCESSOR(JointDesc, MotorDescPtr, Motor, Motors, new MotorDesc);


} /* namespace dtPhysics */
