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
 * Matthew W. Campbell
 */
#include "dtCore/enginepropertytypes.h"
#include "dtActors/infinitelightactorproxy.h"

#include <dtCore/object.h>

namespace dtActors
{
    void InfiniteLightActorProxy::CreateDrawable()
    {
        SetDrawable(*new dtCore::InfiniteLight(0));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void InfiniteLightActorProxy::BuildPropertyMap()
    {
        const std::string GROUPNAME = "Light";
        BaseLightActorProxy::BuildPropertyMap();
        TransformableActorProxy::BuildPropertyMap();

        ////Infinite lights only have a direction property so add it.
        //AddProperty(new dtCore::Vec3ActorProperty("Direction","Direction",
        //    dtCore::SetFuncType(this,&InfiniteLightActorProxy::SetDirection),
        //    dtCore::GetFuncType(*this,&InfiniteLightActorProxy::GetDirection),
        //    "Determines the direction that the light is pointing.",GROUPNAME));
    }
}
