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

#include "dtGame/export.h"
#include "dtGame/message.h"
#include "dtGame/messageparameter.h"
#include <osg/Referenced>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>


namespace dtGame
{
   class GameManager;

   class DT_GAMEMANAGER_EXPORT GMComponent : public osg::Referenced
   {
      public:

         /// Constructor
         GMComponent();

         /// Destructor
         virtual ~GMComponent();

         /**
          * Sends a message
          * @param The message
          */
         virtual void SendMessage(const Message& message);

         /**
          * Processes a message
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
         
         
      private:
         friend class GameManager;
         /**
          * Sets the game manager that owns this component
          * @see dtGame::GameManager
          */
         void SetGameManager(GameManager* gameManager) { mParent = gameManager; }

         GameManager* mParent;
   };
}
#endif
