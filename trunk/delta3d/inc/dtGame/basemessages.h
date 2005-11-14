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
#ifndef DELTA_BASEMESSAGES
#define DELTA_BASEMESSAGES

#include "dtGame/message.h"
#include "dtGame/messageparameter.h"

namespace dtGame
{
   class DT_GAMEMANAGER_EXPORT TickMessage : public Message
   {
      public:

         /// Constructor
         TickMessage() : Message()
         {
            AddParameter(new FloatMessageParameter("DeltaSimTime"));
            AddParameter(new FloatMessageParameter("DeltaRealTime"));
            AddParameter(new FloatMessageParameter("SimTimeScale"));
            AddParameter(new LongIntMessageParameter("SimTimeOfDay"));
         }
         
         /**
          * Gets the delta sim time variable associated with this message
          * @return The deltaSimTime
          */
         float GetDeltaSimTime();

         /**
          * Gets the delta real time variable associated with this message
          * @return The deltaRealTime
          */
         float GetDeltaRealTime();

         /**
          * Gets the sim time scale variable associated with this message
          * @return The simTimeScale
          */
         float GetSimTimeScale();

         /**
          * Gets the sim time of day variable associated with this message
          * @return The simTimeOfDay
          */
         long GetSimTimeOfDay();
         
         /**
          * Sets the delta sim time variable associated with this message
          * @param The new time
          */
         void SetDeltaSimTime(float newTime);

         /**
          * Sets the delta real time variable associated with this message
          * @param The new time
          */
         void SetDeltaRealTime(float newTime);

         /**
          * Sets the sim time scale variable associated with this message
          * @param the new scale
          */
         void SetSimTimeScale(float newScale);

         /**
          * Sets the sim time of day variable associated with this message
          * @param The new simTimeOfDay
          */
         void SetSimTimeOfDay(long newTimeOfDay);
      protected:
         /// Destructor
         virtual ~TickMessage() { }

         
   };

   class DT_GAMEMANAGER_EXPORT TimerElapsedMessage : public Message
   {
      public:

         /// Constructor
         TimerElapsedMessage() : Message()
         {
            AddParameter(new StringMessageParameter("TimerName"));
            AddParameter(new LongIntMessageParameter("SimTimeOfDay"));
            AddParameter(new FloatMessageParameter("LateTime"));
         }

         /**
          * Gets the sim time of day variable associated with this message
          * @return The simTimeOfDay
          */
         const std::string& GetTimerName();

         /**
          * Gets the sim time of day variable associated with this message
          * @return The simTimeOfDay
          */
         long GetSimTimeOfDay();

         /**
          * Gets the sim time of day variable associated with this message
          * @return The simTimeOfDay
          */
         float GetLateTime();

         /**
          * Gets the sim time of day variable associated with this message
          * @return The simTimeOfDay
          */
         void SetTimerName(const std::string& name);

         /**
          * Gets the sim time of day variable associated with this message
          * @return The simTimeOfDay
          */
         void SetSimTimeOfDay(long newTime);

         /**
          * Gets the sim time of day variable associated with this message
          * @return The simTimeOfDay
          */
         void SetLateTime(float newTime);
      protected:
         /// Destructor
         virtual ~TimerElapsedMessage() { }

   };

   class DT_GAMEMANAGER_EXPORT MapLoadedMessage : public Message
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
         const std::string& GetLoadedMapName();

         /**
          * Sets the name of the map that was loaded
          * @param The name of the map
          */
         void SetLoadedMapName(const std::string &name);
      protected:
         /// Destructor
         virtual ~MapLoadedMessage() { }
      
   };

   class DT_GAMEMANAGER_EXPORT ActorPublishedMessage : public Message
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

   class DT_GAMEMANAGER_EXPORT ActorDeletedMessage : public Message
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

   class DT_GAMEMANAGER_EXPORT NetServerRejectMessage : public Message
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
         const std::string& GetRejectionMessage();

         /**
          * Sets the rejection message 
          * @param The message
          */
         void SetRejectionMessage(const std::string &msg);
      protected:
         /// Destructor
         virtual ~NetServerRejectMessage() { }
         
   };

   class DT_GAMEMANAGER_EXPORT RestartMessage : public Message
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
         const std::string& GetReason();

         /**
          * Sets the restart reason
          * @param The reason
          */
         void SetReason(const std::string &reason);

      protected:
         ///Destructor
         virtual ~RestartMessage() {};
   };

   class DT_GAMEMANAGER_EXPORT ServerMessageRejected : public Message
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
         const std::string& GetCause();

         /**
          * Sets the reason for the rejection
          * @param The cause of the rejection
          */
         void SetCause(const std::string &cause);
      protected:
         ///Destructor
         virtual ~ServerMessageRejected() {}
   };

   class DT_GAMEMANAGER_EXPORT CommandLoadMapMessage : public Message
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
         const std::string& GetMapName();

         /**
         * Sets the name of the map that was loaded
         * @param The name of the map
         */
         void SetMapName(const std::string &name);
         
      protected:
         /// Destructor
         virtual ~CommandLoadMapMessage() { }

   };

   class DT_GAMEMANAGER_EXPORT RequestLoadMapMessage : public Message
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
         const std::string& GetRequestedMapName();

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
