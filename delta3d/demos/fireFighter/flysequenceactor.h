/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 */
#ifndef DELTA_FIRE_FIGHTER_FLY_SEQUENCE_ACTOR
#define DELTA_FIRE_FIGHTER_FLY_SEQUENCE_ACTOR

#include <dtGame/gameactor.h>
#include <fireFighter/export.h>
#include <dtCore/transform.h>

// Forward declarations
namespace dtGame
{
   class Message;
}

namespace dtAudio
{
   class Sound;
}

namespace dtCore
{
   class Camera;
}

namespace dtCore
{
   class ActorProxyIcon;
}

// Waypoint class to hold waypoint data
class Waypoint
{
   public:

      double x;
      double y;
      double z;
};

class PlayerActor;

class FIRE_FIGHTER_EXPORT FlySequenceActor : public dtGame::GameActor
{
   public:

      /// Constructor
      FlySequenceActor(dtGame::GameActorProxy& parent);

      /// Invokable to update the camera
      void OnTickLocal(const dtGame::TickMessage& msg);

      /// Starts moving the player
      void StartFlying();

      /// Stops moving the player
      void StopFlying();

      /**
       * Sets the player actor this FlySequence will manipulate
       * @param player The new player actor
       */
      void SetPlayerActor(PlayerActor& player) { mPlayer = &player; }

   protected:

      /// Destructor
      virtual ~FlySequenceActor();

      /// Called when the actor is added to the game manager
      virtual void OnEnteredWorld();

   private:

      // Computes a unit vector pointing from the previous waypoint to the next waypoint
      void ComputeDirTravel(const Waypoint &next, const Waypoint &prev);

      // Determines if the camera is within a given distance (delta) of the waypoint
      bool CompareCameraToWaypoint(const double* cam, const Waypoint &wp, float delta);

      // Prepares the camera path to transit
      void ResetCameraPath();

      dtCore::Transform pos;
      double actPos[3];
      osg::Vec3 camPos, lookAt, up;

      Waypoint* wayptArray;
      double dirOfTransit[3], speed, delta;
      int prevWaypoint, numWaypoints;
      bool mFlyBy;
      dtCore::RefPtr<dtAudio::Sound> snd;
      PlayerActor *mPlayer;
};

class FIRE_FIGHTER_EXPORT FlySequenceActorProxy : public dtGame::GameActorProxy
{
   public:

      /// Constructor
      FlySequenceActorProxy();

      /// Builds this actor's properties
      virtual void BuildPropertyMap();

      /// Builds the invokables
      virtual void BuildInvokables();

      /// Creates the actor
      virtual void CreateDrawable() { SetDrawable(*new FlySequenceActor(*this)); }

      /// Called when the actor is added to the game manager
      virtual void OnEnteredWorld();

      // Used in STAGE
      dtCore::ActorProxyIcon* GetBillBoardIcon();

      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
      }

   protected:

      /// Destructor
      virtual ~FlySequenceActorProxy();

   private:
};


#endif
