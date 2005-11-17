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

namespace dtGame
{
   float TickMessage::GetDeltaSimTime()
   {
      FloatMessageParameter *mp = static_cast<FloatMessageParameter*>(GetParameter("DeltaSimTime"));
      return mp->GetValue(); 
   }

   float TickMessage::GetDeltaRealTime()
   {
      FloatMessageParameter *mp = static_cast<FloatMessageParameter*>(GetParameter("DeltaRealTime"));
      return mp->GetValue();
   }

   float TickMessage::GetSimTimeScale()
   {
      FloatMessageParameter *mp = static_cast<FloatMessageParameter*>(GetParameter("SimTimeScale"));
      return mp->GetValue();
   }

   long TickMessage::GetSimTimeOfDay()
   {
      LongIntMessageParameter *mp = static_cast<LongIntMessageParameter*>(GetParameter("SimTimeOfDay"));
      return mp->GetValue();
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

   void TickMessage::SetSimTimeOfDay(long newTimeOfDay)
   {
      LongIntMessageParameter *mp = static_cast<LongIntMessageParameter*>(GetParameter("SimTimeOfDay"));
      mp->SetValue(newTimeOfDay);
   }

   const std::string& TimerElapsedMessage::GetTimerName()
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("TimerName"));
      return mp->GetValue();
   }

   long TimerElapsedMessage::GetSimTimeOfDay()
   {
      LongIntMessageParameter *mp = static_cast<LongIntMessageParameter*>(GetParameter("SimTimeOfDay"));
      return mp->GetValue();
   } 

   float TimerElapsedMessage::GetLateTime()
   {
      FloatMessageParameter *mp = static_cast<FloatMessageParameter*> (GetParameter("LateTime"));
      return mp->GetValue();
   }

   void TimerElapsedMessage::SetTimerName(const std::string &name)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("TimerName"));
      mp->SetValue(name);
   }

   void TimerElapsedMessage::SetSimTimeOfDay(long newTime)
   {  
      LongIntMessageParameter *mp = static_cast<LongIntMessageParameter*> (GetParameter("SimTimeOfDay"));
      mp->SetValue(newTime);
   }

   void TimerElapsedMessage::SetLateTime(float newTime)
   {
      FloatMessageParameter *mp = static_cast<FloatMessageParameter*> (GetParameter("LateTime"));
      mp->SetValue(newTime);
   }

   const std::string& MapLoadedMessage::GetLoadedMapName()
   {
      const StringMessageParameter *mp = static_cast<const StringMessageParameter*> (GetParameter("LoadedMapName"));
      return mp->GetValue();
   }

   void MapLoadedMessage::SetLoadedMapName(const std::string &name)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("LoadedMapName"));
      mp->SetValue(name);
   }

   const std::string& NetServerRejectMessage::GetRejectionMessage()
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("RejectionMessage"));
      return mp->GetValue();
   }

   void NetServerRejectMessage::SetRejectionMessage(const std::string &msg)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("RejectionMessage"));
      mp->SetValue(msg);
   }

   const std::string& RestartMessage::GetReason()
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("Reason"));
      return mp->GetValue();
   }

   void RestartMessage::SetReason(const std::string &reason)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("Reason"));
      mp->SetValue(reason);
   }

   const std::string& ServerMessageRejected::GetCause()
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("Cause"));
      return mp->GetValue();
   }

   void ServerMessageRejected::SetCause(const std::string &cause)
   {
      StringMessageParameter *mp = static_cast<StringMessageParameter*> (GetParameter("Cause"));
      mp->SetValue(cause);
   }
}
