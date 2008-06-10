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
#ifndef DELTA_TASKACTOR
#define DELTA_TASKACTOR

#include <dtGame/gameactor.h>


#include <dtUtil/mathdefines.h>

#include <dtDAL/plugin_export.h>
#include <dtDAL/actorproxyicon.h>

#include <vector>

namespace dtDAL 
{
   class NamedActorParameter;
   class NamedGroupParameter;
}

namespace dtActors
{
   class TaskActorProxy;

   /**
    * This class is the base task actor in the Delta3D task system.  This
    * class should be extended by any classes wishing to provide specialized
    * task behavior.  Each task has general properties such as score, passing
    * score, etc.  In addition, all tasks have a parent child relationship thus
    * facilitating the concept of nested tasks and task dependencies. 
    * 
    * All Task Actors are in one of 3 states:  IsComplete(), IsFailed(), or neither. 
    * Generally, a Task can change it's score and other values until it becomes Complete or 
    * Failed. At that point, it is no longer mutable and shouldn't change much.  
    * The complete state is typically determined automatically - when a minimal 
    * score is reached like when all child tasks are complete. The Failed stage 
    * is typically set manually when you want to force a task to be failed. Sometimes a 
    * task will be marked Failed automatically based on parent or container task
    * behaviors (such as a Ordered Failing task done out of order).  
    * @see TaskActorGameEvent
    * @see TaskActorOrderedTask
    * @see TaskActorRollupTask
    */
   class DT_PLUGIN_EXPORT TaskActor : public dtGame::GameActor
   {
      public:

         /**
          * Constructs a default task actor.
          * @param proxy The actor proxy owning this task actor.
          * @param desc An optional description of this task actor.
          */
         TaskActor(dtGame::GameActorProxy &proxy);

         /**
          * Sets the description of this task.
          * @param desc The new description to assign to this task.
          */
         void SetDescription(const std::string &desc) { mDescription = desc; }

         /**
          * Gets the description of this task.
          * @return The task's description.
          */
         std::string GetDescription() const { return mDescription; }

         /**
          * Sets the display name of this task. This allows you to have a pretty value for the user.
          * @param displayName The new display name to assign to this task.
          */
         void SetDisplayName(const std::string &displayName) { mDisplayName = displayName; }

         /**
          * Gets the display name of this task. Allows for a pretty value for the user.
          * @return The task's display name.
          */
         std::string GetDisplayName() const { return mDisplayName; }

         /**
          * Sets the passing score of this task.  The passing score is used
          * by the task and its subclasses to determine if a player managed
          * to complete "enough" of the task to be considered complete.
          * @param score The new passing score for this task.
          */
         void SetPassingScore(float score) { mPassingScore = score; }

         /**
          * Gets the passing score of this task.
          * @return This task's current passing score.
          */
         float GetPassingScore() const { return mPassingScore; }

         /**
          * Sets the score of this task.  The score reflects how "well" the
          * player did in accomblishing this task.
          * @param score The score to assign to this task.  This value is clamped from 0.0 - 1.0.
          */
         void SetScore(float score) { dtUtil::Clamp(score,0.0f,1.0f); mScore = score; }

         /**
          * Gets the score currently assigned to this task.
          * @return This task's passing score.
          */
         float GetScore() const { return mScore; }

         /**
          * Sets the weight of this task.  This property allows tasks to have
          * more meaning of be more important than other tasks.  Weighted tasks
          * are especially important where there is a collection of tasks and task
          * siblings.
          * @param weight The new weight to give this task.  The default weight of
          *    a task is 1.0.
          */
         void SetWeight(float weight) { mWeight = weight; }

         /**
          * Gets the weight currently assigned to this task.
          * @return This task's current weight.
          */
         float GetWeight() const { return mWeight; }

         /**
          * Gets the simulation time at which this task was completed or failed.
          * @return The time in simulation seconds that this task
          *    was marked complete or failed.
          * @note Only this class or its subclasses should set this property
          *    which is done when the task determines it is complete or failed.
          */
         double GetCompletedTimeStamp() const { return mCompletedTimeStamp; }

         /**
          * Sets the simulation time at which this task was marked complete or failed.
          * @param time The simulation time (in seconds) this task was completed/failed.
          * @note Should be used with caution if setting outside of this or a subclasses
          *  method as most of the time, the task itself will set this value when it
          *  has determined it is complete or failed.
          */
         void SetCompletedTimeStamp(double time) { mCompletedTimeStamp = time; }

         /**
          * Gets the status of this task in terms of "completeness".
          * @return True if this task has been completed, false otherwise.
          * @note Only this class or its subclasses may set this property since
          *    tasks evaluate themselves for "completeness".
          */
         bool IsComplete() const { return mComplete; }

         /**
          * Sets the status of this task to complete.
          * @param flag True if this task should be marked complete, false otherwise.
          * @note If the flag is true, then this method also sets the completed time
          *    stamp.
          */
         void SetComplete(bool flag);

         /**
          * Gets the status of this task in terms of "failedness". Note that a task can never
          * be both failed and complete - mutually exclusive. A failed task may or may 
          * not affect it's parent. 'Failed' is unrelated to the task's score
          * @return True if this task has been failed, false otherwise.
          * @note Being 'failed' does not have any direct bearing on the failed or score status 
          *    of either our parent task or any of our children. 
          */
         bool IsFailed() const { return mFailed; }

         /**
          * Sets the status of this task to failed or not failed. Note that a task can never
          * be both failed and complete - mutually exclusive. A failed task may or may 
          * not affect it's parent. Setting Failed to true has no effect on score by design.  
          * Setting failed to true sets the completed time stamp.
          * @param flag True if this task should be marked failed, false otherwise.
          * @note Being 'failed' does not have any direct bearing on the failed status 
          *    of either our parent task or any of our children. Same with the score. 
          */
         void SetFailed(bool flag);

         /**
          * Convienence method to reset the properties of this task to their
          * default values.
          * @par
          *    PassingScore = 1.0 <br>
          *    Score = 0.0 <br>
          *    Weight = 1.0 <br>
          *    Complete = false <br>
          *    Failed = false <br>
          *    Completed Time Stamp = -1.0 <br>
		    *    NotifyLMSOnUpdate = false
          * @note Calling this method does not change the parent child task hierarchy
          *    if it exists.
          */
         virtual void Reset();

   		/**
   		 * Gets the value that determines if this task should notify
   		 * a Learning Management System (LMS) when it is updated.
   		 * @return True if this task should notify an LMS when it is updated, false
   		 * otherwise.
   		 * @note The actual notification of an LMS is handled by the
   		 * TaskComponent class.
   		 */
   		bool GetNotifyLMSOnUpdate() const { return mNotifyLMSOnUpdate; }
   
   		/**
   		 * Sets the value that determines if this task should notify
   		 * a Learning Management System (LMS) when it is updated.
   		 * @param flag True if this task should notify an LMS when it is updated, false
   		 * otherwise.
   		 * @note The actual notification of an LMS is handled by the
   		 * TaskComponent class.
   	    */
   		void SetNotifyLMSOnUpdate(bool flag) { mNotifyLMSOnUpdate = flag; }

      protected:

         /**
          * Destroys this task.
          */
         virtual ~TaskActor();

      private:

         //Basic properties of a task...
         std::string mDescription;
         std::string mDisplayName;
         float mPassingScore;
         float mScore;
         float mWeight;
         double mCompletedTimeStamp;
         bool mComplete; // mutually exclusive with mFailed
         bool mFailed; // mutually exclusive with mComplete
         bool mNotifyLMSOnUpdate;
   };

   /**
    * This class is a proxy for the base task actor following the Delta3D
    * game actor design philosophy.
    */
   class DT_PLUGIN_EXPORT TaskActorProxy : public dtGame::GameActorProxy
   {
      public:

         /**
          * Constructs the task actor proxy.
          */
         TaskActorProxy();

         /**
          * Builds the property map for the task actor proxy.  These properties
          * wrap the specified properties located in the task actor.
          */
         virtual void BuildPropertyMap();

         /**
          * Registers any invokables used by the task actor proxy.  The invokables
          * allow the task actor to hook into the game manager messages system.
          */
         virtual void BuildInvokables();

         ///Task actors are global actors, so they are not placeable. 
         virtual bool IsPlaceable() const { return false; };

         /**
          * This method is called by a child task on its parent to request
          * from its parent permission to change its score.  This method allows
          * the parent task control over whether or not its children may be changed.
          * For example, the TaskActorOrdered task uses this to ensure that if a
          * subtask or any of its children are not completed out of order.
          * @param childTask The child task in question.
          * @param origTask The tree node in the task list that first instigated
          *    the request.
          * @return True if a score change is allowed, false otherwise.
          * @note This method is a recurive method does a bottom up traversal of the
          *   path from the root of the task tree to the leaf task that first called
          *   this method.
          * @note This method is different from IsChildTaskAllowedToChange in that this expects
          *   that a change is imminent. This method will sometimes cause a parent to fail itself.
          * @see TaskActorOrdered
          * @see IsChildTaskAllowedToChange
          */
         virtual bool RequestScoreChange(const TaskActorProxy &childTask, const TaskActorProxy &origTask);

         /**
          * Informs the parent of this task that the score was changed.
          * This is useful for tasks whos score is dependent on the cummulative scores
          * of its children.
          * @param childTask The child task that issued the score changed notify.
          * @see TaskActorRollup
          */
         virtual void NotifyScoreChanged(const TaskActorProxy &childTask);

         /**
          * Determines if conditions are right so that this Task could change it's 
          * IsComplete, IsFailed, or Score. This will typically mean that it is not already
          * Complete or Failed and that it's parent is not some sort of task (such as a blocking
          * ordered task) that would prevent change.
          * @note This method is almost never used by the task actor itself. It is intended to 
          *   support game behavior or changes to a user interface.  
          */ 
         virtual bool IsCurrentlyMutable();

         /** 
          * Similar to RequestScoreChange() except that in this case, there is no intent to 
          * actually change the score. This is used by IsCurrentlyMutable() in order to 
          * give a parent task a change to say that a child task is not in fact in a mutable state.
          * Calling this method should have NO side effects whatsoever on either the parent or the child. 
          * @param childTask The child task in question.
          * @see RequestScoreChange
          */
         virtual bool IsChildTaskAllowedToChange(const TaskActorProxy &childTask) const;

         /**
          * This method is called when a task property is changed.
          * @note For optimal performance, the individual task implementations should
          *   called this method only once if a group of properties changes in one tick.
          */
         virtual void NotifyActorUpdate();

         /**
          * Gets a const pointer to the parent of this task.
          * @return The parent task or NULL if this task is not a subtask of another.
          */
         const TaskActorProxy *GetParentTask() const { return mParentTaskProxy; }

         /**
          * Gets a pointer to the parent of this task.
          * @return The parent task or NULL if this task is not a subtask of another.
          */
         TaskActorProxy *GetParentTask() { return mParentTaskProxy; }

         /**
          * Adds a new sub task to this task.  If the subtask is already a subtask
          * of a different parent, the subtask is reparented and its old parent task
          * is notified.
          * @param subTask The task to add as a subtask to this one.
          */
         void AddSubTask(TaskActorProxy &subTask);

         /**
          * Removes an existing task from this task's list of children.
          * @param subTask The task to remove.
          */
         void RemoveSubTask(const TaskActorProxy &subTask);

         /**
          * Removes an existing task from this task's list of children.
          * @param name The name of the task to remove.
          */
         void RemoveSubTask(const std::string &name);

         /**
          * Searches this task's list of sub tasks for the specified child.
          * @param name The name of the child task to find.
          * @return The requested proxy or NULL if it could not be found.
          */
         TaskActorProxy *FindSubTask(const std::string &name);

         /**
          * Searches this task's list of sub tasks for the specified child.
          * @param id The unique id of the task to search for.
          * @return The requested proxy or NULL if it could not be found.
          */
         TaskActorProxy *FindSubTask(const dtCore::UniqueId &id);

         /**
          * @return A const list of sub tasks owned by this task.
          */
         const std::vector<dtCore::RefPtr<TaskActorProxy> > &GetAllSubTasks() const
         {
            return mSubTaskProxies;
         }

         /**
          * Fills the specified vector with all of this task's direct children.
          */
         void GetAllSubTasks(std::vector<TaskActorProxy*>& toFill);
         
         /**
          * Fills the specified vector with all of this task's direct children.
          */
         void GetAllSubTasks(std::vector<const TaskActorProxy*>& toFill) const;

         /**
          * Gets the number of sub tasks owned by this task.
          * @return The number of sub tasks.
          */
         unsigned GetSubTaskCount() const { return unsigned(mSubTaskProxies.size()); }

         /**
          * Gets whether or not this task is a top level task.  A top level task is a task
          * with no parent.
          * @return True if this is a top level task (i.e. This task has no parent.)
          */
         bool IsTopLevelTask() const { return mParentTaskProxy == NULL; }

         /**
          * Wrapper for the actor method
          */
         float GetPassingScore() const { return static_cast<const TaskActor&>(GetGameActor()).GetPassingScore(); }

         /**
          * Wrapper for the actor method
          */
         float GetScore() const { return static_cast<const TaskActor&>(GetGameActor()).GetScore(); }

         /**
          * Gets the billboard used to represent static mesh if this proxy's
          * render mode is RenderMode::DRAW_BILLBOARD_ICON. Used by STAGE.
          * @return billboard icon to use
          */
         virtual dtDAL::ActorProxyIcon* GetBillBoardIcon()
         {
            if(!mBillBoardIcon.valid())
            {
               mBillBoardIcon =
                  new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::GENERIC);
            }

            return mBillBoardIcon.get();
         }

         /**
          * Gets the method by which this static mesh is rendered. This is used by STAGE.
          * @return If there is no geometry currently assigned, this
          *  method will return RenderMode::DRAW_BILLBOARD_ICON.  If
          *  there is geometry assigned to this static mesh, RenderMode::DRAW_ACTOR
          *  is returned.
          */
         virtual const ActorProxy::RenderMode& GetRenderMode()
         {
            return ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
         }

      protected:

         /**
          * Sets the group parameter to populate the list of subtasks.
          * @param subTasks the group of tasks.  It should contain NamedActorParameters with the ids of all the subtask proxies.
          * @see dtDAL::NamedGroupParameter
          * @see dtDAL::NamedActorParameter
          */
         void SetSubTaskGroup(const dtDAL::NamedGroupParameter& subTasks);
         
         /**
          * @return a new NamedGroupParameter containing NamedActorParameters with the ids of all the subtask proxies.
          * @see dtDAL::NamedGroupParameter
          * @see dtDAL::NamedActorParameter
          */
         dtCore::RefPtr<dtDAL::NamedGroupParameter> GetSubTaskGroup() const;

         /**
          * Destroys the task actor proxy.
          */
         virtual ~TaskActorProxy();

         /**
          * Called by the game manager during creation of the proxy.  This method
          * creates a TaskActor and returns it.
          */
         virtual void CreateActor();

         /**
          * Called when the proxy has been added to the game manager.
          */
         virtual void OnEnteredWorld();

         /**
          * Sets the parent task to this one.  Called when a subtask is added.
          * @param The parent task.
          */
         void SetParentTaskProxy(TaskActorProxy *parent) { mParentTaskProxy = parent; }

      private:

         //Parent task if this task is a subtask of another.
         TaskActorProxy *mParentTaskProxy;

         //List of subtasks or child tasks of this task.
         std::vector<dtCore::RefPtr<TaskActorProxy> > mSubTaskProxies;
   };

}

#endif
