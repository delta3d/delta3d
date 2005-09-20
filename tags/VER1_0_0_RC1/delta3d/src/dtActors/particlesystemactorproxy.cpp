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

#include "dtActors/particlesystemactorproxy.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/actorproxyicon.h"

using namespace dtCore;
using namespace dtDAL;

namespace dtActors
{
    void ParticleSystemActorProxy::CreateActor()
    {
        mActor = new dtCore::ParticleSystem;
    }

    //////////////////////////////////////////////////////////////////////////
    void ParticleSystemActorProxy::BuildPropertyMap()
    {
        const std::string &GROUPNAME = "Particle System";
        TransformableActorProxy::BuildPropertyMap();

        ParticleSystem *ps = dynamic_cast<ParticleSystem*>(mActor.get());
        if(!ps)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::ParticleSystem");

        // This property toggles the enabling of a Particle System.
        // A value of true enables the particle system, which in turn
        // makes it visible in a scene. A vaule of false will make the
        // Particle System not visible.
        // Default is true
        AddProperty(new BooleanActorProperty("Enable", "Enabled",
            MakeFunctor(*ps, &ParticleSystem::SetEnabled),
            MakeFunctorRet(*ps, &ParticleSystem::IsEnabled),
            "Toggles the visibility of a particle system to the camera.", GROUPNAME));

        // This property toggles whether or not a Particle System is parent
        // relative. Sets the parent-relative state of this particle system.
        // If parent-relative mode is enabled, the entire particle system
        // will be positioned relative to the parent.
        // Default is false
        AddProperty(new BooleanActorProperty("Parent Relative", "Parent Relative",
            MakeFunctor(*ps, &ParticleSystem::SetParentRelative),
            MakeFunctorRet(*ps, &ParticleSystem::IsParentRelative),
            "Sets if a partical system is relative to its parent, if any. ", GROUPNAME));

        // This property enables the loading of a particle resource file.
        AddProperty(new ResourceActorProperty(*this, DataType::PARTICLE_SYSTEM, "Particle(s) File",
            "particle file", MakeFunctor(*this, &ParticleSystemActorProxy::LoadFile),
            "Sets the resource file of this particle system", GROUPNAME));
    }

    //////////////////////////////////////////////////////////////////////////
    dtDAL::ActorProxyIcon* ParticleSystemActorProxy::GetBillBoardIcon()
    {
        if(!mBillBoardIcon.valid())
        {
            mBillBoardIcon =
                new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::PARTICLESYSTEM);
        }

        return mBillBoardIcon.get();
    }

}
