/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * William E. Johnson II
 */

#ifndef DELTA_GMCOMPONENT
#define DELTA_GMCOMPONENT

#include <string>
#include <dtGame/gamemanager.h>
#include <dtCore/base.h>

namespace dtGame
{

   class Message;

   class DT_GAME_EXPORT GMComponent : public dtCore::Base
   {
   public:
      /// Constructor
      /// @param name client code must supply a unique name for this instance.
      GMComponent(const std::string& name);

   protected:
      /// Destructor
      virtual ~GMComponent();

   public:
      /**
       * handles a sent a message
       * @param The message
       */
      virtual void DispatchNetworkMessage(const Message& message);

      /**
       * handles a processed a message
       * @param The message
       */
      virtual void ProcessMessage(const Message& message);

      /**
       * Gets the game manager that owns this component
       * @return The game manager
       * @see dtGame::GameManager
       */
      GameManager* GetGameManager() const { return mParent; }

      /**
       * Get the priority of this component.
       * @return the value of the priority.  It is only valid when GetGameManager() is not NULL.
       */
      const GameManager::ComponentPriority& GetComponentPriority() const;

      /**
       * Called immediately after a component is added to the GM. Override this
       * to do init type behavior that needs access to the GameManager.
       */
      virtual void OnAddedToGM();

      /**
       * Called immediately after a component is removed from the GM. This is
       * where any previously allocated memory should be deallocated, files unloaded,
       * resources free'd, etc.  This gets called when the GMComponent gets removed
       * from the GameManager and when the GameManager gets shut down.
       * @see GameManager::RemoveComponent()
       * @see GameManager::Shutdown()
       */
      virtual void OnRemovedFromGM();

   private:
      friend class GameManager;
      /**
       * Sets the game manager that owns this component
       * @see dtGame::GameManager
       */
      void SetGameManager(GameManager* gameManager);
      /**
       * Sets the component priority on this component.
       * @see dtGame::GameManager::ComponentPriority
       */
      void SetComponentPriority(const GameManager::ComponentPriority& newPriority);

      GameManager* mParent;
      const GameManager::ComponentPriority* mPriority;

      // -----------------------------------------------------------------------
      //  Unimplemented constructors and operators
      // -----------------------------------------------------------------------
      GMComponent(const GMComponent&);
      GMComponent& operator=(const GMComponent&);
   };

} // namespace dtGame

#endif // DELTA_GMCOMPONENT
