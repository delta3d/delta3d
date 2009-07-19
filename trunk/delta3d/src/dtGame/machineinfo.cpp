/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, Alion Science and Technology.
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
 * William E. Johnson II, David Guthrie
 */

#include <dtGame/machineinfo.h>

namespace dtGame
{
   ////////////////////////////////////////////////
   MachineInfo::MachineInfo(const std::string& name) : mName(name), mTimeStamp(0) { }

   ////////////////////////////////////////////////
   MachineInfo::~MachineInfo() { }

   ////////////////////////////////////////////////
   const std::string& MachineInfo::GetName() const { return mName; }

   ////////////////////////////////////////////////
   const std::string& MachineInfo::GetHostName() const { return mHostName; }

   ////////////////////////////////////////////////
   const std::string& MachineInfo::GetIPAddress() const { return mIPAddress; }

   ////////////////////////////////////////////////
   unsigned long MachineInfo::GetTimeStamp() const { return mTimeStamp; }

   ////////////////////////////////////////////////
   unsigned int MachineInfo::GetPing() const { return mPing; }

   ////////////////////////////////////////////////
   const dtCore::UniqueId& MachineInfo::GetUniqueId() const { return mUniqueId; }

   ////////////////////////////////////////////////
   void MachineInfo::SetName(const std::string& newName) { mName = newName; }

   ////////////////////////////////////////////////
   void MachineInfo::SetHostName(const std::string& newHostName) { mHostName = newHostName; }

   ////////////////////////////////////////////////
   void MachineInfo::SetIPAddress(const std::string& newIPAddress) { mIPAddress = newIPAddress; }

   ////////////////////////////////////////////////
   void MachineInfo::SetTimeStamp(unsigned long newTimeStamp) { mTimeStamp = newTimeStamp; }

   ////////////////////////////////////////////////
   void MachineInfo::SetPing(unsigned int newPing) { mPing = newPing; }

   ////////////////////////////////////////////////
   void MachineInfo::SetUniqueId(const dtCore::UniqueId &newId) { mUniqueId = newId; }

   ////////////////////////////////////////////////
   MachineInfo& MachineInfo::operator=(const MachineInfo& assignFrom)
   {
      mName = assignFrom.mName;
      mUniqueId = assignFrom.mUniqueId;
      mHostName = assignFrom.mHostName;
      mIPAddress = assignFrom.mIPAddress;
      mTimeStamp = assignFrom.mTimeStamp;
      mPing = assignFrom.mPing;
      return *this;
   }

}
