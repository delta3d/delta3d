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
#include "dtGame/exceptionenum.h"
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
   bool TaskActor::NotifyParent()
   {
      //TODO Return the parent's notify parent.  Have to get the parent and
      //children from the proxy.
      return false;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActor::SetComplete(bool flag)
   {
      if (flag)
      {
         //TODO Set time stamp here...
         //SetCompletedTimeStamp(
      }

      mComplete = flag;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActor::Reset()
   {
      mDescription = "";
      mPassingScore = 1.0f;
      mScore = 0.0f;
      mWeight = 1.0f;
      mCompletedTimeStamp = -1.0;
      mComplete = false;
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
      dtGame::GameActorProxy::BuildPropertyMap();
      TaskActor &task = static_cast<TaskActor &>(GetGameActor());

      //Description...
      AddProperty(new dtDAL::StringActorProperty("Description","Description",
         dtDAL::MakeFunctor(task,&TaskActor::SetDescription),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetDescription)));

      //Passing Score...
      AddProperty(new dtDAL::FloatActorProperty("PassingScore","Passing Score",
         dtDAL::MakeFunctor(task,&TaskActor::SetPassingScore),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetPassingScore)));

      //Score...
      AddProperty(new dtDAL::FloatActorProperty("Score","Score",
         dtDAL::MakeFunctor(task,&TaskActor::SetScore),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetScore)));

      //Weight...
      AddProperty(new dtDAL::FloatActorProperty("Weight","Weight",
         dtDAL::MakeFunctor(task,&TaskActor::SetWeight),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetWeight)));
         
      //Complete...
      AddProperty(new dtDAL::BooleanActorProperty("Complete","Complete",
         dtDAL::MakeFunctor(task,&TaskActor::SetComplete),
         dtDAL::MakeFunctorRet(task,&TaskActor::IsComplete)));
      GetProperty("Complete")->SetReadOnly(true);
      
      //IsTopLevel...
      AddProperty(new dtDAL::BooleanActorProperty("IsTopLevel","Top Level Task",
         dtDAL::MakeFunctor(*this,&TaskActorProxy::SetTopLevelTask),
         dtDAL::MakeFunctorRet(*this,&TaskActorProxy::IsTopLevelTask)));
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
   void TaskActorProxy::AddSubTaskProxy(TaskActorProxy &subTask)
   {
      if (FindSubTaskProxy(subTask.GetGameActor().GetUniqueId()) != NULL)
         EXCEPT(dtGame::ExceptionEnum::INVALID_PARAMETER,"Cannot add a duplicate "
            "sub task.");

      if (subTask.GetParentTaskProxy() != NULL)
         subTask.GetParentTaskProxy()->RemoveSubTaskProxy(subTask);

      subTask.SetParentTaskProxy(this);
      mSubTaskProxies.insert(std::make_pair(subTask.GetGameActor().GetUniqueId(),&subTask));
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::RemoveSubTaskProxy(const TaskActorProxy &subTask)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<TaskActorProxy> >::iterator itor =
         mSubTaskProxies.find(subTask.GetGameActor().GetUniqueId());

      if (itor == mSubTaskProxies.end())
      {
         LOG_WARNING("Task: " + subTask.GetGameActor().GetName() + " is not a sub task of task: " +
            GetGameActor().GetName() + "  Cannot remove.");
      }
      else
      {
         itor->second->SetParentTaskProxy(NULL);
         mSubTaskProxies.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::RemoveSubTaskProxy(const std::string &name)
   {
      //Have to do a linear search since our map is stored by unique id.
      std::map<dtCore::UniqueId,dtCore::RefPtr<TaskActorProxy> >::iterator itor;
      for (itor=mSubTaskProxies.begin(); itor!=mSubTaskProxies.end(); ++itor)
      {
         if (itor->second->GetName() == name)
            break;
      }

      if (itor == mSubTaskProxies.end())
      {
         LOG_WARNING("Task: " + name + " is not a sub task of task: " +
            GetGameActor().GetName() + "  Cannot remove.");
      }
      else
      {
         itor->second->SetParentTaskProxy(NULL);
         mSubTaskProxies.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<TaskActorProxy> TaskActorProxy::FindSubTaskProxy(const std::string &name)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<TaskActorProxy> >::iterator itor;

      for (itor=mSubTaskProxies.begin(); itor!=mSubTaskProxies.end(); ++itor)
      {
         if (itor->second->GetName() == name)
            break;
      }

      if (itor != mSubTaskProxies.end())
         return itor->second;
      else
         return NULL;
   }

   //////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<TaskActorProxy> TaskActorProxy::FindSubTaskProxy(const dtCore::UniqueId &id)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<TaskActorProxy> >::iterator itor =
         mSubTaskProxies.find(id);

      if (itor != mSubTaskProxies.end())
         return itor->second;
      else
         return NULL;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::GetAllSubTaskProxies(std::vector<dtCore::RefPtr<TaskActorProxy> > &subTasks)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<TaskActorProxy> >::iterator itor;

      subTasks.clear();
      subTasks.reserve(mSubTaskProxies.size());
      for (itor=mSubTaskProxies.begin(); itor!=mSubTaskProxies.end(); ++itor)
         subTasks.push_back(itor->second);
   }
}
