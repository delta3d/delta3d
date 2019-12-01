/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * Curtiss Murphy
 */

#ifndef DELTA_GAMEMESHACTOR_H
#define DELTA_GAMEMESHACTOR_H

#include <dtGame/gameactor.h>
#include <dtGame/gameactorproxy.h>
#include <dtCore/plugin_export.h>
#include <dtCore/loadable.h>
#include <dtCore/model.h>
#include <dtUtil/getsetmacros.h>

//#include <map>
//#include <vector>

namespace dtCore
{
   class Scene;
}

namespace dtActors
{
   /**
    * This class is the proxy for a basic game object that has a single static mesh.
    * Use this base class if you are using the Game Manager and want to create a
    * simple game object that has one mesh.  It is basically a GameActorProxy with the
    * code from StaticMeshActorProxy in dtActors. Note, this class does NOT cache
    * geometry in memory. The reasoning is that since this is a dynamic GameActor
    * that is intended to respond to message and have behavior, changes to its
    * geometry (damaged states, etc.) should happen to each instances. If you
    * want to share geometry for a static actor, check out StaticMeshActorProxy.
    * @see GameActorProxy
    * @see GameMeshActor
    * @see StaticMeshActorProxy
    */
   class DT_PLUGIN_EXPORT GameMeshActor : public dtGame::GameActorProxy
   {
   public:

      /**
       * Constructs the proxy.
       */
      GameMeshActor();

      /**
       * Builds the property map for the task actor proxy.  These properties
       * wrap the specified properties located in the actor.
       */
      virtual void BuildPropertyMap();

      /**
       * Registers any invokables used by the proxy.  The invokables
       * allow the actor to hook into the game manager messages system.
       */
      virtual void BuildInvokables();

      /**
       * Gets the method by which this static mesh is rendered. This is used by STAGE.
       * @return If there is no geometry currently assigned, this
       *  method will return RenderMode::DRAW_BILLBOARD_ICON.  If
       *  there is geometry assigned to this static mesh, RenderMode::DRAW_ACTOR
       *  is returned.
       */
      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode();

      /**
       * Gets the billboard used to represent static mesh if this proxy's
       * render mode is RenderMode::DRAW_BILLBOARD_ICON. Used by STAGE.
       * @return
       */
      virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

   protected:

      /**
       * Destroys the proxy.
       */
      virtual ~GameMeshActor();

      /**
       * Called by the game manager during creation of the proxy.  This method
       * creates the real actor and returns it.
       */
      virtual void CreateDrawable();

   private:
   };

} // namespace dtActors

#endif // DELTA_GAMEMESHACTOR_H
