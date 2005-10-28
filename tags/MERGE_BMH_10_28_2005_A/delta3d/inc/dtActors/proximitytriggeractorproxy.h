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
* @author Chris Osborn
*/

#ifndef _PROXIMITY_TRIGGER_ACTOR_PROXY_H_
#define _PROXIMITY_TRIGGER_ACTOR_PROXY_H_

#include <dtDAL/plugin_export.h>
#include <dtDAL/transformableactorproxy.h>

namespace dtABC
{
   class Action;
}

namespace dtActors 
{
    /**
     * @class TriggerActorProxy
     * @brief This proxy wraps placeable triggers.
     */
    class DT_PLUGIN_EXPORT ProximityTriggerActorProxy : public dtDAL::TransformableActorProxy 
    {
    public:

        /**
         * Constructor
         */
        ProximityTriggerActorProxy() { SetClassName("dtABC::ProximityTrigger"); }

        /**
         * Adds the properties that are common to all Delta3D physical objects.
         */
        virtual void BuildPropertyMap();

        /**
        * Characters can be placed in a scene
        */
        virtual bool IsPlaceable() const { return true; }

        /**
        * Gets the method by which a particle system is rendered.
        * @return ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.
        */
        virtual const ActorProxy::RenderMode& GetRenderMode() 
        {
           return ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
        }

        /**
        * Gets the billboard used to represent static meshes if this proxy's
        * render mode is RenderMode::DRAW_BILLBOARD_ICON.
        * @return
        */
        dtDAL::ActorProxyIcon* GetBillBoardIcon();

        /**
        * Wrapper to set the Action on the internal Trigger.
        */
        void SetAction( dtABC::Action* action );

    protected:

        /**
         * Creates a new trigger actor.
         */
        virtual void CreateActor();

        /**
         * Destructor
         */
        virtual ~ProximityTriggerActorProxy() {}
    };
}

#endif //_PROXIMITY_TRIGGER_ACTOR_PROXY_H_
