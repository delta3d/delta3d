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
#include <dtActors/spotlightactorproxy.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>

using namespace dtCore;
using namespace dtDAL;

namespace dtActors 
{
    void SpotlightActorProxy::CreateActor()
    {
        mActor = new dtCore::SpotLight(0);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SpotlightActorProxy::BuildPropertyMap()
    {
        const std::string &GROUPNAME = "Light";
        PositionalLightActorProxy::BuildPropertyMap();

        SpotLight *sl = dynamic_cast<SpotLight*>(mActor.get());
        if(!sl)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::SpotLight");


        // This property manipulates the cutoff oa spotlight. It uses
        // a float type to represent the cutoff factor.
        // Default is 22.5f
        AddProperty(new dtDAL::FloatActorProperty("Cutoff", "Cutoff",
            dtDAL::MakeFunctor(*sl, &dtCore::SpotLight::SetSpotCutoff),
            dtDAL::MakeFunctorRet(*sl, &dtCore::SpotLight::GetSpotCutoff),
            "Sets the spotlight cutoff factor.", GROUPNAME));

        // This property manipulates the exponent of a spotlight. It uses
        // a float type to represent the exponent factor. The higher the
        // SpotExponent, the more concentrated the light will be in the
        // center of the cone.
        // Default is 1.0f
        AddProperty(new dtDAL::FloatActorProperty("Spot Exponent", "Spot Exponent",
            dtDAL::MakeFunctor(*sl, &dtCore::SpotLight::SetSpotExponent),
            dtDAL::MakeFunctorRet(*sl, &dtCore::SpotLight::GetSpotExponent),
            "Sets the concentration of the light in the center of its cone.", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    dtDAL::ActorProxyIcon* SpotlightActorProxy::GetBillBoardIcon()
    {
       if( !mBillBoardIcon.valid() )
       {
          mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::LIGHT);
       }

       return mBillBoardIcon.get();
    }
}
