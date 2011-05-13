/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2010, MOVES Institute
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
 * Erik Johnson
 */
#include <prefix/dtutilprefix.h>
#include <dtUtil/logobserverconsole.h>
#include <iostream>
#include <iomanip>

////////////////////////////////////////////////////////////////////////////////
dtUtil::LogObserverConsole::LogObserverConsole()
{
}

////////////////////////////////////////////////////////////////////////////////
dtUtil::LogObserverConsole::~LogObserverConsole()
{
}

//////////////////////////////////////////////////////////////////////////
void dtUtil::LogObserverConsole::LogMessage(const LogData& logData)
{
   std::cout << Log::GetLogLevelString(logData.type) << " " 
      << std::setw(2) << std::setfill('0') << logData.time.tm_hour
      << std::setw(2) << std::setfill('0') << logData.time.tm_min 
      << std::setw(2) << std::setfill('0') << logData.time.tm_sec;
     
   if (!logData.logName.empty())
   {
      std::cout << " '" << logData.logName << "'";
   }

   if (!logData.file.empty())
   {
      std::cout << " " << logData.file;
   }

   if (!logData.method.empty())
   {
      std::cout << ":" << logData.method;
   }

   if (logData.line > 0)
   {
      std:: cout << ":" << logData.line;
   }

   std::cout << "-" <<  logData.msg << std::endl;
}

