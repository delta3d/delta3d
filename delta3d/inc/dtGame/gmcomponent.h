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
 * @author William E. Johnson II
 */

#ifndef DELTA_GMCOMPONENT
#define DELTA_GMCOMPONENT

#include <string>
#include "dtGame/export.h"
#include "dtGame/message.h"
#include "dtGame/messageparameter.h"
#include "dtGame/gamemanager.h"
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/base.h>

namespace dtGame
{
   class DT_GAME_EXPORT GMComponent : public dtCore::Base
   {
      public:

         /// Constructor
         GMComponent(const std::string& name = "GMComponent");

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
         const GameManager* GetGameManager() const { return mParent; }

         /**
          * Gets the game manager that owns this component
          * @return The game manager
          * @see dtGame::GameManager
          */
         GameManager* GetGameManager() { return mParent; }
         
         /**
          * Get the priority of this component.
          * @return the value of the priority.  It is only valid when GetGameManager() is not NULL.
          */
         const GameManager::ComponentPriority& GetComponentPriority() const { return *mPriority; };
         
         /**
          * Called immediately after a component is added to the GM. Override this 
          * to do init type behavior that needs access to the GameManager.
          */
         virtual void OnAddedToGM() { }

      private:
         friend class GameManager;
         /**
          * Sets the game manager that owns this component
          * @see dtGame::GameManager
          */
         void SetGameManager(GameManager* gameManager) { mParent = gameManager; }
         /**
          * Sets the component priority on this component.
          * @see dtGame::GameManager::ComponentPriority
          */
         void SetComponentPriority(const GameManager::ComponentPriority& newPriority) { mPriority = &newPriority; }

         GameManager* mParent;
         const GameManager::ComponentPriority* mPriority;
         
         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         GMComponent(const GMComponent&) {}
         GMComponent& operator=(const GMComponent&) {return *this;}
         
   };
}
#endif
