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

#ifndef DELTA_SERVERFRAMESYNCMESSAGE
#define DELTA_SERVERFRAMESYNCMESSAGE

#include <dtNetGM/export.h>
#include <dtCore/refptr.h>
#include <dtGame/message.h>
#include <dtGame/messageparameter.h>

namespace dtNetGM
{
   /**
    * @class ServerFrameSyncMessage
    * @brief A ServerFrameSyncMessage is sent from the server at the end of EVERY frame. This message
    * is used by the client to be able to determine which messages belong to which frames. Specifically
    * this it is used to keep a client tightly in sync with the server for systems that are publishing 
    * at high rates (ex 60 publishes per second). This message is only sent if this behavior
    * is activated on the server. At that point, the server will publish a ServerSyncControlMessage.
    * In most cases, this behavior is disabled so this message will never be sent.
    *
    * NOTE - This message is eaten by ClientNetworkComponent. It is NOT forwarded to the GM message queue.
    * 
    * @see dtNetGM::ServerSyncControlMessage
    */
   class DT_NETGM_EXPORT ServerFrameSyncMessage : public dtGame::Message
   {
   public:

      // Constructor
      ServerFrameSyncMessage();

      /**
       * Gets the ServerSimTimeSinceStartup param (in seconds). This is how long the server has
       * been running. You probably won't need this data, but it may help clients determine
       * how old the data is and do a better job keeping the server in sync with the client. .
       * @return The ServerSimTimeSinceStartup value sent from the server
       */
      double GetServerSimTimeSinceStartup() const;

      /**
       * Gets the ServerSimTimeSinceStartup param (in seconds). This is how long the server has
       * been running. You probably won't need this data, but it may help clients determine
       * how old the data is and do a better job keeping the server in sync with the client. .
       * @param The ServerSimTimeSinceStartup value (set this on the server)
       */
      void SetServerSimTimeSinceStartup(double newValue);

      /**
       * Gets the ServerTimeScale param. This is the current timescale used by the server. 
       * Timescale indicates whether we are running in real time (1.0), faster than real (ex 2.0), or 
       * slower (ex 0.5). This may help the client do a better job keeping in sync.
       * @return The ServerTimeScale value
       */
      float GetServerTimeScale() const;

      /**
       * Gets the ServerTimeScale param. This is the current timescale used by the server. 
       * Timescale indicates whether we are running in real time (1.0), faster than real (ex 2.0), or 
       * slower (ex 0.5). This may help the client do a better job keeping in sync.
       * @param The ServerTimeScale value
       */
      void SetServerTimeScale(float newValue);

   protected:
      /// Destructor
      virtual ~ServerFrameSyncMessage() { }
   };
}

#endif // DELTA_SERVERFRAMESYNCMESSAGE
