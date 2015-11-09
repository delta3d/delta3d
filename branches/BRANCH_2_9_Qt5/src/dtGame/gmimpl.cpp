/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2010, Alion Science and Technology
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
 */

#include <prefix/dtgameprefix.h>
#include <dtGame/gmimpl.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>

namespace dtGame
{
//////////////////////////////////////////////////////////////////////////
void GMImpl::ClearTimerSingleSet(std::set<TimerInfo> &timerSet,
                                         const std::string &name,
                                         const GameActorProxy *proxy)
{
   std::set<TimerInfo>::iterator i = timerSet.begin();
   while (i != timerSet.end())
   {
      std::set<TimerInfo>::iterator toDelete;
      const TimerInfo& timer = *i;
      if (timer.name == name &&  (proxy == NULL || timer.aboutActor == proxy->GetId()))
      {
         toDelete = i;
         ++i;
         timerSet.erase(toDelete);
      }
      else
      {
         ++i;
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void GMImpl::ClearTimersForActor(std::set<TimerInfo>& timerSet, const GameActorProxy& actor)
{
   std::set<TimerInfo>::iterator i = timerSet.begin();
   while (i != timerSet.end())
   {
      std::set<TimerInfo>::iterator toDelete;
      const TimerInfo& timer = *i;
      if (timer.aboutActor == actor.GetId())
      {
         toDelete = i;
         ++i;
         timerSet.erase(toDelete);
      }
      else
      {
         ++i;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
GMImpl::GMImpl(dtCore::Scene& scene) : mGMStatistics()
, mMachineInfo( new MachineInfo())
, mEnvironment(NULL)
//, mSendCreatesAndDeletes(true)
//, mAddActorsToScene(true)
, mFactory("GameManager MessageFactory", *mMachineInfo, "")
, mScene(&scene)
, mLibMgr(&dtCore::ActorFactory::GetInstance())
, mApplication(NULL)
, mLogger(&dtUtil::Log::GetInstance("gamemanager.cpp"))
, mGMSettings(new GMSettings())
, mRemoveGameEventsOnMapChange(true)
, mShuttingDown(false)
{

}
////////////////////////////////////////////////////////////////////////////////
void GMImpl::ProcessTimers(GameManager& gm, std::set<TimerInfo>& listToProcess, dtCore::Timer_t clockTime)
{
   std::set<TimerInfo>::iterator itor;
   std::set<TimerInfo> repeatingTimers;
   for (itor=listToProcess.begin(); itor!=listToProcess.end();)
   {
      if (itor->time <= clockTime)
      {
         dtCore::RefPtr<TimerElapsedMessage> timerMsg =
            static_cast<TimerElapsedMessage*>(mFactory.CreateMessage(MessageType::INFO_TIMER_ELAPSED).get());

         timerMsg->SetTimerName(itor->name);
         float lateTime = float((clockTime - itor->time));
         // convert from microseconds to seconds
         lateTime /= 1e6;
         timerMsg->SetLateTime(lateTime);
         timerMsg->SetAboutActorId(itor->aboutActor);
         gm.SendMessage(*timerMsg.get());
         if (itor->repeat)
         {
            TimerInfo tInfo = *itor;
            tInfo.time += tInfo.interval;
            repeatingTimers.insert(tInfo);
         }

         std::set<TimerInfo>::iterator toDelete = itor;
         ++itor;
         listToProcess.erase(toDelete);
      }
      else
      {
         break;
      }
   }

   // Repeating timers have to be readded so they are processed again later
   listToProcess.insert(repeatingTimers.begin(), repeatingTimers.end());
}

////////////////////////////////////////////////////////////////////////////////
void GMImpl::ReparentDanglingDrawables(GameManager& gm, dtCore::DeltaDrawable* dd)
{
   if (dd == NULL || dd->GetParent() == NULL) return;
   for (unsigned c = 0; c < dd->GetNumChildren();)
   {
      dtCore::DeltaDrawable* curChildDD = dd->GetChild(c);
      dtCore::BaseActorObject* actorWithDrawableAsChild = gm.FindActorById(curChildDD->GetUniqueId());
      if (actorWithDrawableAsChild != NULL && curChildDD == actorWithDrawableAsChild->GetDrawable())
      {
         curChildDD->Emancipate();
         if (actorWithDrawableAsChild->IsGameActor())
         {
            if (AddActorToScene(*static_cast<GameActorProxy*>(actorWithDrawableAsChild)))
            {
               // This happens when the actor passed is the environment actor.
               LOG_ERROR("Internal Error:  The environment was in the subscene.  This shouldn't be possible.");
            }
         }
         else
         {
            AddActorToScene(*actorWithDrawableAsChild);
         }
      }
      else
      {
         ++c;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
//void GMImpl::RemoveActorFromScene(GameManager& gm, dtCore::BaseActorObject& proxy)
//{
//   // All of the this behavior is deprecated. Only the base actor objects do this.
//
//   dtCore::DeltaDrawable& dd = *proxy.GetDrawable();
//
//   if (dd.GetSceneParent() != mScene.get())
//   {
//      return;
//   }
//
//   // find all of the children that have actor proxies associated with them to move them up
//   // one level in the scene.
//   std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > childrenToMove;
//   for (unsigned i = 0; i < dd.GetNumChildren(); ++i)
//   {
//      dtCore::DeltaDrawable& child = *dd.GetChild(i);
//      dtCore::BaseActorObject* childProxy = gm.FindActorById(child.GetUniqueId());
//      if (childProxy != NULL)
//      {
//         childrenToMove.push_back(childProxy);
//      }
//   }
//
//   if (dd.GetParent() != NULL)
//   {
//      // add all the children to the parent drawable.
//      for (size_t i = 0; i < childrenToMove.size(); ++i)
//      {
//         dtCore::DeltaDrawable* child = childrenToMove[i]->GetDrawable();
//         child->Emancipate();
//         dd.GetParent()->AddChild(child);
//      }
//      // remove the proxy drawable from the parent.
//      dd.Emancipate();
//   }
//}

////////////////////////////////////////////////////////////////////////////////
void GMImpl::InternalMarkSingleActorForRemoval(GameManager& gm, GameActorProxy& gameActor)
{
   if (gameActor.IsInGM())
   {
      mDeleteList.push_back(&gameActor);
      gameActor.SetIsInGM(false);
      gameActor.SetDeleted(true);
      if (!gameActor.IsRemote())
      {
         dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ACTOR_DELETED);
         msg->SetAboutActorId(gameActor.GetId());

         gm.SendMessage(*msg);
      }
   }
   else
   {
      LOG_INFO("Deleting Actor twice: \"" + gameActor.GetId().ToString() + "\" Name: \"" + gameActor.GetName() +
         "\" Type: \"" + gameActor.GetActorType().GetFullName() + "\".");
   }
}

////////////////////////////////////////////////////////////////////////////////
void GMImpl::SendEnvironmentChangedMessage(GameManager& gm, IEnvGameActorProxy* envActor)
{
   dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ENVIRONMENT_CHANGED);
   msg->SetAboutActorId(mEnvironment.valid() ? envActor->GetDrawable()->GetUniqueId() : dtCore::UniqueId(""));
   msg->SetSource(*mMachineInfo);
   gm.SendMessage(*msg);
}

////////////////////////////////////////////////////////////////////////////////
void GMImpl::AddActorToWorld(GameManager& gm, dtGame::GameActorProxy& actor)
{
   if (actor.IsInGM())
      return;

   actor.SetIsInGM(true);

   try
   {
      // If publishing fails. we need to delete the actor as well.
      if (actor.IsPublished())
      {
         gm.PublishActor(actor);
      }

      actor.InvokeEnteredWorld();
   }
   catch (const dtUtil::Exception& ex)
   {
      ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
      gm.DeleteActor(actor);
      throw;
   }

}
void GMImpl::AddActorToScene(dtCore::BaseActorObject& actor)
{
   bool hasDrawable = actor.GetDrawable() != NULL;

   bool hasNoParent = hasDrawable && actor.GetDrawable()->GetParent() == NULL;

   if (hasDrawable && mEnvironment.valid())
   {
      if (mEnvironment.get() != &actor)
      {
         IEnvGameActor* ea = NULL;
         mEnvironment->GetDrawable(ea);
         if (ea == NULL)
         {
            LOG_ERROR("An environment actor has an invalid drawable");
            return;
         }
         if (hasNoParent)
         {
            ea->AddActor(*actor.GetDrawable());
         }
      }
      else
      {
         LOG_ERROR("The environment actor was passed as base actor object internally.");
      }
   }
   else
   {
      if (hasDrawable && hasNoParent)
      {
         mScene->AddChild(actor.GetDrawable());
      }
   }
}

// Adds a actor to the scene.  The return bool is if it changed the environment actor.
bool GMImpl::AddActorToScene(GameActorProxy& actor)
{
   bool hasNoParent = actor.GetParentActor() == NULL;
   //TODO - Drawable shouldn't be referenced here.
   bool hasDrawable = actor.GetDrawable() != NULL;
   bool envChanged = false;

   if (mEnvironment.valid())
   {
      if (mEnvironment.get() != &actor && hasNoParent)
      {
         actor.SetParentActor(mEnvironment);
      }
      else
      {
         if (mEnvironment.get() == &actor && hasDrawable && actor.GetDrawable()->GetParent() != mScene)
         {
            mScene->AddChild(mEnvironment->GetDrawable());
            envChanged = true;
         }
      }
   }
   else
   {
      if (hasDrawable && hasNoParent)
      {
         mScene->AddChild(actor.GetDrawable());
      }
   }
   return envChanged;
}

}
