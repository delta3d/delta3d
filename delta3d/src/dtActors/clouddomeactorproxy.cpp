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

#include "dtActors/clouddomeactorproxy.h"
#include "dtDAL/enginepropertytypes.h"
#include <dtCore/deltadrawable.h>

using namespace dtDAL;
using namespace dtCore;

namespace dtActors
{
    void CloudDomeActorProxy::CreateActor()
    {
        mActor = new CloudDome(6, 1, 6, 1, .3, 0.96, 256, 1800);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void CloudDomeActorProxy::BuildPropertyMap()
    {
        CloudDome *cd = dynamic_cast<dtCore::CloudDome*>(mActor.get());
        const std::string GROUPNAME = "dtCore_CloudPlane";

        if(!cd)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::CloudDome");

        AddProperty(new FloatActorProperty("Scale", "Scale",
            MakeFunctor(*cd, &CloudDome::SetScale),
            MakeFunctorRet(*cd, &CloudDome::GetScale),
            "Sets the scaling factor of a cloud dome.", GROUPNAME));

        AddProperty(new FloatActorProperty("Exponent", "Exponent",
            MakeFunctor(*cd, &CloudDome::SetExponent),
            MakeFunctorRet(*cd, &CloudDome::GetExponent),
            "Sets the exponent of a cloud dome.", GROUPNAME));

        AddProperty(new FloatActorProperty("Cutoff", "Cutoff",
            MakeFunctor(*cd, &CloudDome::SetCutoff),
            MakeFunctorRet(*cd, &CloudDome::GetCutoff),
            "Sets the cutoff factor of a cloud dome.", GROUPNAME));

        AddProperty(new FloatActorProperty("SpeedX", "SpeedX",
            MakeFunctor(*cd, &CloudDome::SetSpeedX),
            MakeFunctorRet(*cd, &CloudDome::GetSpeedX),
            "Sets the speed x of a cloud dome.", GROUPNAME));

        AddProperty(new FloatActorProperty("SpeedY", "SpeedY",
            MakeFunctor(*cd, &CloudDome::SetSpeedY),
            MakeFunctorRet(*cd, &CloudDome::GetSpeedY),
            "Sets the speed y of a cloud dome.", GROUPNAME));

        AddProperty(new FloatActorProperty("Bias", "Bias",
            MakeFunctor(*cd, &CloudDome::SetBias),
            MakeFunctorRet(*cd, &CloudDome::GetBias),
            "Sets the bias of a cloud dome.", GROUPNAME));

        AddProperty(new BooleanActorProperty("EnableShaders", "EnableShaders",
            MakeFunctor(*cd, &CloudDome::SetShaderEnable),
            MakeFunctorRet(*cd, &CloudDome::GetEnable),
            "Enables shaders on a cloud dome.", GROUPNAME));

        AddProperty(new Vec3ActorProperty("Cloud Color", "Cloud Color",
            MakeFunctor(*this, &CloudDomeActorProxy::SetCloudColor),
            MakeFunctorRet(*this, &CloudDomeActorProxy::GetCloudColor),
            "Sets the cloud color of a cloud dome.", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void CloudDomeActorProxy::SetCloudColor(const osg::Vec3 &color)
    {
        CloudDome *cd = dynamic_cast<CloudDome*> (mActor.get());
        if(cd == NULL)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::CloudDome");

        cd->SetCloudColor(color);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 CloudDomeActorProxy::GetCloudColor()
    {
        CloudDome *cd = dynamic_cast<CloudDome*> (mActor.get());
        if(cd == NULL)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::CloudDome");

        return cd->GetCloudColor();
    }
}
