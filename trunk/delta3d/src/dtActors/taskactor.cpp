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
 * @author Matthew W. Campbell
 */
#include "dtActors/taskactor.h"
#include <dtGame/exceptionenum.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtDAL/enginepropertytypes.h>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   TaskActor::TaskActor(dtGame::GameActorProxy &proxy) : dtGame::GameActor(proxy)
   {
      Reset();
   }

   TaskActor::~TaskActor()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActor::SetComplete(bool flag)
   {
      if (flag)
      {
         if (GetGameActorProxy().GetGameManager() == NULL)
         {
            LOG_ERROR("Error setting complete flag.  Game Manager was invalid on this actor.");
         }
         else
         {
            double currSimTime = GetGameActorProxy().GetGameManager()->GetSimulationTime();
            SetCompletedTimeStamp(currSimTime);
         }
      }
      else
      {
         SetCompletedTimeStamp(-1.0);
      }

      mComplete = flag;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActor::Reset()
   {
      mPassingScore = 1.0f;
      mScore = 0.0f;
      mWeight = 1.0f;
      mCompletedTimeStamp = -1.0;
      mComplete = false;

      TaskActorProxy &proxy = static_cast<TaskActorProxy&>(GetGameActorProxy());
      proxy.NotifyActorUpdate();
   }

   ////////////////////////////END TASK ACTOR////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   ///////////////////////////BEGIN TASK PROXY///////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   TaskActorProxy::TaskActorProxy()
   {
      SetClassName("dtActors::TaskActor");
      mParentTaskProxy = NULL;
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorProxy::~TaskActorProxy()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "BaseTask";

      dtGame::GameActorProxy::BuildPropertyMap();
      TaskActor &task = static_cast<TaskActor &>(GetGameActor());

      //Description...
      AddProperty(new dtDAL::StringActorProperty("Description","Description",
         dtDAL::MakeFunctor(task,&TaskActor::SetDescription),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetDescription),
         "Sets/gets the description of this task.",GROUPNAME));

      //Passing Score...
      AddProperty(new dtDAL::FloatActorProperty("PassingScore","Passing Score",
         dtDAL::MakeFunctor(task,&TaskActor::SetPassingScore),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetPassingScore),
         "Sets/gets the passing score for this task.",GROUPNAME));

      //Score...
      AddProperty(new dtDAL::FloatActorProperty("Score","Score",
         dtDAL::MakeFunctor(task,&TaskActor::SetScore),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetScore),
         "Sets/gets the current score of this task.",GROUPNAME));

      //Weight...
      AddProperty(new dtDAL::FloatActorProperty("Weight","Weight",
         dtDAL::MakeFunctor(task,&TaskActor::SetWeight),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetWeight),
         "Sets/gets the current weight assigned to this task.",GROUPNAME));

      //Complete...
      AddProperty(new dtDAL::BooleanActorProperty("Complete","Complete",
         dtDAL::MakeFunctor(task,&TaskActor::SetComplete),
         dtDAL::MakeFunctorRet(task,&TaskActor::IsComplete),
         "Gets the complete status of this task.",GROUPNAME));

      //Completed time...
      AddProperty(new dtDAL::DoubleActorProperty("CompleteTime","Complete Time",
                  dtDAL::MakeFunctor(task,&TaskActor::SetCompletedTimeStamp),
                  dtDAL::MakeFunctorRet(task,&TaskActor::GetCompletedTimeStamp),
                  "Gets the simulation time in which this task was completed.",GROUPNAME));

      //IsTopLevel...
      AddProperty(new dtDAL::BooleanActorProperty("IsTopLevel","Top Level Task",
         dtDAL::MakeFunctor(*this,&TaskActorProxy::SetTopLevelTask),
         dtDAL::MakeFunctorRet(*this,&TaskActorProxy::IsTopLevelTask),
         "Sets/gets whether or not this task contains a parent task.",GROUPNAME));
      GetProperty("IsTopLevel")->SetReadOnly(true);
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::BuildInvokables()
   {
      dtGame::GameActorProxy::BuildInvokables();
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::OnEnteredWorld()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::CreateActor()
   {
      mActor = new TaskActor(*this);
   }

   //////////////////////////////////////////////////////////////////////////////
   bool TaskActorProxy::RequestScoreChange(const TaskActorProxy &childTask, const TaskActorProxy &origTask)
   {
      //Default implementation just returns true by default if this task is
      //a top level task, else just passes the request up the chain.  Note, the original
      //task is the task that this method was first called on.
      if (GetParentTaskProxy() != NULL)
         return true && GetParentTaskProxy()->RequestScoreChange(*this,origTask);
      else
         return true;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::NotifyScoreChanged(const TaskActorProxy &childTask)
   {
      //Default implementation just needs to pass the notification up the chain.
      if (GetParentTaskProxy() != NULL)
         return GetParentTaskProxy()->NotifyScoreChanged(*this);
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::AddSubTaskProxy(TaskActorProxy &subTask)
   {
      if (FindSubTaskProxy(subTask.GetGameActor().GetUniqueId()) != NULL)
         EXCEPT(dtGame::ExceptionEnum::INVALID_PARAMETER,"Cannot add a duplicate "
            "sub task.");

      if (subTask.GetParentTaskProxy() != NULL)
         subTask.GetParentTaskProxy()->RemoveSubTaskProxy(subTask);

      subTask.SetParentTaskProxy(this);
      mSubTaskProxies.push_back(&subTask);
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::RemoveSubTaskProxy(const TaskActorProxy &subTask)
   {
      std::vector<dtCore::RefPtr<TaskActorProxy> >::iterator itor;

      for (itor=mSubTaskProxies.begin(); itor!=mSubTaskProxies.end(); ++itor)
      {
         if ((*itor)->GetId() == subTask.GetId())
            break;
      }

      if (itor == mSubTaskProxies.end())
      {
         LOG_WARNING("Task: " + subTask.GetGameActor().GetName() + " is not a sub task of task: " +
            GetGameActor().GetName() + "  Cannot remove.");
      }
      else
      {
         (*itor)->SetParentTaskProxy(NULL);
         mSubTaskProxies.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::RemoveSubTaskProxy(const std::string &name)
   {
      std::vector<dtCore::RefPtr<TaskActorProxy> >::iterator itor;
      for (itor=mSubTaskProxies.begin(); itor!=mSubTaskProxies.end(); ++itor)
      {
         if ((*itor)->GetName() == name)
            break;
      }

      if (itor == mSubTaskProxies.end())
      {
         LOG_WARNING("Task: " + name + " is not a sub task of task: " +
            GetGameActor().GetName() + "  Cannot remove.");
      }
      else
      {
         (*itor)->SetParentTaskProxy(NULL);
         mSubTaskProxies.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorProxy *TaskActorProxy::FindSubTaskProxy(const std::string &name)
   {
      std::vector<dtCore::RefPtr<TaskActorProxy> >::iterator itor;

      for (itor=mSubTaskProxies.begin(); itor!=mSubTaskProxies.end(); ++itor)
      {
         if ((*itor)->GetName() == name)
            break;
      }

      if (itor != mSubTaskProxies.end())
         return itor->get();
      else
         return NULL;
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorProxy *TaskActorProxy::FindSubTaskProxy(const dtCore::UniqueId &id)
   {
      std::vector<dtCore::RefPtr<TaskActorProxy> >::iterator itor;
      for (itor=mSubTaskProxies.begin(); itor!=mSubTaskProxies.end(); ++itor)
      {
         if ((*itor)->GetId() == id)
            break;

      }

      if (itor != mSubTaskProxies.end())
         return itor->get();
      else
         return NULL;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::NotifyActorUpdate()
   {
      if (GetGameManager() == NULL || GetGameActor().IsRemote())
         return;

      dtCore::RefPtr<dtGame::Message> updateMsg =
            GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
      dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *>(updateMsg.get());
      PopulateActorUpdate(*message);
      GetGameManager()->ProcessMessage(*updateMsg);
   }
}
