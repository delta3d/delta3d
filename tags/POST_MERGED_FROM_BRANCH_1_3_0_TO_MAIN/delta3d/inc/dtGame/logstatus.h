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
 * @author Curtiss Murphy
 */
#ifndef DELTA_LOGSTATUS
#define DELTA_LOGSTATUS

#include "dtGame/export.h"
#include <dtCore/uniqueid.h>
#include <dtUtil/enumeration.h>
#include <string>
#include <ostream>

namespace dtGame
{

   /**
    * Simple enumeration for the states of the logger.  This is used as part of the LogStatus
    * object and is driven primarily by the server logger component.
    */
   class DT_GAME_EXPORT LogStateEnumeration : public dtUtil::Enumeration
   {
      DECLARE_ENUM(LogStateEnumeration);
      public:

         static const LogStateEnumeration LOGGER_STATE_PLAYBACK;
         static const LogStateEnumeration LOGGER_STATE_RECORD;
         static const LogStateEnumeration LOGGER_STATE_IDLE;

      private:
         LogStateEnumeration(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };

   /**
    * @class LogStatus
    * A simple data class for basic Log Status information. This class has
    * a copy constructor plus the = operator.
    */
   class DT_GAME_EXPORT LogStatus
   {
      public:

         // Constructor
         LogStatus() : mCurrentSimTime(0), mAutoRecordKeyframeInterval(0),
            mCurrentRecordDuration(0), mNumMessages(0)
         {
            mStateEnum = &LogStateEnumeration::LOGGER_STATE_IDLE;
         }

         virtual ~LogStatus() {}

         /**
          * Get the state enum.  This indicates the server logger component's recording/playback/idle state.
          * @return The server logger comonent's state.
          */
         const LogStateEnumeration &GetStateEnum() const { return *mStateEnum; }

         /**
          * Set the state enum.  This is typically set by the server and indicates the
          * server logger components state.
          * @param newStateEnum the new state.
          */
         void SetStateEnum(const LogStateEnumeration &newStateEnum) { mStateEnum = &newStateEnum; }

         /**
          * Gets the current simulation time as believed by the server component (as opposed to real time)
          * @return The server's believed simulation time stamp (as opposed to real time)
          */
         double GetCurrentSimTime() const { return mCurrentSimTime; }

         /**
          * Sets the current simulation time as believed by the server component (as opposed to real time)
          * @param The new simulation time stamp (as opposed to real time)
          */
         void SetCurrentSimTime(double newCurrentSimTime) { mCurrentSimTime = newCurrentSimTime; }

         /**
          * Gets the map name that was active when this keyframe was generated
          * @return The map name
          */
         const std::string& GetActiveMap() const { return mActiveMap; }

         /**
          * Sets the map name that was active when this keyframe was generated
          * @param The map name
          */
         void SetActiveMap(const std::string &newActiveMap) { mActiveMap = newActiveMap; }

         /**
          * Gets the log file
          * @return The logfile
          */
         const std::string& GetLogFile() const { return mLogFile; }

         /**
          * Sets the log file
          * @param The new log file
          */
         void SetLogFile(const std::string &newLogFile) { mLogFile = newLogFile; }

         /**
          * Gets the auto record keyframe interval.  0 indicates no automatic interval.
          * Any non-0 means that the server will attempt to make a keyframe every X seconds.
          * Only the server should set this value.
          * @return The keyframe auto record interval (in seconds)
          */
         double GetAutoRecordKeyframeInterval() const { return mAutoRecordKeyframeInterval; }

         /**
          * Sets the auto record keyframe interval.  0 indicates no automatic interval.
          * Any non-0 means that the server will attempt to make a keyframe every X seconds.
          * Only the server should set this value.
          * @param The new auto record keyframe interval (in seconds)
          */
         void SetAutoRecordKeyframeInterval(double newAutoRecordKeyframeInterval) { mAutoRecordKeyframeInterval = newAutoRecordKeyframeInterval; }

         /**
          * Sets the length (in seconds) of the current recording.  This should only be non-zero
          * during a record session.  This should ONLY be set by the server log component.
          * @return The length of the current record (in seconds).
          */
         double GetCurrentRecordDuration() const { return mCurrentRecordDuration; }

         /**
          * Sets the length (in seconds) of the current recording.  This should only be non-zero
          * during a record session.  This should ONLY be set by the server log component.
          * @param The length of the current record (in seconds).
          */
         void SetCurrentRecordDuration(double newCurrentRecordDuration) { mCurrentRecordDuration = newCurrentRecordDuration; }

         /**
          * Gets the number of messages that have been processed/sent by the server logger component.
          * @return The number of messages.
          */
         unsigned long GetNumMessages() const { return mNumMessages; }

         /**
          * Sets the number of messages that have been processed/sent by the server logger component.
          * This should only be set by the server logger component itself.
          * @param numMessages The number of messages.
          */
         void SetNumMessages(unsigned long numMessages)
         {
            mNumMessages = numMessages;
         }

         /**
          * Prints the log status to a stream.
          * @param stream Standard stream
          * @param me The log status object to print.
          */
         friend std::ostream &operator<<(std::ostream &stream,const LogStatus &me)
         {
            stream << "LogStatus: State[" << me.mStateEnum->GetName() <<
               "], SimTime[" << me.mCurrentSimTime << "], Map[" << me.mActiveMap <<
                  "], LogFile[" << me.mLogFile << "], #Messagse[" << me.mNumMessages << "]";
            return stream;
         }

      private:

         const LogStateEnumeration *mStateEnum;
         double mCurrentSimTime;
         std::string mActiveMap;
         std::string mLogFile;
         double mAutoRecordKeyframeInterval;
         double mCurrentRecordDuration;
         unsigned long mNumMessages;

   };
}

#endif
