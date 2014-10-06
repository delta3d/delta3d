/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * William E. Johnson II
 */
#include <prefix/dtcoreprefix.h>
#include <dtCore/physicalactorproxy.h>

#include <dtCore/scene.h>

#include <dtCore/booleanactorproperty.h>

#include <dtCore/floatactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/physical.h>

namespace dtCore
{
   const dtUtil::RefString PhysicalActorProxy::PROPERTY_ENABLE_DYNAMICS("Enable Dynamics");
   const dtUtil::RefString PhysicalActorProxy::PROPERTY_MASS("Mass");

   /////////////////////////////////////////////////////////////////////////////
   void PhysicalActorProxy::BuildPropertyMap()
   {
      static const dtUtil::RefString GROUPNAME("ODE Physics");

      TransformableActorProxy::BuildPropertyMap();

      if (!GetHideDTCorePhysicsProps())
      {
         dtCore::Physical* phys = GetDrawable<Physical>();
         if (phys != NULL)
         {

            static const dtUtil::RefString ODE_PREFIX("ODE ");
            //PHYSICS PROPS...
            AddProperty(new BooleanActorProperty(PROPERTY_ENABLE_DYNAMICS, ODE_PREFIX + PROPERTY_ENABLE_DYNAMICS,
                     BooleanActorProperty::SetFuncType(phys, &dtCore::Physical::EnableDynamics),
                     BooleanActorProperty::GetFuncType(phys, &dtCore::Physical::DynamicsEnabled),
                     "Enables physics calculations on this actor (using ODE).", GROUPNAME));

            AddProperty(new FloatActorProperty(PROPERTY_MASS, ODE_PREFIX + PROPERTY_MASS,
                     FloatActorProperty::SetFuncType(this, &PhysicalActorProxy::SetMass),
                     FloatActorProperty::GetFuncType(this, &PhysicalActorProxy::GetMass),
                     "Sets the mass of this actor (using ODE).",GROUPNAME));
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicalActorProxy::SetMass(float mass)
   {
      dtCore::Physical* phys = GetDrawable<Physical>();
      if (phys == NULL) return;

      phys->SetMass(mass);
   }

   /////////////////////////////////////////////////////////////////////////////
   float PhysicalActorProxy::GetMass() const
   {
      const dtCore::Physical* phys = GetDrawable<Physical>();
      if (phys == NULL) return 0.0f;

      return phys->GetMass();
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicalActorProxy::SetCenterOfGravity(const osg::Vec3& g)
   {
      dtCore::Physical* phys = GetDrawable<Physical>();
      if (phys == NULL) return;

      phys->SetCenterOfGravity(g);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 PhysicalActorProxy::GetCenterOfGravity() const
   {
      const dtCore::Physical* phys = GetDrawable<Physical>();

      osg::Vec3 r;
      if (phys != NULL)
         phys->GetCenterOfGravity(r);
      return r;
   }
}
