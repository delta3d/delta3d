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

#include <dtActors/positionallightactorproxy.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>

namespace dtActors 
{
    void PositionalLightActorProxy::CreateActor()
    {
        mActor = new dtCore::PositionalLight(0);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PositionalLightActorProxy::BuildPropertyMap()
    {
        const std::string &GROUPNAME = "Light";
        BaseLightActorProxy::BuildPropertyMap();
        TransformableActorProxy::BuildPropertyMap();

        // This property sets that values that control how fast light fades
        // as one moves away from the light. It uses 3 values to manipulate
        // the attenuation. The values should be in the following form:
        // constant, linear. quadratic.
        AddProperty(new dtDAL::Vec3ActorProperty("Attenuation", "Attenuation",
            MakeFunctor(*this, &dtActors::PositionalLightActorProxy::SetAttenuation),
            MakeFunctorRet(*this, &dtActors::PositionalLightActorProxy::GetAttenuation),
            "Sets a light's attenuation.",GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    dtDAL::ActorProxyIcon* PositionalLightActorProxy::GetBillBoardIcon()
    {
       if( !mBillBoardIcon.valid() )
       {
          mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::LIGHT);
       }

       return mBillBoardIcon.get();
    }
}
