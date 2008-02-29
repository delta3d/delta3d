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

#include <dtActors/deltaobjectactorproxy.h>
#include <dtCore/object.h>
#include <dtDAL/enginepropertytypes.h>

namespace dtActors 
{
    void DeltaObjectActorProxy::CreateActor()
    {
        SetActor(*new dtCore::Object);
        //std::cout << "Creating actor proxy." << std::endl;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void DeltaObjectActorProxy::BuildPropertyMap()
    {
        PhysicalActorProxy::BuildPropertyMap();

        AddProperty(new dtDAL::Vec3ActorProperty("Scale", "Scale", 
           dtDAL::MakeFunctor(*this, &DeltaObjectActorProxy::SetScale), 
           dtDAL::MakeFunctorRet(*this, &DeltaObjectActorProxy::GetScale), 
           "Specifies the scale of the object"));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void DeltaObjectActorProxy::SetScale(const osg::Vec3 &xyz)
    {
       dtCore::Object *actor = NULL;
       GetActor(actor);
       actor->GetModel().SetScale(xyz);
    }

    osg::Vec3 DeltaObjectActorProxy::GetScale() const
    {
       osg::Vec3 xyz;
       const dtCore::Object *actor = NULL;
       GetActor(actor);
       actor->GetModel().GetScale(xyz);
       return xyz;
    }
}
