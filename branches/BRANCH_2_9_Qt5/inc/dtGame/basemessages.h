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
 * David Guthrie
 */

#ifndef DELTA_BASEMESSAGES
#define DELTA_BASEMESSAGES

#include <dtGame/message.h>
#include <dtGame/messagemacros.h>
#include <dtGame/messageparameter.h>
#include <dtCore/gameevent.h>

namespace dtGame
{
   typedef Message ActorDeletedMessage;
   typedef Message ActorPublishedMessage;


   DT_DECLARE_MESSAGE_BEGIN(TickMessage, Message, DT_GAME_EXPORT)
      /// Parameter that represents the change in the simulation time since last frame.
      DECLARE_PARAMETER_INLINE(float, DeltaSimTime)
      /// Parameter that represents the change in the real time since last frame.
      DECLARE_PARAMETER_INLINE(float, DeltaRealTime)
      /// The time scale, that is the factor of realtime that simulation time uses.
      DECLARE_PARAMETER_INLINE(float, SimTimeScale)
      /// The actual simulation time.
      DECLARE_PARAMETER_INLINE(double, SimulationTime)
   DT_DECLARE_MESSAGE_END()

   typedef TickMessage SystemMessage;

   /**
     * Used by the GameManager to indicate a timer has triggered.
     * @see GameManager::SetTimer()
     */
   DT_DECLARE_MESSAGE_BEGIN(TimerElapsedMessage, Message, DT_GAME_EXPORT)
      /// The name of the timer that elapsed
      DECLARE_PARAMETER_INLINE(std::string, TimerName)
      /// The amount of time the timer is late, since the frame time won't exactly match up with the specified timer.
      DECLARE_PARAMETER_INLINE(float, LateTime)
   DT_DECLARE_MESSAGE_END()


   DT_DECLARE_MESSAGE_BEGIN(TimeChangeMessage, Message, DT_GAME_EXPORT)
      /// The Time scale, i.e. the factor of real time at which simulation time advances.
      DECLARE_PARAMETER_INLINE(float, TimeScale)
      /// The simulation time as a count of seconds.
      DECLARE_PARAMETER_INLINE(double, SimulationTime)
      /// The simulation clock time as seconds since the epoch.
      DECLARE_PARAMETER_INLINE(double, SimulationClockTime)
   DT_DECLARE_MESSAGE_END()

   /**
    * Message used for all of the map change, close, load, etc messages.  It
    * has a list of map names.
    */
   class DT_GAME_EXPORT MapMessage : public Message
   {
      public:
         static const dtUtil::RefString PARAM_MAP_NAMES;

         /// Constructor
         MapMessage();

         /**
          * Gets the names of the map associated with this message
          */
         void GetMapNames(std::vector<std::string>& toFill) const;

         /**
          * Sets the name of the map that was loaded
          * @param The name of the map
          */
         void SetMapNames(const std::vector<std::string>& nameVec);

      protected:
         /// Destructor
         virtual ~MapMessage();
         dtCore::RefPtr<ArrayMessageParameter> mMapNames;
   };

   class DT_GAME_EXPORT GameEventMessage : public Message
   {
      public:

         /**
          * Constructs the game event message.  The message has one
          * parameter called "GameEvent".
          */
         GameEventMessage()
         {
            AddParameter(new GameEventMessageParameter("GameEvent"));
         }

         /**
          * Sets the game event contained within this message.
          * @param event The game event.
          */
         void SetGameEvent(const dtCore::GameEvent& event);

         /**
          * Gets a reference to the game event contained within this message.
          * @return A const reference to the game event.
          */
         const dtCore::GameEvent* GetGameEvent() const;

      protected:
         virtual ~GameEventMessage();
   };

   DT_DECLARE_MESSAGE_BEGIN(NetServerRejectMessage, Message, DT_GAME_EXPORT)
      /// Describes the reason for rejection
      DECLARE_PARAMETER_INLINE(std::string, RejectionMessage)
   DT_DECLARE_MESSAGE_END()


   class DT_GAME_EXPORT RestartMessage : public Message
   {
      public:
         /// Constructor
         RestartMessage() : Message()
         {
            AddParameter(new StringMessageParameter("Reason"));
         }

         /**
          * Gets the reason why the restart occured
          * @return The reason
          */
         const std::string& GetReason() const;

         /**
          * Sets the restart reason
          * @param The reason
          */
         void SetReason(const std::string &reason);

      protected:
         ///Destructor
         virtual ~RestartMessage();
   };

   class DT_GAME_EXPORT ServerMessageRejected : public Message
   {
      public:
         /// Constructor
         ServerMessageRejected() : Message()
         {
            AddParameter(new StringMessageParameter("Cause"));
         }

         /**
          * Retrieves the reason for the rejection
          * @return The cause
          */
         const std::string& GetCause() const;

         /**
          * Sets the reason for the rejection
          * @param The cause of the rejection
          */
         void SetCause(const std::string &cause);

      protected:
         ///Destructor
         virtual ~ServerMessageRejected();
   };

   /**
    * @class MachineInfoMessage
    * @brief A MachineInfoMessage contains a dtGame::MachineInfo to be used with different messages
    * to send information about a GameManager accross the network
    * @see dtGame::MachineInfo
    */
   class DT_GAME_EXPORT MachineInfoMessage : public dtGame::Message
   {
   public:

      // Constructor
      MachineInfoMessage();
      /**
       * Gets the Name from the contained MachineInfo
       * @return The name
       */
      const std::string& GetMachineInfoName() const;

      /**
       * Sets the name associated with the MachineInfo
       * @param The new name
       */
      void SetMachineInfoName(const std::string& name);

      /**
       * Gets the UniqueId from the contained MachineInfo as string
       * @return The uniqueid
       */
      const std::string& GetUniqueId() const;

      /**
       * Sets the UniqueId associated with the MachineInfo
       * @param The new uniqueid as string
       */
      void SetUniqueId(const std::string& strId);

      /**
       * Gets the HostName from the contained MachineInfo
       * @return The hostname
       */
      const std::string& GetHostName() const;

      /**
       * Sets the HostName associated with the MachineInfo
       * @param The new hostname
       */
      void SetHostName(const std::string& hostname);

      /**
       * Gets the IpAddress from the contained MachineInfo
       * @return The ipaddress
       */
      const std::string& GetIPAddress() const;

      /**
       * Sets the IpAddress associated with the MachineInfo
       * @param The new ipaddress
       */
      void SetIPAddress(const std::string& ipAddress);

      /**
       * Gets the TimeStamp from the contained MachineInfo
       * @return The timestamp
       */
      unsigned long GetTimeStamp() const;

      /**
       * Sets the TimeStamp associated with the MachineInfo
       * @param The new timestamp
       */
      void SetTimeStamp(unsigned long timeStamp);

      /**
       * Gets the Ping from the contained MachineInfo
       * @return The ping
       */
      unsigned int GetPing() const;

      /**
       * Sets the Ping associated with the MachineInfo
       * @param The new ping
       */
      void SetPing(unsigned int ping);

      /**
       * Gets the MachineInfo from the message
       * @return The machineinfo
       */
      dtCore::RefPtr<dtGame::MachineInfo> GetMachineInfo() const;

      /**
       * Sets the MachineInfo
       * @param The new machineinfo
       */
      void SetMachineInfo(const dtGame::MachineInfo& machineInfo);

   protected:
      /// Destructor
      virtual ~MachineInfoMessage();
   };

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
   class DT_GAME_EXPORT ServerFrameSyncMessage : public dtGame::Message
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
      virtual ~ServerFrameSyncMessage();
   };

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
   class DT_GAME_EXPORT ServerSyncControlMessage : public dtGame::Message
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
      virtual ~ServerSyncControlMessage();
   };
}

#endif
