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

#include "dtActors/terrainactorproxy.h"
#include "dtDAL/enginepropertytypes.h"

using namespace dtCore;
using namespace dtDAL;

namespace dtActors 
{
    void TerrainActorProxy::CreateActor()
    {
        mActor = new dtCore::Terrain;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void TerrainActorProxy::BuildPropertyMap()
    {
        const std::string &GROUPNAME = "Terrain";
        TransformableActorProxy::BuildPropertyMap();

        Terrain *ter = dynamic_cast<Terrain*>(mActor.get());
        if(!ter)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Terrain");


        // This property sets the location of the origin in geographic coordinates.
        // It uses 3 values to manipulate the geographic coordinates.
        // Default is 0, 0, 0
        AddProperty(new Vec3ActorProperty("Geo Origin", "Geo Origin",
            MakeFunctor(*this, &TerrainActorProxy::SetGeoOrigin),
            MakeFunctorRet(*this, &TerrainActorProxy::GetGeoOrigin),
            "Sets origin of a terrain's geometry.", GROUPNAME));

        // This property sets the load distance of a Terrain. It uses
        // a float type to manipulate load distance. The load distance
        // is the distance from the eyepoint for which terrain is guaranteed
        // to be loaded (if present)
        // Default is 0, 0, 0
        AddProperty(new FloatActorProperty("Load Distance", "Load Distance",
            MakeFunctor(*ter, &Terrain::SetLoadDistance),
            MakeFunctorRet(*ter, &Terrain::GetLoadDistance),
            "Sets the distance from the eyepoint the terrain is gauranteed to be loaded.", GROUPNAME));

        // This property sets the cache path of a Terrain. It uses a string
        // to represent the path that the Terrain uses to cache.
        // Default is ""
        AddProperty(new StringActorProperty("Cache Path", "Cache Path",
            MakeFunctor(*this, &TerrainActorProxy::SetCachePath),
            MakeFunctorRet(*this, &TerrainActorProxy::GetCachePath),
            "Sets the path the terrain caches with.", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void TerrainActorProxy::SetGeoOrigin(const osg::Vec3 &origin)
    {
        Terrain *t = dynamic_cast<Terrain*>(mActor.get());
        if(!t)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Terrain");

        t->SetGeoOrigin(origin[0], origin[1], origin[2]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 TerrainActorProxy::GetGeoOrigin()
    {
        Terrain *t = dynamic_cast<Terrain*>(mActor.get());
        if(!t)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Terrain");

        double la, lo, el;
        t->GetGeoOrigin(&la, &lo, &el);
        return osg::Vec3(la, lo, el);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void TerrainActorProxy::SetCachePath(const std::string &path)
    {
        Terrain *t = dynamic_cast<Terrain*>(mActor.get());
        if(!t)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Terrain");

        t->SetCachePath(path);
    }

    ///////////////////////////////////////////////////////////////////////////////
    std::string TerrainActorProxy::GetCachePath()
    {
        Terrain *t = dynamic_cast<Terrain*>(mActor.get());
        if(!t)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Terrain");

        return t->GetCachePath();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void TerrainActorProxy::AddDTEDPath(std::string &fileName)
    {
        dtCore::Terrain *t = dynamic_cast<dtCore::Terrain*>(mActor.get());
        if(!t)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Terrain");

        t->AddDTEDPath(fileName);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void TerrainActorProxy::RemoveDTEDPath(std::string &fileName)
    {
        Terrain *t = dynamic_cast<Terrain*>(mActor.get());
        if(!t)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Terrain");

        t->RemoveDTEDPath(fileName);
    }

}
