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
#ifndef DELTA_LOGKEYFRAME
#define DELTA_LOGKEYFRAME

#include "dtGame/export.h"
#include <dtCore/uniqueid.h>
#include <string>

namespace dtGame
{
   /**
    * @class LogKeyframe 
    * A simple data class for basic Log Keyframe information. This class has 
    * a copy constructor plus ==, !=, =, <, and > operators.
    */
   class DT_GAME_EXPORT LogKeyframe
   {
      public:

         virtual ~LogKeyframe() {}

         // Constructor
         LogKeyframe() : mSimTimeStamp(0), mLogFileOffset(0) { }

         // Copy Constructor
         LogKeyframe(const LogKeyframe& toCopy) 
         { 
            mName = toCopy.mName;
            mDescription = toCopy.mDescription;
            mUniqueId = toCopy.mUniqueId;
            mSimTimeStamp = toCopy.mSimTimeStamp;
            mActiveMap = toCopy.mActiveMap;
            mLogFileOffset = toCopy.mLogFileOffset;
         }

         /**
          * Gets the name 
          * @return The name
          */
         const std::string& GetName() const { return mName; }

         /**
          * Sets the name
          * @param The new name
          */
         void SetName(const std::string &newName) { mName = newName; }

         /**
          * Gets the description
          * @return The description
          */
         const std::string& GetDescription() const { return mDescription; }

         /**
          * Sets the description
          * @param The new description
          */
         void SetDescription(const std::string &newDescription) { mDescription = newDescription; }

         /**
          * Gets the simulation timestamp (as opposed to real time)
          * @return The simulation based time stamp (as opposed to real time)
          */
         double GetSimTimeStamp() const { return mSimTimeStamp; }

         /**
          * Sets the sim time stamp (as opposed to real time)
          * @param The new simulation time stamp (as opposed to real time)
          */
         void SetSimTimeStamp(double newSimTimeStamp) { mSimTimeStamp = newSimTimeStamp; }
         
         /**
          * Gets the unique id
          * @return The unique id
          * @see dtCore::UniqueId
          */
         const dtCore::UniqueId& GetUniqueId() const { return mUniqueId; }

         /**
          * Sets the unique id
          * @param The new unique id
          */
         void SetUniqueId(const dtCore::UniqueId &newId) { mUniqueId = newId; }

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
          * Gets the file offset used by the server.  Note that this value is 
          * only relevant to the server logger component.  Anyone else doesn't 
          * have access to the relevant file.  This is not part of the message params.
          * @return The server's file offset for the keyframe
          */
         long GetLogFileOffset() const { return mLogFileOffset; }

         /**
          * Sets the file offset used by the server.  Note that this value is 
          * only relevant to the server logger component.  Anyone else doesn't 
          * have access to the relevant file.  Should only be called by the server.
          * This is not part of the message params.
          * @param The new server file offset value
          */
         void SetLogFileOffset(long newLogFileOffset) { mLogFileOffset = newLogFileOffset; }
         


         bool operator==(const LogKeyframe& compareTo) const
         {
            return mUniqueId == compareTo.mUniqueId;
         }

         bool operator!=(const LogKeyframe& compareTo) const
         {
            return mUniqueId != compareTo.mUniqueId;
         }

         bool operator<(const LogKeyframe& compareTo) const
         {
            return mUniqueId < compareTo.mUniqueId;
         }

         bool operator>(const LogKeyframe& compareTo) const
         {
            return mUniqueId > compareTo.mUniqueId;
         }

         LogKeyframe& operator=(const LogKeyframe& assignFrom) 
         {
            if (&assignFrom == this)
               return *this;

            mName = assignFrom.mName;
            mDescription = assignFrom.mDescription;
            mUniqueId = assignFrom.mUniqueId;
            mSimTimeStamp = assignFrom.mSimTimeStamp;
            mActiveMap = assignFrom.mActiveMap;
            mLogFileOffset = assignFrom.mLogFileOffset;
            return *this;
         }
         
      private:
         
         std::string mName;
         std::string mDescription;
         dtCore::UniqueId mUniqueId;
         double mSimTimeStamp;
         std::string mActiveMap;
         long mLogFileOffset;

   };
}

#endif
