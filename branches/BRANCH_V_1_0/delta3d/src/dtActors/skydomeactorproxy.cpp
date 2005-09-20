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

#include "dtActors/skydomeactorproxy.h"
#include "dtDAL/enginepropertytypes.h"
#include <dtCore/deltadrawable.h>

using namespace dtCore;
using namespace dtDAL;

namespace dtActors 
{
    void SkyDomeActorProxy::CreateActor()
    {
        mActor = new dtCore::SkyDome;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SkyDomeActorProxy::BuildPropertyMap()
    {
        const std::string &GROUPNAME = "Sky";
        SkyDome *sd = dynamic_cast<SkyDome*> (mActor.get());
        if(!sd)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::SkyDome");

        AddProperty(new Vec3ActorProperty("Base Color", "Base Color",
            MakeFunctor(*this, &SkyDomeActorProxy::SetBaseColor),
            MakeFunctorRet(*this, &SkyDomeActorProxy::GetBaseColor),
            "Sets the color of a sky dome.", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 SkyDomeActorProxy::GetBaseColor()
    {
        SkyDome *sd = dynamic_cast<SkyDome*> (mActor.get());
        if(!sd)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::SkyDome");

        osg::Vec3 color;
        sd->GetBaseColor(color);
        return color;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SkyDomeActorProxy::SetBaseColor(const osg::Vec3 &color)
    {
        SkyDome *sd = dynamic_cast<SkyDome*> (mActor.get());
        if(!sd)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::SkyDome");

        sd->SetBaseColor(color);
    }

}
