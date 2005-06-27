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
#ifndef DELTA_CHARACTER_ACTOR_PROXY
#define DELTA_CHARACTER_ACTOR_PROXY

#include "dtDAL/plugin_export.h"
#include "dtChar/dtchar.h"
#include "dtDAL/transformableactorproxy.h"
#include "dtDAL/exception.h"

namespace dtActors
{
    /**
     * @class CharacterActorProxy
     * @brief This proxy wraps the Character Delta3D object.
     */
    class DT_PLUGIN_EXPORT CharacterActorProxy : public dtDAL::TransformableActorProxy
    {
    public:

        /**
         * Constructor
         */
        CharacterActorProxy() { SetClassName("dtChar::Character"); }

        /**
         * Adds the properties that are common to all Delta3D transformable objects.
         */
        virtual void BuildPropertyMap();

        /**
         * Characters can be placed in a scene
         */
        virtual bool IsPlaceable() const { return true; }

        /**
         * Load in an object's file
         * @param fileName The path to the file to load
         */
        void LoadFile(const std::string &fileName);

        /**
         * Gets the method by which this static mesh is rendered.
         * @return If there is no geometry currently assigned, this
         *  method will return RenderMode::DRAW_BILLBOARD_ICON.  If
         *  there is geometry assigned to this static mesh, RenderMode::DRAW_ACTOR
         *  is returned.
         */
        const dtDAL::ActorProxy::RenderMode& GetRenderMode();

        /**
         * Gets the billboard used to represent static meshes if this proxy's
         * render mode is RenderMode::DRAW_BILLBOARD_ICON.
         * @return
         */
        dtDAL::ActorProxyIcon* GetBillBoardIcon();

    protected:
        /**
         * Initializes the actor
         */
        virtual void CreateActor();

        /**
         * Destructor
         */
        virtual ~CharacterActorProxy() { }
    };
}

#endif
