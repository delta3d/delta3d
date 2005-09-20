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
#include <dtDAL/exceptionenum.h>
#include <dtCore/scene.h>

namespace dtDAL
{
    ///////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_ENUM(PhysicalActorProxy::CollisionGeomType);
    PhysicalActorProxy::CollisionGeomType
        PhysicalActorProxy::CollisionGeomType::NONE("NONE");
    PhysicalActorProxy::CollisionGeomType
        PhysicalActorProxy::CollisionGeomType::SPHERE("SPHERE");
    PhysicalActorProxy::CollisionGeomType
        PhysicalActorProxy::CollisionGeomType::CYLINDER("CYLINDER");
    PhysicalActorProxy::CollisionGeomType
        PhysicalActorProxy::CollisionGeomType::CUBE("CUBE");
    PhysicalActorProxy::CollisionGeomType
        PhysicalActorProxy::CollisionGeomType::RAY("RAY");
    PhysicalActorProxy::CollisionGeomType
        PhysicalActorProxy::CollisionGeomType::MESH("MESH");


    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::BuildPropertyMap()
    {
        const std::string GROUPNAME = "Physics";
        const std::string COLLISION_GROUP = "Collision";

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

        //COLLISION PROPS...
        AddProperty(new BooleanActorProperty("Collision Geometry", "Show Collision Geometry",
            MakeFunctor(*this, &PhysicalActorProxy::SetRenderCollisionGeometry),
            MakeFunctorRet(*this, &PhysicalActorProxy::GetRenderCollisionGeometry),
            "Enables/Disables the rendering of collision geometry assigned to this actor.",
            COLLISION_GROUP));

        AddProperty(new EnumActorProperty<CollisionGeomType>(
            "Collision Type","Collision Type",
            MakeFunctor(*this,&PhysicalActorProxy::SetCollisionType),
            MakeFunctorRet(*this,&PhysicalActorProxy::GetCollisionType),
            "Sets the type of geometry to use for collision detection",
            COLLISION_GROUP));

        AddProperty(new FloatActorProperty("Collision Radius","Collision Radius",
            MakeFunctor(*this,&PhysicalActorProxy::SetCollisionRadius),
            MakeFunctorRet(*this,&PhysicalActorProxy::GetCollisionRadius),
            "Sets the radius for collision calculations. This value is used differently "
            "depending on the type of collision assigned to this actor.  For example, "
            "if the collision type is set to SPHERE, this will be the sphere's radius.",
            COLLISION_GROUP));

        AddProperty(new FloatActorProperty("Collision Length","Collision Length",
            MakeFunctor(*this,&PhysicalActorProxy::SetCollisionLength),
            MakeFunctorRet(*this,&PhysicalActorProxy::GetCollisionLength),
            "Sets the length of the collision geometry. This value is used differently "
            "depending on the type of collision assigned to this actor.  For example, "
            "if the collision type is set to CYLINDER, this will be the cylinder's length.",
            COLLISION_GROUP));

        AddProperty(new Vec3ActorProperty("Collision Box","Collision Box",
            MakeFunctor(*this, &PhysicalActorProxy::SetCollisionBoxDims),
            MakeFunctorRet(*this, &PhysicalActorProxy::GetCollisionBoxDims),
            "Sets the size of the bounding box used for collision detection.  This property "
            "is used if the collision type is set to BOX.",
            COLLISION_GROUP));
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
    void PhysicalActorProxy::SetRenderCollisionGeometry(bool enable)
    {
        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        phys->RenderCollisionGeometry(enable);
    }

    ///////////////////////////////////////////////////////////////////////////////
    bool PhysicalActorProxy::GetRenderCollisionGeometry() const
    {
        const dtCore::Physical *phys = dynamic_cast<const dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        return phys->GetRenderCollisionGeometry();
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

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetCollisionType(PhysicalActorProxy::CollisionGeomType &type)
    {
        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        mCollisionType = &type;
        if (mCollisionType == &CollisionGeomType::NONE)
            phys->ClearCollisionGeometry();
        else if (mCollisionType == &CollisionGeomType::CUBE)
            SetBoxCollision();
        else if (mCollisionType == &CollisionGeomType::SPHERE)
            SetSphereCollision();
        else if (mCollisionType == &CollisionGeomType::CYLINDER)
            SetCylinderCollision();
        else if (mCollisionType == &CollisionGeomType::RAY)
            SetRayCollision();
        else if (mCollisionType == &CollisionGeomType::MESH)
            SetMeshCollision();
    }

    ///////////////////////////////////////////////////////////////////////////////
    PhysicalActorProxy::CollisionGeomType &PhysicalActorProxy::GetCollisionType()
    {
        return *mCollisionType;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetCollisionRadius(float radius)
    {
        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        mCollisionRadius = radius;
        if (mCollisionType == &CollisionGeomType::CYLINDER)
            SetCylinderCollision();
        else if (mCollisionType == &CollisionGeomType::SPHERE)
            SetSphereCollision();
    }

    ///////////////////////////////////////////////////////////////////////////////
    float PhysicalActorProxy::GetCollisionRadius() const
    {
        return mCollisionRadius;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetCollisionBoxDims(const osg::Vec3 &dims)
    {
        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        mCollisionBoxDims = dims;        
        SetBoxCollision();
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 PhysicalActorProxy::GetCollisionBoxDims() const
    {
        return mCollisionBoxDims;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetCollisionLength(float length)
    {
        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        mCollisionLength = length;
        if (mCollisionType == &CollisionGeomType::CYLINDER)
            SetCylinderCollision();
        else if (mCollisionType == &CollisionGeomType::RAY)
            SetRayCollision();
    }

    ///////////////////////////////////////////////////////////////////////////////
    float PhysicalActorProxy::GetCollisionLength() const
    {
        return mCollisionLength;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetBoxCollision()
    {
        if (mCollisionType != &CollisionGeomType::CUBE)
            return;

        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        phys->ClearCollisionGeometry();
        if (mCollisionBoxDims.x() == 0.0f || mCollisionBoxDims.y() == 0.0f ||
            mCollisionBoxDims.z() == 0.0f)
        {
            phys->SetCollisionBox(NULL);
        }
        else
        {
            phys->SetCollisionBox(mCollisionBoxDims.x(),mCollisionBoxDims.y(),
                                  mCollisionBoxDims.z());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetSphereCollision()
    {
        if (mCollisionType != &CollisionGeomType::SPHERE)
            return;

        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        phys->ClearCollisionGeometry();
        if (mCollisionRadius == 0.0f)
            phys->SetCollisionSphere((osg::Node *)NULL);
        else
            phys->SetCollisionSphere(mCollisionRadius);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetCylinderCollision()
    {
        if (mCollisionType != &CollisionGeomType::CYLINDER)
            return;

        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        phys->ClearCollisionGeometry();
        if (mCollisionRadius == 0.0f || mCollisionLength == 0.0f)
            phys->SetCollisionCappedCylinder(NULL);
        else
            phys->SetCollisionCappedCylinder(mCollisionRadius,mCollisionLength);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetRayCollision()
    {
        if (mCollisionType != &CollisionGeomType::RAY)
            return;

        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        phys->ClearCollisionGeometry();
        phys->SetCollisionRay(mCollisionLength);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PhysicalActorProxy::SetMeshCollision()
    {
        if (mCollisionType != &CollisionGeomType::MESH)
            return;

        dtCore::Physical *phys = dynamic_cast<dtCore::Physical*>(mActor.get());
        if (phys == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Physical.");

        phys->ClearCollisionGeometry();
        phys->SetCollisionMesh(NULL);
    }

}
