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

#include "dtGame/basemessages.h"
#include "dtDAL/gameeventmanager.h"

namespace dtGame
{
   float TickMessage::GetDeltaSimTime() const
   {
      const FloatMessageParameter *mp = static_cast<const FloatMessageParameter*>(GetParameter("DeltaSimTime"));
      return mp->GetValue();
   }

   float TickMessage::GetDeltaRealTime() const
   {
      const FloatMessageParameter *mp = static_cast<const FloatMessageParameter*>(GetParameter("DeltaRealTime"));
      return mp->GetValue();
   }

   float TickMessage::GetSimTimeScale() const
   {
      const FloatMessageParameter *mp = static_cast<const FloatMessageParameter*>(GetParameter("SimTimeScale"));
      return mp->GetValue();
   }

   double TickMessage::GetSimulationTime() const
   {
      const DoubleMessageParameter *mp = static_cast<const DoubleMessageParameter*>(GetParameter("SimulationTime"));
      return mp->GetValue();
   }

   void TickMessage::SetSimulationTime(double newSimulationTime)
   {
      DoubleMessageParameter *mp = static_cast<DoubleMessageParameter*>(GetParameter("SimulationTime"));
      mp->SetValue(newSimulationTime);
   }

   void TickMessage::SetDeltaSimTime(float newTime)
   {
      FloatMessageParameter *mp = static_cast<FloatMessageParameter*>(GetParameter("DeltaSimTime"));
      mp->SetValue(newTime);
   }

   void TickMessage::SetDeltaRealTime(float newTime)
   {
      FloatMessageParameter *mp = static_cast<FloatMessageParameter*>(GetParameter("DeltaRealTime"));
      mp->SetValue(newTime);
   }

   void TickMessage::SetSimTimeScale(float newScale)
   {
      FloatMessageParameter *mp = static_cast<FloatMessageParameter*>(GetParameter("SimTimeScale"));
      mp->SetValue(newScale);
   }

   //////////////////////////////////////////////////////////////////////////////

   const std::string& TimerElapsedMessage::GetTimerName() const
   {
      const StringMessageParameter *mp = static_cast<const StringMessageParameter*> (GetParameter("TimerName"));
      return mp->GetValue();
   }

   float TimerElapsedMessage::GetLateTime() const
   {
      const FloatMessageParameter *mp = static_cast<const FloatMessageParameter*> (GetParameter("LateTime"));
      return mp->GetValue();
   }

   void TimerElapsedMessage::SetTimerName(const std::string &name)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("TimerName"));
      mp->SetValue(name);
   }

   void TimerElapsedMessage::SetLateTime(float newTime)
   {
      FloatMessageParameter *mp = static_cast<FloatMessageParameter*> (GetParameter("LateTime"));
      mp->SetValue(newTime);
   }

   //////////////////////////////////////////////////////////////////////////////
   float TimeChangeMessage::GetTimeScale() const
   {
      return static_cast<const FloatMessageParameter*>(GetParameter("TimeScale"))->GetValue();
   }

   void TimeChangeMessage::SetTimeScale(float newTimeScale)
   {
      FloatMessageParameter *mp = static_cast<FloatMessageParameter*>(GetParameter("TimeScale"));
      mp->SetValue(newTimeScale);
   }

   double TimeChangeMessage::GetSimulationTime() const
   {
      return static_cast<const DoubleMessageParameter*>(GetParameter("SimulationTime"))->GetValue();
   }

   void TimeChangeMessage::SetSimulationTime(double newSimulationTime)
   {
      DoubleMessageParameter *mp = static_cast<DoubleMessageParameter*>(GetParameter("SimulationTime"));
      mp->SetValue(newSimulationTime);
   }

   double TimeChangeMessage::GetSimulationClockTime() const
   {
      return static_cast<const DoubleMessageParameter*>(GetParameter("SimulationClockTime"))->GetValue();
   }

   void TimeChangeMessage::SetSimulationClockTime(double newSimClockTime)
   {
      DoubleMessageParameter *mp = static_cast<DoubleMessageParameter*>(GetParameter("SimulationClockTime"));
      mp->SetValue(newSimClockTime);
   }

   //////////////////////////////////////////////////////////////////////////////

   const std::string& MapLoadedMessage::GetLoadedMapName() const
   {
      const StringMessageParameter *mp = static_cast<const StringMessageParameter*> (GetParameter("LoadedMapName"));
      return mp->GetValue();
   }

   void MapLoadedMessage::SetLoadedMapName(const std::string &name)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("LoadedMapName"));
      mp->SetValue(name);
   }

   //////////////////////////////////////////////////////////////////////////////

   void GameEventMessage::SetGameEvent(dtDAL::GameEvent &event)
   {
      GameEventMessageParameter *mp = static_cast<GameEventMessageParameter*>(GetParameter("GameEvent"));
      mp->SetValue(event.GetUniqueId());
   }

   const dtDAL::GameEvent *GameEventMessage::GetGameEvent() const
   {
      const GameEventMessageParameter *mp = static_cast<const GameEventMessageParameter*>(GetParameter("GameEvent"));
      const dtCore::UniqueId id = mp->GetValue();

      //Need to look up in the event manager for the specified event.
      dtDAL::GameEvent *event = dtDAL::GameEventManager::GetInstance().FindEvent(id);
      if (event == NULL)
         LOG_WARNING("Game event message parameter had an invalid game event id.");
      return event;
   }

   //////////////////////////////////////////////////////////////////////////////

   const std::string& NetServerRejectMessage::GetRejectionMessage() const
   {
      const StringMessageParameter *mp = static_cast<const StringMessageParameter*> (GetParameter("RejectionMessage"));
      return mp->GetValue();
   }

   void NetServerRejectMessage::SetRejectionMessage(const std::string &msg)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("RejectionMessage"));
      mp->SetValue(msg);
   }

   //////////////////////////////////////////////////////////////////////////////

   const std::string& RestartMessage::GetReason() const
   {
      const StringMessageParameter *mp = static_cast<const StringMessageParameter*> (GetParameter("Reason"));
      return mp->GetValue();
   }

   void RestartMessage::SetReason(const std::string &reason)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("Reason"));
      mp->SetValue(reason);
   }

   //////////////////////////////////////////////////////////////////////////////

   const std::string& ServerMessageRejected::GetCause() const
   {
      const StringMessageParameter *mp = static_cast<const StringMessageParameter*> (GetParameter("Cause"));
      return mp->GetValue();
   }

   void ServerMessageRejected::SetCause(const std::string &cause)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("Cause"));
      mp->SetValue(cause);
   }
}
