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
 * Matthew W. Campbell, Curtiss Murphy
 */
#include <dtActors/taskactorordered.h>

#include <dtActors/engineactorregistry.h>

#include <dtCore/enumactorproperty.h>
#include <dtCore/functor.h>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(TaskActorOrdered::FailureType);
   TaskActorOrdered::FailureType TaskActorOrdered::FailureType::CAUSE_FAILURE("CAUSE_FAILURE");
   TaskActorOrdered::FailureType TaskActorOrdered::FailureType::BLOCK("BLOCK");

   //////////////////////////////////////////////////////////////////////////////
   TaskActorOrdered::TaskActorOrdered(dtGame::GameActorProxy& parent) : TaskActor(parent)
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
      proxy.mFailingTask = NULL;
   }

   ////////////////////////////END ACTOR////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   ///////////////////////////BEGIN PROXY///////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   TaskActorOrderedProxy::TaskActorOrderedProxy()
   : mFailingTask(NULL)
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
      TaskActorOrdered* task = GetDrawable<TaskActorOrdered>();

      const std::string GROUPNAME = "Order Properties";

      //Add the failure type enumeration property.
      AddProperty(new dtCore::EnumActorProperty<TaskActorOrdered::FailureType>("Order Enforcement",
                  "Order Enforcement",
                  dtCore::EnumActorProperty<TaskActorOrdered::FailureType>::SetFuncType(task,&TaskActorOrdered::SetFailureType),
                  dtCore::EnumActorProperty<TaskActorOrdered::FailureType>::GetFuncType(task,&TaskActorOrdered::GetFailureType),
                  "Sets the way in which the ordered task actor handles out of order task updates.",
                  GROUPNAME));
   }

   //////////////////////////////////////////////////////////////////////////////
   bool TaskActorOrderedProxy::RequestScoreChange(const TaskActorProxy &childTask, const TaskActorProxy &origTask)
   {
      TaskActor* myActor;
      std::vector<TaskActorProxy*> subTasks;
      GetAllSubTasks(subTasks);
      std::vector<TaskActorProxy*>::const_iterator itor;
      bool result = false;

      // If we're already failed, then no way are we approving the request.
      GetDrawable(myActor);
      if (myActor->IsFailed())
         return false;

      //Need to see if all the tasks added prior to the task in question have been
      //completed.  If not we have to reject.
      for (itor=subTasks.begin(); itor!=subTasks.end(); ++itor)
      {
         const TaskActorProxy &task = *(*itor);

         //If we encounter a task before the task in question that has not yet been
         //completed, we cannot continue.
         bool completed = task.GetDrawable<TaskActor>()->IsComplete();
         bool idsMatch  = task.GetId() == childTask.GetId();
         if (!completed && !idsMatch)
         {
            //Set the task that got rejected so a user can find out why we failed.
            SetFailingTaskProxy(origTask);

            // If we're a ORDERED Failing task, then we just failed.
            // Note, we don't fail the child. Though it is recorded as the failing task proxy
            if (static_cast<TaskActorOrdered*>(GetDrawable())->GetFailureType() ==
                TaskActorOrdered::FailureType::CAUSE_FAILURE)
            {
               myActor->SetFailed(true);
            }

            break;
         }

         //If we got here, then all the tasks before the current task have been completed.
         //So we can accept it and stopping checking...
         if((*itor) == &childTask)
         {
            result = true;
            break;
         }
      }

      //Make sure to forward the request up the task's chain of command so to speak...
      if (GetParentTask() != NULL)
         return result && GetParentTask()->RequestScoreChange(*this,origTask);

      return result;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorOrderedProxy::NotifyScoreChanged(const TaskActorProxy& childTask)
   {
      //This method is called when a child task has changed its score.  Need to
      //loop through the children of this task.  For any that are complete, we
      //need to factor that in to the score of this task.
      std::vector<TaskActorProxy*> subTasks;
      GetAllSubTasks(subTasks);
      std::vector<TaskActorProxy*>::const_iterator itor;
      TaskActor *taskActor = NULL;
      float totalWeightedScore = 0.0f;
      float totalWeight = 0.0f;

      for (itor=subTasks.begin(); itor!=subTasks.end(); ++itor)
      {
         const TaskActor *subTask = dynamic_cast<const TaskActor *>((*itor)->GetDrawable());
         totalWeightedScore += (subTask->GetScore() * subTask->GetWeight());
         totalWeight += subTask->GetWeight();
      }

      GetDrawable(taskActor);

      //We actually do not need to request a score change in the case of the
      //rollup task.  A rollup task's score can only change if one of its
      //children, grandchildren, ect. has its score updated which must have
      //already been approaved before reaching this method.
      taskActor->SetScore(totalWeightedScore / totalWeight);
      if (taskActor->GetScore() >= taskActor->GetPassingScore())
         taskActor->SetComplete(true);

      NotifyFullActorUpdate();

      if (GetParentTask() != NULL)
         GetParentTask()->NotifyScoreChanged(*this);
   }

   //////////////////////////////////////////////////////////////////////////////
   bool TaskActorOrderedProxy::IsChildTaskAllowedToChange(const TaskActorProxy& childTask) const
   {
      std::vector<const TaskActorProxy*> subTasks;
      GetAllSubTasks(subTasks);
      std::vector<const TaskActorProxy*>::const_iterator itor;
      bool parentGivesOK = true; // no parent means we have approval.
      bool bOKToChangeChildTask = false;

      const TaskActorOrdered* myActor;
      GetDrawable(myActor);

      // First check to see if our parent allows us to be changed.
      if (GetParentTask() != NULL)
         parentGivesOK = GetParentTask()->IsChildTaskAllowedToChange(*this);

      // We always give permission to our children if we are failing because we ALLOW
      // them to do it out of order.
      if (myActor->GetFailureType() == TaskActorOrdered::FailureType::CAUSE_FAILURE)
      {
         bOKToChangeChildTask = true;
      }
      // If we aren't failed and parent allows, so check to see if the child is the next task in line.
      else if (parentGivesOK && !myActor->IsFailed())
      {
         //Need to see if all the tasks added prior to the task in question have been
         //completed.  If not we have to reject.
         for (itor=subTasks.begin(); itor!=subTasks.end(); ++itor)
         {
            const TaskActorProxy &task = *(*itor);

            //If we got here, then all the tasks before the current task have been completed.
            //So we can accept it and stopping checking...
            if (&task == &childTask)
            {
               bOKToChangeChildTask = true;
               break;
            }

            // If we find an incomplete task before the child in question, then we are done.
            bool completed = task.GetDrawable<TaskActor>()->IsComplete();
            if (!completed)
            {
               break;
            }
         }
      }

      return bOKToChangeChildTask;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorOrderedProxy::CreateDrawable()
   {
      SetDrawable(*new TaskActorOrdered(*this));
   }

}
