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

#include "dtActors/skyboxactorproxy.h"
#include "dtDAL/enginepropertytypes.h"

namespace dtActors 
{
    void SkyBoxActorProxy::CreateActor()
    {
        mActor = new dtCore::SkyBox;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SkyBoxActorProxy::BuildPropertyMap()
    {
        const std::string GROUPNAME = "SkyBox Textures";

        AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TEXTURE,
                "Top Texture", "Top Texture",
                MakeFunctor(*this, &SkyBoxActorProxy::SetTopTexture),
                "Sets the texture on the top of the sky box", GROUPNAME));

        AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TEXTURE,
            "Bottom Texture", "Bottom Texture",
            MakeFunctor(*this, &SkyBoxActorProxy::SetBottomTexture),
            "Sets the texture on the bottom of the sky box", GROUPNAME));

        AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TEXTURE,
            "Left Texture", "Left Texture",
            MakeFunctor(*this, &SkyBoxActorProxy::SetLeftTexture),
            "Sets the texture on the left of the sky box", GROUPNAME));

        AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TEXTURE,
            "Right Texture", "Right Texture",
            MakeFunctor(*this, &SkyBoxActorProxy::SetRightTexture),
            "Sets the texture on the right of the sky box", GROUPNAME));

        AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TEXTURE,
            "Front Texture", "Front Texture",
            MakeFunctor(*this, &SkyBoxActorProxy::SetFrontTexture),
            "Sets the texture on the front of the sky box", GROUPNAME));

        AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TEXTURE,
            "Back Texture", "Back Texture",
            MakeFunctor(*this, &SkyBoxActorProxy::SetBackTexture),
            "Sets the texture on the back of the sky box", GROUPNAME));
    }
}
