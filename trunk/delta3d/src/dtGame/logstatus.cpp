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
 * Curtiss Murphy
 */
#include <prefix/dtgameprefix.h>
#include <dtGame/logstatus.h>

namespace dtGame
{

   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(LogStateEnumeration);
   const LogStateEnumeration LogStateEnumeration::LOGGER_STATE_PLAYBACK("LOGGER_STATE_PLAYBACK");
   const LogStateEnumeration LogStateEnumeration::LOGGER_STATE_RECORD("LOGGER_STATE_RECORD");
   const LogStateEnumeration LogStateEnumeration::LOGGER_STATE_IDLE("LOGGER_STATE_IDLE");

   /////////////////////////////////////////////////////////////
   LogStatus::LogStatus()
      : mStateEnum(&LogStateEnumeration::LOGGER_STATE_IDLE)
      , mCurrentSimTime(0)
      , mAutoRecordKeyframeInterval(0)
      , mCurrentRecordDuration(0)
      , mNumMessages(0)
   {
   }

   /////////////////////////////////////////////////////////////
   void LogStatus::SetActiveMaps(const NameVector& newActiveMaps)
   {
      mActiveMaps = newActiveMaps;
   }

   /////////////////////////////////////////////////////////////
   std::ostream& operator<<(std::ostream &stream,const LogStatus &me)
   {
      stream << "LogStatus: State[" << me.mStateEnum->GetName() <<
         "], SimTime[" << me.mCurrentSimTime << "], Map[" << me.mActiveMaps[0] <<
            "], LogFile[" << me.mLogFile << "], #Messagse[" << me.mNumMessages << "]";
      return stream;
   }

} // namespace dtGame
