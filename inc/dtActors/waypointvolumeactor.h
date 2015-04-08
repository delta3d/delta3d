/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
 * Michael Guerrero
 */

#ifndef DELTA_WAYPOINTVOLUMEACTOR
#define DELTA_WAYPOINTVOLUMEACTOR

#include <dtGame/gameactorproxy.h>
#include <dtUtil/mathdefines.h>

#include <dtCore/plugin_export.h>
#include <dtCore/actorproxyicon.h>

#include <vector>

namespace dtActors
{
   class WaypointVolumeActorProxy;

   /**
    * This class is used to define a volume within which waypoints are allowed
    * to be created when using the Waypoint Explorer tool to generate your waypoints.
    */
   class DT_PLUGIN_EXPORT WaypointVolumeActor : public dtCore::Transformable
   {
      DECLARE_MANAGEMENT_LAYER(WaypointVolumeActor)
   public:

      WaypointVolumeActor(const std::string& name = "WaypointVolumeActor");

      /**
       * Set the distance between waypoints that the Waypoint
       * Explorer tool will use when generating waypoints
       */
      void SetWaypointSpacing(float spacing) { mSpacing = spacing; }

      /**
       * Gets the distance between waypoints that the Waypoint
       * Explorer tool will use when generating waypoints
       */
      float GetWaypointSpacing() { return mSpacing; }

      bool IsPointInVolume(float x, float y, float z);

   protected:

      /**
       * Destroys this task.
       */
      virtual ~WaypointVolumeActor();

   private:

      float mSpacing;
   };

   /**
    * This class is a proxy for the WaypointVolumeActor following the Delta3D
    * game actor design philosophy.
    */
   class DT_PLUGIN_EXPORT WaypointVolumeActorProxy : public dtCore::TransformableActorProxy
   {
   public:

      /**
       * Constructs the WaypointVolumeActor proxy.
       */
      WaypointVolumeActorProxy();

      /**
       * Builds the property map for the WaypointVolumeActor proxy.  These properties
       * wrap the specified properties located in the task actor.
       */
      virtual void BuildPropertyMap();

      /**
       * Can be placed in a scene
       */
      virtual bool IsPlaceable() const { return true; }

      /**
       * Gets the billboard used to represent static mesh if this proxy's
       * render mode is RenderMode::DRAW_BILLBOARD_ICON. Used by STAGE.
       * @return billboard icon to use
       */
      virtual dtCore::ActorProxyIcon* GetBillBoardIcon()
      {
         if (!mBillBoardIcon.valid())
         {
            mBillBoardIcon =
               new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_GENERIC);
         }

         return mBillBoardIcon.get();
      }

      /**
       * Gets the method by which this system is rendered.
       * @return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.
       */
      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
      }

   protected:

      /**
       * Destroys the task actor proxy.
       */
      virtual ~WaypointVolumeActorProxy();

      /**
       * Called by the game manager during creation of the proxy.  This method
       * creates a TaskActor and returns it.
       */
      virtual void CreateDrawable();

      /**
       * Called when the proxy has been added to the game manager.
       */
      virtual void OnEnteredWorld();


   private:
   };
}

#endif // DELTA_WAYPOINTVOLUMEACTOR
