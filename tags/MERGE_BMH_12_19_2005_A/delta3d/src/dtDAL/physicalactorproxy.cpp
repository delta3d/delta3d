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
* @author William E. Johnson II
*/

#include <dtDAL/physicalactorproxy.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/exceptionenum.h>
#include <dtCore/scene.h>

namespace dtDAL
{
    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::BuildPropertyMap()
    {
        const std::string GROUPNAME = "Physics";
        
        TransformableActorProxy::BuildPropertyMap();

        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
        {
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException,
                "Actor should be type dtCore::Physical.");
        }

        //PHYSICS PROPS...
        AddProperty(new BooleanActorProperty("Enable Dynamics", "Enable Dynamics",
            MakeFunctor(*phys, &dtCore::Physical::EnableDynamics),
            MakeFunctorRet(*phys, &dtCore::Physical::DynamicsEnabled),
            "Enables physics calculations on this actor.", GROUPNAME));

        AddProperty(new FloatActorProperty("Mass", "Mass",
            MakeFunctor(*this, &PhysicalActorProxy::SetMass),
            MakeFunctorRet(*this, &PhysicalActorProxy::GetMass),
            "Sets the mass of this actor.",GROUPNAME));

        AddProperty(new Vec3ActorProperty("Center of Gravity", "Center of Gravity",
            MakeFunctor(*this, &PhysicalActorProxy::SetCenterOfGravity),
            MakeFunctorRet(*this, &PhysicalActorProxy::GetCenterOfGravity),
            "Sets the center of gravity for this actor.", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetMass(float mass)
    {
        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        phys->SetMass(mass);
    }

    ///////////////////////////////////////////////////////////////////////////////
    float PhysicalActorProxy::GetMass() const
    {
        const dtCore::Physical *phys = dynamic_cast<const dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        return phys->GetMass();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetCenterOfGravity(const osg::Vec3 &g)
    {
        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        phys->SetCenterOfGravity(g);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 PhysicalActorProxy::GetCenterOfGravity() const
    {
        const dtCore::Physical *phys = dynamic_cast<const dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        osg::Vec3 r;
        phys->GetCenterOfGravity(r);
        return r;
    }
}
