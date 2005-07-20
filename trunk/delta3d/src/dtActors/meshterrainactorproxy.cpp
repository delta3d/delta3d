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

#include "dtActors/meshterrainactorproxy.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/resourcedescriptor.h"
#include "dtDAL/actorproxyicon.h"

#include <dtCore/object.h>

using namespace dtCore;
using namespace dtDAL;

namespace dtActors
{
    void MeshTerrainActorProxy::BuildPropertyMap()
    {
        const std::string &GROUPNAME = "Terrain";
        DeltaObjectActorProxy::BuildPropertyMap();

        Object *obj = dynamic_cast<Object*> (mActor.get());
        if(!obj)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Object");

        AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TERRAIN,
                    "terrain mesh", "Terrain Mesh", MakeFunctor(*this, &MeshTerrainActorProxy::LoadFile),
                    "The mesh that defines the geometry of the terrain.", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MeshTerrainActorProxy::LoadFile(const std::string &fileName)
    {
        dtCore::Object *obj = dynamic_cast<dtCore::Object*>(mActor.get());
        if(!obj)
        {
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException,
                "Actor should be type dtCore::Object");
        }

        obj->LoadFile(fileName);
        if (obj->GetOSGNode() == NULL)
        {
            LOG_ERROR("Error loading terrain mesh file: " + fileName);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    const dtDAL::ActorProxy::RenderMode &MeshTerrainActorProxy::GetRenderMode()
    {
        dtDAL::ResourceDescriptor *resource = GetResource("terrain mesh");
        if (resource != NULL)
        {
            if (resource->GetResourceIdentifier().empty() || mActor->GetOSGNode() == NULL)
                return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
            else
                return dtDAL::ActorProxy::RenderMode::DRAW_ACTOR;
        }
        else
            return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
    }

    //////////////////////////////////////////////////////////////////////////
    dtDAL::ActorProxyIcon *MeshTerrainActorProxy::GetBillBoardIcon()
    {
        if(!mBillBoardIcon.valid())
        {
            mBillBoardIcon =
                new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::MESHTERRAIN);
        }

        return mBillBoardIcon.get();
    }
}
