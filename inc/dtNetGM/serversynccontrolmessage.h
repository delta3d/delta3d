/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2010, Alion Science and Technology.
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
 * Curtiss Murphy
 */

#ifndef DELTA_SERVERSYNCCONTROLMESSAGE
#define DELTA_SERVERSYNCCONTROLMESSAGE

#include <dtNetGM/export.h>
#include <dtCore/refptr.h>
#include <dtGame/message.h>
#include <dtGame/messageparameter.h>

namespace dtNetGM
{
   /**
    * @class ServerSyncControlMessage
    * @brief A ServerSyncControlMessage is sent from the server whenever a client establishes a 
    * connection to the server or when the frame sync behavior changes. Server frame syncs are 
    * used to keep a server & client tightly coupled. In most cases, this behavior is disabled.
    * 
    * This behavior is useful when there is a server with clients setup to display the same 
    * environment at 60 Hz. The entity data is published at a high frequency. At this rate, even 
    * variance between monitor VSync rates can cause obvious and distracting 'jiggles'. 
    * 
    * @see dtNetGM::ServerFrameSyncMessage
    */
   class DT_NETGM_EXPORT ServerSyncControlMessage : public dtGame::Message
   {
   public:

      // Constructor
      ServerSyncControlMessage();

      /**
       * Gets the SyncEnabled param. If true, frame syncs will be sent at the end of each frame
       * @return The SyncEnabled value (default is false)
       */
      bool GetSyncEnabled() const;

      /**
       * Sets the SyncEnabled param. If true, frame syncs will be sent at the end of each frame.
       * @param The SyncEnabled value (default is false)
       */
      void SetSyncEnabled(bool newValue);

      /**
       * Gets the NumSyncsPerSecond param. This is how often the server will publish a ServerFrameSyncMessage. 
       * The client will use this value to determine when to wait for the sync and when to push network traffic. 
       * @return The NumSyncsPerSecond value (default 60)
       * @see dtNetGM::ServerFrameSyncMessage
       */
      unsigned int GetNumSyncsPerSecond() const;

      /**
       * Sets the NumSyncsPerSecond param. This is how often the server will publish a ServerFrameSyncMessage. 
       * The client will use this value to determine when to wait for the sync and when to push network traffic. 
       * @param The NumSyncsPerSecond value (default 60)
       */
      void SetNumSyncsPerSecond(unsigned int newValue);

      /**
       * Gets the MaxWaitTime param (in milliseconds). This is how long the client should wait when expecting
       * the next ServerFrameSyncMessage. If the client doesn't get a message before this time, then it gives up. 
       * Note - Messages are not sent until a frame sync is received (except in extreme error conditions). 
       * Note - The client forces a thread BLOCK while waiting. Long WaitTimes could impact performance.
       * @return The MaxWaitTime value (default 4.0ms)
       */
      float GetMaxWaitTime() const;

      /**
       * Sets the MaxWaitTime param (in milliseconds). This is how long the client should wait when expecting
       * the next ServerFrameSyncMessage. If the client doesn't get a message before this time, then it gives up. 
       * Note - Messages are not sent until a frame sync is received (except in extreme error conditions). 
       * Note - The client forces a thread BLOCK while waiting. Long WaitTimes could impact performance.
       * @param The MaxWaitTime value (default 4.0ms)
       */
      void SetMaxWaitTime(float newValue);


   protected:
      /// Destructor
      virtual ~ServerSyncControlMessage() { }
   };
}

#endif // DELTA_SERVERSYNCCONTROLMESSAGE
