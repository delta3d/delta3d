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
#include "dtActors/cameraactorproxy.h"
#include "dtDAL/enginepropertytypes.h"

#include <dtCore/camera.h>

using namespace dtCore;
using namespace dtDAL;

namespace dtActors 
{

    ///////////////////////////////////////////////////////////////////////////////
    void CameraActorProxy::CreateActor()
    {
        mActor = new dtCore::Camera;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void CameraActorProxy::BuildPropertyMap()
    {
        const std::string &GROUPNAME = "Camera";
        TransformableActorProxy::BuildPropertyMap();

        Camera *cam = dynamic_cast<Camera*>(mActor.get());
        if(!cam)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be dtCore::Camera.");

        // This property is used for the manipulation of the clear color
        // of a camera. Uses 4 values in the RGBA format for color
        // representation. All values are clamped between 0 - 1.
        // Default is 0, 0, 1, 0 (blue)
        AddProperty(new ColorRgbaActorProperty("Clear Color", "Clear Color",
            MakeFunctor(*this, &dtActors::CameraActorProxy::SetClearColor),
            MakeFunctorRet(*this, &dtActors::CameraActorProxy::GetClearColor),
            "Sets the camera's clear color, which can be thought of as the background color", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec4f CameraActorProxy::GetClearColor()
    {
        Camera *cam = dynamic_cast<Camera*>(mActor.get());
        if(!cam)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be dtCore::Camera.");

        float r, g, b, a;
        cam->GetClearColor(&r, &g, &b, &a);
        return osg::Vec4f(r, g, b, a);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void CameraActorProxy::SetClearColor(const osg::Vec4f &color)
    {
        Camera *cam = dynamic_cast<Camera*>(mActor.get());
        if(!cam)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be dtCore::Camera.");

        cam->SetClearColor(color[0], color[1], color[2], color[3]);
    }
}
