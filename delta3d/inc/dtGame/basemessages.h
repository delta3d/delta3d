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
 * @author William E. Johnson II and David Guthrie
 */
#ifndef DELTA_BASEMESSAGES
#define DELTA_BASEMESSAGES

#include "dtGame/message.h"
#include "dtGame/messageparameter.h"
#include "dtDAL/gameevent.h"

namespace dtGame
{
   class DT_GAME_EXPORT TickMessage : public Message
   {
      public:

         /// Constructor
         TickMessage() : Message()
         {
            AddParameter(new FloatMessageParameter("DeltaSimTime"));
            AddParameter(new FloatMessageParameter("DeltaRealTime"));
            AddParameter(new FloatMessageParameter("SimTimeScale"));
            AddParameter(new DoubleMessageParameter("SimulationTime"));
         }

         /**
          * Gets the delta sim time variable associated with this message
          * @return The deltaSimTime
          */
         float GetDeltaSimTime() const;

         /**
          * Sets the delta sim time variable associated with this message
          * @param The new time
          */
         void SetDeltaSimTime(float newTime);

         /**
          * Gets the delta real time variable associated with this message
          * @return The deltaRealTime
          */
         float GetDeltaRealTime() const;

         /**
          * Sets the delta real time variable associated with this message
          * @param The new time
          */
         void SetDeltaRealTime(float newTime);

         /**
          * Gets the sim time scale variable associated with this message
          * @return The simTimeScale
          */
         float GetSimTimeScale() const;

         /**
          * Sets the sim time scale variable associated with this message
          * @param the new scale
          */
         void SetSimTimeScale(float newScale);

         /**
          * Gets the simulation time
          * @return The current value of simulation time.
          */
         double GetSimulationTime() const;

         /**
          * Sets the simulation time
          * @param newTime the new time to set
          */
         void SetSimulationTime(double newSimulationTime);


      protected:
         /// Destructor
         virtual ~TickMessage() { }


   };

   class DT_GAME_EXPORT TimerElapsedMessage : public Message
   {
      public:

         /// Constructor
         TimerElapsedMessage() : Message()
         {
            AddParameter(new StringMessageParameter("TimerName"));
            AddParameter(new FloatMessageParameter("LateTime"));
         }

         /**
          * Gets the sim time of day variable associated with this message
          * @return The simTimeOfDay
          */
         const std::string& GetTimerName() const;

         /**
          * Gets the sim time of day variable associated with this message
          * @return The simTimeOfDay
          */
         float GetLateTime() const;

         /**
          * Gets the sim time of day variable associated with this message
          * @return The simTimeOfDay
          */
         void SetTimerName(const std::string& name);

         /**
          * Gets the sim time of day variable associated with this message
          * @return The simTimeOfDay
          */
         void SetLateTime(float newTime);

      protected:
         /// Destructor
         virtual ~TimerElapsedMessage() { }

   };


   class DT_GAME_EXPORT TimeChangeMessage : public Message
   {
      public:

         /// Constructor
         TimeChangeMessage() : Message()
         {
            AddParameter(new FloatMessageParameter("TimeScale", 1.0f));
            AddParameter(new DoubleMessageParameter("SimulationTime"));
            AddParameter(new DoubleMessageParameter("SimulationClockTime"));
         }

         /**
          * Gets the time scale
          * @return The current value of time scale.
          */
         float GetTimeScale() const;

         /**
          * Sets the time scale
          * @param newTimeScale the new time scale to set
          */
         void SetTimeScale(float newTimeScale);

         /**
          * Gets the simulation time
          * @return The current value of simulation time.
          */
         double GetSimulationTime() const;

         /**
          * Sets the simulation time
          * @param newTime the new time to set
          */
         void SetSimulationTime(double newTime);

         /**
          * Gets the simulated clock time.  That is, the wall clock date/time of the simulation
          * @return The current value of the simulation clock time.
          */
         double GetSimulationClockTime() const;

         /**
          * Sets the simulated clock time.  That is, the wall clock date/time of the simulation
          * @param newSimClockTime the new simulatied clock time to set
          */
         void SetSimulationClockTime(double newSimClockTime);

      protected:
         /// Destructor
         virtual ~TimeChangeMessage() { }

   };

   class DT_GAME_EXPORT MapLoadedMessage : public Message
   {
      public:

         /// Constructor
         MapLoadedMessage() : Message()
         {
            AddParameter(new StringMessageParameter("LoadedMapName"));
         }

         /**
          * Gets the name of the map that was loaded
          */
         const std::string& GetLoadedMapName() const;

         /**
          * Sets the name of the map that was loaded
          * @param The name of the map
          */
         void SetLoadedMapName(const std::string &name);
      protected:
         /// Destructor
         virtual ~MapLoadedMessage() { }

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
         void SetGameEvent(dtDAL::GameEvent &event);

         /**
          * Gets a reference to the game event contained within this message.
          * @return A const reference to the game event.
          */
         const dtDAL::GameEvent *GetGameEvent() const;

      protected:
         virtual ~GameEventMessage() { }
   };

   class DT_GAME_EXPORT ActorPublishedMessage : public Message
   {
      public:

         /// Constructor
         ActorPublishedMessage() : Message()
         {

         }

      protected:
         /// Destructor
         virtual ~ActorPublishedMessage() { }
   };

   class DT_GAME_EXPORT ActorDeletedMessage : public Message
   {
      public:

         /// Constructor
         ActorDeletedMessage() : Message()
         {

         }

      protected:
         /// Destructor
         virtual ~ActorDeletedMessage() { }
   };

   class DT_GAME_EXPORT NetServerRejectMessage : public Message
   {
      public:

         /// Constructor
         NetServerRejectMessage() : Message()
         {
            AddParameter(new StringMessageParameter("RejectionMessage"));
         }

         /**
          * Gets the rejection reason of the message
          * @return The message string
          */
         const std::string& GetRejectionMessage() const;

         /**
          * Sets the rejection message
          * @param The message
          */
         void SetRejectionMessage(const std::string &msg);
      protected:
         /// Destructor
         virtual ~NetServerRejectMessage() { }

   };

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
         virtual ~RestartMessage() {};
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
         virtual ~ServerMessageRejected() {}
   };

   class DT_GAME_EXPORT CommandLoadMapMessage : public Message
   {
      public:

         /// Constructor
         CommandLoadMapMessage() : Message()
         {
            AddParameter(new StringMessageParameter("RequestedMapName"));
         }

         /**
         * Gets the name of the map that was loaded
         */
         const std::string& GetMapName() const;

         /**
         * Sets the name of the map that was loaded
         * @param The name of the map
         */
         void SetMapName(const std::string &name);

      protected:
         /// Destructor
         virtual ~CommandLoadMapMessage() { }

   };

   class DT_GAME_EXPORT RequestLoadMapMessage : public Message
   {
      public:

         /// Constructor
         RequestLoadMapMessage() : Message()
         {
           AddParameter(new StringMessageParameter("RequestedMapName"));
         }

         /**
          * Gets the name of the map that was loaded
          */
         const std::string& GetRequestedMapName() const;

         /**
          * Sets the name of the map that was loaded
          * @param The name of the map
          */
         void SetRequestedMapName(const std::string &name);

      protected:
         /// Destructor
         virtual ~RequestLoadMapMessage() { }

   };
}

#endif
