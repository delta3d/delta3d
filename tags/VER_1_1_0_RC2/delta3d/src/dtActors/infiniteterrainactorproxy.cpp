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

#include "dtActors/infiniteterrainactorproxy.h"
#include "dtDAL/enginepropertytypes.h"

using namespace dtDAL;
using namespace dtCore;

namespace dtActors 
{
    void InfiniteTerrainActorProxy::CreateActor()
    {
        mActor = new dtCore::InfiniteTerrain();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void InfiniteTerrainActorProxy::BuildPropertyMap()
    {
        const std::string &GROUPNAME = "InfiniteTerrain";
        TransformableActorProxy::BuildPropertyMap();

        InfiniteTerrain *ter = dynamic_cast<InfiniteTerrain*>(mActor.get());
        if(!ter)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::InfiniteTerrain");

        // This property manipulates the segment size of an InfiniteTerrain.
        // Uses a float type to represent the segment size.
        // Default is 1000.0f
        AddProperty(new FloatActorProperty("Segment Size", "Segment Size",
            MakeFunctor(*ter, &InfiniteTerrain::SetSegmentSize),
            MakeFunctorRet(*ter, &InfiniteTerrain::GetSegmentSize),
            "Sets the size of each segment of the terrain. 0 would display no terrain", GROUPNAME));

        // This property manipulates the segment divisions of an InfiniteTerrain.
        // Uses an int type to represent the divisions.
        // Default is 128
        AddProperty(new IntActorProperty("Segment Divisions", "Segment Divisions",
            MakeFunctor(*ter, &InfiniteTerrain::SetSegmentDivisions),
            MakeFunctorRet(*ter, &InfiniteTerrain::GetSegmentDivisions),
            "Sets the amount of divisions in each terrain segment.", GROUPNAME));

        // This property manipulates the horizontal scale of an InfiniteTerrain.
        // Uses a float type to represent the scale.
        // Default is 0.1f
        AddProperty(new FloatActorProperty("Horizontal Scale", "Horizontal Scale",
            MakeFunctor(*ter, &InfiniteTerrain::SetHorizontalScale),
            MakeFunctorRet(*ter, &InfiniteTerrain::GetHorizontalScale),
            "Sets the horizontal scale of a terrain. ", GROUPNAME));

        // This property manipulates the vertical scale of an InfiniteTerrain.
        // Uses a float type to represent the scale.
        // Default is 25.0f
        AddProperty(new FloatActorProperty("Vertical Scale", "Vertical Scale",
            MakeFunctor(*ter, &InfiniteTerrain::SetVerticalScale),
            MakeFunctorRet(*ter, &InfiniteTerrain::GetVerticalScale),
            "Sets the vertical scale of a terrain.", GROUPNAME));

        // This property manipulates the build distance of an InfiniteTerrain.
        // Uses a float type to represent the scale. Represents how far from the
        // eyepoint the terrain in generated.
        // Default is 3000.0f
        AddProperty(new FloatActorProperty("Build Distance", "Build Distance",
            MakeFunctor(*ter, &InfiniteTerrain::SetBuildDistance),
            MakeFunctorRet(*ter, &InfiniteTerrain::GetBuildDistance),
            "Sets the distance away from the camera that terrain is rendered.", GROUPNAME));

        // This property manipulates the collision method of an InfiniteTerrain.
        // True enables the usage of the underlying noise to determine collision,
        // as opposed to collision with the actual triangle mesh.
        // Default is false
        AddProperty(new BooleanActorProperty("Smooth Collision", "Smooth Collision",
            MakeFunctor(*ter, &InfiniteTerrain::EnableSmoothCollisions),
            MakeFunctorRet(*ter, &InfiniteTerrain::SmoothCollisionsEnabled),
            "Toggles smooth collision between a terrain and other dtCore::Transformable", GROUPNAME));
    }
}
