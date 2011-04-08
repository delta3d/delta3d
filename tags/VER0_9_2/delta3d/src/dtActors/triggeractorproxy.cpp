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

#include "dtActors/triggeractorproxy.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/exception.h"
#include <dtCore/transformable.h>

using namespace dtCore;
using namespace dtDAL;

namespace dtActors 
{
    void TriggerActorProxy::CreateActor()
    {
        //Obviously, when Delta3D supports triggers, that needs to be created instead of a
        //transformable.
        mActor = new dtCore::Transformable;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void TriggerActorProxy::BuildPropertyMap()
    {
        TransformableActorProxy::BuildPropertyMap();

        Transformable *t = dynamic_cast<Transformable*>(mActor.get());
        if(!t)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Transformable");
    }
}