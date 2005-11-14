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
#ifndef DELTA_SERVERGAMEMANAGER
#define DELTA_SERVERGAMEMANAGER

#include "dtGame/gamemanager.h"

namespace dtGame
{
   class MachineInfo;
   class ServerNetworkComponent;

   class DT_GAMEMANAGER_EXPORT ServerGameManager : public GameManager
   {
      public:

         /// Constructor
         ServerGameManager(dtCore::Scene &scene);

         /// Destructor
         virtual ~ServerGameManager();

         /**
          * Sets the server network component of this server game manager
          * @param The component to set
          */
         void SetServerNetworkComponent(ServerNetworkComponent &newComponent) { mComponent = &newComponent; } 

         /**
          * Enables a client to listen to messages from this server game manager
          */
         void StartClientListening();

         /**
          * Disables a client to listen to messages from this server game manager
          */
         void StopClientListening();

         /**
          * Drops a client from the list of clients listening
          */
         void DropClient();

         /**
          * Retrieves a vector of clients listening to this server game manager
          * @return The vector of clients listening
          */
         const std::vector<MachineInfo*>& GetClients() const { return mClientMachineInfoList; }
         
      private:

         std::vector<MachineInfo*> mClientMachineInfoList;
         ServerNetworkComponent *mComponent;
   };
}
#endif
