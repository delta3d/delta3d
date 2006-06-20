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
 * @author Curtiss Murphy
 */
#ifndef DELTA_GAMEMESHACTOR_H
#define DELTA_GAMEMESHACTOR_H

#include <dtGame/gameactor.h>
//#include "dtUtil/mathdefines.h"
#include <dtDAL/plugin_export.h>
#include <dtCore/loadable.h>

//#include <map>
//#include <vector>

namespace dtActors
{
   class GameMeshActorProxy;

   /**
    * This class is the game actor for a basic game object that has a single static mesh.
    * Use this base class if you are using the Game Manager and want to create a 
    * simple game object that has one mesh.  It is basically a GameActor with the 
    * code from StaticMeshActor in dtActors.
    * @see GameActor
    * @see GameMeshProxy
    * @see StaticMeshActor
    */
   class DT_PLUGIN_EXPORT GameMeshActor : public dtGame::GameActor
   {
      public:

         /**
          * Constructs a default task actor.
          * @param proxy The actor proxy owning this task actor.
          * @param desc An optional description of this task actor.
          */
         GameMeshActor(dtGame::GameActorProxy &proxy);

        /**
         * Loads a mesh file.
         * @param fileName The filename of the mesh to load.
         */
        virtual void SetMesh(const std::string &meshFile);

         /**
          * Called when the actor has been added to the game manager.
          */
         virtual void OnEnteredWorld();

      protected:

         /**
          * Destroys this actor.
          */
         virtual ~GameMeshActor();


      private:
         /**
          * Inner class for implementing the loadable interface
          */
         class GameMeshLoader : public dtCore::Loadable
         {
            public:
               GameMeshLoader() {};
               ~GameMeshLoader() {};
         };

         // tracks whether we should do a notify when we change the mesh
         bool mAlreadyInWorld;

   };

   /**
    * This class is the proxy for a basic game object that has a single static mesh.
    * Use this base class if you are using the Game Manager and want to create a 
    * simple game object that has one mesh.  It is basically a GameActorProxy with the 
    * code from StaticMeshActorProxy in dtActors.
    * @see GameActorProxy
    * @see GameMeshActor
    * @see StaticMeshActorProxy
    */
   class DT_PLUGIN_EXPORT GameMeshActorProxy : public dtGame::GameActorProxy
   {
      public:

         /**
          * Constructs the proxy.
          */
         GameMeshActorProxy();

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
         virtual const ActorProxy::RenderMode& GetRenderMode();

         /**
          * Gets the billboard used to represent static mesh if this proxy's
          * render mode is RenderMode::DRAW_BILLBOARD_ICON. Used by STAGE.
          * @return
          */
         virtual dtDAL::ActorProxyIcon* GetBillBoardIcon();


      protected:

         /**
          * Destroys the proxy.
          */
         virtual ~GameMeshActorProxy();

         /**
          * Called by the game manager during creation of the proxy.  This method
          * creates the real actor and returns it.
          */
         virtual void CreateActor();

      private:
   };

}

#endif
