/* -*-c++-*-
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
#ifndef DELTA_TASKCOMPONENT
#define DELTA_TASKCOMPONENT

#include <dtGame/gmcomponent.h>
#include <set>
#include <vector>

namespace dtGame
{
   class Message;

   /**
    * This task component is a game manager component responsible for managing a
    * central repository of tasks in a given game or simulation instance.
    */
   class DT_GAME_EXPORT TaskComponent : public GMComponent
   {
   public:

      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;
      static const std::string DEFAULT_NAME;

      /**
       * Constructs the task component.
       */
      TaskComponent(dtCore::SystemComponentType& type = *TYPE);

      /**
       * Called when a message arrives at the Game Manager.  These messages are trapped
       * anytime an actor create,destroy or update message arrives and are used to update
       * this components list of managed task actors.
       */
      virtual void ProcessMessage(const Message &message);

      /**
       * Gets the number of top level tasks found by the task component.  A top level
       * task is a task with no parent task.
       * @return Only the number of top level tasks.
       */
      unsigned int GetNumTopLevelTasks() const { return mTopLevelTaskList.size(); }

      /**
       * Gets the total number of tasks seen by the task component.
       * @return The total number of tasks in the task component at the time this method
       *     was called.
       */
      unsigned int GetNumTasks() const { return mTaskList.size(); }

      /**
       * Causes the task component to generate actor update messages for all tasks it is
       * currently watching.
       */
      void GenerateTaskUpdates();

      /**
       * Fills the specified vector with all the top level tasks located in the task
       * component.
       * @param toFill The vector to fill with task actors.
       */
      void GetTopLevelTasks(std::vector<dtCore::RefPtr<GameActorProxy> > &toFill);

      /**
       * Fills the specified vector will all tasks in the task component.
       * @param toFill The vector to fill with task actors.
       */
      void GetAllTasks(std::vector<dtCore::RefPtr<GameActorProxy> > &toFill);

      /**
       * Searches for the specified task via its assigned name.
       * @param name The name of the task to look for.
       * @return The first task found with a matching name or NULL if a task could not
       *    be found.
       */
      GameActorProxy* GetTaskByName(const std::string &name);

      /**
       * Removes all the tasks currently registered with the task component.
       * @note This should rarely ever need to be used as the actor delete messages
       *   are responded to accordingly.  Only in certain cases should this be used if
       *   for some reason the app is being prevented from getting the delete messages.
       */
      void ClearTaskList();

      /**
       * This method ensures that any changes to the task hierarchy are properly
       * reflected in the task component.
       * @note This method exists because the task hierarchy does not yet support
       *   actor properties for subtasks.  When this occurs, this method will become
       *   obsolete.
       */
      void CheckTaskHierarchy();

   protected:

      /**
       * Destroys the task component.
       */
      virtual ~TaskComponent();

      /**
       * Inserts a new task into the components list of managed tasks.  If the task is
       * already in the list, this method is a no-op.  This is called when the component
       * receives either an ACTOR_CREATE, ACTOR_PUBLISHED, or ACTOR_UPDATE.
       * @param taskProxy The task game actor proxy to insert.
       * @note If the new task proxy does not have a parent task, it is considered a top
       *    level task
       * @see GetTopLevelTasks()
       */
      void InsertTaskActor(GameActorProxy &taskProxy);

      /**
       * Removes an existing task from the components list of tasks it is tracking.  This is
       * called when the component receives an ACTOR_DELETED message from the game manager.
       * @param toRemove The proxy to remove.
       */
      void RemoveTaskActor(GameActorProxy &toRemove);

      /**
       * This method is called when the component receives an INFO_MAP_LOADED message.
       * It clears its list of tasks and looks in the Game Manager for the list of tasks
       * loaded from the map.
       */
      void HandleMapLoaded();

   private:
      std::map<std::string,dtCore::RefPtr<GameActorProxy> > mTopLevelTaskList;
      std::map<std::string,dtCore::RefPtr<GameActorProxy> > mTaskList;
   };

}

#endif
