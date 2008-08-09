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
* @author Pjotr van Amerongen
*/
#ifndef DELTA_MACHINEINFOMESSAGE
#define DELTA_MACHINEINFOMESSAGE

#include <string>
#include <dtNetGM/export.h>
#include <dtCore/refptr.h>
#include <dtGame/message.h>
#include <dtGame/messageparameter.h> 

namespace dtNetGM
{
    /**
    * @class MachineInfoMessage
    * @brief A MachineInfoMessage contains a dtGame::MachineInfo to be used with different messages 
    * to send information about a GameManager accross the network
    * @see dtGame::MachineInfo
    */
    class DT_NETGM_EXPORT MachineInfoMessage : public dtGame::Message
    {
    public:

        // Constructor
        MachineInfoMessage() : dtGame::Message()
        {
            AddParameter(new dtGame::StringMessageParameter("Name"));
            AddParameter(new dtGame::StringMessageParameter("UniqueId"));
            AddParameter(new dtGame::StringMessageParameter("HostName"));
            AddParameter(new dtGame::StringMessageParameter("IPAddress"));
            AddParameter(new dtGame::LongIntMessageParameter("TimeStamp"));
            AddParameter(new dtGame::UnsignedIntMessageParameter("Ping"));
        }

        /**
        * Gets the Name from the contained MachineInfo
        * @return The name
        */
        const std::string& GetMachineInfoName() const;

        /**
        * Sets the name associated with the MachineInfo
        * @param The new name
        */
        void SetMachineInfoName(const std::string &name);

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
        void SetHostName(const std::string &hostname);

        /**
        * Gets the IpAddress from the contained MachineInfo
        * @return The ipaddress
        */
        const std::string& GetIPAddress() const;

        /**
        * Sets the IpAddress associated with the MachineInfo
        * @param The new ipaddress
        */
        void SetIPAddress(const std::string &ipAddress);

        /**
        * Gets the TimeStamp from the contained MachineInfo
        * @return The timestamp
        */
        const unsigned long& GetTimeStamp() const;

        /**
        * Sets the TimeStamp associated with the MachineInfo
        * @param The new timestamp
        */
        void SetTimeStamp(const unsigned long &timeStamp);

        /**
        * Gets the Ping from the contained MachineInfo
        * @return The ping
        */
        const unsigned int& GetPing() const;

        /**
        * Sets the Ping associated with the MachineInfo
        * @param The new ping
        */
        void SetPing(const unsigned int &ping);

        /**
        * Gets the MachineInfo from the message
        * @return The machineinfo
        */
        dtCore::RefPtr<dtGame::MachineInfo> GetMachineInfo() const;

        /**
        * Sets the MachineInfo
        * @param The new machineinfo
        */
        void SetMachineInfo(const dtGame::MachineInfo &machineInfo);

    protected:
        /// Destructor
        virtual ~MachineInfoMessage() { }
    };
}

#endif // DELTA_MACHINEINFOMESSAGE
