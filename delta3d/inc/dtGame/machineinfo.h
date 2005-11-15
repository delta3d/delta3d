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
#ifndef DELTA_MACHINEINFO
#define DELTA_MACHINEINFO

#include "dtGame/export.h"
#include <dtCore/uniqueid.h>
#include <string>

namespace dtGame
{
   /**
    * @class A data class representing basic machine information.
    */
   class DT_GAME_EXPORT MachineInfo : public osg::Referenced
   {
      public:

         /// Constructor
         MachineInfo(const std::string &name = "") : mName(name), mTimeStamp(0) { }

         /**
          * Gets the name of a machine info
          * @return The name
          */
         const std::string& GetName() const      { return mName; }

         /**
          * Gets the host name of a machine info
          * @return The host name
          */
         const std::string& GetHostName() const  { return mHostName; }

         /**
          * Gets the ip address of a machine info
          * @return The ip address
          */
         const std::string& GetIPAddress() const { return mIPAddress; }

         /**
          * Gets the timestamp of the last time this machine was heard from.
          * @return The time stamp
          */
         unsigned long GetTimeStamp() const { return mTimeStamp; }

         /**
          * Gets the ping of a machine info
          * @return The ping time, in milliseconds
          */
         unsigned int GetPing() const { return mPing; }

         /**
          * Gets the unique id of a machine info
          * @return The unique id
          * @see dtCore::UniqueId
          */
         const dtCore::UniqueId& GetUniqueId() const { return mUniqueId; }

         /**
          * Sets the name of a machine info
          * @param The new name
          */
         void SetName(const std::string &newName) { mName = newName; }

         /**
          * Sets the host name of a machine info
          * @param The new host name
          */
         void SetHostName(const std::string &newHostName) { mHostName = newHostName; }

         /**
          * Sets the ip address of a machine info
          * @param The new ip address
          */
         void SetIPAddress(const std::string &newIPAddress) { mIPAddress = newIPAddress; }
         
         /**
          * Sets the time stamp of a machine info
          * @param The new time stamp
          */
         void SetTimeStamp(unsigned long newTimeStamp) { mTimeStamp = newTimeStamp; }
         
         /**
          * Sets the ping of a machine info
          * @param The new ping, in milliseconds
          */
         void SetPing(unsigned int newPing) { mPing = newPing; }

         /**
          * Sets the unique id of a machine info
          * @param The new unique id
          */
         void SetUniqueId(dtCore::UniqueId &newId) { mUniqueId = newId; }

         MachineInfo& operator=(const MachineInfo& assignFrom) 
         {
            mName = assignFrom.mName;
            mUniqueId = assignFrom.mUniqueId;
            mHostName = assignFrom.mHostName;
            mIPAddress = assignFrom.mIPAddress;
            mTimeStamp = assignFrom.mTimeStamp;
            mPing = assignFrom.mPing;
            return *this;
         }
         
         bool operator==(const MachineInfo& compareTo) const
         {
            return mUniqueId == compareTo.mUniqueId;
         }

         bool operator!=(const MachineInfo& compareTo) const
         {
            return mUniqueId != compareTo.mUniqueId;
         }

         bool operator<(const MachineInfo& compareTo) const
         {
            return mUniqueId < compareTo.mUniqueId;
         }

         bool operator>(const MachineInfo& compareTo) const
         {
            return mUniqueId > compareTo.mUniqueId;
         }
      private:
         ~MachineInfo() {}
         
         std::string mName;
         dtCore::UniqueId mUniqueId;
         std::string mHostName;
         std::string mIPAddress;
         unsigned long mTimeStamp;
         unsigned int mPing;
   };
}

#endif
