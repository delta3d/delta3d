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

#include <rbody/config_error.h>

#include "dtActors/characteractorproxy.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/actorproxyicon.h"

using namespace dtDAL;
using namespace dtChar;

namespace dtActors
{
    ///////////////////////////////////////////////////////////////////////////////
    void CharacterActorProxy::CreateActor()
    {
        mActor = new dtChar::Character;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void CharacterActorProxy::BuildPropertyMap()
    {
        const std::string &GROUPNAME = "Character";
        TransformableActorProxy::BuildPropertyMap();

        Character *c = dynamic_cast<Character*>(mActor.get());

        if(c == NULL)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtChar::Character.");

        // This property is used for the manipulation of the velocity
        // of a Character. Uses a float type to represent the velocity
        // Default is 0.0f


        //DO NOT RE-ADD this property
        //AddProperty(new FloatActorProperty("Velocity", "Velocity",
        //    MakeFunctor(*c, &Character::SetVelocity),
        //    MakeFunctorRet(*c, &Character::GetVelocity),
         //   "Sets the velocity of a character. This effects how fast it walks.", GROUPNAME));

        AddProperty(new ResourceActorProperty(*this, DataType::CHARACTER, "model", "Character Model",
            MakeFunctor(*this, &CharacterActorProxy::LoadFile),
            "This associates a drawable model with a character",GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void CharacterActorProxy::LoadFile(const std::string &fileName)
    {
        dtChar::Character *c = dynamic_cast<dtChar::Character*>(mActor.get());
        if (c == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtChar::Character");

        try
        {
            c->LoadFile(fileName);
        }
        catch(const rbody::config_error& ex)
        {
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Error loading character \"%s\": %s",
                fileName.c_str(), ex.what());
            //HACK -- the actor is broken internally now, so it needs to be deleted.
            std::string oldName = GetName();
            dtCore::UniqueId oldId = GetId();
            CreateActor();
            SetName(oldName);
            SetId(oldId);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    const dtDAL::ActorProxy::RenderMode& CharacterActorProxy::GetRenderMode()
    {
        dtDAL::ResourceDescriptor *resource = GetResource("model");
        if (resource != NULL)
        {
            if (resource->GetResourceIdentifier().empty() || mActor->GetOSGNode() == NULL)
                return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
            else
                return dtDAL::ActorProxy::RenderMode::DRAW_ACTOR;
        }
        else
            return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
    }

    //////////////////////////////////////////////////////////////////////////
    dtDAL::ActorProxyIcon *CharacterActorProxy::GetBillBoardIcon()
    {
        if(!mBillBoardIcon.valid())
        {
            mBillBoardIcon =
                new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::CHARACTER);
        }

        return mBillBoardIcon.get();
    }

}
