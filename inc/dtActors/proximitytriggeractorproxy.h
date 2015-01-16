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
* William E. Johnson II
* Chris Osborn
*/

#ifndef _PROXIMITY_TRIGGER_ACTOR_PROXY_H_
#define _PROXIMITY_TRIGGER_ACTOR_PROXY_H_

#include <dtCore/plugin_export.h>
#include <dtCore/transformableactorproxy.h>

namespace dtABC
{
   class Action;
   class ProximityTrigger;
}

namespace dtActors 
{
    /**
     * This proxy wraps placeable triggers.
     */
    class DT_PLUGIN_EXPORT ProximityTriggerActorProxy : public dtCore::TransformableActorProxy 
    {
    public:

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
        * @return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.
        */
        virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode() 
        {
           return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
        }

        /**
        * Gets the billboard used to represent static meshes if this proxy's
        * render mode is RenderMode::DRAW_BILLBOARD_ICON.
        * @return
        */
        dtCore::ActorProxyIcon* GetBillBoardIcon();

        /**
        * Wrapper to set the Action on the internal Trigger.
        */
        void SetAction(dtCore::BaseActorObject* action);

        /**
        * Wrapper to get the Action on the internal Trigger.
        */
        dtCore::DeltaDrawable* GetAction();

    protected:

        /**
         * Creates a new trigger actor.
         */
        virtual void CreateDrawable();

        /**
         * Performs specific setup for the created ProximityTrigger instance.
         * Override this function if a sub-class of ProximityTrigger is created
         * by a sub-class of ProximityTriggerActorProxy.
         * @param trigger The new trigger created in CreateActor that needs special initializations performed.
         */
        virtual void InitializeTrigger(dtABC::ProximityTrigger& trigger);

        /**
         * Destructor
         */
        virtual ~ProximityTriggerActorProxy() {}
    };
}

#endif //_PROXIMITY_TRIGGER_ACTOR_PROXY_H_
