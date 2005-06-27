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

#include "dtDAL/physicalactorproxy.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/exception.h"
#include <dtCore/scene.h>

using namespace dtCore;
using namespace dtDAL;

namespace dtDAL 
{
    void PhysicalActorProxy::BuildPropertyMap()
    {
        const std::string GROUPNAME = "Physics";
        TransformableActorProxy::BuildPropertyMap();

        Physical *phys = dynamic_cast<Physical*>(mActor.get());
        if (phys == NULL) 
        {
            EXCEPT(ExceptionEnum::InvalidActorException,
                "Actor should be type dtCore::Physical.");
        }

        AddProperty(new BooleanActorProperty("Enable Dynamics", "Enable Dynamics",
            MakeFunctor(*phys, &Physical::EnableDynamics),
            MakeFunctorRet(*phys, &Physical::DynamicsEnabled),
            "Enables the physics of an object", GROUPNAME));

        AddProperty(new FloatActorProperty("Mass", "Mass",
            MakeFunctor(*this, &PhysicalActorProxy::SetMass),
            MakeFunctorRet(*this, &PhysicalActorProxy::GetMass),
            "Sets the mass of object. Helpful when using your own physics like gravity.",
            GROUPNAME));

        AddProperty(new BooleanActorProperty("Collision Geometry", "Collision Geometry",
            MakeFunctor(*this, &PhysicalActorProxy::SetRenderCollisionGeometry),
            MakeFunctorRet(*this, &PhysicalActorProxy::GetRenderCollisionGeometry),
            "Enables the rendering of collision geometry", GROUPNAME));

        AddProperty(new Vec3ActorProperty("Center of Gravity", "Center of Gravity",
            MakeFunctor(*this, &PhysicalActorProxy::SetCenterOfGravity),
            MakeFunctorRet(*this, &PhysicalActorProxy::GetCenterOfGravity),
            "Sets the center of gravity of an object", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetMass(float mass)
    {
        Physical *phys = dynamic_cast<Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        phys->SetMass(mass);
    }

    ///////////////////////////////////////////////////////////////////////////////
    float PhysicalActorProxy::GetMass()
    {
        Physical *phys = dynamic_cast<Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        return phys->GetMass();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetRenderCollisionGeometry(bool enable)
    {
        Physical *phys = dynamic_cast<Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        phys->RenderCollisionGeometry(enable);
    }

    ///////////////////////////////////////////////////////////////////////////////
    bool PhysicalActorProxy::GetRenderCollisionGeometry()
    {
        Physical *phys = dynamic_cast<Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        return phys->GetRenderCollisionGeometry();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetCenterOfGravity(const osg::Vec3 &g)
    {
        Physical *phys = dynamic_cast<Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        sgVec3 r;
        r[0] = g[0]; r[1] = g[1]; r[2] = g[2];
        phys->SetCenterOfGravity(r);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 PhysicalActorProxy::GetCenterOfGravity()
    {
        Physical *phys = dynamic_cast<Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");


        osg::Vec3 r;
        sgVec3 g;
        phys->GetCenterOfGravity(g);
        r[0] = g[0]; r[1] = g[1]; r[2] = g[2];
        return r;
    }
}
