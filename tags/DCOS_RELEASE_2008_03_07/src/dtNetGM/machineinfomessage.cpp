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

#include <dtNetGM/machineinfomessage.h>
#include <dtGame/message.h>
#include <dtGame/machineinfo.h>
#include <dtGame/messageparameter.h>

namespace dtNetGM
{
    const std::string& MachineInfoMessage::GetMachineInfoName() const
    {
        const dtGame::StringMessageParameter *mp = static_cast<const dtGame::StringMessageParameter*> (GetParameter("Name"));
        return mp->GetValue();
    }

    void MachineInfoMessage::SetMachineInfoName(const std::string &name)
    {
        dtGame::StringMessageParameter *mp = static_cast<dtGame::StringMessageParameter*> (GetParameter("Name"));
        mp->SetValue(name);
    }

    const std::string& MachineInfoMessage::GetUniqueId() const
    {
        const dtGame::StringMessageParameter *mp = static_cast<const dtGame::StringMessageParameter*> (GetParameter("UniqueId"));
        return mp->GetValue();
    }

    void MachineInfoMessage::SetUniqueId(const std::string& strId)
    {
        dtGame::StringMessageParameter *mp = static_cast<dtGame::StringMessageParameter*> (GetParameter("UniqueId"));
        mp->SetValue(strId);
    }

    const std::string& MachineInfoMessage::GetHostName() const
    {
        const dtGame::StringMessageParameter *mp = static_cast<const dtGame::StringMessageParameter*> (GetParameter("HostName"));
        return mp->GetValue();
    }

    void MachineInfoMessage::SetHostName(const std::string &hostname)
    {
        dtGame::StringMessageParameter *mp = static_cast<dtGame::StringMessageParameter*> (GetParameter("HostName"));
        mp->SetValue(hostname);
    }

    const std::string& MachineInfoMessage::GetIPAddress() const
    {
        const dtGame::StringMessageParameter *mp = static_cast<const dtGame::StringMessageParameter*> (GetParameter("IPAddress"));
        return mp->GetValue();
    }

    void MachineInfoMessage::SetIPAddress(const std::string &ipAddress)
    {
        dtGame::StringMessageParameter *mp = static_cast<dtGame::StringMessageParameter*> (GetParameter("IPAddress"));
        mp->SetValue(ipAddress);
    }

    const unsigned long& MachineInfoMessage::GetTimeStamp() const
    {
        const dtGame::UnsignedLongIntMessageParameter *mp = static_cast<const dtGame::UnsignedLongIntMessageParameter*> (GetParameter("TimeStamp"));
        return mp->GetValue();
    }

    void MachineInfoMessage::SetTimeStamp(const unsigned long &timeStamp)
    {
        dtGame::UnsignedLongIntMessageParameter *mp = static_cast<dtGame::UnsignedLongIntMessageParameter*> (GetParameter("TimeStamp"));
        mp->SetValue(timeStamp);
    }

    const unsigned int& MachineInfoMessage::GetPing() const
    {
        const dtGame::UnsignedIntMessageParameter *mp = static_cast<const dtGame::UnsignedIntMessageParameter*> (GetParameter("Ping"));
        return mp->GetValue();
    }

    void MachineInfoMessage::SetPing(const unsigned int &ping)
    {
        dtGame::UnsignedIntMessageParameter *mp = static_cast<dtGame::UnsignedIntMessageParameter*> (GetParameter("Ping"));
        mp->SetValue(ping);
    }

    dtCore::RefPtr<dtGame::MachineInfo> MachineInfoMessage::GetMachineInfo() const
    {
        dtCore::RefPtr<dtGame::MachineInfo> machineInfo = new dtGame::MachineInfo();
        machineInfo->SetName(GetMachineInfoName());
        machineInfo->SetUniqueId(dtCore::UniqueId(GetUniqueId()));
        machineInfo->SetHostName(GetHostName());
        machineInfo->SetIPAddress(GetIPAddress());
        machineInfo->SetTimeStamp(GetTimeStamp());
        machineInfo->SetPing(GetPing());

        return machineInfo;
    }

    void MachineInfoMessage::SetMachineInfo(const dtGame::MachineInfo &machineInfo)
    {
        SetMachineInfoName(machineInfo.GetName());
        SetUniqueId(machineInfo.GetUniqueId().ToString());
        SetHostName(machineInfo.GetHostName());
        SetIPAddress(machineInfo.GetIPAddress());
        SetTimeStamp(machineInfo.GetTimeStamp());
        SetPing(machineInfo.GetPing());
    }
}

//////////////////////////////////////////////////////////////////////////////