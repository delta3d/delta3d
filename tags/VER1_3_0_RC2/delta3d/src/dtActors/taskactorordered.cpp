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
#include "dtActors/taskactorordered.h"
#include <dtDAL/enginepropertytypes.h>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(TaskActorOrdered::FailureType);
   TaskActorOrdered::FailureType TaskActorOrdered::FailureType::CAUSE_FAILURE("CAUSE_FAILURE");
   TaskActorOrdered::FailureType TaskActorOrdered::FailureType::BLOCK("BLOCK");

   //////////////////////////////////////////////////////////////////////////////
   TaskActorOrdered::TaskActorOrdered(dtGame::GameActorProxy &proxy) : TaskActor(proxy)
   {
      Reset();
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorOrdered::~TaskActorOrdered()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorOrdered::Reset()
   {
      TaskActor::Reset();
      mFailureType = &FailureType::BLOCK;
      TaskActorOrderedProxy &proxy = static_cast<TaskActorOrderedProxy&>(GetGameActorProxy());
      proxy.mAlwaysFail = false;
      proxy.mFailingTask = NULL;
   }

   ////////////////////////////END ACTOR////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   ///////////////////////////BEGIN PROXY///////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   TaskActorOrderedProxy::TaskActorOrderedProxy()
   {
      SetClassName("dtActors::OrderedTaskActor");
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorOrderedProxy::~TaskActorOrderedProxy()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorOrderedProxy::BuildPropertyMap()
   {
      TaskActorProxy::BuildPropertyMap();
      TaskActorOrdered &task = static_cast<TaskActorOrdered&>(GetGameActor());

      const std::string GROUPNAME = "Order Properties";

      //Add the failure type enumeration property.
      AddProperty(new dtDAL::EnumActorProperty<TaskActorOrdered::FailureType>("Order Enforcement",
                  "Order Enforcement",
                  dtDAL::MakeFunctor(task,&TaskActorOrdered::SetFailureType),
                  dtDAL::MakeFunctorRet(task,&TaskActorOrdered::GetFailureType),
                  "Sets the way in which the ordered task actor handles out of order task updates.",
                  GROUPNAME));
   }

   //////////////////////////////////////////////////////////////////////////////
   bool TaskActorOrderedProxy::RequestScoreChange(const TaskActorProxy &childTask, const TaskActorProxy &origTask)
   {
      const std::vector<dtCore::RefPtr<TaskActorProxy> > &subTasks = GetAllSubTaskProxies();
      std::vector<dtCore::RefPtr<TaskActorProxy> >::const_iterator itor;
      bool result = false;

      if (mAlwaysFail)
         return false;

      //Need to see if all the tasks added prior to the task in question have been
      //completed.  If not we have to reject.
      for (itor=subTasks.begin(); itor!=subTasks.end(); ++itor)
      {
         const TaskActorProxy &task = *itor->get();

         //If we encounter a task before the task in question that has not yet been
         //completed, we cannot continue.
         if (task.GetProperty("Complete")->GetStringValue() == "false" && task.GetId() != childTask.GetId())
         {
            //Set the task that got rejected so a user can query for this a report
            //additional information.
            SetFailingTaskProxy(origTask);
            if (GetProperty("Order Enforcement")->GetStringValue() ==
                TaskActorOrdered::FailureType::CAUSE_FAILURE.GetName())
            {
               mAlwaysFail = true;
            }

            break;
         }

         //If we got here, then all the tasks before the current task have been completed.
         //So we can accept it and stopping checking...
         if (itor->get() == &childTask)
         {
            result = true;
            break;
         }
      }

      //Make sure to forward the request up the task's chain of command so to speak...
      if (GetParentTaskProxy() != NULL)
         return result && GetParentTaskProxy()->RequestScoreChange(*this,origTask);
      else
         return result;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorOrderedProxy::NotifyScoreChanged(const TaskActorProxy &childTask)
   {
      //This method is called when a child task has changed its score.  Need to
      //loop through the children of this task.  For any that are complete, we
      //need to factor that in to the score of this task.
      const std::vector<dtCore::RefPtr<TaskActorProxy> > &subTasks = GetAllSubTaskProxies();
      std::vector<dtCore::RefPtr<TaskActorProxy> >::const_iterator itor;
      TaskActor *taskActor;
      float totalWeightedScore = 0.0f;
      float totalWeight = 0.0f;

      for (itor=subTasks.begin(); itor!=subTasks.end(); ++itor)
      {
         const TaskActor *subTask = dynamic_cast<const TaskActor *>((*itor)->GetActor());
         totalWeightedScore += (subTask->GetScore() * subTask->GetWeight());
         totalWeight += subTask->GetWeight();
      }

      taskActor = dynamic_cast<TaskActor *>(GetActor());

      //We actually do not need to request a score change in the case of the
      //rollup task.  A rollup task's score can only change if one of its
      //children, grandchildren, ect. has its score updated which must have
      //already been approaved before reaching this method.
      taskActor->SetScore(totalWeightedScore / totalWeight);
      if (taskActor->GetScore() >= taskActor->GetPassingScore())
         taskActor->SetComplete(true);

      NotifyActorUpdate();

      if (GetParentTaskProxy() != NULL)
         GetParentTaskProxy()->NotifyScoreChanged(*this);
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorOrderedProxy::CreateActor()
   {
      mActor = new TaskActorOrdered(*this);
   }

}
