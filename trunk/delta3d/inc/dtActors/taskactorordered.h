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
#ifndef DELTA_TASKACTORORDERED
#define DELTA_TASKACTORORDERED

#include <dtUtil/enumeration.h>
#include <dtActors/taskactor.h>

namespace dtActors
{
   /**
    * This ordered task actor is similar to the rollup task actor with the exception
    * of additional constraints.  An ordered task implies that its children must be
    * completed in a specified order.  The results of out of order execution vary,
    * however, the task has some notion of order.
    */
   class DT_PLUGIN_EXPORT TaskActorOrdered : public TaskActor
   {
      public:

         /**
          * This enumeration contains the different types of failures that may
          * result in the rejection of a child task completed out of order.
          */
         class DT_PLUGIN_EXPORT FailureType : public dtUtil::Enumeration
         {
            DECLARE_ENUM(FailureType);
            public:

               /**
                * This type of failure causes the ordered task to fail implying
                * that is cannot be completed until the task itself is reset.
                * @note This is the default type of failure for an ordered task.
                */
               static FailureType CAUSE_FAILURE;

               /**
                * This type of failure just blocks the request for a child task
                * to adjust its score whilst leaving the ordered task in question
                * unchanged.
                */
               static FailureType BLOCK;

            private:
               FailureType(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         /**
          * Constructs the ordered task actor.
          * @param proxy Proxy owning this actor.
          */
         TaskActorOrdered(dtGame::GameActorProxy& parent);

         /**
          * Sets the failure type for this ordered task.
          * @param type Enumeration defining the type of failure.
          */
         void SetFailureType(FailureType &type)
         {
            mFailureType = &type;
         }

         /**
          * Gets the failure type for this ordered task.
          * @return An enumeration of the current type.
          */
         FailureType &GetFailureType() const
         {
            return *mFailureType;
         }

         /**
          * Sets the task back to its default state.  The default failure type is BLOCK.
          */
         virtual void Reset();


      protected:

         /**
          * Destroys this task actor.
          */
         virtual ~TaskActorOrdered();

      private:
         FailureType *mFailureType;
   };

   /**
    * Proxy for the game event task actor.  It contains properties to track the
    * number of occurances of an event as well as determine how many times a game
    * event is listened for before completeness can occur.
    */
   class DT_PLUGIN_EXPORT TaskActorOrderedProxy : public TaskActorProxy
   {
      public:

         /**
          * Constructs the ordered task actor proxy.
          */
         TaskActorOrderedProxy();

         /**
          * Builds the property map for the ordered task proxy.  These properties
          * wrap the specified properties located in the ordered task actor.
          */
         virtual void BuildPropertyMap();

         /**
          * Overriding this method applies the ordering rules inherent to the ordered
          * task actor.  If a task is completed out of order one of two things may
          * happen depending on the value of the failure property and the child task
          * requesting the change is rejected.  Also note that the original task that
          * first instigated the change (a leaf node in the task tree) is slated as the
          * cause for the failure.
          * @param childTask The child task seeking approval.
          * @param origTask The tree node in the task list that first instigated
          *    the request.
          * @return True if a score change is allowed, false otherwise.
          * @see SetFailureType
          * @see GetFailureType
          */
         virtual bool RequestScoreChange(const TaskActorProxy &childTask, const TaskActorProxy &origTask);

         /**
          * Updates this tasks total score tracking based on the newly changed child task.
          * @param childTask The child task that issued the score changed notify.
          */
         virtual void NotifyScoreChanged(const TaskActorProxy &childTask);

         /** 
          * Similar to RequestScoreChange() except that in this case, there is no intent to 
          * actually change the score. This is used by IsCurrentlyMutable() in order to 
          * give a parent task a change to say that a child task is not in fact in a mutable state.
          * Calling this method should have NO side effects whatsoever on either the parent or the child. 
          *
          * Overridden from base behavior to also check to see if the task in question is the next
          * task in the ordered list. If not, it returns false. 
          *
          * @param childTask The child task in question.
          * @see RequestScoreChange
          */
         virtual bool IsChildTaskAllowedToChange(const TaskActorProxy &childTask) const;

         /**
          * Gets the last task that requested a score change out of order.
          * @return The "bad" task.
          */
         const TaskActorProxy *GetFailingTaskProxy() const { return mFailingTask; }

      protected:

         /**
          * Destroys the proxy.
          */
         virtual ~TaskActorOrderedProxy();


         /**
          * Create the underlying rollup task actor to be managed by this proxy.
          */
         virtual void CreateDrawable();

         /**
          * Sets the last known task that attempted to change its status out of order.
          * @param task The "bad" task.
          */
         void SetFailingTaskProxy(const TaskActorProxy &task) { mFailingTask = &task; }

      private:
         friend class TaskActorOrdered;

         const TaskActorProxy *mFailingTask;
   };
}

#endif
