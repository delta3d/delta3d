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
 * Matthew W. Campbell
 */
#include <dtActors/taskactorrollup.h>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   TaskActorRollup::TaskActorRollup(dtGame::GameActorProxy& parent) : TaskActor(parent)
   {
      Reset();
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorRollup::~TaskActorRollup()
   {
   }

   ////////////////////////////END ACTOR////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   ///////////////////////////BEGIN PROXY///////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   TaskActorRollupProxy::TaskActorRollupProxy()
   {
      SetClassName("dtActors::RollupTaskActor");
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorRollupProxy::~TaskActorRollupProxy()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorRollupProxy::NotifyScoreChanged(const TaskActorProxy& childTask)
   {
      //This method is called when a child task has changed its score.  Need to
      //loop through the children of this task.  For any that are complete, we
      //need to factor that in to the score of this task.
      std::vector<TaskActorProxy*> subTasks;
      GetAllSubTasks(subTasks);
      std::vector<TaskActorProxy*>::const_iterator itor;
      TaskActor* taskActor = NULL;
      float totalWeightedScore = 0.0f;
      float totalWeight = 0.0f;

      for (itor=subTasks.begin(); itor!=subTasks.end(); ++itor)
      {
         const TaskActor* subTask = NULL; (*itor)->GetDrawable(subTask);
         totalWeightedScore += (subTask->GetScore() * subTask->GetWeight());
         totalWeight += subTask->GetWeight();
      }

      GetDrawable(taskActor);

      //We actually do not need to request a score change in the case of the
      //rollup task.  A rollup task's score can only change if one of its
      //children, grandchildren, etc. has its score updated which must have
      //already been approaved before reaching this method.
      if (totalWeight > 0.0f)
      {
         taskActor->SetScore(totalWeightedScore / totalWeight);
      }
      else
      {
         taskActor->SetScore(0.0f);
      }

      if (taskActor->GetScore() >= taskActor->GetPassingScore())
      {
         taskActor->SetComplete(true);
      }

      NotifyFullActorUpdate();

      if (GetParentTask() != NULL)
         GetParentTask()->NotifyScoreChanged(*this);
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorRollupProxy::CreateDrawable()
   {
      SetDrawable(*new TaskActorRollup(*this));
   }

}
