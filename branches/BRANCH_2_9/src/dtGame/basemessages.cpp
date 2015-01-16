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

#include <prefix/dtgameprefix.h>
#include <dtGame/basemessages.h>
#include <dtCore/gameeventmanager.h>
#include <dtUtil/stringutils.h>
#include <algorithm>
#include <sstream>

namespace dtGame
{

   DT_IMPLEMENT_MESSAGE_BEGIN(TickMessage)
      DT_ADD_PARAMETER(float, DeltaSimTime)
      DT_ADD_PARAMETER(float, DeltaRealTime)
      DT_ADD_PARAMETER_WITH_DEFAULT(float, SimTimeScale, 1.0f)
      DT_ADD_PARAMETER(double, SimulationTime)
   DT_IMPLEMENT_MESSAGE_END()


   DT_IMPLEMENT_MESSAGE_BEGIN(TimerElapsedMessage)
      DT_ADD_PARAMETER(std::string, TimerName)
      DT_ADD_PARAMETER(float, LateTime)
   DT_IMPLEMENT_MESSAGE_END()


   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   DT_IMPLEMENT_MESSAGE_BEGIN(TimeChangeMessage)
      DT_ADD_PARAMETER_WITH_DEFAULT(float, TimeScale, 1.0f)
      DT_ADD_PARAMETER(double, SimulationTime)
      DT_ADD_PARAMETER(double, SimulationClockTime)
   DT_IMPLEMENT_MESSAGE_END()


   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   const dtUtil::RefString MapMessage::PARAM_MAP_NAMES("MapNames");

   class GetStringParameterFunc
   {
      public:
         GetStringParameterFunc(std::vector<std::string>& toFill):
            mVec(toFill)
         {
         }
         
         void operator() (const dtCore::RefPtr<MessageParameter>& parameter)
         {
            mVec.push_back(parameter->ToString());
         }
      
         std::vector<std::string>& mVec;
   };
   
   class InsertStringParameterFunc
   {
      public:
         InsertStringParameterFunc(ArrayMessageParameter& param):
            mCount(0),
            mArrayParam(param)
         {
         }

         void operator() (const std::string& str)
         {
            static const dtUtil::RefString name("map");
            mArrayParam.AddParameter(*new StringMessageParameter(name, str));
            ++mCount;
         }

         int mCount;
         ArrayMessageParameter& mArrayParam;
   };
   //////////////////////////////////////////////////////////////////////////////
   /// Constructor
   MapMessage::MapMessage() : Message()
   {
      mMapNames = new ArrayMessageParameter(PARAM_MAP_NAMES);
      AddParameter(mMapNames.get());
   }

   //////////////////////////////////////////////////////////////////////////////
   void MapMessage::GetMapNames(std::vector<std::string>& toFill) const
   {  
      toFill.clear();
      toFill.reserve(mMapNames->GetSize());
      GetStringParameterFunc parameterFunc(toFill);
      mMapNames->ForEachParameter(parameterFunc);
   }

   //////////////////////////////////////////////////////////////////////////////
   void MapMessage::SetMapNames(const std::vector<std::string>& nameVec)
   {
      InsertStringParameterFunc parameterFunc(*mMapNames);
      std::for_each(nameVec.begin(), nameVec.end(), parameterFunc);
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   void GameEventMessage::SetGameEvent(const dtCore::GameEvent& event)
   {
      GameEventMessageParameter* mp = static_cast<GameEventMessageParameter*>(GetParameter("GameEvent"));
      mp->SetValue(event.GetUniqueId());
   }

   //////////////////////////////////////////////////////////////////////////////
   const dtCore::GameEvent* GameEventMessage::GetGameEvent() const
   {
      const GameEventMessageParameter* mp = static_cast<const GameEventMessageParameter*>(GetParameter("GameEvent"));
      const dtCore::UniqueId id = mp->GetValue();

      //Need to look up in the event manager for the specified event.
      dtCore::GameEvent *event = dtCore::GameEventManager::GetInstance().FindEvent(id);
      if (event == NULL)
         LOG_WARNING("Game event message parameter had an invalid game event id.");
      
      return event;
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_MESSAGE_BEGIN(NetServerRejectMessage)
      DT_ADD_PARAMETER(std::string, RejectionMessage)
   DT_IMPLEMENT_MESSAGE_END()

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   const std::string& RestartMessage::GetReason() const
   {
      const StringMessageParameter *mp = static_cast<const StringMessageParameter*> (GetParameter("Reason"));
      return mp->GetValue();
   }

   //////////////////////////////////////////////////////////////////////////////
   void RestartMessage::SetReason(const std::string &reason)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("Reason"));
      mp->SetValue(reason);
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   const std::string& ServerMessageRejected::GetCause() const
   {
      const StringMessageParameter *mp = static_cast<const StringMessageParameter*> (GetParameter("Cause"));
      return mp->GetValue();
   }

   //////////////////////////////////////////////////////////////////////////////
   void ServerMessageRejected::SetCause(const std::string &cause)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("Cause"));
      mp->SetValue(cause);
   }

   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////
   // Constructor
   MachineInfoMessage::MachineInfoMessage()
   {
      AddParameter(new dtGame::StringMessageParameter("Name"));
      AddParameter(new dtGame::StringMessageParameter("UniqueId"));
      AddParameter(new dtGame::StringMessageParameter("HostName"));
      AddParameter(new dtGame::StringMessageParameter("IPAddress"));
      AddParameter(new dtGame::UnsignedIntMessageParameter("TimeStamp"));
      AddParameter(new dtGame::UnsignedIntMessageParameter("Ping"));
   }

   ////////////////////////////////////////////////////////////////////
   const std::string& MachineInfoMessage::GetMachineInfoName() const
   {
      const dtGame::StringMessageParameter* mp = static_cast<const dtGame::StringMessageParameter*>(GetParameter("Name"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void MachineInfoMessage::SetMachineInfoName(const std::string& name)
   {
      dtGame::StringMessageParameter* mp = static_cast<dtGame::StringMessageParameter*>(GetParameter("Name"));
      mp->SetValue(name);
   }

   ////////////////////////////////////////////////////////////////////
   const std::string& MachineInfoMessage::GetUniqueId() const
   {
      const dtGame::StringMessageParameter* mp = static_cast<const dtGame::StringMessageParameter*>(GetParameter("UniqueId"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void MachineInfoMessage::SetUniqueId(const std::string& strId)
   {
      dtGame::StringMessageParameter* mp = static_cast<dtGame::StringMessageParameter*>(GetParameter("UniqueId"));
      mp->SetValue(strId);
   }

   ////////////////////////////////////////////////////////////////////
   const std::string& MachineInfoMessage::GetHostName() const
   {
      const dtGame::StringMessageParameter* mp = static_cast<const dtGame::StringMessageParameter*>(GetParameter("HostName"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void MachineInfoMessage::SetHostName(const std::string& hostname)
   {
      dtGame::StringMessageParameter* mp = static_cast<dtGame::StringMessageParameter*>(GetParameter("HostName"));
      mp->SetValue(hostname);
   }

   ////////////////////////////////////////////////////////////////////
   const std::string& MachineInfoMessage::GetIPAddress() const
   {
      const dtGame::StringMessageParameter* mp = static_cast<const dtGame::StringMessageParameter*>(GetParameter("IPAddress"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void MachineInfoMessage::SetIPAddress(const std::string& ipAddress)
   {
      dtGame::StringMessageParameter* mp = static_cast<dtGame::StringMessageParameter*>(GetParameter("IPAddress"));
      mp->SetValue(ipAddress);
   }

   ////////////////////////////////////////////////////////////////////
   unsigned long MachineInfoMessage::GetTimeStamp() const
   {
      const dtGame::UnsignedIntMessageParameter* mp = static_cast<const dtGame::UnsignedIntMessageParameter*>(GetParameter("TimeStamp"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void MachineInfoMessage::SetTimeStamp(unsigned long timeStamp)
   {
      dtGame::UnsignedIntMessageParameter* mp = static_cast<dtGame::UnsignedIntMessageParameter*>(GetParameter("TimeStamp"));
      mp->SetValue(timeStamp);
   }

   ////////////////////////////////////////////////////////////////////
   unsigned int MachineInfoMessage::GetPing() const
   {
      const dtGame::UnsignedIntMessageParameter* mp = static_cast<const dtGame::UnsignedIntMessageParameter*>(GetParameter("Ping"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void MachineInfoMessage::SetPing(unsigned int ping)
   {
      dtGame::UnsignedIntMessageParameter* mp = static_cast<dtGame::UnsignedIntMessageParameter*>(GetParameter("Ping"));
      mp->SetValue(ping);
   }

   ////////////////////////////////////////////////////////////////////
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

   ////////////////////////////////////////////////////////////////////
   void MachineInfoMessage::SetMachineInfo(const dtGame::MachineInfo& machineInfo)
   {
      SetMachineInfoName(machineInfo.GetName());
      SetUniqueId(machineInfo.GetUniqueId().ToString());
      SetHostName(machineInfo.GetHostName());
      SetIPAddress(machineInfo.GetIPAddress());
      SetTimeStamp(machineInfo.GetTimeStamp());
      SetPing(machineInfo.GetPing());
   }

   ////////////////////////////////////////////////////////////////////
   ServerFrameSyncMessage::ServerFrameSyncMessage()
   {
      AddParameter(new dtGame::DoubleMessageParameter("ServerSimTimeSinceStartup"));
      AddParameter(new dtGame::FloatMessageParameter("ServerTimeScale"));
   }

   ////////////////////////////////////////////////////////////////////
   double ServerFrameSyncMessage::GetServerSimTimeSinceStartup() const
   {
      const dtGame::DoubleMessageParameter* mp = static_cast<const dtGame::DoubleMessageParameter*>(GetParameter("ServerSimTimeSinceStartup"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void ServerFrameSyncMessage::SetServerSimTimeSinceStartup(double newValue)
   {
      dtGame::DoubleMessageParameter* mp = static_cast<dtGame::DoubleMessageParameter*>(GetParameter("ServerSimTimeSinceStartup"));
      mp->SetValue(newValue);
   }

   ////////////////////////////////////////////////////////////////////
   float ServerFrameSyncMessage::GetServerTimeScale() const
   {
      const dtGame::FloatMessageParameter* mp = static_cast<const dtGame::FloatMessageParameter*>(GetParameter("ServerTimeScale"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void ServerFrameSyncMessage::SetServerTimeScale(float newValue)
   {
      dtGame::FloatMessageParameter* mp = static_cast<dtGame::FloatMessageParameter*>(GetParameter("ServerTimeScale"));
      mp->SetValue(newValue);
   }

   ////////////////////////////////////////////////////////////////////
   ServerSyncControlMessage::ServerSyncControlMessage()
   {
      AddParameter(new dtGame::BooleanMessageParameter("SyncEnabled", false));
      AddParameter(new dtGame::UnsignedIntMessageParameter("NumSyncsPerSecond", 60));
      AddParameter(new dtGame::FloatMessageParameter("MaxWaitTime", 4.0f));
   }


   ////////////////////////////////////////////////////////////////////
   bool ServerSyncControlMessage::GetSyncEnabled() const
   {
      const dtGame::BooleanMessageParameter* mp = static_cast<const dtGame::BooleanMessageParameter*>(GetParameter("SyncEnabled"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void ServerSyncControlMessage::SetSyncEnabled(bool newValue)
   {
      dtGame::BooleanMessageParameter* mp = static_cast<dtGame::BooleanMessageParameter*>(GetParameter("SyncEnabled"));
      mp->SetValue(newValue);
   }

   ////////////////////////////////////////////////////////////////////
   unsigned int ServerSyncControlMessage::GetNumSyncsPerSecond() const
   {
      const dtGame::UnsignedIntMessageParameter* mp = static_cast<const dtGame::UnsignedIntMessageParameter*>(GetParameter("NumSyncsPerSecond"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void ServerSyncControlMessage::SetNumSyncsPerSecond(unsigned int newValue)
   {
      dtGame::UnsignedIntMessageParameter* mp = static_cast<dtGame::UnsignedIntMessageParameter*>(GetParameter("NumSyncsPerSecond"));
      mp->SetValue(newValue);
   }

   ////////////////////////////////////////////////////////////////////
   float ServerSyncControlMessage::GetMaxWaitTime() const
   {
      const dtGame::FloatMessageParameter* mp = static_cast<const dtGame::FloatMessageParameter*>(GetParameter("MaxWaitTime"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void ServerSyncControlMessage::SetMaxWaitTime(float newValue)
   {
      dtGame::FloatMessageParameter* mp = static_cast<dtGame::FloatMessageParameter*>(GetParameter("MaxWaitTime"));
      mp->SetValue(newValue);
   }


   // Destructors
   ServerSyncControlMessage::~ServerSyncControlMessage() { }
   ServerFrameSyncMessage::~ServerFrameSyncMessage() { }
   MachineInfoMessage::~MachineInfoMessage() { }
   ServerMessageRejected::~ServerMessageRejected() {}
   MapMessage::~MapMessage() { }
   GameEventMessage::~GameEventMessage() { }
   RestartMessage::~RestartMessage() { }

}
