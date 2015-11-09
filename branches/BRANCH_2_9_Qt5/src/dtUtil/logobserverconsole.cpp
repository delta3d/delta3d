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
 * Erik Johnson, Curtiss Murphy
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
   // Print out the time, the message, the log level, then where it came from
   std::cout << "[" << logData.time.ToString(dtUtil::DateTime::TimeFormat::CLOCK_TIME_24_HOUR_FORMAT);

   if (logData.frameNumber > 0)
   {
	   std::cout << " Frm# " << logData.frameNumber;
   }

   std::cout << " " << Log::GetLogLevelString(logData.type) << "] ";

   std::cout << logData.msg << " [";

   if (!logData.logName.empty())
   {
      std::cout << "'" << logData.logName << "' ";
   }

   if (!logData.method.empty())
   {
      std::cout << logData.method << "()";
   }

   if (!logData.file.empty())
   {
      std::cout << " " << logData.file;

      if (logData.line > 0)
      {
         std:: cout << "(" << logData.line << ")";
      }
   }

   std::cout << "]" << std::endl;
}

