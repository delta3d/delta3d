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
 */
#ifndef DELTA_MACHINEINFO
#define DELTA_MACHINEINFO

#include <dtGame/export.h>
#include <dtCore/uniqueid.h>
#include <dtCore/refptr.h>
#include <osg/Referenced>
#include <string>

namespace dtGame
{
   /**
    * @class MachineInfo
    * A data class representing basic machine information.
    */
   class DT_GAME_EXPORT MachineInfo : public osg::Referenced
   {
      public:

         /// Constructor
         MachineInfo(const std::string& name = "");

         /**
          * Gets the name of a machine info
          * @return The name
          */
         const std::string& GetName() const;

         /**
          * Gets the host name of a machine info
          * @return The host name
          */
         const std::string& GetHostName() const;

         /**
          * Gets the ip address of a machine info
          * @return The ip address
          */
         const std::string& GetIPAddress() const;

         /**
          * Gets the timestamp of the last time this machine was heard from.
          * @return The time stamp
          */
         unsigned long GetTimeStamp() const;

         /**
          * Gets the ping of a machine info
          * @return The ping time, in milliseconds
          */
         unsigned int GetPing() const;

         /**
          * Gets the unique id of a machine info
          * @return The unique id
          * @see dtCore::UniqueId
          */
         const dtCore::UniqueId& GetUniqueId() const;

         /**
          * Sets the name of a machine info
          * @param The new name
          */
         void SetName(const std::string& newName);

         /**
          * Sets the host name of a machine info
          * @param The new host name
          */
         void SetHostName(const std::string& newHostName);

         /**
          * Sets the ip address of a machine info
          * @param The new ip address
          */
         void SetIPAddress(const std::string& newIPAddress);

         /**
          * Sets the time stamp of a machine info
          * @param The new time stamp
          */
         void SetTimeStamp(unsigned long newTimeStamp);

         /**
          * Sets the ping of a machine info
          * @param The new ping, in milliseconds
          */
         void SetPing(unsigned int newPing);

         /**
          * Sets the unique id of a machine info
          * @param The new unique id
          */
         void SetUniqueId(const dtCore::UniqueId& newId);

         MachineInfo& operator=(const MachineInfo& assignFrom);

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
      protected:
         virtual ~MachineInfo();

      private:
         std::string mName;
         dtCore::UniqueId mUniqueId;
         std::string mHostName;
         std::string mIPAddress;
         unsigned long mTimeStamp;
         unsigned int mPing;
   };

   class MachineInfoCompare
   {
      public:
         bool operator()(dtCore::RefPtr<const MachineInfo> first, dtCore::RefPtr<const MachineInfo> second)
         {
            if (!first.valid() && !second.valid())
               return true;

            if (first.valid() != second.valid())
               return false;

            return *first == *second;
         }

         bool operator()(dtCore::RefPtr<MachineInfo> first, dtCore::RefPtr<MachineInfo> second)
         {
            if (!first.valid() && !second.valid())
               return true;

            if (first.valid() != second.valid())
               return false;

            return *first == *second;
         }
   };
}

#endif
