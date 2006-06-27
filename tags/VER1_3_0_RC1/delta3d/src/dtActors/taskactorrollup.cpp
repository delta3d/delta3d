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
#include "dtActors/taskactorrollup.h"

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   TaskActorRollup::TaskActorRollup(dtGame::GameActorProxy &proxy) : TaskActor(proxy)
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
   void TaskActorRollupProxy::NotifyScoreChanged(const TaskActorProxy &childTask)
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
   void TaskActorRollupProxy::CreateActor()
   {
      mActor = new TaskActorRollup(*this);
   }

}
