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
#include <dtDAL/gameeventmanager.h>
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

//   const std::string& TimerElapsedMessage::GetTimerName() const
//   {
//      const StringMessageParameter *mp = static_cast<const StringMessageParameter*> (GetParameter("TimerName"));
//      return mp->GetValue();
//   }
//
//   //////////////////////////////////////////////////////////////////////////////
//   float TimerElapsedMessage::GetLateTime() const
//   {
//      const FloatMessageParameter *mp = static_cast<const FloatMessageParameter*> (GetParameter("LateTime"));
//      return mp->GetValue();
//   }
//
//   //////////////////////////////////////////////////////////////////////////////
//   void TimerElapsedMessage::SetTimerName(const std::string &name)
//   {
//      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("TimerName"));
//      mp->SetValue(name);
//   }
//
//   //////////////////////////////////////////////////////////////////////////////
//   void TimerElapsedMessage::SetLateTime(float newTime)
//   {
//      FloatMessageParameter *mp = static_cast<FloatMessageParameter*> (GetParameter("LateTime"));
//      mp->SetValue(newTime);
//   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   float TimeChangeMessage::GetTimeScale() const
   {
      return static_cast<const FloatMessageParameter*>(GetParameter("TimeScale"))->GetValue();
   }

   //////////////////////////////////////////////////////////////////////////////
   void TimeChangeMessage::SetTimeScale(float newTimeScale)
   {
      FloatMessageParameter *mp = static_cast<FloatMessageParameter*>(GetParameter("TimeScale"));
      mp->SetValue(newTimeScale);
   }

   //////////////////////////////////////////////////////////////////////////////
   double TimeChangeMessage::GetSimulationTime() const
   {
      return static_cast<const DoubleMessageParameter*>(GetParameter("SimulationTime"))->GetValue();
   }

   //////////////////////////////////////////////////////////////////////////////
   void TimeChangeMessage::SetSimulationTime(double newSimulationTime)
   {
      DoubleMessageParameter *mp = static_cast<DoubleMessageParameter*>(GetParameter("SimulationTime"));
      mp->SetValue(newSimulationTime);
   }

   //////////////////////////////////////////////////////////////////////////////
   double TimeChangeMessage::GetSimulationClockTime() const
   {
      return static_cast<const DoubleMessageParameter*>(GetParameter("SimulationClockTime"))->GetValue();
   }

   //////////////////////////////////////////////////////////////////////////////
   void TimeChangeMessage::SetSimulationClockTime(double newSimClockTime)
   {
      DoubleMessageParameter *mp = static_cast<DoubleMessageParameter*>(GetParameter("SimulationClockTime"));
      mp->SetValue(newSimClockTime);
   }

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
         InsertStringParameterFunc(GroupMessageParameter& param):
            mCount(0),
            mGroupParam(param)
         {
         }

         void operator() (const std::string& str)
         {
            std::string name;
            dtUtil::MakeIndexString(mCount, name);
            mGroupParam.AddParameter(*new StringMessageParameter(name, str));
            ++mCount;
         }

         int mCount;
         GroupMessageParameter& mGroupParam;
   };
   //////////////////////////////////////////////////////////////////////////////
   /// Constructor
   MapMessage::MapMessage() : Message()
   {
      mMapNames = new GroupMessageParameter(PARAM_MAP_NAMES); 
      AddParameter(mMapNames.get());
   }

   //////////////////////////////////////////////////////////////////////////////
   void MapMessage::GetMapNames(std::vector<std::string>& toFill) const
   {  
      toFill.clear();
      toFill.reserve(mMapNames->GetParameterCount());
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

   void GameEventMessage::SetGameEvent(const dtDAL::GameEvent& event)
   {
      GameEventMessageParameter* mp = static_cast<GameEventMessageParameter*>(GetParameter("GameEvent"));
      mp->SetValue(event.GetUniqueId());
   }

   //////////////////////////////////////////////////////////////////////////////
   const dtDAL::GameEvent* GameEventMessage::GetGameEvent() const
   {
      const GameEventMessageParameter* mp = static_cast<const GameEventMessageParameter*>(GetParameter("GameEvent"));
      const dtCore::UniqueId id = mp->GetValue();

      //Need to look up in the event manager for the specified event.
      dtDAL::GameEvent *event = dtDAL::GameEventManager::GetInstance().FindEvent(id);
      if (event == NULL)
         LOG_WARNING("Game event message parameter had an invalid game event id.");
      
      return event;
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   const std::string& NetServerRejectMessage::GetRejectionMessage() const
   {
      const StringMessageParameter *mp = static_cast<const StringMessageParameter*> (GetParameter("RejectionMessage"));
      return mp->GetValue();
   }

   //////////////////////////////////////////////////////////////////////////////
   void NetServerRejectMessage::SetRejectionMessage(const std::string &msg)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("RejectionMessage"));
      mp->SetValue(msg);
   }

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
      AddParameter(new dtGame::UnsignedLongIntMessageParameter("TimeStamp"));
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
      const dtGame::UnsignedLongIntMessageParameter* mp = static_cast<const dtGame::UnsignedLongIntMessageParameter*>(GetParameter("TimeStamp"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void MachineInfoMessage::SetTimeStamp(unsigned long timeStamp)
   {
      dtGame::UnsignedLongIntMessageParameter* mp = static_cast<dtGame::UnsignedLongIntMessageParameter*>(GetParameter("TimeStamp"));
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
}
