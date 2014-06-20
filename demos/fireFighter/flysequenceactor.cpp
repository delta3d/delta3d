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

#include <fireFighter/flysequenceactor.h>
#include <fireFighter/playeractor.h>
#include <fireFighter/messages.h>
#include <fireFighter/messagetype.h>
#include <fireFighter/gamestate.h>
#include <dtGame/invokable.h>
#include <dtGame/messagetype.h>
#include <dtGame/gamemanager.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagefactory.h>
#include <dtABC/application.h>
#include <dtAudio/audiomanager.h>
#include <dtCore/camera.h>
#include <dtCore/actorproxyicon.h>
#include <dtUtil/datapathutils.h>
#include <fstream>

///////////////////////////////////////////////////////////
FlySequenceActorProxy::FlySequenceActorProxy()
{

}

FlySequenceActorProxy::~FlySequenceActorProxy()
{

}

void FlySequenceActorProxy::BuildPropertyMap()
{
   dtGame::GameActorProxy::BuildPropertyMap();
}

void FlySequenceActorProxy::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
}

dtCore::ActorProxyIcon* FlySequenceActorProxy::GetBillBoardIcon()
{
   if (!mBillBoardIcon.valid())
   {
      mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_GENERIC);
   }
   return mBillBoardIcon.get();
}

void FlySequenceActorProxy::OnEnteredWorld()
{
   RegisterForMessages(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
}

///////////////////////////////////////////////////////////
FlySequenceActor::FlySequenceActor(dtGame::GameActorProxy& parent)
   : dtGame::GameActor(parent)
   , lookAt(0.0f, 0.0f, 100.0f)
   , up(0.0f, 0.0f, 1.0f)
   , wayptArray(NULL)
   , speed(0.4)
   , delta(speed * 0.95)
   , prevWaypoint(0)
   , mFlyBy(true)
   , mPlayer(NULL)
{

}

FlySequenceActor::~FlySequenceActor()
{
   mPlayer = NULL;
   if (snd != NULL)
   {
      snd->UnloadFile();
      dtAudio::Sound* sound = snd.release();
      dtAudio::AudioManager::GetInstance().FreeSound(sound);
      snd = NULL;
   }
   if (wayptArray != NULL)
   {
      delete [] wayptArray;
      wayptArray = NULL;
   }
}

void FlySequenceActor::OnEnteredWorld()
{
   std::string waypointsFile = dtUtil::FindFileInPathList("waypoints.txt");
   std::ifstream input(waypointsFile.c_str());
   if (!input.is_open())
   {
      LOG_ERROR("Could not open waypoints.txt - Creating single waypoint at the origin");
      numWaypoints = 1;
      wayptArray = new Waypoint[numWaypoints];
      wayptArray[0].x = 0.0;
      wayptArray[0].y = 0.0;
      wayptArray[0].z = 0.0;
   }
   else
   {
      input >> numWaypoints;
      wayptArray = new Waypoint[numWaypoints];
      int i = 0;
      while (input >> wayptArray[i].x >> wayptArray[i].y >> wayptArray[i].z)
      {
         i++;
      }

      input.close();
   }

   snd = dtAudio::AudioManager::GetInstance().NewSound();
   snd->LoadFile("Sounds/spokenIntro.wav");
   snd->SetLooping(false);
}

void FlySequenceActor::OnTickLocal(const dtGame::TickMessage& msg)
{
   if (mPlayer == NULL)
   {
      return;
   }

   if (mFlyBy)
   {
      if (CompareCameraToWaypoint(actPos, wayptArray[prevWaypoint + 1], delta))
      {
         // If completed transit of ship, exit
         if (++prevWaypoint == numWaypoints - 1)
         {
            StopFlying();
         }

         // Set actual position at this waypoint
         actPos[0] = wayptArray[prevWaypoint].x;
         actPos[1] = wayptArray[prevWaypoint].y;
         actPos[2] = wayptArray[prevWaypoint].z;

         ComputeDirTravel(wayptArray[prevWaypoint + 1], wayptArray[prevWaypoint]);
      }
      else
      {
         // Verify movement not too great to miss way point
         double vel = speed * static_cast<const dtGame::TickMessage&>(msg).GetDeltaSimTime() * 100.0;
         if (vel > delta) vel = delta;

         actPos[0] += dirOfTransit[0] * vel;
         actPos[1] += dirOfTransit[1] * vel;
         actPos[2] += dirOfTransit[2] * vel;
      }

      // Put the actual position into a sgVec3 to set Transform
      camPos[0] = actPos[0];
      camPos[1] = actPos[1];
      camPos[2] = actPos[2];

      pos.Set(camPos, lookAt, up);
      mPlayer->SetTransform(pos);
   }
}

void FlySequenceActor::StartFlying()
{
   mPlayer->GetTransform(pos);

   prevWaypoint = 0;

   camPos[0] = actPos[0] = wayptArray[0].x;
   camPos[1] = actPos[1] = wayptArray[0].y;
   camPos[2] = actPos[2] = wayptArray[0].z;

   // If only one waypoint, don't move
   if (numWaypoints > 1)
   {
      ComputeDirTravel(wayptArray[prevWaypoint + 1], wayptArray[prevWaypoint]);
   }
   else
   {
      dirOfTransit[0] = 0.0;
      dirOfTransit[1] = 0.0;
      dirOfTransit[2] = 0.0;
   }

   pos.Set(camPos, lookAt, up);
   mPlayer->SetTransform(pos);

   mFlyBy = true;

   snd->Play();
}

void FlySequenceActor::StopFlying()
{
   prevWaypoint = 0;

   mFlyBy = false;

   if (snd != NULL && snd->IsPlaying())
   {
      snd->Stop();
   }

   GetGameActorProxy().UnregisterForMessages(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);

   dtGame::MessageFactory& mf = GetGameActorProxy().GetGameManager()->GetMessageFactory();
   dtCore::RefPtr<dtGame::Message> msg = mf.CreateMessage(FireFighterMessageType::GAME_STATE_CHANGED);
   GameStateChangedMessage& gscm = static_cast<GameStateChangedMessage&>(*msg);
   gscm.SetOldState(GameState::STATE_INTRO);
   gscm.SetNewState(GameState::STATE_RUNNING);
   GetGameActorProxy().GetGameManager()->SendMessage(gscm);
}

void FlySequenceActor::ComputeDirTravel(const Waypoint& next, const Waypoint& prev)
{
   dirOfTransit[0] = next.x - prev.x;
   dirOfTransit[1] = next.y - prev.y;
   dirOfTransit[2] = next.z - prev.z;

   double length = sqrt((dirOfTransit[0] * dirOfTransit[0]) + (dirOfTransit[1] * dirOfTransit[1]) + (dirOfTransit[2] * dirOfTransit[2]));
   dirOfTransit[0] /= length;
   dirOfTransit[1] /= length;
   dirOfTransit[2] /= length;
}

bool FlySequenceActor::CompareCameraToWaypoint(const double* cam, const Waypoint& wp, float delta)
{
   if ((cam[0] + delta) < wp.x)
   {
      return false;
   }
   if ((wp.x + delta) < cam[0])
   {
      return false;
   }
   if ((cam[1] + delta) < wp.y)
   {
      return false;
   }
   if ((wp.y + delta) < cam[1])
   {
      return false;
   }
   if ((cam[2] + delta) < wp.z)
   {
      return false;
   }
   if ((wp.z + delta) < cam[2])
   {
      return false;
   }

   return true;
}

void FlySequenceActor::ResetCameraPath()
{
   camPos[0] = actPos[0] = wayptArray[0].x;
   camPos[1] = actPos[1] = wayptArray[0].y;
   camPos[2] = actPos[2] = wayptArray[0].z;

   prevWaypoint = 0;

   // If only one waypoint, don't move
   if (numWaypoints > 1)
   {
      ComputeDirTravel(wayptArray[prevWaypoint + 1], wayptArray[prevWaypoint]);
   }
   else
   {
      dirOfTransit[0] = 0.0;
      dirOfTransit[1] = 0.0;
      dirOfTransit[2] = 0.0;
   }

   lookAt[0] = 0.0f;
   lookAt[1] = 0.0f;
   lookAt[2] = 100.0f;

   up[0] = 0.0f;
   up[1] = 0.0f;
   up[2] = 1.0f;

   pos.Set(camPos, lookAt, up);
   mPlayer->SetTransform(pos);

   mFlyBy = true;
}
