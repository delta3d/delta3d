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
#ifndef DELTA_TASKACTOR
#define DELTA_TASKACTOR

#include "dtGame/gameactor.h"
#include "dtDAL/plugin_export.h"

#include <map>
#include <vector>

namespace dtActors
{
   class TaskActorProxy;
   
   /**
    * This class is the base task actor in the Delta3D task system.  This
    * class should be extended by any classes wishing to provide specialized
    * task behavior.  Each task has general properties such as score, passing
    * score, etc.  In addition, all tasks have a parent child relationship thus
    * facilitating the concept of nested tasks and task dependencies.
    * @see EventTaskActor
    * @see OrderedTaskActor
    * @see RollupTaskActor
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
          * @param score The score to assign to this task.
          */
         void SetScore(float score) { mScore = score; }
         
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
          * Gets the simulation time at which this task was completed.
          * @return The time in simulation seconds that this task 
          *    was marked complete.
          * @note Only this class or its subclasses may set this property
          *    which is done when the task determines it is complete.
          */
         double GetCompletedTimeStamp() const { return mCompletedTimeStamp; }
         
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
          * This method is called by children of this task in order to notiry their
          * parent task that a critical aspect of their state has been modified.
          * As an example, if an event occurs that causes the child task to be 
          * considered complete, it would notify its parent task.
          * @return The parent should return true if the operation is valid, and false
          *    otherwise.  This allows the parent tasks to maintain some degree of
          *    control over their children.
          */
         virtual bool NotifyParent();        
         
         /**
          * Convienence method to reset the properties of this task to their 
          * default values.
          * <par>
          *    Description = ""; <br>
          *    PassingScore = 1.0 <br>
          *    Score = 0.0 <br>
          *    Weight = 1.0 <br>
          * </par>
          * @note Calling this method does not change the parent child task hierarchy
          *    if it exists.
          */
         void Reset();         
                
      protected:
      
         /**
          * Destroys this task.
          */
         virtual ~TaskActor();         
         
         /**
          * Sets the simulation time at which this task was marked complete.
          * @param time The simulation time (in seconds) this task was completed.
          * @note This should be set at the same time as the complete flag was set.
          */
         void SetCompletedTimeStamp(double time) { mCompletedTimeStamp = time; }         
         
      private:
            
         //Basic properties of a task...
         std::string mDescription;
         float mPassingScore;
         float mScore;
         float mWeight;
         double mCompletedTimeStamp;
         bool mComplete;         
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
         
         /**
          * Gets a const pointer to the parent of this task.
          * @return The parent task or NULL if this task is not a subtask of another.
          */
         const TaskActorProxy *GetParentTaskProxy() const { return mParentTaskProxy; }
         
         /**
          * Gets a pointer to the parent of this task.
          * @return The parent task or NULL if this task is not a subtask of another.
          */
         TaskActorProxy *GetParentTaskProxy() { return mParentTaskProxy; }
         
         /**
          * Adds a new sub task to this task.  If the subtask is already a subtask
          * of a different parent, the subtask is reparented and its old parent task
          * is notified.
          * @param subTask The task to add as a subtask to this one.
          */
         void AddSubTaskProxy(TaskActorProxy &subTask);
         
         /**
          * Removes an existing task from this task's list of children.
          * @param subTask The task to remove.
          */
         void RemoveSubTaskProxy(const TaskActorProxy &subTask);
         
         /**
          * Removes an existing task from this task's list of children.
          * @param name The name of the task to remove.
          */
         void RemoveSubTaskProxy(const std::string &name);
         
         /**
          * Searches this task's list of sub tasks for the specified child.
          * @param name The name of the child task to find.
          */
         dtCore::RefPtr<TaskActorProxy> FindSubTaskProxy(const std::string &name);
         
         /**
          * Searches this task's list of sub tasks for the specified child.
          * @param id The unique id of the task to search for.
          */
         dtCore::RefPtr<TaskActorProxy> FindSubTaskProxy(const dtCore::UniqueId &id);
         
         /**
          * Fills the specified vector with all of this task's direct children.
          * @param subTasks A vector to be filled.  Note, this is cleared before filling.
          */
         void GetAllSubTaskProxies(std::vector<dtCore::RefPtr<TaskActorProxy> > &subTasks);
         
         /**
          * Gets the number of sub tasks owned by this task.
          * @return The number of sub tasks.
          */
         unsigned int GetNumSubTaskProxies() const { return mSubTaskProxies.size(); }
         
         /**
          * This method exists for the property system.  Note, this method is a no-op.
          * IsTopLevelTask() is set if this task is added as a child to another.
          * @param flag 
          */
         void SetTopLevelTask(bool flag) { }
         
         /**
          * Gets whether or not this task is a top level task.  A top level task is a task
          * with no parent.
          * @return True if this is a top level task (i.e. This task has no parent.)
          */
         bool IsTopLevelTask() const { return mParentTaskProxy == NULL; }
         
      protected:
      
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
         std::map<dtCore::UniqueId,dtCore::RefPtr<TaskActorProxy> > mSubTaskProxies;
   };

}

#endif 