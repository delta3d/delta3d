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

////////////////////////////////////////////////////////////////////////////////
void dtUtil::LogObserverConsole::LogMessage(Log::LogMessageType type, int hour, int min, int sec,
                                            const std::string& source, int line, const std::string& msg)
{
   std::cout << Log::GetLogLevelString(type) << ": "
      << std::setw(2) << std::setfill('0') << hour << ":"
      << std::setw(2) << std::setfill('0') << min << ":"
      << std::setw(2) << std::setfill('0') << sec << ":<"
      << source;

   if (line > 0)
   {
      std:: cout << ":" << line;
   }

   std::cout << ">" << msg << std::endl;
}
