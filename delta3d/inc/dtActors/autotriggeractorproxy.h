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
* @AUTHOR Bradley Anderegg
*/

#ifndef _AUTO_TRIGGER_ACTOR_PROXY_H_
#define _AUTO_TRIGGER_ACTOR_PROXY_H_

#include <dtCore/plugin_export.h>
#include <dtCore/actorproxy.h>

namespace dtABC
{
   class Action;
}

namespace dtActors 
{
   /**
   * @class AutoTriggerActorProxy
   * @brief This proxy wraps auto triggers.
   */
   class DT_PLUGIN_EXPORT AutoTriggerActorProxy : public dtCore::BaseActorObject 
   {
   public:

      /**
      * Constructor
      */
      AutoTriggerActorProxy() { SetClassName("dtABC::AutoTrigger"); }

      /**
      * Adds the properties that are common to all Delta3D physical objects.
      */
      virtual void BuildPropertyMap();

      /**
      * Characters can be placed in a scene
      */
      virtual bool IsPlaceable() const { return false; }

      /**
      * Wrapper to set the Action on the internal Trigger.
      */
      dtCore::DeltaDrawable* GetAction();

      /**
      * Wrapper to set the Action on the internal Trigger.
      */
      void SetAction( dtCore::BaseActorObject* action );

   protected:

      /**
      * Creates a new trigger actor.
      */
      virtual void CreateDrawable();

      /**
      * Destructor
      */
      virtual ~AutoTriggerActorProxy() {}
   };
}

#endif //_AUTO_TRIGGER_ACTOR_PROXY_H_
