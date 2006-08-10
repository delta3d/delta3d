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
* @author Matthew W. Campbell
*/
#include "dtActors/baselightactorproxy.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/actorproxyicon.h"

namespace dtActors
{
    ///////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_ENUM(BaseLightActorProxy::LightModeEnum);
    BaseLightActorProxy::LightModeEnum
        BaseLightActorProxy::LightModeEnum::GLOBAL("GLOBAL");
    BaseLightActorProxy::LightModeEnum
        BaseLightActorProxy::LightModeEnum::LOCAL("LOCAL");

    ///////////////////////////////////////////////////////////////////////////////
    void BaseLightActorProxy::BuildPropertyMap()
    {
        const std::string GROUPNAME = "Light";

        dtCore::Light *light = dynamic_cast<dtCore::Light *>(mActor.get());
        if(!light)
        {
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException,
                "Actor should be type dtCore::Light.");
        }

        //Now, add all of the properties for the actor.
        AddProperty(new dtDAL::BooleanActorProperty("Enable","Enabled",
            dtDAL::MakeFunctor(*light,&dtCore::Light::SetEnabled),
            dtDAL::MakeFunctorRet(*light,&dtCore::Light::GetEnabled),
            "Sets whether or not the light is enabled in the scene.", GROUPNAME));

        //In this property, the actor object itself did not have a set method
        //that matched the method signature expected in the property class.
        //Therefore, the setter is implemented in the proxy and it is passed
        //to the property as the setter method.
        AddProperty(new dtDAL::IntActorProperty("LightNum","Light Number",
            dtDAL::MakeFunctor(*this,&BaseLightActorProxy::SetNumber),
            dtDAL::MakeFunctorRet(*light,&dtCore::Light::GetNumber),
            "Sets the OpenGL light number for this light actor.", GROUPNAME));

        //Property for the light's lighting mode. Uses a dtDAL::Enumeration
        // to represent the lighting mode. Can be either GLOBAL or LOCAL.
        // Default is GLOBAL.
        AddProperty(new dtDAL::EnumActorProperty<LightModeEnum>("Lighting Mode","Lighting Mode",
            dtDAL::MakeFunctor(*this,&BaseLightActorProxy::SetLightingMode),
            dtDAL::MakeFunctorRet(*this,&BaseLightActorProxy::GetLightingMode),
            "Sets the lighting mode for this light actor.", GROUPNAME));

        //Property for the light's ambient color. Uses 4 values represented
        // in the RGBA format to manipulate the ambient color of a light.
        // All values are clamped from 0 - 1.
        AddProperty(new dtDAL::ColorRgbaActorProperty("Ambient","Ambient Color",
            dtDAL::MakeFunctor(*this,&BaseLightActorProxy::SetAmbient),
            dtDAL::MakeFunctorRet(*this,&BaseLightActorProxy::GetAmbient),
            "Sets the ambient color value for this light actor.", GROUPNAME));

        //Property for the light's diffuse color. Uses 4 values represented
        // in the RGBA format to manipulate the diffuse color of a light.
        // All values are clamped from 0 - 1.
        AddProperty(new dtDAL::ColorRgbaActorProperty("Diffuse","Diffuse Color",
            dtDAL::MakeFunctor(*this,&BaseLightActorProxy::SetDiffuse),
            dtDAL::MakeFunctorRet(*this,&BaseLightActorProxy::GetDiffuse),
            "Sets the diffuse color value for this light actor.", GROUPNAME));

        //Property for the light's specular color. Uses 4 values represented
        // in the RGBA format to manipulate the specular color of a light.
        // All values are clamped from 0 - 1.
        AddProperty(new dtDAL::ColorRgbaActorProperty("Specular","Specular Color",
            dtDAL::MakeFunctor(*this,&BaseLightActorProxy::SetSpecular),
            dtDAL::MakeFunctorRet(*this,&BaseLightActorProxy::GetSpecular),
            "Sets the specular color value for this light actor.", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    dtDAL::ActorProxyIcon* BaseLightActorProxy::GetBillBoardIcon()
    {
        if(!mBillBoardIcon.valid())
        {
            mBillBoardIcon =
                 new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::LIGHT);
        }

        return mBillBoardIcon.get();
    }

}
